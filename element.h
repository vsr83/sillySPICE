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
    std::vector <std::string> typeList;
    std::vector <double> valueList;
};

#endif // ELEMENT_H
