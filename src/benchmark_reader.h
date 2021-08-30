#ifndef BENCHMARK_READER_H
#define BENCHMARK_READER_H

#include "conjunction_clause.h"
#include <fstream>
#include <string>

class Benchmark_reader{
public:
    // Read a benchmark file, put the read CNF into cc.
    Benchmark_reader(std::string file_name);

    Conjunction_clause get_formula(){
        return this->cc;
    }

private:
    Conjunction_clause cc;
};



#endif