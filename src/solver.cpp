#include "solver.h"
#include "disjunction_clause.h"
#include "heuristic.h"
#include "solver_exception.h"
#include "debug.h"

Solver::Solver(Conjunction_clause c, bool v) : clause(c), unassigned_variables(c.get_variables_in_clause()), decision_level(0), verbose(v) {}

void Solver::backtrack(int backtrack_level){
    /*
    Backtrack to a decision level. Update assignment, unassigned_variables, and implication_graph. Reset decision_level.
    */
    auto erased_nodes = ig.backtrack(backtrack_level);

    // backtrack assignment and unassigned_variables
    for(auto n : erased_nodes){
        auto lit = n->get_literal();
        auto iter = std::find_if(assignment.begin(), assignment.end(), [lit](const std::pair<Literal, Disjunction_clause> &p) -> bool {return p.first == lit;});
        if(iter != assignment.end()){
            assignment.erase(iter);
        }

        auto var = lit.get_variable();
        unassigned_variables.push_back(var);
    }
    decision_level = backtrack_level;
}

bool Solver::solve(){
    /*
    Check the satisfiability of the given CNF in this solver by CDCL algorithm.
    If the CNF is satisfiable, this function returns true, and the solution is saved in assignment.
    Otherwise, this function returns false. 
    */

    while(true){
        while(boolean_constraint_propagation()){
            int backtrack_level = analyze_conflict();
            if(backtrack_level < 0){
                return false;
            }
            backtrack(backtrack_level);
        }
        bool dec = decide();
        if(!dec){
            return true;
        }
    }
}

void Solver::update_implication_graph_if_conflict(const Disjunction_clause &dc){
    /*
    Add conflict edges from each node for literals in dc to conflict node.
    */
    for(auto lit : dc.get_literals()){
        Node n(Literal(), decision_level);
        try{
            n = ig.get_node_from_literal(lit);
        }
        catch(const std::runtime_error &e){
            n = ig.get_node_from_literal(!lit);
        }
        ig.add_conflict_edge(n, dc);
    }
}

void Solver::trace_new_assignment(const Literal &l, const Disjunction_clause &dc){
    /*
    Update assignment and unassigned_variables when new literal is assigned.
    */
    assignment.push_back({l, dc});
    for(auto iter = unassigned_variables.begin(); iter != unassigned_variables.end(); ++iter){
        if(*iter == l.get_variable()){
            unassigned_variables.erase(iter);
            break;
        }
    }
}

void Solver::update_implication_graph_with_new_propagation(const Literal &l, const Disjunction_clause &dc){
    /*
    Construct new node and add new edges when new literal is assigned.
    Specifically, for clause (a,b,c) => a, add edge !b => a and !c => a with label (a,b,c)
    */
    
    for(auto lit : dc.get_literals()){
        if(lit != l){
            Node head = ig.get_node_from_literal(!lit);
            Node tail = Node(l, decision_level);
            ig.add_edge(head, tail, dc);
        }
        if(decision_level == 0){
            ig.add_edge(*(ig.root), Node(l, decision_level), dc);
        }
    }
}

std::vector<Disjunction_clause> Solver::get_all_clauses(){
    /*
    Return the sum of original clauses and the learned clauses.
    Make sure the learned clauses are consider first.
    */

    std::vector<Disjunction_clause> all_clauses = clause.get_clauses();
    all_clauses.insert(all_clauses.begin(), learned_clauses.begin(), learned_clauses.end());
    return all_clauses;
}

void Solver::record_a_propagation(const Literal &propagated_literal, const Disjunction_clause &by_clause){
    /*
    update the underlying assignment, unassigned_variables, and implication_graph if new literal is propagated.
    */
    trace_new_assignment(propagated_literal, by_clause);
    update_implication_graph_with_new_propagation(propagated_literal, by_clause);
}

bool Solver::boolean_constraint_propagation(){
    /*
    Propagate literals from each DNF in the given CNF based on current assignment. If the propagation leads to a conflict,
    return true. Otherwise return false.
    */
    bool new_literal_propagated;
    do{
        new_literal_propagated = false;

        std::vector<Disjunction_clause> all_clauses = get_all_clauses();
        for(auto clause : all_clauses){
            Literal propagated_literal;
            try{
                propagated_literal = Disjunction_clause::propagate_clause(clause, assignment);
            }
            catch(const ConflictClauseException &e){
                // conflict found on clause.
                update_implication_graph_if_conflict(clause);
                this->current_conflict_clause = clause;
                return true;
            }
            catch(const PropagationException &e){
                continue;
            }

            new_literal_propagated = true;
            record_a_propagation(propagated_literal, clause); 
        }
    }while(new_literal_propagated);

    return false;
}

