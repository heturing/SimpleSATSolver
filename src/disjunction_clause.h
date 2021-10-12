#ifndef DISJUNCTION_CLAUSE_H
#define DISJUNCTION_CLAUSE_H

#include <vector>
#include <iostream>
#include <tuple>
#include "literal.h"

class ClauseHash;

class Disjunction_clause{
friend std::ostream& operator<<(std::ostream &os, const Disjunction_clause &dc);
friend class ClauseHash;
public:
    Disjunction_clause(std::vector<Literal> v = std::vector<Literal>()) : literals(v) {}

    std::vector<Literal> get_literals(){
        return this->literals;
    }

    const std::vector<Literal> get_literals() const{
        return this->literals;
    }

    void add_literal(Literal v){
        this->literals.push_back(v);
    }

    bool operator==(const Disjunction_clause &d) const{
        if(literals.size() != d.literals.size()){
            return false;
        }

        for(size_t i = 0; i != literals.size(); ++i){
            if(literals[i] != d.literals[i]){
                return false;
            }
        }
        return true;
    }

    static Disjunction_clause resolve(Disjunction_clause dc1, Disjunction_clause dc2, Variable v);
    static std::pair<Literal, Disjunction_clause> propagate_clause(Disjunction_clause dc, std::vector<std::pair<Literal, Disjunction_clause>> assignment);


private:
    std::vector<Literal> literals;
};

class ClauseHash{
public:
    size_t operator()(const Disjunction_clause &d) const{
        size_t hashValue = 0;
        for(size_t i = 0; i != d.literals.size(); ++i){
            hashValue ^= (LiteralHash()(d.literals[i]) << i);
        }
        return hashValue;
    }
};

std::ostream& operator<<(std::ostream &os, const Disjunction_clause &dc);

#endif