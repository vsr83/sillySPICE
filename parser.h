#ifndef PARSER_H
#define PARSER_H

#include "cirFile.h"
#include "element.h"
#include "nodeList.h"
#include "value.h"

#include <stdlib.h>

class Parser {
public:
    Parser(std::vector<cirStatement> &statList);
    ~Parser();

    std::vector <Element> elements;
    std::set <std::string> nodeSet;

    std::map <unsigned int, unsigned int> elemTypeCount;

    NodeList *nodeList;

    enum{ANALYSIS_EMPTY, ANALYSIS_DC, ANALYSIS_AC, ANALYSIS_TRAN, ANALYSIS_TF,
         ANALYSIS_OP};
    unsigned int analysisType;

    enum{ANALYSIS_DC_LIN, ANALYSIS_DC_DEC, ANALYSIS_DC_OCT};
    unsigned int analysisTypeDC, analysisDCnpoints;
    std::string analysisDCvar;
    double analysisDCstartValue, analysisDCendValue, analysisDCinc;

    enum{ANALYSIS_AC_LIN, ANALYSIS_AC_DEC, ANALYSIS_AC_OCT};
    unsigned int analysisTypeAC;
    unsigned int analysisACnpoints, analysisACstartFreq, analysisACendFreq;
};

#endif // PARSER_H
