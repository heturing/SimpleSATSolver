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