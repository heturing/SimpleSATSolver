#ifndef NODE_H
#define NODE_H

#include "literal.h"
#include "disjunction_clause.h"

#include <map>
#include <memory>
#include <iostream>
#include <unordered_set>
#include <vector>


class Node{
    // A node in the implication graph.
    // If the underlying var has empty name, then this node represents the conflict node.
friend class Implication_graph;
friend class PtrToNodeHash;
friend class PtrToNodeCompare;
friend std::ostream& operator<<(std::ostream &os, const Node &n);
public:
    Node(Literal v, size_t dl) : lit(v), decision_level(dl), num_of_parents(0) {}

    bool operator==(const Node &n) const {return this->lit == n.lit && this->decision_level == n.decision_level;}
    bool operator!=(const Node &n) const {return !this->operator==(n);}
    bool operator<(const Node &n) const {return this->decision_level < n.decision_level || this->lit.get_variable().get_name() < n.lit.get_variable().get_name();}

    Literal get_literal() const {return lit;}
    size_t get_decision_level() const {return decision_level;}
    size_t get_parent_number() const {return parents.size();}
    void add_parents(std::shared_ptr<Node> node_ptr) {parents.push_back(node_ptr);}
    void remove_parent(std::shared_ptr<Node> node_ptr);
    std::vector<std::shared_ptr<Node>> get_parents() {return parents;}
private:
    Literal lit;
    // num of parents for this node.
    size_t decision_level;
    // edge (h,t) with label c means h.children[t] == c.
    std::vector<std::shared_ptr<Node>> parents;
    size_t num_of_parents;

};

std::ostream& operator<<(std::ostream &os, const Node &n);

class PtrToNodeHash{
    // Compute hash based on underlying Literal and decision_level.
public:
    size_t operator()(const std::shared_ptr<Node> &n) const{
        return LiteralHash()(n->lit) ^ (std::hash<size_t>()(n->decision_level) << 1);
    }
};

class PtrToNodeCompare{
public:
    bool operator()(const std::shared_ptr<Node> &s1, const std::shared_ptr<Node> &s2) const{
        return *s1 == *s2;
    }
};

class NodeCompare{
public:
    bool operator()(const Node &s1, const Node &s2) const{
        return s1 == s2;
    }
};


#endif