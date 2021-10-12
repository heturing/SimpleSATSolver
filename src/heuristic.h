#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "literal.h"
#include <vector>
#include <algorithm>

class Heuristic{
    // define sort method
public:
    Literal choose_decide_literal(std::vector<Variable> v){
        std::sort(v.begin(), v.end(), [](Literal va, Literal vb) -> bool {return va.get_variable().get_name() < vb.get_variable().get_name();});
        return v.front();
    }
private:

};




#endif