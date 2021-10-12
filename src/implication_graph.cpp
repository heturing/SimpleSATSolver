#include "implication_graph.h"
#include <string>
#include <queue>
#include <set>
#include <algorithm>
#include <memory>
#include <vector>

Implication_graph::Implication_graph() : all_nodes(std::unordered_set<std::shared_ptr<Node>, PtrToNodeHash, PtrToNodeCompare>()), 
                                         all_edges(std::unordered_multimap<std::shared_ptr<Node>, std::pair<std::shared_ptr<Node>, Disjunction_clause>, PtrToNodeHash, PtrToNodeCompare>()),
                                         root(create_new_node(Node(Literal(Variable("root"), true), 0))), 
                                         conflict((create_new_node(Node(Literal(Variable("conflict"), true), 0)))) {}


std::shared_ptr<Node> Implication_graph::create_new_node(const Node &n){
    /*
        Assumption: There is NOT a shared_ptr in this graph that points to the same content with n.
        Create a Node object that has the same content with n. Return the shared_ptr to the created Node object

        Even we have a Node object that we want to use, it might not belongs to this graph. To manipulate this graph, we have to
        operate on the Node object that belongs to the graph.
    */

    std::shared_ptr<Node> node_ptr(new Node(n));
    all_nodes.insert(node_ptr);

    return node_ptr;
}

std::shared_ptr<Node> Implication_graph::find_node_ptr(const Node &n){
    /*
        Try to find the shared_ptr in this graph that points a node with the same content as n. If no such shared_ptr,
        return null.
    */
   auto node_ptr = all_nodes.find(std::shared_ptr<Node>(new Node(n)));
   if(node_ptr == all_nodes.end()){
       return nullptr;
   }
   else{
       return *node_ptr;
   }
}

void Implication_graph::add_edge(const Node &head, const Node &tail, const Disjunction_clause &clause){
    /*  
        Add edge (head, tail) with annotation clause.
        An edge (head, tail) with annotation clause in this graph is represented by storing a pair {tail, clause} in head.children
        For record, also add the shared_ptr to head into tail.parents.
    */
    
    std::shared_ptr<Node> head_ptr = find_node_ptr(head);
    if(head_ptr == nullptr){
        head_ptr = create_new_node(head);
    }
    std::shared_ptr<Node> tail_ptr = find_node_ptr(tail);
    if(tail_ptr == nullptr){
        tail_ptr = create_new_node(tail);
    }

    // if edge is already exist, jusr return.
    auto range = all_edges.equal_range(head_ptr);
    auto iter = std::find_if(range.first, range.second, [tail_ptr](const std::pair<std::shared_ptr<Node>, std::pair<std::shared_ptr<Node>, Disjunction_clause>> &p) -> bool {return *p.second.first == *tail_ptr;});
    if(iter != range.second){
        return;
    }

    all_edges.insert({head_ptr, {tail_ptr, clause}});
    tail_ptr->num_of_parents += 1;
    tail_ptr->add_parents(head_ptr);

    last_append_node = tail_ptr;

    // tail_ptr->num_parent += 1;
    // tail_ptr->parents.insert(head_ptr);

    // head_ptr->children.insert({tail_ptr, clause});
}

void Implication_graph::add_decision_node(const Literal &v, size_t dl){
    /*
        Create a Node object that represents a decision. Then add an edge from the root node to the created object.

        For making decision, user cannot call add_edge bc they do not know the address of root node.
    */
    Node n(v,dl);
    auto node_ptr = create_new_node(n);
    add_edge(*root, *node_ptr, Disjunction_clause());
}

void Implication_graph::add_conflict_edge(const Node &n, const Disjunction_clause &dc){
    /*
        Add an edge from n to conflict node with annotation dc.
    */
    auto node_ptr = find_node_ptr(n);
    if(node_ptr == nullptr){
        node_ptr = create_new_node(n);
    }
    add_edge(*node_ptr, *conflict, dc);
}

void Implication_graph::remove_unreachable_node(const std::shared_ptr<Node> &sn){
    /*
    Assumption: The node that sn points to is unreachable from the root node.
    */
    // std::cout << "Removing node " << *sn << std::endl;
    // auto range = all_edges.equal_range(sn);

    if(*sn == *conflict){
        return;
    }
    
    auto iter = all_edges.find(sn);
    while(iter != all_edges.end()){

        remove_edge(*sn, *(iter->second.first));
        iter = all_edges.find(sn);
    }
    all_nodes.erase(sn);
}

