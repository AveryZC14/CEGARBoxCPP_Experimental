#include "GlucoseSimpProver.h"

shared_ptr<Glucose::SimpSolver> GlucoseSimpProver::completeSolver = shared_ptr<Glucose::SimpSolver>(new Glucose::SimpSolver());


GlucoseSimpProver::GlucoseSimpProver(bool onesat) {
  // solver->random_var_freq = 0;
  // solver->rnd_init_act = true;
  // solver->ccmin_mode = 1;
  // solver->clause_decay = 0;
  // solver->var_decay = 0.5;
  // solver->luby_restart = false;
  // solver->

    onesat = false;
  if (onesat) {
        completeSolver->eliminate(true);
        //completeSolver->verbosity=2;
    calcSolver = completeSolver;
    //calcSolver = solver;

  } else {
        solver->eliminate(true);
        //solver->verbosity=2;
      calcSolver = solver;
  }
 // calcSolver->verbosity = 2;
}
GlucoseSimpProver::~GlucoseSimpProver() {}

modal_names_map GlucoseSimpProver::prepareSAT(FormulaTriple clauses,
                                          name_set extra) {

  modal_names_map newExtra;
  prepareModalClauses(clauses.getDiamondClauses(), newExtra, diamondLits,
                      diamondFromRight);
  prepareModalClauses(clauses.getBoxClauses(), newExtra, boxLits, boxFromRight);
  prepareExtras(extra);
  prepareFalse();
  prepareClauses(clauses.getClauses());

  return newExtra;
}

void GlucoseSimpProver::prepareLtlfSat(LtlFormulaTriple clauses, Literal initialLiteral, bool succInSat) {

    // If we did the ~ <> a -> ~ X a optimisation
                              // Bias literals the opposite way
    for (literal_set clause : clauses.getClauses()) {
        for (Literal lit : clause) {
            // check if literal starts with ex$
            if (lit.getName().substr(0, 3) == "ex$") {
                biasOpposite.insert(lit.getName().substr(3));
            }
        }

        //cout << "BIAS: " << biasOpposite.size() << endl;; 
    }

  createOrGetVariable(initialLiteral.getName(), Glucose::lbool((uint8_t)0));
  //createOrGetVariable("$E~tail", Glucose::lbool((uint8_t)1));
  createOrGetVariable("tail", Glucose::lbool((uint8_t)0));
  //createOrGetVariable("$loop_start", Glucose::lbool((uint8_t)1));


  prepareLtlClauses(clauses.getEventualityClauses(), ltlEventualityImplications, true, succInSat);
  prepareLtlClauses(clauses.getStepClauses(), ltlStepImplications, false, succInSat);
  /*
    for (literal_set clause : clauses.getClauses()) {
        for (Literal lit : clause) createOrGetVariable(lit.getName());
        calcSolver->addClause(*convertAssumptions(clause));
    }
    */
  prepareClauses(clauses.getClauses());
  prepareFalse();
  /*
  if (succInSat) {
    calcSolver->addClause(~Glucose::mkLit(createOrGetVariable("$false'")));

      for (literal_set clause : clauses.getClauses()) {
          for (Literal lit : clause) createOrGetVariable(lit.getName() +"'");
          calcSolver->addClause(*convertAssumptions(clause));
        }
  }
  */
}
/*
void GlucoseSimpProver::prepareLtlSat(LtlFormulaTriple clauses, Literal initialLiteral) {
  prepareLtlSat(clauses, Literal("$initial"), false);

  */

void GlucoseSimpProver::prepareFalse() {
    calcSolver->addClause(~Glucose::mkLit(createOrGetVariable("$false")));
}

void GlucoseSimpProver::prepareExtras(name_set extra) {
  for (string name : extra) {
    createOrGetVariable(name);//, Glucose::lbool((uint8_t)0));
  }
}

void GlucoseSimpProver::prepareClauses(clause_set clauses) {
  for (Clause clause : clauses) {
    if (clause.formula->getType() == FOr) {
      Glucose::vec<Glucose::Lit> literals;
      for (shared_ptr<Formula> subformula :
           dynamic_cast<Or *>(clause.formula.get())->getSubformulas()) {
        literals.push(makeLiteral(subformula));
      }

      calcSolver->addClause(literals);

    } else {
        calcSolver->addClause(makeLiteral(clause.formula));
    }
  }
}

void GlucoseSimpProver::prepareClauses(clause_list clauses) {
    // For the ltl prover
  for (literal_set clause : clauses) {
 
      for (Literal lit : clause) createOrGetVariable(lit.getName(), Glucose::lbool((uint8_t)lit.getPolarity()));

      calcSolver->addClause(*convertAssumptions(clause));
    }
  }
