#include "disjunction_clause.h"
#include <algorithm>

std::ostream& operator<<(std::ostream &os, const Disjunction_clause &dc){
    if(!dc.variables.size()){
        return os;
    }
    for(size_t sz = 0; sz != dc.variables.size()-1; ++sz){
        os << dc.variables[sz] << " \u2228 ";
    }
    os << dc.variables.back();
    return os;
}

Variable propagate_clause(Disjunction_clause &dc, std::vector<std::pair<Variable, size_t>> &assignment){
    auto v = dc.variables;
    for(auto dec : assignment){
        // search for !var
        auto var = dec.first;
        std::vector<Variable>::iterator iter = std::find(v.begin(), v.end(), !var);
        if(iter != v.end()){
            v.erase(iter);
        }

        iter = std::find(v.begin(), v.end(), !var);
        if(iter != v.end()){
            // dc is evaluated to true.
            return Variable();
        }
    }
    if(v.size() == 1){
        return v.back();
    }
    return Variable();
}