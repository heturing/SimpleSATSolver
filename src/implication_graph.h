#ifndef IMPLICATION_GRAPH_H
#define IMPLICATION_GRAPH_H

#include "node.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

class Implication_graph{
friend std::ostream& operator<<(std::ostream &os, const Implication_graph &ig);
friend class Solver;
public:
    // implemented
    Implication_graph();
    void add_edge(const Node &head, const Node &tail, const Disjunction_clause &clause);
    void add_decision_node(const Literal &v, size_t dl);
    void add_conflict_edge(const Node &n, const Disjunction_clause &dc);
    void remove_edge(const Node &head, const Node &tail);
    std::vector<std::shared_ptr<Node>> backtrack(int dl);
    bool is_conflict();
    // todo: considering remove.
    Disjunction_clause get_current_conflict_clause();
    size_t find_decision_level_of_variable(const Variable &l);
    Node get_first_UIP(size_t decision_level);
    Implication_graph get_partial_implication_graph(size_t decision_level);
    Node get_node_from_literal(Literal l);

    
private:
    size_t get_decision_level_of_literal(const Literal &l);

    std::unordered_set<std::shared_ptr<Node>, PtrToNodeHash, PtrToNodeCompare> all_nodes;
    std::unordered_multimap<std::shared_ptr<Node>, std::pair<std::shared_ptr<Node>, Disjunction_clause>, PtrToNodeHash, PtrToNodeCompare> all_edges;
    std::shared_ptr<Node> root, conflict;
    std::shared_ptr<Node> last_append_node;

    // Need to store predecessors. Maybe keep that infor in Node
    // Need postdecessors

    
    
    // Helper functions.
    std::shared_ptr<Node> create_new_node(const Node &n);
    std::shared_ptr<Node> find_node_ptr(const Node &n);
    void remove_unreachable_node(const std::shared_ptr<Node> &sn);
    Disjunction_clause get_clause_of_edge(std::shared_ptr<Node> head_ptr, std::shared_ptr<Node> tail_ptr);
    std::shared_ptr<Node> get_decision_node(size_t decision_level);
    std::vector<std::shared_ptr<Node>> get_children_nodes(std::shared_ptr<Node> node_ptr);
    Node copy_node(const Node &n);

};

std::ostream& operator<<(std::ostream &os, const Implication_graph &ig);


#endif