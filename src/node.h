#ifndef NODE_H
#define NODE_H

#include "variable.h"
#include "disjunction_clause.h"

#include <map>
#include <memory>
#include <iostream>
#include <unordered_set>

class Implication_graph;

class Node{
    // A node in the implication graph.
    // If the underlying var has empty name, then this node represents the conflict node.
friend class Implication_graph;
friend std::ostream& operator<<(std::ostream &os, const Node &n);
public:
    Node(Variable v, size_t dl) : var(v), decision_level(dl), children(std::map<std::shared_ptr<Node>, Disjunction_clause>()) {}

    bool operator==(const Node &n) const {return this->var == n.var && this->decision_level == n.decision_level;}
    bool operator!=(const Node &n) const {return !this->operator==(n);}

    Variable get_variable() const {return var;}
    size_t get_decision_level() const {return decision_level;}
    size_t get_children_number() const {return children.size();}
    size_t get_parent_number() const {return parents.size();}
private:
    Variable var;
    // num of parents for this node.
    size_t decision_level;
    // edge (h,t) with label c means h.children[t] == c.
    std::map<std::shared_ptr<Node>, Disjunction_clause> children;
    std::unordered_set<std::shared_ptr<Node>> parents;
};

std::ostream& operator<<(std::ostream &os, const Node &n);



#endif