void GlucoseSimpProver::prepareModalClauses(modal_clause_set modal_clauses,
                                        modal_names_map &newExtra,
                                        modal_lit_implication &modalLits,
                                        modal_lit_implication &modalFromRight) {
  for (ModalClause clause : modal_clauses) {
    if (clause.left->getType() == FAtom) {
      createOrGetVariable(getPrimitiveName(clause.left),
                          Glucose::lbool((uint8_t)0));
    } else if (clause.left->getType() == FNot) {
      createOrGetVariable(getPrimitiveName(clause.left),
                          Glucose::lbool((uint8_t)1));
    }
    newExtra[clause.modality].insert(getPrimitiveName(clause.right));


    createModalImplication(clause.modality, toLiteral(clause.left),
                           toLiteral(clause.right), modalLits, modalFromRight);
  }
}

void GlucoseSimpProver::prepareLtlClauses(ltl_clause_list modal_clauses,
                                        ltl_implications &ltlImplications, bool isEventuality, bool succInSat) {
  for (LtlClause clause : modal_clauses) {
      for (Literal lit : clause.left) {
        createOrGetVariable(lit.getName(), Glucose::lbool((uint8_t)!lit.getPolarity()));
        if (succInSat) {
            createOrGetVariable(lit.getName() + "'");
        }
      }
      string rightName = clause.right.getName();
      if (isEventuality) {
          createOrGetVariable(clause.right.getName(), Glucose::lbool((uint8_t)!clause.right.getPolarity()));
          rightName = "$E" + clause.right.toString();
          Literal eRight = Literal(rightName, 1);
          litToEventuality.emplace(clause.right, eRight);
          eventualityToLit.emplace(eRight, clause.right);

          // If every eventuality has X a -> <> a optimisation, then bias the eventuality opposite
          // +1 for tail
          //cout << biasOpposite.size() << " / " << modal_clauses.size() << endl;
          if (biasOpposite.size() + 1 == modal_clauses.size()) {
              //cout << "BIASING " << endl;

            createOrGetVariable(eRight.getName(), Glucose::lbool((uint8_t)!clause.right.getPolarity()));
          } else {
            createOrGetVariable(eRight.getName(), Glucose::lbool((uint8_t)!clause.right.getPolarity()));
          }

        // Don't trigger if evenetuality fulfilled
        clause.left.insert(~clause.right);
        if (succInSat) {
            createOrGetVariable(eRight.getName() + "'");
            createOrGetVariable(clause.right.getName() + "'");
        }
        //createLtlImplication(clause.left, eRight, ltlImplications, succInSat);
        // convert LTL clause above to classical clause
        literal_set classical {eRight};
        for (auto x : clause.left) classical.insert(~x);
        //cout << "CLASSICAL: " << litsetString(classical) << endl;
        addClause(classical);
    
        createLtlImplication({eRight, ~clause.right}, eRight, ltlImplications, succInSat);
      } else {
            createOrGetVariable(clause.right.getName(), Glucose::lbool((uint8_t)!clause.right.getPolarity()));
        if (succInSat) {
            createOrGetVariable(clause.right.getName() + "'");
        }
        createLtlImplication(clause.left, clause.right, ltlImplications, succInSat);
      }
    }     
}

Glucose::Var GlucoseSimpProver::createOrGetVariable(string name,
                                                Glucose::lbool polarity) {
  if (variableMap.find(name) == variableMap.end()) {
    Glucose::Var newVar =calcSolver->newVar(polarity);  
    variableMap[name] = newVar;
    nameMap[variableMap[name]] = name;
  }
  return variableMap[name];
}

Glucose::Lit GlucoseSimpProver::makeLiteral(shared_ptr<Formula> formula) {
  if (formula->getType() == FAtom) {
    string name = dynamic_cast<Atom *>(formula.get())->getName();
    return Glucose::mkLit(createOrGetVariable(name));
  } else if (formula->getType() == FNot) {
    string name = dynamic_cast<Atom *>(
                      dynamic_cast<Not *>(formula.get())->getSubformula().get())
                      ->getName();
    return ~Glucose::mkLit(createOrGetVariable(name));
  }
  throw invalid_argument("Expected Atom or Not but got " + formula->toString());
}

