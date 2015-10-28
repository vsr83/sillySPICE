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

#include "nodeList.h"
#include <sstream>

NodeList::NodeList(std::vector<std::string> &_nodeStrings) {
    std::set<std::string> nodeStrSet;
    for (unsigned int vecInd = 0; vecInd < _nodeStrings.size(); vecInd++) {
        nodeStrSet.insert(_nodeStrings[vecInd]);
    }
    parseSet(nodeStrSet);
}

NodeList::NodeList(std::set<std::string> &_nodeStrings) {
    parseSet(_nodeStrings);
}

NodeList::NodeList(const NodeList &nodeList) {
    mapNodeString = nodeList.mapNodeString;
    mapStringNode = nodeList.mapStringNode;
    numNodes      = nodeList.numNodes;
}

void
NodeList::parseSet(std::set<std::string> &_nodeStrings) {
    unsigned int nodeInd = 0;
    bool gndFound = false;

    mapStringNode[std::string("0")] = 0;
    mapNodeString.push_back(std::string("0"));

    for (std::set<std::string>::iterator it=_nodeStrings.begin(); it!=_nodeStrings.end(); ++it) {
        std::string nodeStr = *it;

//        std::cout << "node:" << nodeStr << std::endl;
        assert(nodeStr.length() > 0);
        if (nodeStr == "0") {
            gndFound = true;
        } else {
            nodeInd++;
            mapStringNode[nodeStr] = nodeInd;
            mapNodeString.push_back(nodeStr);
        }
    }
    assert(gndFound);
    numNodes = mapNodeString.size();
//    std::cout << numNodes << std::endl;
}

unsigned int
NodeList::addNode(std::string &nodeStr) {
    assert(!nodeExists(nodeStr));

    mapStringNode[nodeStr] = numNodes;
    mapNodeString.push_back(nodeStr);
    numNodes++;

    return numNodes-1;
}

unsigned int
NodeList::addNode() {
    unsigned int strint = numNodes;
    std::stringstream ss;
    std::string s;

    // If the string constructed from the new node number is reserved, select
    // smallest free positive integer for the name.
    while (1) {
        ss.str(std::string());
        ss << strint;
        s = ss.str();
        if  (!nodeExists(s)) break;
        strint++;
    }
    return addNode(s);
}

bool
NodeList::nodeExists(std::string &nodeStr) {
    return mapStringNode.find(nodeStr) != mapStringNode.end();
}

void
NodeList::disp() {
    for (std::map<std::string, unsigned int>::iterator it=mapStringNode.begin(); it!=mapStringNode.end(); ++it) {
        std::cout << "\"" << it->first << "\" -> " << it->second << std::endl;
    }
}

NodeList::~NodeList() {
}

#ifdef NODELIST_TEST
int
main(int argc, char **argv) {
    std::set<std::string> strset;
    strset.insert(std::string("foo"));
    strset.insert(std::string("omena"));
    strset.insert(std::string("omena"));
    strset.insert(std::string("0"));
    NodeList nl(strset);

    std::string kikka("4");
    unsigned int node = nl.addNode(kikka), node2 = nl.addNode();

    std::cout << node << std::endl;
    std::cout << nl.mapNodeString[0] << "<->"
              << nl.mapStringNode[std::string("0")] << std::endl;
    std::cout << nl.mapNodeString[1] << "<->"
              << nl.mapStringNode[std::string("foo")] << std::endl;
    std::cout << nl.mapNodeString[2] << "<->"
              << nl.mapStringNode[std::string("omena")] << std::endl;
    std::cout << nl.mapNodeString[3] << "<->"
              << nl.mapStringNode[std::string("4")] << std::endl;
    std::cout << nl.mapNodeString[4] << "<->"
              << nl.mapStringNode[std::string("5")] << std::endl;
}
#endif
