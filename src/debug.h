#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include "solver.h"

template<typename T> std::ostream& operator<<(std::ostream &os, const std::vector<T> &v){  
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

void dump_debug_info(const Solver &s){
    std::cout << "Current assignment:";
    for(auto p : s.assignment){
        std::cout << p.first << " " << p.second << std::endl;
    }
}


#endif