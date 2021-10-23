#include "disjunction_clause.h"
#include "solver_exception.h"
#include <algorithm>

std::ostream& operator<<(std::ostream &os, const Disjunction_clause &dc){
    if(!dc.literals.size()){
        return os;
    }
    for(size_t sz = 0; sz != dc.literals.size()-1; ++sz){
        os << dc.literals[sz] << " \u2228 ";
    }
    os << dc.literals.back();
    return os;
}

Literal Disjunction_clause::propagate_clause(Disjunction_clause dc, std::vector<std::pair<Literal, Disjunction_clause>> assignment){
    /*
    Propagate literals for a disjunction clause based on the current assignment.
    Return the propagated literal.
    */
    
    std::vector<Literal> literals_in_clause = dc.literals;
    for(auto dec : assignment){
        Literal lit = dec.first;
        std::vector<Literal>::iterator iter = std::find(literals_in_clause.begin(), literals_in_clause.end(), !lit);
        if(iter != literals_in_clause.end()){
            // if !lit is in clause, the rest lits must have at least one true to make the problem satisfiable.
            literals_in_clause.erase(iter);
        }

        iter = std::find(literals_in_clause.begin(), literals_in_clause.end(), lit);
        if(iter != literals_in_clause.end()){
            //if lit is in clause, we cannot propagate from this clause.
            throw TrueClauseException("This clause is evaluated to true.");
        }
    }

    if(literals_in_clause.size() == 0){
        throw ConflictClauseException("Propagation leads to Conflict.");
    }
    else if(literals_in_clause.size() == 1){
        return literals_in_clause.back();
    }
    else{
        // more than 1 unassigned lit in this clause, cannot propagate.
        throw NoLiteralToPropagateException("Cannot propagate anymore.");
    }

}

Disjunction_clause Disjunction_clause::resolve(Disjunction_clause dc1, Disjunction_clause dc2, Variable v){
    // std::cout << "Try to resolve " << dc1 << " and " << dc2 << " on variable " << v << std::endl;
    
    Disjunction_clause dc;
    for(auto lit : dc1.get_literals()){
        if(lit.get_variable() != v){
            dc.add_literal(lit);
        }
    }

    for(auto lit : dc2.get_literals()){
        if(lit.get_variable() != v){
            auto existing_literals = dc.get_literals();
            auto iter = std::find(existing_literals.begin(), existing_literals.end(), lit);
            if(iter != existing_literals.end()){
                // std::cout << *iter << " = " << lit << " is " << (*iter == lit) << " in " << dc << std::endl;
                continue;
            }
            // std::cout << "Add " << *iter << " to " << dc << std::endl;

            dc.add_literal(lit);
        }
    }

    return dc;
}
