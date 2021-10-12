#include "node.h"


void Node::remove_parent(std::shared_ptr<Node> node_ptr){
    auto iter = std::find_if(parents.begin(), parents.end(), [node_ptr](std::shared_ptr<Node> p) -> bool {return *node_ptr == *p;});
    parents.erase(iter);
}

std::ostream& operator<<(std::ostream &os, const Node &n){
    os << n.get_literal() << "@" << n.get_decision_level();
    return os;
}