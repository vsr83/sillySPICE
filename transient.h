#ifndef TRANSIENT_H
#define TRANSIENT_H

#include "parser.h"
#include "cirFile.h"
#include "element.h"
#include "nodeList.h"
#include "value.h"
#include "statements.h"
#include "topology.h"

#include "matrix.h"

class Transient {
public:
    Transient(Parser *_parser, double _dt, double _t2, double _t1=0, double _theta=1);
    ~Transient();

    double dt, t1, t2, theta;
private:
    Parser *parser;

    std::vector <std::vector <int> > companionInd;
};

#endif // TRANSIENT_H
