/* sillySPICE - A SPICE-like Circuit Solver
   Copyright (C) 2015 Ville Räisänen <vsr at vsr.name>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <list>
#include <vector>
#include <set>
#include <queue>

#include "nodeList.h"

// This file contains code for constructing spanning trees for circuits by
// Breadth-First Search algorithm. BFS enables the evaluation of the number
// of components in a circuit.

// In SPICE, all nodes must be connected to the ground node "0" . Non-connected
// nodes are called "floating nodes" and can be easily found with BFS. When BFS
// is applied to the ground node, all nodes connected to the ground are given
// positive distances. Thereafter, floating nodes are exactly the nodes not
// reached by VFS.

// Each edge in the graph corresponds to two Link-objects, where dir = 1 holds
// when the direction of the link agrees with the original edge and dir = -1
// otherwise.

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

private:
    // As suggested in Cormen - Introduction to Algorithms, graph edges
    // adjacent to each node is assembled to a list.
    std::vector<std::list<Link> > edges;
};

#endif // TOPOLOGY_H
