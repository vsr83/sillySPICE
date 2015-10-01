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
    NodeList(std::set<std::string> &_nodeStrings);
    NodeList(std::vector<std::string> &_nodeStrings);

    ~NodeList();

    std::map <std::string, unsigned int> mapStringNode;
    std::vector <std::string> mapNodeString;
private:
    void parseSet(std::set <std::string> &_nodeStrings);
};

#endif // NODELIST_H
