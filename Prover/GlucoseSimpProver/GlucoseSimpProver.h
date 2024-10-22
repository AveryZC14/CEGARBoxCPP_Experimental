#ifndef GLUCOSESIMP_PROVER_H
#define GLUCOSESIMP_PROVER_H

#include "../../Clausifier/FormulaTriple/FormulaTriple.h"
#include "../../Clausifier/LtlFormulaTriple/LtlFormulaTriple.h"
#include "../../Formula/Atom/Atom.h"
#include "../../Formula/FEnum/FEnum.h"
#include "../../Formula/Not/Not.h"
#include "../../Formula/Or/Or.h"
#include "../Prover/Prover.h"
#include "../../Defines/Defines.h"
#include <exception>
#include <memory>
// #include <minisat/core/Solver.h>
#include <minisat/simp/SimpSolver.h>
#include "../../GlucoseSolver/glucose/simp/SimpSolver.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class GlucoseSimpProver : public Prover {
private:
  shared_ptr<Glucose::SimpSolver> solver =
      shared_ptr<Glucose::SimpSolver>(new Glucose::SimpSolver());

  unordered_map<string, Glucose::Var> variableMap;
  unordered_map<Glucose::Var, string> nameMap;

  void prepareFalse();
  void prepareExtras(name_set extra);
  void prepareClauses(clause_list clauses);
  void prepareClauses(clause_set clauses);
  void prepareLtlClauses(ltl_clause_list modal_clauses,
                           ltl_implications &ltlImplications,
                           bool isEventuality, bool succInSat);

  void prepareModalClauses(modal_clause_set modal_clauses,
                           modal_names_map &newExtra,
                           modal_lit_implication &modalLits,
                           modal_lit_implication &modalFromRight);

  Glucose::Var
  createOrGetVariable(string name,
                      Glucose::lbool polarity = Glucose::lbool((uint8_t)2));
  Glucose::Lit makeLiteral(shared_ptr<Formula> formula);
  Glucose::Lit makeLiteral(Literal literal);

  shared_ptr<Glucose::vec<Glucose::Lit>>
  convertAssumptions(literal_set assumptions);
  literal_set
  convertConflictToAssumps(Glucose::vec<Glucose::Lit> &conflictLits);

  bool modelSatisfiesAssump(Literal assumption);

  virtual int getLiteralId(Literal literal);

  int numLits = 0;
  unordered_set<string> biasOpposite = {};

public:
  GlucoseSimpProver(bool onesat = false);
  ~GlucoseSimpProver();

  modal_names_map prepareSAT(FormulaTriple clauses,
                             name_set extra = name_set());
  void prepareLtlfSat(LtlFormulaTriple clauses, Literal initialLiteral, bool succInSat);
  void prepareSAT(LtlFormulaTriple clauses);
  Solution solve(const literal_set &assumptions = literal_set());
  Solution solveReduced(const literal_set &assumptions = literal_set());
  void reduce_conflict(literal_set& conflict);
  void addClause(literal_set clause);

  void printModel();

  literal_set getModel();


    static shared_ptr<Glucose::SimpSolver> completeSolver;
    shared_ptr<Glucose::SimpSolver> calcSolver;
};

#endif
