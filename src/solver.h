/*
    Solver class. Takes a conjunction_clause object. Call solve() to get result.
*/

#ifndef SOLVER_H
#define SOLVER_H

#include "conjunction_clause.h"
#include "implication_graph.h"
#include <vector>
#include <set>

/*
    not refactored:

    Solver(Conjunction_clause c, bool v = false);
    std::vector<Literal> get_model();

    bool BCP();
    int analyze_conflict();
    bool decide();
    void backtrack(int backtrack_level);
    Implication_graph ig;
    std::vector<Disjunction_clause> learned_clauses;


    Node get_first_UIP(size_t decision_level);
    bool stop_criterion_met(Disjunction_clause &cl, const Node &uip);
    Literal get_last_assigned_literal(Disjunction_clause dc);
    Disjunction_clause get_antecedent_clause(Literal l);
    void add_learned_clause(Disjunction_clause dc);
    int get_backtrack_level(Disjunction_clause dc);
    bool check_conflict();

*/


class Implication_graph;

class Solver{
public:
    Solver(Conjunction_clause c, bool v = false);
    bool solve();
    std::vector<Literal> get_model();
private:
    bool boolean_constraint_propagation();
    int analyze_conflict();
    bool decide();
    void backtrack(int backtrack_level);
    Implication_graph ig;
    std::vector<Disjunction_clause> learned_clauses;


    Node get_first_UIP(size_t decision_level);
    bool stop_criterion_met(Disjunction_clause &cl, const Node &uip);
    Literal get_last_assigned_literal(Disjunction_clause dc);
    Disjunction_clause get_antecedent_clause(Literal l);
    void add_learned_clause(Disjunction_clause dc);
    int get_backtrack_level(Disjunction_clause dc);
    bool check_conflict();



    Conjunction_clause clause;
    std::vector<std::pair<Literal, Disjunction_clause>> assignment;
    std::vector<Variable> unassigned_variables;
    int decision_level;
    bool verbose;
};

#endif