/*
    Solver class. Takes a conjunction_clause object. Call solve() to get result.
*/

#ifndef SOLVER_H
#define SOLVER_H

#include "conjunction_clause.h"
#include "variable.h"
#include <vector>
#include <set>

class Solver{
public:
    Solver(Conjunction_clause c, std::vector<Variable> v) : clause(c), assignment(std::vector<std::pair<Variable, size_t>>()), decision_level(0) {}
    bool solve();
private:
    bool BCP();
    int analyze_conflict();
    Variable decide();


    Conjunction_clause clause;
    // assignment should not only keep the variable value, but also the decision level for choosing that value.
    std::vector<std::pair<Variable, size_t>> assignment;
    std::vector<Variable> unassigned_variables;

    int decision_level;
};

#endif