Glucose::Lit GlucoseSimpProver::makeLiteral(Literal literal) {
    if (literal.getPolarity())                                        {
        return Glucose::mkLit(createOrGetVariable(literal.getName()));
  } else {
        return ~Glucose::mkLit(createOrGetVariable(literal.getName()));
  }
}
shared_ptr<Glucose::vec<Glucose::Lit>>
GlucoseSimpProver::convertAssumptions(literal_set assumptions) {
  shared_ptr<Glucose::vec<Glucose::Lit>> literals =
      shared_ptr<Glucose::vec<Glucose::Lit>>(new Glucose::vec<Glucose::Lit>());

  for (Literal assumption : assumptions) {
    Glucose::Var variable = variableMap.at(assumption.getName());
    literals->push(assumption.getPolarity() ? Glucose::mkLit(variable)
                                            : ~Glucose::mkLit(variable));

  } 
  return literals;
}

bool GlucoseSimpProver::modelSatisfiesAssump(Literal assumption) {
  if (variableMap.find(assumption.getName()) == variableMap.end()) {
    return false;
  }
  int lbool =
      Glucose::toInt(calcSolver->modelValue(variableMap[assumption.getName()]));
  if (lbool == 2) {
    throw runtime_error("Model value for " + assumption.getName() +
                        " is undefined");
  }
  return (lbool == 0 && assumption.getPolarity()) ||
         (lbool == 1 && !assumption.getPolarity());
}

literal_set GlucoseSimpProver::convertConflictToAssumps(
    Glucose::LSet &conflictLits) {
  literal_set conflict;
  for (int i = 0; i < conflictLits.size(); i++) {
    conflict.insert(Literal(nameMap.at(Glucose::var(conflictLits[i])),
                            Glucose::sign(conflictLits[i])));
  }
  return conflict;
}

Solution GlucoseSimpProver::solve(const literal_set &assumptions) {
  Solution solution;
  shared_ptr<Glucose::vec<Glucose::Lit>> vecAssumps =
      convertAssumptions(assumptions);
  solution.satisfiable = calcSolver->solve(*vecAssumps);
  if (!solution.satisfiable) {
    solution.conflict = convertConflictToAssumps(calcSolver->conflict);
  }
  return solution;
}


Solution GlucoseSimpProver::solveReduced(const literal_set &assumptions) {
    literal_set trigs;

    for (auto modalityLitImplication : diamondLits) {
        for (auto literalImplication : modalityLitImplication.second) {
            trigs.insert(~literalImplication.first);
        }
    }


    for (auto modalityLitImplication : boxLits) {
        for (auto literalImplication : modalityLitImplication.second) {
            trigs.insert(~literalImplication.first);
        }
    }

    //cout << "BEGIN: ";
    while (true) {
        literal_set newAssumps;
        newAssumps.insert(assumptions.begin(), assumptions.end());
        newAssumps.insert(trigs.begin(), trigs.end());

      Solution solution;
      shared_ptr<Glucose::vec<Glucose::Lit>> vecAssumps =
          convertAssumptions(newAssumps);
      //cout << "+";
      solution.satisfiable = calcSolver->solve(*vecAssumps);
      if (!solution.satisfiable) {
        solution.conflict = convertConflictToAssumps(calcSolver->conflict);
        if (trigs.empty()) return solution;
        bool containsTrig = false;
        for (auto lit : solution.conflict)  {
            if ((trigs.find(lit) != trigs.end())) {
                containsTrig = true;
                trigs.erase(lit);
            }
        }
        if (!containsTrig) return solution;
      } else {
        return solution;
      }
    }
}

void GlucoseSimpProver::reduce_conflict(literal_set& conflict) {
    literal_set all_lits = conflict;
    int i = 0;
    //cout << "REDUCE: " << conflict.size() ;
    for (auto lit_to_remove : all_lits) {
        literal_set new_conflict;
        for (auto x : conflict) if (x != lit_to_remove) new_conflict.insert(x);
        if (new_conflict.size() < conflict.size()) {
            Solution sol = solve(new_conflict);
            if (!sol.satisfiable) {
                conflict = sol.conflict;
            }
        }
        i++;
    }
    //cout << " -> " << conflict.size() << endl;
}



void GlucoseSimpProver::addClause(literal_set clause) {
  calcSolver->addClause(*convertAssumptions(clause));
}

void GlucoseSimpProver::printModel() {
  for (auto varName : nameMap) {
    cout << varName.second << " is "
         << Glucose::toInt(calcSolver->modelValue(varName.first)) << endl;
  }
}

int GlucoseSimpProver::getLiteralId(Literal literal) {
  return variableMap[literal.getName()];
}

literal_set GlucoseSimpProver::getModel() {
    literal_set model;
    for (auto varName : nameMap) {
        model.insert(
            Literal(varName.second, 1 - Glucose::toInt(calcSolver->modelValue(varName.first)))
        );
    }
    return model;
                            
}