void Implication_graph::remove_edge(const Node &head, const Node &tail){
    /*
        Remove an edge in this graph. If the edge is not in current graph, throw an exception.

        Removing an edge (h,t) means remove the entry {t,c} in h.children.
        Also, insert the shared_ptr to the head node to tail_ptr->parents.
    */

    std::shared_ptr<Node> head_ptr = find_node_ptr(head);
    std::shared_ptr<Node> tail_ptr = find_node_ptr(tail);
    if(!head_ptr || !tail_ptr){
        std::cerr << "Trying to remove an edge (" << head << " --> " << tail << ") that is not in the graph.";
        return;
        // throw std::runtime_error("Trying to remove an edge not in the graph.");
    }
    auto range = all_edges.equal_range(head_ptr);

    for(auto beg = range.first; beg != range.second; ++beg){
        if(*(beg->second.first) == tail){
            all_edges.erase(beg);
            tail_ptr->num_of_parents -= 1;
            tail_ptr->remove_parent(beg->first);

            if(tail_ptr->num_of_parents == 0){
                remove_unreachable_node(tail_ptr);
            }

            return;
        }
    }
    std::cerr << "Trying to remove an edge (" << head << " --> " << tail << ") that is not in the graph.";
    return;
}

std::vector<std::shared_ptr<Node>> Implication_graph::backtrack(int dl){
    /*
        Backtrack the graph to decision level dl, which means to invalidate all decisions and implications made by decision
        level larger than dl.
    */

    std::vector<std::shared_ptr<Node>> erased_node;
    auto range = all_edges.equal_range(root);

    for(auto iter = all_edges.begin(); iter != all_edges.end();){
        if(iter->second.first->decision_level > dl || iter->first->decision_level > dl || iter->second.first == conflict){
            // std::cout << "remove edge " << *(iter->first) << " " << *(iter->second.first) << std::endl;
            iter->second.first->num_of_parents -= 1;
            iter->second.first->remove_parent(iter->first);
            iter = all_edges.erase(iter);
            continue;
        }
        ++iter;
    }

    for(auto iter = all_nodes.begin(); iter != all_nodes.end();){
        if((*iter)->num_of_parents == 0 && *iter != root && *iter != conflict){
            erased_node.push_back(*iter);
            iter = all_nodes.erase(iter);
            continue;
        }
        ++iter;
    }
    return erased_node;
}

// New functions

Disjunction_clause Implication_graph::get_clause_of_edge(std::shared_ptr<Node> head_ptr, std::shared_ptr<Node> tail_ptr){
    auto range = all_edges.equal_range(head_ptr);
    for(auto iter = range.first; iter != range.second; ++iter){
        if(*(iter->second.first) == *(tail_ptr)){
            return iter->second.second;
        }
    }
    throw std::runtime_error("Cannot find edge:" + std::string(head_ptr->lit) + " " + std::string(tail_ptr->lit));
}

Disjunction_clause Implication_graph::get_current_conflict_clause(){
    /*
    Return the clause that leads to conflict under current assignment.
    */
    auto parent_nodes = conflict->get_parents();
    if(parent_nodes.empty()){
        throw std::runtime_error("No conflict clause:");
    }

    auto cl = get_clause_of_edge(parent_nodes[0], conflict);
    return cl;
}

size_t Implication_graph::find_decision_level_of_variable(const Variable &v){
    for(auto node_ptr : all_nodes){
        if(node_ptr->get_literal().get_variable() == v){
            // std::cout << "decision level for " << v << " is " << node_ptr->decision_level << std::endl;
            return node_ptr->decision_level;
        }
    }
    throw std::runtime_error("No node corresponds to variable " + v.get_name());
}

std::vector<std::shared_ptr<Node>> Implication_graph::get_children_nodes(std::shared_ptr<Node> node_ptr){
    std::vector<std::shared_ptr<Node>> v;
    auto range = all_edges.equal_range(node_ptr);
    for(auto iter = range.first; iter != range.second; ++iter){
        v.push_back(iter->second.first);
    }
    return v;
}

std::shared_ptr<Node> Implication_graph::get_decision_node(size_t decision_level){
    auto children_of_root = get_children_nodes(root);
    auto iter = std::find_if(children_of_root.begin(), children_of_root.end(), [decision_level](std::shared_ptr<Node> p) -> bool {return p->get_decision_level() == decision_level;});
    if(iter != children_of_root.end()){
        return *iter;
    }
    throw std::runtime_error("No decision node at decision level " + std::to_string(decision_level));
}

Node Implication_graph::copy_node(const Node &n){
    /*
    Copy a content of node, but remove all its parents.
    */

   Node new_node(n.get_literal(), n.get_decision_level());
   return new_node;
}

