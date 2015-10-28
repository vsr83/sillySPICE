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

#include "topology.h"

Topology::Topology(NodeList &_nodeList) : NodeList(_nodeList) {
    //std::cout << numNodes << std::endl;
    for (unsigned int indNode = 0; indNode < numNodes; indNode++) {
        std::list<Link> ll;
        edges.push_back(ll);
    }
}

Topology::~Topology() {
}

void
Topology::addEdge(unsigned int node1, unsigned int node2, unsigned int globInd) {
    assert(edges.size() >= node1);
    assert(edges.size() >= node2);

    // For each graph edge, add two Link-objects. See topology.h for explanation.

    Link link1(node1, node2, 1, globInd);
    Link link2(node2, node1, -1, globInd);
    edges[node1].push_back(link1);
    edges[node2].push_back(link2);
}

void
Topology::addEdge(std::string node1, std::string node2, unsigned int globInd) {
    addEdge(mapStringNode[node1], mapStringNode[node2], globInd);
}

void
Topology::BFS(std::vector<int> &dist,
              std::vector<int> &parent,
              std::list<Link> &spanningTree,
              unsigned int &numTraversed,
              unsigned int startNode) {
    std::vector<int> color(numNodes, 0),
                     _dist(numNodes, -1),
                     _parent(numNodes, -1);
    numTraversed = 1;
    spanningTree.clear();

    _dist[startNode]   = 0;
    color[startNode]  = 1;
    _parent[startNode] =-1;

    std::queue<int> nodeQueue;
    nodeQueue.push(startNode);

    while (!nodeQueue.empty()) {
        unsigned int node = nodeQueue.front();
        nodeQueue.pop();

        // std::cout << mapNodeString[node] << "-" << _dist[node] << std::endl;

        std::list<Link> llist = edges[node];
        for (std::list<Link>::iterator it=llist.begin(); it!=llist.end(); ++it) {
            Link link = *it;

            unsigned int adjNode = link.node2;
            if (color[adjNode] == 0) {
                numTraversed++;
                color[adjNode] = 1;
                _dist[adjNode] = _dist[node] + 1;
                _parent[adjNode] = node;

                spanningTree.push_back(link);
                nodeQueue.push(adjNode);
            }
        }
        color[node] = 2;
    }
    dist = _dist;
    parent = _parent;
}

void
Topology::dispEdges() {
    for (unsigned int indNode=0; indNode < numNodes; indNode++) {
        std::cout << "Node " << indNode << " / " << numNodes-1
                  << " \"" << mapNodeString[indNode] << "\""
                  << std::endl;
        std::list<Link> llist = edges[indNode];
        for (std::list<Link>::iterator it=llist.begin(); it!=llist.end(); ++it) {
            Link link = *it;
            std::cout << "  (" << link.globInd << ")";
            if (link.dir == 1) {
                std::cout << " Link -> ";
            } else if (link.dir == -1) {
                std::cout << " Link <- ";
            }
            std::cout << link.node2 << " \"" << mapNodeString[link.node2]
                      << "\"" << std::endl;
        }
    }
}


#ifdef TOPOLOGY_TEST2

int
main(int argc, char **argv) {
    std::set<std::string> strset;
    // Note that std::string has a constructor for const char *.
    // Thus, when strset.insert is called with an argument of type const char *,
    // the argument is first passed to a constructor std::string and the
    // resulting object is then passed to strset.insert.

    strset.insert("0");
    strset.insert("1");
    strset.insert("2");
    strset.insert("3");
    strset.insert("4");
    strset.insert("5");
    strset.insert("6");
    strset.insert("7");
    strset.insert("8");
    NodeList nl(strset);
    Topology top(nl);
    top.disp();

    /*      0      1
     *  (0) => (1) => (2)    (0) -- (1) ++ (2)     2  --  1  --  2
     * 2 |    3 I    4 |             -                    |
     *   v  5   v  6   v             -                    |
     *  (3) => (4) => (5)    (3) -- [4] ++ (5)     1  --  0  --  1
     * 7 I    8 I    9 I      +      +      +      |      |      |
     *   v      v      v      +      +      +      |      |      |
     *  (6) -> (7) -> (8)    (6)    (7)    (8)     2      1      2
     *      10     11
     */

    top.addEdge(0, 1, 0);
    top.addEdge(1, 2, 1);
    top.addEdge(0, 3, 2);
    top.addEdge(1, 4, 3);
    top.addEdge(2, 5, 4);
    top.addEdge(3, 4, 5);
    top.addEdge(4, 5, 6);
    top.addEdge(3, 6, 7);
    top.addEdge(4, 7, 8);
    top.addEdge(5, 8, 9);
    top.addEdge(6, 7, 10);
    top.addEdge(7, 8, 11);

    top.dispEdges();
    std::vector<int> dist, parent;
    std::list<Link> spanningTree;
    unsigned int numTraversed;
    top.BFS(dist, parent, spanningTree, numTraversed, 4);

    std::cout << "Spanning tree:" << std::endl;
    for (std::list<Link>::iterator it=spanningTree.begin(); it!=spanningTree.end(); ++it) {
        Link link = *it;
        std::cout << link.globInd << " " << link.dir << std::endl;
    }
    std::cout << "numTraversed: " << numTraversed << std::endl;

    for (unsigned int indNode=0; indNode < dist.size(); indNode++) {
        std::cout << top.mapNodeString[indNode] << " d="
                  << dist[indNode] << std::endl;
    }
}
#endif

#ifdef TOPOLOGY_TEST

int
main(int argc, char **argv) {
    std::set<std::string> strset;
    strset.insert("0");
    strset.insert("s");
    strset.insert("t");
    strset.insert("u");

    strset.insert("v");
    strset.insert("w");
    strset.insert("x");
    strset.insert("y");
    NodeList nl(strset);
    Topology top(nl);
    top.disp();

    /*         1            6
     *    (0) => (s)    (t) => (u)
     *     I      I  3>  |      | 9
     * 0   v    2 v  //  v      v
     *    (v)    (w) => (x) => (y)
     *               4      8
     */

    top.addEdge("0", "v", 0);
    top.addEdge("0", "s", 1);
    top.addEdge("s", "w", 2);
    top.addEdge("w", "t", 3);
    top.addEdge("w", "x", 4);
    top.addEdge("t", "x", 5);
    top.addEdge("t", "u", 6);
    top.addEdge("x", "u", 7);
    top.addEdge("x", "y", 8);
    top.addEdge("u", "y", 9);

    top.dispEdges();

    std::vector<int> dist, parent;
    std::list<Link> spanningTree;
    unsigned int numTraversed;
    top.BFS(dist, parent, spanningTree, numTraversed, 0);

    std::cout << "Spanning tree:" << std::endl;
    for (std::list<Link>::iterator it=spanningTree.begin(); it!=spanningTree.end(); ++it) {
        Link link = *it;
        std::cout << link.globInd << " " << link.dir << std::endl;
    }
    std::cout << "numTraversed: " << numTraversed << std::endl;
}

#endif

