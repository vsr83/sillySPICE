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

#ifndef NODELIST_H
#define NODELIST_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <assert.h>

// The nodes in a SPICE circuits are labeled with strings. A NodeList object
// maps the names of the nodes in a NetList to integer indices. The mandatory
// ground node "0" is always given the integer value dof 0.

// The constructor is given a vector or set of node names.

class NodeList {
public:
    NodeList(const NodeList &nodeList);
    NodeList(std::set<std::string> &_nodeStrings);
    NodeList(std::vector<std::string> &_nodeStrings);

    ~NodeList();

    // Addition of dummy nodes is necessary when replacing an element with
    // a series combination of elements.
    bool nodeExists(std::string &nodeStr);
    unsigned int addNode(std::string &nodeStr);
    unsigned int addNode();
    void disp();

    std::map <std::string, unsigned int> mapStringNode;
    std::vector <std::string> mapNodeString;

    unsigned int numNodes;
private:
    void parseSet(std::set <std::string> &_nodeStrings);
};

#endif // NODELIST_H
