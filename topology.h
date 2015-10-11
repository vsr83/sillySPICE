#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <list>
#include <vector>
#include <set>
#include <queue>

#include "nodeList.h"

// Orientation of the circuit branches should not be confused with oriented
// edges in application of Breadth-first search. Thus, the data structures
// used here are similar to those often suggested for non-oriented graphs.
// Otherwise, searching for adjacent nodes with BFS would be difficult.

class Link {
public:
    Link(unsigned int _node1, unsigned int _node2, unsigned int _dir,
         unsigned int _globInd) {
        node1 = _node1;
        node2 = _node2;
        dir = _dir;
        globInd = _globInd;
    }

    unsigned int node1, node2;
    int dir;
    unsigned int globInd;
};

class
Topology : public NodeList {
public:
    Topology(NodeList &_nodeList);
    ~Topology();
    void addEdge(unsigned int node1, unsigned int node2, unsigned int globInd = 0);
    void addEdge(std::string node1, std::string node2, unsigned int globInd = 0);

    void dispEdges();
    void BFS(std::vector<int> &dist,
             std::vector<int> &parent,
             std::list<Link> &spanningTree,
             unsigned int &numTraversed,
             unsigned int startNode = 0);

    std::vector<int> dist, parent;
private:
    std::vector<std::list<Link> > edges;
};

#endif // TOPOLOGY_H
