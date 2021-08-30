#include "node.h"

std::ostream& operator<<(std::ostream &os, const Node &n){
    os << n.get_variable() << "@" << n.get_decision_level();
    return os;
}