#include "nodeList.h"

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
    std::cout << nl.mapNodeString[0] << "<->"
              << nl.mapStringNode[std::string("0")] << std::endl;
    std::cout << nl.mapNodeString[1] << "<->"
              << nl.mapStringNode[std::string("foo")] << std::endl;
    std::cout << nl.mapNodeString[2] << "<->"
              << nl.mapStringNode[std::string("omena")] << std::endl;
}
#endif
