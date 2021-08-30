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
public:
    Variable(std::string n="", bool v=false) : name(n), value(v) {}

    std::string get_name(){
        return this->name;
    }

    bool get_value(){
        return this->value;
    }

    Variable operator!(){
        return Variable(name, !value);
    }

    explicit operator bool() const{
        return name.empty() ? false : true;
    }

    bool operator==(const Variable &v) const {return this->name == v.name;}

private:
    // if name is "", then it's an invalid variable.
    std::string name;
    bool value;
};

std::ostream& operator<<(std::ostream &os, const Variable &v);
bool conflict(std::vector<std::pair<Variable, size_t>> &v);

#endif