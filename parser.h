#ifndef PARSER_H
#define PARSER_H

#include "cirFile.h"
#include "element.h"
#include "nodeList.h"
#include "value.h"
#include "statements.h"
#include "topology.h"

#include <stdlib.h>

/*
 * This class parses the list of SPICE statements into the objects necessary
 * for description of nodes, elements, analysis and post-processing.
 */

class Parser {
public:
    Parser(std::vector<cirStatement> &statList);
    ~Parser();

    NodeList *nodeList;
    ElementList *elemList;

    // Topology object is used to check whether the circuit has floating
    // nodes.
    Topology *topology;
    std::list<Link> spanningTree;

    // Analysis parameters:
    enum{ANALYSIS_EMPTY, ANALYSIS_DC, ANALYSIS_AC, ANALYSIS_TRAN, ANALYSIS_TF,
         ANALYSIS_OP};
    unsigned int analysisType;

    enum{ANALYSIS_DC_LIN, ANALYSIS_DC_DEC, ANALYSIS_DC_OCT};
    unsigned int analysisTypeDC, analysisDCnpoints;
    std::string  analysisDCvar;
    double analysisDCstartValue, analysisDCendValue, analysisDCinc;

    enum{ANALYSIS_AC_LIN, ANALYSIS_AC_DEC, ANALYSIS_AC_OCT};
    unsigned int analysisTypeAC;
    unsigned int analysisACnpoints, analysisACstartFreq, analysisACendFreq;

private:
    // Circuit elemnts.
    std::vector <Element> elements;
    std::set <std::string> nodeSet;

};

#endif // PARSER_H
