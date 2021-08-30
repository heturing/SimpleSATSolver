#ifndef IMPLICATION_GRAPH_H
#define IMPLICATION_GRAPH_H

#include "node.h"
#include "variable.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <stack>
#include <iostream>

class Implication_graph{
friend std::ostream& operator<<(std::ostream &os, const Implication_graph &ig);
public:
    Implication_graph();

    void add_edge(const Node &head, const Node &tail, const Disjunction_clause &clause);
    void add_edge(std::shared_ptr<Node> head_ptr, std::shared_ptr<Node> tail_ptr, const Disjunction_clause &clause);
    void remove_edge(const Node &head, const Node &tail);
    void add_decision_node(const Variable &v, size_t dl);
    void add_conflict_edge(const Node &n, const Disjunction_clause &dc);
    void backtrack(int dl);


private:
    void deregister_node(std::shared_ptr<Node> n);
    void check_and_remove_node(std::shared_ptr<Node> node_ptr);
    std::shared_ptr<Node> find_node_ptr(const Node &n, bool create_if_absent = true);
    std::shared_ptr<Node> create_new_node(const Node &n);

    // In class instance, we store the list of all nodes for the convenience.
    std::vector<std::shared_ptr<Node>> all_nodes;
    // Each graph has an extra node root which represents the root of the graph. All decision nodes should be child of this node.
    // Conflict node represents the graph reach a conflict.
    std::shared_ptr<Node> root, conflict;
};

std::ostream& operator<<(std::ostream &os, const Implication_graph &ig);


#endif