Node Solver::get_first_UIP(size_t decision_level){
    /*
    Return the node for first UIP for current implication graph.
    */
    return ig.get_first_UIP(decision_level);
}

bool Solver::stop_criterion_met(Disjunction_clause &cl, const Node &uip){
    /*
    Return true iff cl contains the negation of the first UIP as its single literal at the current decision level.
    Assumption: All literals in cl have been assigned.
    */

    size_t decision_level_of_UIP = uip.get_decision_level();

    // Count how many literals in cl has the same decision level as n.
    size_t count = 0;
    auto literals_in_clause = cl.get_literals();
    for(auto l : literals_in_clause){
        if(ig.get_decision_level_of_literal(l) == decision_level_of_UIP){
            ++count;
        }
    }
    if(count != 1){
        return false;
    }

    // check if the the negation of the literal of uip is in cl.
    auto iter = std::find(literals_in_clause.begin(), literals_in_clause.end(), !(uip.get_literal()));
    if(iter != literals_in_clause.end()){
        return true;
    }
    return false;
}

Literal Solver::get_last_assigned_literal(Disjunction_clause dc){
    /*
    Return the literal that is the latest assigned in dc.
    */
    auto literals = dc.get_literals();
    for(auto iter = assignment.rbegin(); iter != assignment.rend(); ++iter){
        // if iter->first in dc.literals
        auto it = std::find_if(literals.begin(), literals.end(), [iter](Literal l) -> bool {return iter->first.get_variable() == l.get_variable();});
        if(it != literals.end()){
            return *it;
        }
    }
    throw std::runtime_error("None of literals in this clause is assigned.");
}

Disjunction_clause Solver::get_antecedent_clause(Literal l){
    /*
    Return the clause that implies the assignmend of l. For decision node, we return an empty clause.
    */
   auto iter = std::find_if(assignment.begin(), assignment.end(), [l](std::pair<Literal, Disjunction_clause> p)-> bool {return p.first == l;});
   if(iter != assignment.end()){
       return iter->second;
   }
   throw std::runtime_error("Literals is not assigned.");
}

void Solver::add_learned_clause(Disjunction_clause dc){
    /*
    Store learned clause.
    */
    learned_clauses.push_back(dc);
}

int Solver::get_backtrack_level(const Disjunction_clause &dc){
    /*
    Return the second largest decision level among literals in dc.
    */
    std::vector<int> v;
    auto literals = dc.get_literals();
    for(auto lit : literals){
        int dl = ig.get_decision_level_of_literal(lit);
        v.push_back(dl);
    }

    std::sort(v.begin(), v.end());

    if(v.size() == 1){
        return 0;
    }
    else if(v.size() > 1){
        return v[v.size()-2];
    }
    else{
        throw std::runtime_error("Learned nothing.");
    }
}

int Solver::analyze_conflict(){
    /*
    Return the decision level that we should backtrack to.
    Assumption: current assignment leads to a conflict.
    */

    if(decision_level == 0){
        return -1;
    }

    Node uip = get_first_UIP(decision_level);

    while(!stop_criterion_met(this->current_conflict_clause, uip)){
        Literal l = get_last_assigned_literal(this->current_conflict_clause);
        Variable v = l.get_variable();
        // since assigning l leads to conflict, not l must has been assigned before.
        Disjunction_clause antecedent = get_antecedent_clause(!l);
        this->current_conflict_clause = Disjunction_clause::resolve(this->current_conflict_clause, antecedent, v);
    }

    add_learned_clause(this->current_conflict_clause);
    int dl = get_backtrack_level(this->current_conflict_clause);
    return dl;
}

bool Solver::decide(){
    /*
        Choose an unassigned variable and assign a value to it.
        Various heuristic may apply when choosing the variable to assign.
    */

    if(unassigned_variables.empty()){
        return false;
    }

    Heuristic h;
    Literal variable_chosen_by_heuristic = h.choose_decide_literal(unassigned_variables);

    // record new dicision.
    trace_new_assignment(variable_chosen_by_heuristic, Disjunction_clause());
    ig.add_decision_node(variable_chosen_by_heuristic, ++decision_level);

    return true;
}

std::vector<Literal> Solver::get_model(){
    /*
    Return the literals for current assignment.
    */
    std::vector<Literal> v;
    for(auto p : assignment){
        v.push_back(p.first);
    }
    return v;
}
