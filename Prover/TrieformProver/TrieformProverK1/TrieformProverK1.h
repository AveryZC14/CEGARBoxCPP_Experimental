#ifndef TRIEFORM_PROVER_K1
#define TRIEFORM_PROVER_K1

#include "../../../Bitset/Bitset.h"
#include "../../../Clausifier/Trieform/Trieform.h"
#include "../../../Clausifier/TrieformFactory/TrieformFactory.h"
#include "../../LocalSolutionMemo/LocalSolutionMemo.h"
#include <memory>
#include <string>
#include <unordered_map>

using namespace std;

class TrieformProverK1 : public Trieform {
protected:
  unsigned int assumptionsSize = 0;
  LocalSolutionMemo localMemo;
  unordered_map<string, unsigned int> idMap;

  shared_ptr<Bitset> convertAssumptionsToBitset(literal_set literals);
  void updateSolutionMemo(const shared_ptr<Bitset> &assumptions,
                          Solution solution);

public:
  TrieformProverK1();
  ~TrieformProverK1();


  vector<literal_set> allConflicts;
  
  static shared_ptr<Prover> completeProver; 

  Solution prove(literal_set assumptions = literal_set());
  Solution prove(int depth, literal_set assumptions = literal_set());
  virtual void preprocess();
  virtual void prepareSAT(name_set extra = name_set());
  virtual shared_ptr<Bitset> fleshedOutAssumptionBitset(literal_set model);

  virtual shared_ptr<Trieform> create(const shared_ptr<Formula> &formula);
  virtual shared_ptr<Trieform> create(const shared_ptr<Formula> &formula,
                                      const vector<int> &newModality);
  virtual shared_ptr<Trieform> create(const vector<int> &newModality);
  bool clauseConflictsWithModel(literal_set clause, literal_set model);
  virtual bool isSatisfiable();
};

#endif