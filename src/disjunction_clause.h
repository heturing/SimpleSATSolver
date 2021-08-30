#ifndef DISJUNCTION_CLAUSE_H
#define DISJUNCTION_CLAUSE_H

#include <vector>
#include <iostream>
#include "variable.h"

class Disjunction_clause{
friend std::ostream& operator<<(std::ostream &os, const Disjunction_clause &dc);
friend Variable propagate_clause(Disjunction_clause &dc, std::vector<std::pair<Variable, size_t>> &assignment);
public:
    Disjunction_clause(std::vector<Variable> v = std::vector<Variable>()) : variables(v) {}

    std::vector<Variable> get_variables(){
        return this->variables;
    }

    void add_variable(Variable v){
        this->variables.push_back(v);
    }

private:
    std::vector<Variable> variables;
};

std::ostream& operator<<(std::ostream &os, const Disjunction_clause &dc);
Variable propagate_clause(Disjunction_clause &dc, std::vector<std::pair<Variable, size_t>> &assignment);

#endif