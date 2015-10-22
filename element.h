#ifndef ELEMENT_H
#define ELEMENT_H

#include <iostream>
#include <vector>
#include <map>
#include <assert.h>

#include "cirFile.h"
#include "value.h"
#include "statements.h"

class Element {
public:
    Element();
    Element(const cirStatement &stat);
    ~Element();

    unsigned int elemType, elemClass;
    std::string name;
    std::vector <std::string> nodeList;
    std::vector <std::string> elemList;
    std::vector <std::string> typeList;
    std::vector <double> valueList;
};

class ElementList {
public:
    ElementList(std::vector <Element> &_elements);
    ~ElementList();

    std::vector <Element> elements;
    std::map <std::string, unsigned int> mapNameElem;
    std::map <unsigned int, unsigned int> typeCount;
};

#endif // ELEMENT_H
