#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include "node.h"
#include "disjunction_clause.h"

class Edge{
friend class PtrToEdgeHash;
public:
    bool operator==(const Edge &e) const{
        return (*from == *(e.from)) && (*to == *(e.to)) && (*label == *(e.label));
    }

private:
    std::shared_ptr<Node> from;
    std::shared_ptr<Node> to;
    std::shared_ptr<Disjunction_clause> label;
};

class PtrToEdgeHash{
public:
    size_t operator()(const std::shared_ptr<Edge> &e) const{
        return PtrToNodeHash()(e->from) ^ (PtrToNodeHash()(e->to) << 1) ^ (ClauseHash()(*(e->label)) << 2);
    }
};

class PtrToEdgeCompare{
public:
    bool operator()(const std::shared_ptr<Edge> &e1, const std::shared_ptr<Edge> &e2) const{
        return *e1 == *e2;
    }
};





#endif