#ifndef LITERAL_H
#define LITERAL_H

#include "variable.h"

class Literal{
friend class LiteralHash;
friend std::ostream& operator<<(std::ostream &os, const Literal &l);
public:
    Literal(Variable var = Variable(), bool val = true) : variable(var), value(val) {}

    bool operator==(const Literal &l) const {return variable == l.variable && value == l.value;}
    bool operator!=(const Literal &l) const {return !(*this == l);}
    Literal operator!() {return Literal(variable, !value);}
    operator std::string() const;

    Variable get_variable() const {return variable;}

private:
    Variable variable;
    bool value;

};

std::ostream& operator<<(std::ostream &os, const Literal &l);
class LiteralHash{
    // Compute hash based on underlying name and value.
public:
    size_t operator()(const Literal &v) const{
        return VariableHash()(v.variable) ^ (std::hash<bool>()(v.value) << 1);
    }
};



#endif