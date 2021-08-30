#include "variable.h"
#include <algorithm>
#include <vector>

std::ostream& operator<<(std::ostream &os, const Variable &v){
    if(v.value){
        os << v.name;
    }
    else{
        os << "!" + v.name;
    }
    return os;
}

bool conflict(std::vector<std::pair<Variable, size_t>> &assi){
    std::vector<Variable> assignment;
    for(auto p : assi){
        assignment.push_back(p.first);
    }

    if(assignment.size() < 2){
        return false;
    }

    std::sort(assignment.begin(), assignment.end(), [](Variable &a, Variable &b) -> bool{
        if(a.get_name() != b.get_name()){
            return a.get_name() < b.get_name();
        }
        else{
            return a.get_value() < b.get_value();
        }
        });

    for(size_t sz = 0; sz != assignment.size()-1; ++sz){
        if(assignment[sz].name == assignment[sz+1].name && assignment[sz].value != assignment[sz+1].value){
            return true;
        }
    }
    return false;
}