Implication_graph Implication_graph::get_partial_implication_graph(size_t decision_level){
    /*
    Return the implication graph starts from the decision node for decision_level.
    */
    Implication_graph ig;
    auto node_ptr = get_decision_node(decision_level);
    ig.add_decision_node(node_ptr->lit, node_ptr->decision_level);
    std::queue<std::shared_ptr<Node>> q;
    q.push(node_ptr);

    while(!q.empty()){
        auto p = q.front();
        q.pop();
        auto children_nodes = get_children_nodes(p);

        // std::cout << "Children of " << *p << ":" << std::endl;
        // for(auto child : children_nodes){
        //     std::cout << *child << std::endl;
        // }

        for(auto cn = children_nodes.begin(); cn != children_nodes.end(); ++cn){
            q.push(*cn);
            auto cl = get_clause_of_edge(p, *cn);
            auto new_children_node = copy_node(**cn);
            std::cout << "Add edge from " << *p << " to " << new_children_node << std::endl;
            ig.add_edge(*p, new_children_node, cl);
        }   
   }
    return ig;
}

Node Implication_graph::get_first_UIP(size_t decision_level){
    // get partial implication graph

    Implication_graph pig = get_partial_implication_graph(decision_level);

    std::map<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> dominator_map;
    dominator_map.insert({pig.root, std::vector<std::shared_ptr<Node>>{pig.root}});

    std::cout << "partial implication graph get:\n" << pig << std::endl;
    // std::cout << pig.conflict->get_parent_number() << " parents node for conflict in pig" << std::endl;


    std::queue<std::shared_ptr<Node>> temp;
    temp.push(pig.root);
    while(!temp.empty()){
        auto node_ptr = temp.front();

        // calculate dominator
        std::vector<std::shared_ptr<Node>> dom(pig.all_nodes.begin(), pig.all_nodes.end());

        // std::cout << "Calculating dominator for node:" << *node_ptr << std::endl;
        for(auto par : node_ptr->parents){
            // std::cout << "Consdering parent node:" << *par << std::endl;

            std::vector<std::shared_ptr<Node>> intersection_res;
            // std::cout << "Before intersection dom:" << std::endl;
            // for(auto d: dom){
            //     std::cout << *d << std::endl;
            // }
            // std::cout << "Before intersection par:" << std::endl;
            // for(auto d: dominator_map[par]){
            //     std::cout << *d << std::endl;
            // }
            std::sort(dom.begin(), dom.end(), [](const std::shared_ptr<Node> &sp1, const std::shared_ptr<Node> &sp2) -> bool {return *sp1 < *sp2;});

            std::set_intersection(dom.begin(), dom.end(), dominator_map[par].begin(), dominator_map[par].end(), std::back_inserter(intersection_res), [](const std::shared_ptr<Node> &n1, const std::shared_ptr<Node> &n2) -> bool {return *n1 != *n2;});
            std::sort(intersection_res.begin(), intersection_res.end(), [](const std::shared_ptr<Node> &sp1, const std::shared_ptr<Node> &sp2) -> bool {return *sp1 < *sp2;});
            dom = intersection_res;
        }
        // std::cout << "After intersection:" << std::endl;
        // for(auto d: dom){
        //     std::cout << *d << std::endl;
        // }

        if(node_ptr->parents.empty()){
            dominator_map[node_ptr] = std::vector<std::shared_ptr<Node>>{node_ptr};
        }
        else{
            dom.push_back(node_ptr);
            dominator_map[node_ptr] = dom;
        }
        // std::cout << "Dominators set of " << *node_ptr << std::endl;
        // for(auto d: dominator_map[node_ptr]){
        //     std::cout << *d << std::endl;
        // }


        auto children = pig.get_children_nodes(node_ptr);
        for(auto child : children){
            temp.push(child);
        }
        temp.pop();
    }
    return *(dominator_map[pig.conflict][dominator_map[pig.conflict].size()-2]);


    throw std::runtime_error("Cannot find first UIP.");
}

bool Implication_graph::is_conflict(){
    return conflict->num_of_parents > 0;
}

Node Implication_graph::get_node_from_literal(Literal l){
    for(auto node_ptr : all_nodes){
        if(node_ptr->lit == l){
            return *node_ptr;
        }
    }
    throw std::runtime_error("No such node:" + std::string(l));
}



std::ostream& operator<<(std::ostream &os, const Implication_graph &ig){
    os << std::string(20, '=') << " Implication Graph " << std::string(20, '=') << "\n";
    os << ig.all_nodes.size() << " nodes stored:\n";
    for(auto n : ig.all_nodes){
        os << "  - " << *n << " with " << n->get_parent_number() << " parents.\n";
    }
    os << ig.all_edges.size() << " edges stored.\n";
    for(auto e : ig.all_edges){
        os << "  - " << *(e.first) << " --> " << *(e.second.first) << " with label (" << e.second.second << ")\n";
    }
    os << std::string(60,'=');

    return os;
}


