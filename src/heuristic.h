#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <variable.h>
#include <vector>
#include <algorithm>

class Heuristic{
    // define sort method
public:
    Variable choose_decide_variable(std::vector<Variable> v){
        std::sort(v.begin(), v.end());
        return v.front();
    }
private:

};




#endif