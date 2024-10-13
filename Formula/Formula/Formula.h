#ifndef FORMULA_H
#define FORMULA_H

#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>

#include "../FEnum/FEnum.h"

using namespace std;

/**
 * This is an abstract class for a logical formula. 
 * Formulae are recursively defined, with one formula object holding one operator, E.G A&B is an object of the "And" class, which have the formulae A and B inside a set as a private variable.
 */
class Formula {
   public:
    //constructor
    Formula();

    //destructor
    virtual ~Formula();

    /**
     * Gives the string representation of this formula
     *
     * @return the string representation of this formula
     */
    virtual string toString() const = 0;

    /**
     * Gets the type for this fomula
     *
     * @return the type for this fomula in the FormulaType enum type
     */
    virtual FormulaType getType() const = 0;

    /**
     * Converts this formula into (NNF) Negated Normal Form
     *
     * @return this formula in Negated Normal Form
     */
    virtual shared_ptr<Formula> negatedNormalForm() = 0;

    /**
     * Converts this formula into (NNF) Negated Normal Form
     *
     * @return this formula in Tail Normal Form
     */
    virtual shared_ptr<Formula> tailNormalForm() = 0;

    /**
     * gives the negation of this fromula
     *
     * @return this formula negated
     */
    virtual shared_ptr<Formula> negate() = 0;


    virtual shared_ptr<Formula> simplify() = 0;
    virtual shared_ptr<Formula> modalFlatten() = 0;
    virtual shared_ptr<Formula> axiomSimplify(int axiom, int depth) = 0;

    virtual shared_ptr<Formula> clone() const = 0;

    virtual bool isPrimitive() const;

    virtual bool operator==(const Formula &other) const = 0;
    virtual bool operator!=(const Formula &other) const = 0;

    virtual size_t hash() const = 0;

    static unordered_set<int> discoveredModalities;
};

struct Deref {
    struct Hash {
        template <typename T>
        std::size_t operator()(std::shared_ptr<T> const &p) const {
            return (*p).hash();
        }
    };
    struct Compare {
        template <typename T>
        size_t operator()(std::shared_ptr<T> const &a,
                          std::shared_ptr<T> const &b) const {
            return *a == *b;
        }
    };
};

typedef unordered_set<shared_ptr<Formula>, Deref::Hash, Deref::Compare>
    formula_set;

struct FormulaDetails {
    // Details about formula in NNF (from KSP-LocalIsBest)
    formula_set diffDia;
    formula_set diffBox;
    int maxLevelDia = 0;
};

#endif

