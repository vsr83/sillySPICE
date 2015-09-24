#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include "value.h"

class Value {
public:
    Value(std::string str);
    ~Value();

    double val;
};

#endif // VALUE_H
