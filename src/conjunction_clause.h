#ifndef CONJUNCTION_CLAUSE_H
#define CONJUNCTION_CLAUSE_H

#include <vector>
#include <iostream>
#include "disjunction_clause.h"

class Conjunction_clause{
friend std::ostream& operator<<(std::ostream &os, const Conjunction_clause &cc);
public:
    Conjunction_clause(std::vector<Disjunction_clause> c = std::vector<Disjunction_clause>()) : clauses(c) {}

    std::vector<Disjunction_clause>::iterator begin(){
        return clauses.begin();
    }

    std::vector<Disjunction_clause>::iterator end(){
        return clauses.end();
    }

    std::vector<Disjunction_clause> get_clauses(){
        return this->clauses;
    }

    void add_clause(Disjunction_clause c){
        this->clauses.push_back(c);
    }

private:
    std::vector<Disjunction_clause> clauses;
    

};

std::ostream& operator<<(std::ostream &os, const Conjunction_clause &cc);


#endif