#include "literal.h"
#include <sstream>

std::ostream& operator<<(std::ostream &os, const Literal &l){
    if(!l.value){
        os << "!";
    }
    os << l.variable;
    return os;
}

Literal::operator std::string() const{
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}