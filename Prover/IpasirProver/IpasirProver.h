#ifndef IPASIR_PROVER_H
#define IPASIR_PROVER_H

#include "../../Clausifier/FormulaTriple/FormulaTriple.h"
#include "../../Formula/Atom/Atom.h"
#include "../../Formula/FEnum/FEnum.h"
#include "../../Formula/Not/Not.h"
#include "../../Formula/Or/Or.h"
#include "../Prover/Prover.h"
#include "../../Defines/Defines.h"
#include <exception>
#include <memory>
// #include <minisat/core/Solver.h>
// #include <minisat/simp/SimpSolver.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "lingeling.h"
using namespace std;

class IpasirProver : public Prover {
private:
  shared_ptr<Lingeling> solver =
      shared_ptr<Lingeling>(new Lingeling());

  unordered_map<string, int> variableMap;
  unordered_map<int, string> nameMap;

  void prepareFalse();
  void prepareExtras(name_set extra);
  void prepareClauses(clause_set clauses);
  void prepareModalClauses(modal_clause_set modal_clauses,
                           modal_names_map &newExtra,
                           modal_lit_implication &modalLits,
                           modal_lit_implication &modalFromRight);

  int createOrGetVariable(string name);
  int makeLiteral(shared_ptr<Formula> formula);

  shared_ptr<vector<int>>
  convertAssumptions(literal_set assumptions);
  literal_set
  convertConflictToAssumps(vector<int> conflictLits);

  bool modelSatisfiesAssump(Literal assumption);

  virtual int getLiteralId(Literal literal);
  literal_set getModel();

public:
  IpasirProver(bool onesat = false);
  ~IpasirProver();

  modal_names_map prepareSAT(FormulaTriple clauses,
                             name_set extra = name_set());
  Solution solve(const literal_set &assumptions = literal_set());
  void reduce_conflict(literal_set& conflict);
  void addClause(literal_set clause);
  void printModel();

    static shared_ptr<Lingeling> completeSolver;
    shared_ptr<Lingeling> calcSolver;
    static shared_ptr<unordered_map<string, int>> completeVariableMap;
    shared_ptr<unordered_map<string, int>> calcVariableMap;

    //patchwork
    void prepareLtlfSat(LtlFormulaTriple clauses, Literal initialLiteral, bool succInSat) = 0;
};

#endif
