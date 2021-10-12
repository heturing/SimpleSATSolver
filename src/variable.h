/*
    class for variable.
*/

#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <iostream>

class Variable{
friend std::ostream& operator<<(std::ostream &os, const Variable &v);
friend bool conflict(std::vector<std::pair<Variable, size_t>> &v);
friend class VariableHash;
public:
    Variable(std::string n="v"+std::to_string(index)) : name(n){}

    std::string get_name() const{
        return this->name;
    }

    explicit operator bool() const{
        return name.empty() ? false : true;
    }

    bool operator==(const Variable &v) const {return this->name == v.name;}
    bool operator!=(const Variable &v) const {return !(*this == v);}

private:
    // if name is "", then it's an invalid variable.
    std::string name;
    static size_t index;
};

std::ostream& operator<<(std::ostream &os, const Variable &v);
bool conflict(std::vector<std::pair<Variable, size_t>> &v);

class VariableHash{
    // Compute hash based on underlying name and value.
public:
    size_t operator()(const Variable &v) const{
        return std::hash<std::string>()(v.name);
    }
};

#endif