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

    assert(elemClass == CLASS_SOURCE || elemClass == CLASS_PASSIVE
           || elemClass == CLASS_NONLINEAR);

    switch (elemClass) {
    case CLASS_PASSIVE:
    {
        assert(stat.strList.size() == 4);

        nodeList.push_back(stat.strList[1]);
        nodeList.push_back(stat.strList[2]);
        Value val(stat.strList[3]);
        valueList.push_back(val.val);
    }
        break;
    case CLASS_SOURCE:
    {
        assert(stat.strList.size() == 4 || stat.strList.size() == 5);

        nodeList.push_back(stat.strList[1]);
        nodeList.push_back(stat.strList[2]);

        if (stat.strList.size() == 4) {
            Value val(stat.strList[3]);
            valueList.push_back(val.val);
        } else {
            typeList.push_back(stat.strList[3]);
            Value val(stat.strList[4]);
            valueList.push_back(val.val);
        }
    }
        break;
    case CLASS_NONLINEAR:
        break;
    }
}

Element::~Element() {
}
