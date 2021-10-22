#include "solver.h"
#include "disjunction_clause.h"
#include "heuristic.h"
#include "solver_exception.h"

Solver::Solver(Conjunction_clause c, bool v) : clause(c), unassigned_variables(c.get_variables_in_clause()), decision_level(0), verbose(v) {
    // Collect all variables in c, put these variables in unassigned_variables.
}

void Solver::backtrack(int backtrack_level){
    auto erased_nodes = ig.backtrack(backtrack_level);

    // backtrack assignment
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

bool Solver::check_conflict(){
    Disjunction_clause con_cl;
    for(auto cl : clause.get_clauses()){
        bool flag = true;
        for(auto lit : cl.get_literals()){
            // conflict if all !lit in assignment.
            auto neg_lit = !lit;
            auto iter = std::find_if(assignment.begin(), assignment.end(), [neg_lit](std::pair<Literal, Disjunction_clause> p) -> bool {return p.first == neg_lit;});
            if(iter == assignment.end()){
                flag = false;
            }
            con_cl = cl;
        }
        if(flag){
            // std::cout << "Conflict on:" << cl << std::endl; 
            // for each lit node in cl, add conflict edge from node to conflict.
            // ig.add_conflict_edge(ig.get_node_from_literal())
            for(auto lit : cl.get_literals()){
                Node n(Literal(), decision_level);
                try{
                    n = ig.get_node_from_literal(lit);
                }
                catch(const std::runtime_error &e){
                    n = ig.get_node_from_literal(!lit);
                }
                ig.add_conflict_edge(n, cl);
            }
            return true;
        }
    }
    return false;
}

bool Solver::boolean_constraint_propagation(){
    /*
    Propagate literals from each DNF in the given CNF based on current assignment.

    */
    bool new_literal_propagated;
    do{
        new_literal_propagated = false;

        auto all_clauses = learned_clauses;
        auto original_clauses = clause.get_clauses();
        all_clauses.insert(all_clauses.end(), original_clauses.begin(), original_clauses.end());

        for(auto clause : all_clauses){
            Literal propagated_literal;
            try{
                propagated_literal = Disjunction_clause::propagate_clause(clause, assignment);
            }
            catch(const PropagationException &e){
                continue;
            }
            new_literal_propagated = true;

            // todo : refactor 107 - 115 to a new function, assign_a_literal(Literal, Disjunction_clause)
            assignment.push_back({propagated_literal, clause});

            // remove literal from unassigned_vars
            for(auto iter = unassigned_variables.begin(); iter != unassigned_variables.end(); ++iter){
                if(*iter == propagated_literal.get_variable()){
                    unassigned_variables.erase(iter);
                    break;
                }
            }

            // todo : update implication_graph. new function.
            // for clause (a,b,c) => a, add edge !b => a and !c => a with label (a,b,c)
            for(auto lit : clause.get_literals()){
                if(lit != propagated_literal){
                    Node head = ig.get_node_from_literal(!lit);
                    Node tail = Node(propagated_literal, decision_level);
                    ig.add_edge(head, tail, clause);
                }
                if(decision_level == 0){
                    ig.add_edge(*(ig.root), Node(propagated_literal, decision_level), clause);
                }
            }
            
            if(check_conflict()){
                return true;
            }
        }
    }while(new_literal_propagated);


    // return true if there is no conflict, else return false.
    return false;
}

Node Solver::get_first_UIP(size_t decision_level){
    return ig.get_first_UIP(decision_level);
}

bool Solver::stop_criterion_met(Disjunction_clause &cl, const Node &uip){
    /*
    Return true iff cl contains the negation of the first UIP as its single literal at the current decision level.
    */

    // std::cout << "Testing if clause met stop criterion:" << cl << std::endl;

    size_t decision_level_of_UIP = uip.get_decision_level();
    std::vector<Literal> vl;
    std::vector<Node> vn;

    // Count how many literals in cl has the same decision level as n.
    size_t count = 0;
    for(auto l : cl.get_literals()){
        if(ig.find_decision_level_of_variable(l.get_variable()) == decision_level_of_UIP){
            ++count;
        }
    }
    // std::cout << count << " lietrals in " << decision_level_of_UIP << std::endl;
    if(count != 1){
        return false;
    }

    auto iter = std::find(cl.get_literals().begin(), cl.get_literals().end(), !(uip.get_literal()));
    if(iter != cl.get_literals().end()){
        return true;
    }
    return false;
}

Literal Solver::get_last_assigned_literal(Disjunction_clause dc){
    // std::cout << "Trying to get last assignment literal of " << dc << std::endl;
    // std::cout << "Current assignment:" << std::endl;
    // for(auto p : assignment){
    //     std::cout << p.first << std::endl;
    // }

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
    when new literal is assigned, we also keep track the clause that leads to this assignment. (for decision node, we put an
    empty clause).
    */
//    std::cout << "Find antecedent clause for " << l << std::endl;
   auto iter = std::find_if(assignment.begin(), assignment.end(), [l](std::pair<Literal, Disjunction_clause> p) -> bool {return p.first.get_variable() == l.get_variable();});
   if(iter != assignment.end()){
       return iter->second;
   }
   throw std::runtime_error("Literals is not assigned.");
}

void Solver::add_learned_clause(Disjunction_clause dc){
    learned_clauses.push_back(dc);
}

int Solver::get_backtrack_level(Disjunction_clause dc){
    std::vector<int> v;
    auto literals = dc.get_literals();
    for(auto lit : literals){
        int dl = ig.find_decision_level_of_variable(lit.get_variable());
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
    */

    // std::cout << "Analyzing conflict on graph:\n" << ig << std::endl;

    if(decision_level == 0){
        return -1;
    }
    Disjunction_clause current_conflict_clause = ig.get_current_conflict_clause();

    // std::cout << "Current conflict clause is:" << current_conflict_clause << std::endl;
    
    Node uip = get_first_UIP(decision_level);
    // std::cout << "First UIP is:" << uip << std::endl;

    while(!stop_criterion_met(current_conflict_clause, uip)){

        Literal l = get_last_assigned_literal(current_conflict_clause);
        // std::cout << "Last assigned literal is:" << l << std::endl;


        Variable v = l.get_variable();
        Disjunction_clause antecedent = get_antecedent_clause(l);
        // std::cout << "Antecedent clause is: " << antecedent << std::endl; 
        current_conflict_clause = Disjunction_clause::resolve(current_conflict_clause, antecedent, v);
        // std::cout << "New learned clause is: " << current_conflict_clause << std::endl; 

    }

    // std::cout << "stop criterion met"<< std::endl;

    add_learned_clause(current_conflict_clause);
    int dl = get_backtrack_level(current_conflict_clause);
    // std::cout << "return to decision level " << dl << std::endl;

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
    // std::cout << "Decide on literal:" << variable_chosen_by_heuristic << std::endl;

    // decide a variable
    // 1. put the literal into assignment
    // 2. remove the variable from unassiged_variable
    // 3. update implication_graph
    assignment.push_back({variable_chosen_by_heuristic, Disjunction_clause()});
    
    for(auto iter = unassigned_variables.begin(); iter != unassigned_variables.end(); ++iter){
        if(*iter == variable_chosen_by_heuristic.get_variable()){
            unassigned_variables.erase(iter);
            break;
        }
    }

    ig.add_decision_node(variable_chosen_by_heuristic, ++decision_level);
    return true;
}

std::vector<Literal> Solver::get_model(){
    std::vector<Literal> v;
    for(auto p : assignment){
        v.push_back(p.first);
    }

    return v;
}
