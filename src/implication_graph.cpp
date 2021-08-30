#include "implication_graph.h"

Implication_graph::Implication_graph() : all_nodes(std::vector<std::shared_ptr<Node>>()), 
                                        root(create_new_node(Node(Variable("root", true), 0))), 
                                        conflict((create_new_node(Node(Variable("conflict", true), 0)))) {}

void Implication_graph::add_edge(const Node &head, const Node &tail, const Disjunction_clause &clause){
    /*  
        Add edge (head, tail) with annotation clause.
        An edge (head, tail) with annotation clause in this graph is represented by storing a pair {tail, clause} in head.children
        For record, also add the shared_ptr to head into tail.parents.
    */
    
    std::shared_ptr<Node> head_ptr = find_node_ptr(head);
    std::shared_ptr<Node> tail_ptr = find_node_ptr(tail);

    // tail_ptr->num_parent += 1;
    tail_ptr->parents.insert(head_ptr);

    head_ptr->children.insert({tail_ptr, clause});
}

void Implication_graph::add_edge(std::shared_ptr<Node> head_ptr, std::shared_ptr<Node> tail_ptr, const Disjunction_clause &clause){
    // See add_edge(Node head, Node tail, Disjunction_clause clause).

    // tail_ptr->num_parent += 1;
    tail_ptr->parents.insert(head_ptr);
    head_ptr->children.insert({tail_ptr, clause});
}

std::shared_ptr<Node> Implication_graph::create_new_node(const Node &n){
    /*
        Create a Node object that has the same content with n. Return the shared_ptr to the created Node object

        Even we have a Node object that we want to use, it might not belongs to this graph. To manipulate this graph, we have to
        operate on the Node object that belongs to the graph.
    */

    std::shared_ptr<Node> node_ptr(new Node(n));
    // Store the shared_ptr to new Node object into the node list.
    all_nodes.push_back(node_ptr);

    return node_ptr;
}

std::shared_ptr<Node> Implication_graph::find_node_ptr(const Node &n, bool create_if_absent){
    /*
        Return the shared_ptr that points to the node which contains the same content with n. If there is no such node and 
        create_if_absent is set to true, create a node that has the same content with n in this graph. If there is no such
        node and create_if_absent is set to false, return nullptr which means false logically.

        Each graph has a list that stores the shared_ptr to nodes in this graph. To perform any operation (add edges, remove edges,
        remove node, etc.), one have to fetch the address of the node.

        To admit more efficient implementation, we can use an unordered_set to store the shared_ptr as this function involves
        finding an element in a container (may of a large size).

        TODO: nothing
    */
    
    auto iter = std::find_if(all_nodes.begin(), all_nodes.end(), 
                [&, n](std::shared_ptr<Node> node_ptr) -> bool {return *node_ptr == n;});
    if(iter != all_nodes.end()){
        return *iter;
    }

    if(create_if_absent){
        return create_new_node(n);
    }
    else{
        return nullptr;
    }
    
}

void Implication_graph::remove_edge(const Node &head, const Node &tail){
    /*
        Remove an edge in this graph. If the edge is not in current graph, throw an exception.

        Removing an edge (h,t) means remove the entry {t,c} in h.children.
        Also, insert the shared_ptr to the head node to tail_ptr->parents.
    */
    std::shared_ptr<Node> head_ptr = find_node_ptr(head, false);
    std::shared_ptr<Node> tail_ptr = find_node_ptr(tail, false);
    if(!head_ptr || !tail_ptr){
        throw std::runtime_error("Trying to remove an edge not in the graph.");
    }

    // Remove edge (head, tail)
    head_ptr->children.erase(tail_ptr);
    // tail_ptr->num_parent -= 1;
    tail_ptr->parents.erase(head_ptr);

    // remove edge (head, tail) may isolate tail. Thus, check if it still reachable from root. If not, remove node tail.
    check_and_remove_node(tail_ptr);
}

void Implication_graph::check_and_remove_node(std::shared_ptr<Node> node_ptr){
    /*
        Check if the node has parents (reachable from root, exclude root and conflict). If node has no parents, remove it from
        the node list. Recursively call this function on all children node of node_ptr.
    */

    if(node_ptr->parents.size() == 0 and *node_ptr != *root and *node_ptr != *conflict){
        deregister_node(node_ptr);

    }
    for(auto value : node_ptr->children){
        check_and_remove_node(value.first);
    }
}

void Implication_graph::deregister_node(std::shared_ptr<Node> node_ptr){
    /*
        Remove the shared_ptr of node from the node list.

        Assumption: node_ptr points to a node with no parents
    */

    // remove node_ptr from its children
    for(auto value : node_ptr->children){
        // value.first->num_parent -= 1;
        value.first->parents.erase(node_ptr);
    }

    auto iter = std::find_if(all_nodes.begin(), all_nodes.end(), 
                [&, node_ptr](std::shared_ptr<Node> ptr) -> bool {return *ptr == *node_ptr;});
    all_nodes.erase(iter);
}

void Implication_graph::add_decision_node(const Variable &v, size_t dl){
    /*
        Create a Node object that represents a decision. Then add an edge from the root node to the created object.

        For making decision, user cannot call add_edge bc they do not know the address of root node.
    */
    Node n(v,dl);
    auto node_ptr = create_new_node(n);
    add_edge(root, node_ptr, Disjunction_clause());
}

void Implication_graph::add_conflict_edge(const Node &n, const Disjunction_clause &dc){
    /*
        Add an edge from n to conflict node with annotation dc.
    */
    auto node_ptr = find_node_ptr(n, false);
    add_edge(node_ptr, conflict, dc);
}

void Implication_graph::backtrack(int dl){
    /*
        Backtrack the graph to decision level dl, which means to invalidate all decisions and implications made by decision
        level larger than dl.
    */

    std::stack<std::shared_ptr<Node>> s;
    s.push(root);

    while(!s.empty()){
        std::shared_ptr<Node> node_ptr = s.top();
        s.pop();
        
        if(!find_node_ptr(*node_ptr)){
            continue;
        }

        for(auto nptr : node_ptr->children){
            s.push(nptr.first);
        }

        if(node_ptr->decision_level > dl){
            // remove all entries in node_ptr->children as the tail node cannot have less dl than head node.
            for(auto parent : node_ptr->parents){
                parent->children.erase(node_ptr);
            }
            deregister_node(node_ptr);
        }
    }
}

std::ostream& operator<<(std::ostream &os, const Implication_graph &ig){
    for(auto n : ig.all_nodes){
        os << *n << " with " << n->get_children_number() << " children and " << n->get_parent_number() << " parents.\n";
    }
    os << ig.all_nodes.size() << " nodes stored.";
    return os;
}


