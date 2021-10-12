#include "disjunction_clause.h"
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

std::pair<Literal, Disjunction_clause> Disjunction_clause::propagate_clause(Disjunction_clause dc, std::vector<std::pair<Literal, Disjunction_clause>> assignment){
    auto l = dc.literals;
    for(auto dec : assignment){
        // search for !var
        auto lit = dec.first;
        std::vector<Literal>::iterator iter = std::find(l.begin(), l.end(), !lit);
        if(iter != l.end()){
            l.erase(iter);
        }

        iter = std::find(l.begin(), l.end(), lit);
        if(iter != l.end()){
            // dc is evaluated to true.
            throw std::runtime_error("This clause is evaluated to true.");
        }
    }
    if(l.size() == 1){
        return std::pair<Literal, Disjunction_clause>{l.back(), dc};
    }
    throw std::runtime_error("Cannot propagate anymore.");
}

Disjunction_clause Disjunction_clause::resolve(Disjunction_clause dc1, Disjunction_clause dc2, Variable v){
    std::cout << "Try to resolve " << dc1 << " and " << dc2 << " on variable " << v << std::endl;
    
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
