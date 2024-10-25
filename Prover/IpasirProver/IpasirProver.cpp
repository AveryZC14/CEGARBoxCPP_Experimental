#include "IpasirProver.h"

shared_ptr<Lingeling> IpasirProver::completeSolver = make_shared<Lingeling>();
shared_ptr<unordered_map<string, int>> IpasirProver::completeVariableMap = make_shared<unordered_map<string, int>>();

IpasirProver::IpasirProver(bool onesat) {
  // solver->random_var_freq = 0;
  // solver->rnd_init_act = true;
  // solver->ccmin_mode = 1;
  // solver->clause_decay = 0;
  // solver->var_decay = 0.5;
  // solver->luby_restart = false;
  // solver->
  // solver->eliminate(true);
  if (onesat) {
    calcSolver = completeSolver;
    calcVariableMap = completeVariableMap;
  } else {
      calcSolver = solver;
      calcVariableMap = make_shared<unordered_map<string, int>>(variableMap);
  }
}
IpasirProver::~IpasirProver() {}

modal_names_map IpasirProver::prepareSAT(FormulaTriple clauses,
                                          name_set extra) {

  prepareExtras(extra);

  modal_names_map newExtra;
  prepareModalClauses(clauses.getBoxClauses(), newExtra, boxLits, boxFromRight);
  prepareModalClauses(clauses.getDiamondClauses(), newExtra, diamondLits,
                      diamondFromRight);

  prepareFalse();
  prepareClauses(clauses.getClauses());

  return newExtra;
}

void IpasirProver::prepareFalse() {
  calcSolver->add_clause(vector<int> {-createOrGetVariable("$false")});
}

void IpasirProver::prepareExtras(name_set extra) {
  for (string name : extra) {
    createOrGetVariable(name);
  }
}

void IpasirProver::prepareClauses(clause_set clauses) {
  for (Clause clause : clauses) {
    if (clause.formula->getType() == FOr) {
      vector<int> literals;
      for (shared_ptr<Formula> subformula :
           dynamic_cast<Or *>(clause.formula.get())->getSubformulas()) {
        literals.push_back(makeLiteral(subformula));
      }
      calcSolver->add_clause(literals);
    } else {
      calcSolver->add_clause(vector<int> {makeLiteral(clause.formula)});
    }
  }
}

void IpasirProver::prepareModalClauses(modal_clause_set modal_clauses,
                                        modal_names_map &newExtra,
                                        modal_lit_implication &modalLits,
                                        modal_lit_implication &modalFromRight) {
  for (ModalClause clause : modal_clauses) {
    if (clause.left->getType() == FAtom) {
      createOrGetVariable(getPrimitiveName(clause.left));
    } else if (clause.left->getType() == FNot) {
      createOrGetVariable(getPrimitiveName(clause.left));
    }
    newExtra[clause.modality].insert(getPrimitiveName(clause.right));
    createModalImplication(clause.modality, toLiteral(clause.left),
                           toLiteral(clause.right), modalLits, modalFromRight);
  }
}

int IpasirProver::createOrGetVariable(string name) {
  if (calcVariableMap->find(name) == calcVariableMap->end()) {
    int newVar = calcSolver->create_variable();
    (*calcVariableMap)[name] = newVar;
    nameMap[(*calcVariableMap)[name]] = name;
  }
    if (nameMap.find((*calcVariableMap)[name]) == nameMap.end()) {
        nameMap[(*calcVariableMap)[name]] = name;
    }
  return (*calcVariableMap)[name];
}

int IpasirProver::makeLiteral(shared_ptr<Formula> formula) {
  if (formula->getType() == FAtom) {
    string name = dynamic_cast<Atom *>(formula.get())->getName();
    return createOrGetVariable(name);
  } else if (formula->getType() == FNot) {
    string name = dynamic_cast<Atom *>(
                      dynamic_cast<Not *>(formula.get())->getSubformula().get())
                      ->getName();
    return -createOrGetVariable(name);
  }
  throw invalid_argument("Expected Atom or Not but got " + formula->toString());
}

shared_ptr<vector<int>>
IpasirProver::convertAssumptions(literal_set assumptions) {
  shared_ptr<vector<int>> literals (new vector<int> ());
  for (Literal assumption : assumptions) {
    int variable = (*calcVariableMap)[assumption.getName()];
    literals->push_back(assumption.getPolarity() ? variable 
                                            : -variable);
  }
  return literals;
}

void IpasirProver::reduce_conflict(literal_set& conflict) {
    literal_set all_lits = conflict;
    for (auto lit_to_remove : all_lits) {
        literal_set new_conflict;
        for (auto x : conflict) if (x != lit_to_remove) new_conflict.insert(x);
        if (new_conflict.size() < conflict.size()) {
            Solution sol = solve(new_conflict);
            if (!sol.satisfiable) {
                conflict = sol.conflict;
            }
        }
    }
}

bool IpasirProver::modelSatisfiesAssump(Literal assumption) {
  if (calcVariableMap->find(assumption.getName()) == calcVariableMap->end()) {
    return false;
  }
  int val = (*calcVariableMap)[assumption.getName()];
  int res = calcSolver->get_model(val, val)[0];
  if (res == 0) {
    throw runtime_error("Model value for " + assumption.getName() +
                        " is undefined");
  }
  return ((res>0) && assumption.getPolarity()) ||
         ((res<0) && !assumption.getPolarity());
}

literal_set IpasirProver::convertConflictToAssumps(
    vector<int> conflictLits) {
  literal_set conflict;

  for (int i = 0; i < conflictLits.size(); i++) {
    conflict.insert(Literal(nameMap[abs(conflictLits[i])],
                            conflictLits[i]>0));
  }
  return conflict;
}

Solution IpasirProver::solve(const literal_set &assumptions) {
  Solution solution;
  shared_ptr<vector<int>> vecAssumps =
      convertAssumptions(assumptions);
  /*
  cout << "SOLVING: " << endl;
  for (auto x : assumptions) cout << x.toString() << " ";
  cout << endl;
  */
  solution.satisfiable = calcSolver->solve(*vecAssumps);
  if (!solution.satisfiable) {
    solution.conflict = convertConflictToAssumps(calcSolver->used_assumptions());
  } else {
  }
  return solution;
}

void IpasirProver::addClause(literal_set clause) {
  calcSolver->add_clause(*convertAssumptions(clause));
}

int IpasirProver::getLiteralId(Literal literal) {
  return (*calcVariableMap)[literal.getName()];
}

void IpasirProver::printModel() {
    cout << "NOT IMPLEMENTED" << endl;
}

literal_set IpasirProver::getModel() {
    literal_set model;

    for (auto x : calcSolver->get_model()) {
        if (nameMap.find(abs(x)) != nameMap.end()) {
            model.insert(
                Literal(nameMap[abs(x)], x > 0)
            );
        } 
    }
    /*
    cout << "RETURNING MODEL: ";
    for (auto x : model) cout << x.toString() << " ";
    cout << endl;
    cout << model.size() << " / " << nameMap.size() << endl;

    cout << "COMPLETE NAME MAP: " << endl;
    for (auto x : nameMap) cout << x.first << ", " << x.second << endl;
    cout << endl;
    */
    return model;
}

void IpasirProver::prepareLtlfSat(LtlFormulaTriple clauses, Literal initialLiteral, bool succInSat){
  return;
}