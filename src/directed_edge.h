#ifndef DIRECTED_EDGE_H
#define DIRECTED_EDGE_H

#include "node.h"
#include "disjunction_clause.h"
#include <memory>

class Directed_edge{
    // edge in the graph.
public:
    Directed_edge(std::shared_ptr<Node> h, std::shared_ptr<Node> t, Disjunction_clause dc) : head(h), tail(t), clause(dc) {}
    std::shared_ptr<Node> get_head() const {return head;}
    std::shared_ptr<Node> get_tail() const {return tail;}
private:
    std::shared_ptr<Node> head, tail;
    Disjunction_clause clause;
};



#endif