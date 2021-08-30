#include "implication_graph.h"
#include "node.h"
#include "variable.h"
#include "disjunction_clause.h"
#include <vector>

int main(){
    Variable x1("x1", true), x2("x2", true), x3("x3", true), x4("x4", true);

    Disjunction_clause c1(std::vector<Variable>{!x1, x2});
    Disjunction_clause c2(std::vector<Variable>{!x2, !x3, x4});
    Disjunction_clause c3(std::vector<Variable>{!x2, !x4});
    
    // x1 with value 1 at dl 1.
    Node n1(x1, 1);
    Node n2(x2, 1);
    Node n3(x3, 2);
    Node n4(x4, 2);

    Implication_graph graph;

    // Add edge test.
    graph.add_decision_node(x1,1);
    graph.add_edge(n1, n2, c1);
    graph.add_decision_node(x3,2);
    graph.add_edge(n3, n4, c2);
    graph.add_edge(n2, n4, c2);
    graph.add_conflict_edge(n2,c3);
    graph.add_conflict_edge(n4,c3);

    std::cout << graph << std::endl;

    // remove edge test.
    graph.remove_edge(n1,n2);
    // n2 should be removed.
    std::cout << graph << std::endl;

    // graph.remove_edge(n2,n4);
    // // nothing
    // std::cout << graph << std::endl;

    // n4 should be removed.
    graph.remove_edge(n3,n4);
    std::cout << graph << std::endl;


    // backtrack test
    graph.add_edge(n1, n2, c1);
    graph.add_edge(n3, n4, c2);
    graph.add_edge(n2, n4, c2);
    graph.add_conflict_edge(n2,c3);
    graph.add_conflict_edge(n4,c3);
    std::cout << graph << std::endl;

    graph.backtrack(1);
    std::cout << graph << std::endl;


    return 0;
}