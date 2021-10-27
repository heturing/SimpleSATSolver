/*
    Solver class. Takes a conjunction_clause object. Call solve() to get result.
*/

#ifndef SOLVER_H
#define SOLVER_H

#include "conjunction_clause.h"
#include "implication_graph.h"
#include <vector>
#include <set>


class Implication_graph;

class Solver{
friend void dump_debug_info(const Solver &s);
public:
    Solver(Conjunction_clause c, bool v = false);
    bool solve();
    std::vector<Literal> get_model();
private:
    void trace_new_assignment(const Literal &l, const Disjunction_clause &dc);
    void update_implication_graph_with_new_propagation(const Literal &l, const Disjunction_clause &dc);
    void update_implication_graph_if_conflict(const Disjunction_clause &dc);
    std::vector<Disjunction_clause> get_all_clauses();
    void record_a_propagation(const Literal &propagated_literal, const Disjunction_clause &by_clause);
    bool boolean_constraint_propagation();
    bool stop_criterion_met(Disjunction_clause &cl, const Node &uip);
    Literal get_last_assigned_literal(Disjunction_clause dc);
    Node get_first_UIP(size_t decision_level);
    int analyze_conflict();
    Disjunction_clause get_antecedent_clause(Literal l);
    void add_learned_clause(Disjunction_clause dc);
    int get_backtrack_level(const Disjunction_clause &dc);
    bool decide();
    void backtrack(int backtrack_level);



    Conjunction_clause clause;
    std::vector<Variable> unassigned_variables;
    int decision_level;
    bool verbose;
    Disjunction_clause current_conflict_clause;
    std::vector<std::pair<Literal, Disjunction_clause>> assignment;
    std::vector<Disjunction_clause> learned_clauses;
    Implication_graph ig;
};

#endif