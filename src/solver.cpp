#include "solver.h"
#include "disjunction_clause.h"
#include "heuristic.h"

bool Solver::solve(){
    // Check the satisfiability of given CNF by CDCL.
    // If satisfiable, return true and fill the answer into model. Else, return false

    while(true){
        while(BCP()){
            int backtrack_level = analyze_conflict();
            if(backtrack_level < 0){
                return false;
            }
            backtrack(backtrack_level);
        }
        Variable decide_var = decide();
        if(!decide_var){
            // set the answer

            return true;
        }
        assignment.push_back({decide_var, ++decision_level});
    }
}

bool Solver::BCP(){
    // return true if there is no conflict, else return false.

    for(auto c : clause){
        Variable var = propagate_clause(c, assignment);
        if(var){
            assignment.push_back({var, decision_level});
            if(conflict(assignment)){
                return false;
            }
            BCP();
        }
    }
    return true;
}

int Solver::analyze_conflict(){
    if(this->decision_level == 0){
        return -1;
    }
    Disjunction_clause current_conflict;
}


Variable Solver::decide(){
    /*
        Choose an unassigned variable and assign a value to it.
        Various heuristic may apply when choosing the variable to assign.
    */

    if(unassigned_variables.empty()){
        return Variable();
    }

    Heuristic h;
    Variable variable_chosen_by_heuristic = h.choose_decide_variable(unassigned_variables);
    return variable_chosen_by_heuristic;
}