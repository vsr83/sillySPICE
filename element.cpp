// Elements can have more than two nodes, whereas branches in a graph have
// two nodes.

#include "element.h"

Element::Element() {
    elemType = STAT_EMPTY;
    elemClass = CLASS_EMPTY;
}

Element::Element(const cirStatement &stat) {
    elemType  = stat.type;
    elemClass = stat.statClass;
    name  = stat.strList[0];

    std::vector <std::string> strList = stat.strList;

    assert(elemClass == CLASS_SOURCE || elemClass == CLASS_PASSIVE
           || elemClass == CLASS_NONLINEAR);

    switch (elemClass) {
    case CLASS_PASSIVE:
    {
        assert(strList.size() == 4);

        nodeList.push_back(strList[1]);
        nodeList.push_back(strList[2]);
        Value val(strList[3]);
        valueList.push_back(val.val);
        assert(val.val != 0);
    }
        break;
    case CLASS_SOURCE:
    {
        switch (elemType) {
        case STAT_VOLTAGESOURCE: {
            assert(strList.size() == 4 || strList.size() == 5);

            nodeList.push_back(strList[1]);
            nodeList.push_back(strList[2]);

            if (strList.size() == 4) {
                Value val(strList[3]);
                valueList.push_back(val.val);
            } else {
                typeList.push_back(strList[3]);
                Value val(strList[4]);
                valueList.push_back(val.val);
            }
        } break;
        case STAT_CURRENTSOURCE: {
            assert(strList.size() == 4 || strList.size() == 5);

            nodeList.push_back(strList[1]);
            nodeList.push_back(strList[2]);

            if (strList.size() == 4) {
                Value val(strList[3]);
                valueList.push_back(val.val);
            } else {
                typeList.push_back(strList[3]);
                Value val(strList[4]);
                valueList.push_back(val.val);
            }
        } break;
        case STAT_CCCS: {
            assert(strList.size() == 5);
            nodeList.push_back(strList[1]); // +n
            nodeList.push_back(strList[2]); // -n
            elemList.push_back(strList[3]); // ref elem
            Value val(strList[4]);
            valueList.push_back(val.val);   // gain
        } break;
        case STAT_CCVS: {
            assert(strList.size() == 5);
            nodeList.push_back(strList[1]); // +n
            nodeList.push_back(strList[2]); // -n
            elemList.push_back(strList[3]); // ref elem
            Value val(strList[4]);
            valueList.push_back(val.val);   // gain
        } break;
        case STAT_VCVS: {
            assert(strList.size() == 6);
            nodeList.push_back(strList[1]); // +n
            nodeList.push_back(strList[2]); // -n
            nodeList.push_back(strList[3]); // +c
            nodeList.push_back(strList[4]); // -c
            Value val(strList[5]);
            valueList.push_back(val.val);   // gain
        } break;
        case STAT_VCCS: {
            assert(strList.size() == 6);
            nodeList.push_back(strList[1]); // +n
            nodeList.push_back(strList[2]); // -n
            nodeList.push_back(strList[3]); // +c
            nodeList.push_back(strList[4]); // -c
            Value val(strList[5]);
            valueList.push_back(val.val);   // gain
        } break;
        default:
            break;
        }
    }
        break;
    case CLASS_NONLINEAR:
        break;
    }
}

Element::~Element() {
}

ElementList::ElementList(std::vector <Element> &_elements) {
    elements = _elements;

    // Create a node list and count the number of elements with each type.
    for (unsigned int elemInd = 0; elemInd < elements.size(); elemInd++) {
        Element elem = elements[elemInd];

        if (typeCount.find(elem.elemType) == typeCount.end()) {
            typeCount[elem.elemType] = 1;
        } else {
            typeCount[elem.elemType]++;
        }

        mapNameElem[elem.name] = elemInd;
    }
}

void
ElementList::disp() {
    std::cout << std::endl << "Element List:" << std::endl;
    // Create a node list and count the number of elements with each type.
    for (unsigned int elemInd = 0; elemInd < elements.size(); elemInd++) {
        Element elem = elements[elemInd];

        std::cout << "Element " << elemInd << " \"" << elem.name << "\""<< " :";
        for (unsigned int nodeInd = 0; nodeInd < elem.nodeList.size(); nodeInd++) {
            std::cout << "\"" << elem.nodeList[nodeInd] << "\", ";
        }
        std::cout << typeCount[elem.elemType];
        std::cout << std::endl;
    }

    std::cout << std::endl << "Element Mapping:" << std::endl;
    // Element list:
    for (std::map<std::string, unsigned int>::iterator it=mapNameElem.begin();
         it != mapNameElem.end(); ++it) {
        std::string  name    = it->first;
        unsigned int elemInd = it->second;
        std::cout << "  " << name << "->" << elemInd << std::endl;

    }
}

ElementList::~ElementList() {

}
