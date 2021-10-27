#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include "solver.h"



void dump_debug_info(const Solver &s){
    std::cout << "Current assignment:";
    for(auto p : s.assignment){
        std::cout << p.first << " " << p.second << std::endl;
    }
}


#endif