#include "conjunction_clause.h"

std::ostream& operator<<(std::ostream &os, const Conjunction_clause &cc){
    if(!cc.clauses.size()){
        return os;
    }
    for(size_t sz = 0; sz != cc.clauses.size()-1; ++sz){
        os << "(" << cc.clauses[sz] << ")" << " \u2227 ";
    }
    os << "(" << cc.clauses.back() << ")";
    return os;
}

std::vector<Variable> Conjunction_clause::get_variables_in_clause(){
    std::vector<Variable> v;
    for(auto cl : clauses){
        for(auto l : cl.get_literals()){
            // if l.variable not in v
            auto iter = std::find_if(v.begin(), v.end(), [l](Variable v) -> bool {return l.get_variable() == v;});
            if(iter == v.end()){
                v.push_back(l.get_variable());
            }
        }
    }
    return v;
}
