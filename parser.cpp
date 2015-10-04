#include "parser.h"
#include <string>

Parser::Parser(std::vector<cirStatement> &statList) {
    analysisType = ANALYSIS_EMPTY;

    std::cout << "Parsing Statements:" << std::endl;

    for (unsigned int indStat = 0; indStat < statList.size(); indStat++) {
        cirStatement stat = statList[indStat];

        std::cout << indStat << ": \"" << stat.strList[0] << "\""
                  << " class=" << stat.statClass
                  << " type=" << stat.type
                  << std::endl;

        if (stat.statClass == CLASS_PASSIVE || stat.statClass == CLASS_SOURCE) {
            Element elem(stat);
            elements.push_back(elem);

            for (unsigned int indNode = 0; indNode < elem.nodeList.size(); indNode++) {
                nodeSet.insert(elem.nodeList[indNode]);
            }
        }

        if (stat.statClass == CLASS_ANALYSIS) {
            switch(stat.type) {
            case STAT_ANALYSISDC:
                analysisType = ANALYSIS_DC;
                if (stat.strList.size() == 6) {
                    if (stat.strList[1] == "LIN") {
                        analysisTypeDC = ANALYSIS_DC_LIN;
                    } else if (stat.strList[1] == "DEC") {
                        analysisTypeDC = ANALYSIS_DC_DEC;
                    } else if (stat.strList[1] == "OCT") {
                        analysisTypeDC = ANALYSIS_DC_OCT;
                    } else {
                        std::cerr << "Unknown DC analysis mode" << std::endl;
                        exit(-1);
                    }
                    analysisDCvar = stat.strList[2];
                    Value startVal(stat.strList[3]),
                          endVal  (stat.strList[4]),
                          stepVal (stat.strList[5]);
                    analysisDCstartValue = startVal.val;
                    analysisDCendValue   = endVal.val;
                    if (analysisTypeDC == ANALYSIS_DC_LIN) {
                       analysisDCinc     = 0;
                       analysisDCnpoints = atoi(stat.strList[5].c_str());
                    } else {
                       analysisDCinc   = stepVal.val;
                       analysisDCnpoints = 0;
                    }
                } else if (stat.strList.size() == 5) {
                    analysisTypeDC = ANALYSIS_DC_LIN;
                    analysisDCvar = stat.strList[1];
                    Value startVal(stat.strList[2]),
                          endVal  (stat.strList[3]);
                    analysisDCstartValue = startVal.val;
                    analysisDCendValue   = endVal.val;
                    analysisDCnpoints = atoi(stat.strList[4].c_str());
                    analysisDCinc = 0;
                } else {
                   std::cerr << "Unknown number of parameters" << std::endl;
                   exit(-1);
                }
                break;
            default:
                break;
            }
        }
    }
    nodeList = new NodeList(nodeSet);

    std::cout << "Analysis Type: ";
    switch (analysisType) {
    case ANALYSIS_DC:
        std::cout << "DC ";
        switch(analysisTypeDC) {
        case ANALYSIS_DC_LIN:
            std::cout << "LIN ";
            break;
        case ANALYSIS_DC_DEC:
            std::cout << "DEC ";
            break;
        case ANALYSIS_DC_OCT:
            std::cout << "OCT ";
            break;
        default:
            std::cerr << "Unknown DC Analysis Type!" << std::endl;
            exit(-1);
        }
        std::cout << std::endl;
        std::cout << "Source: \"" << analysisDCvar << "\" - Sweep : "
                  << analysisDCstartValue << " - " << analysisDCendValue;

        if (analysisDCinc == 0) {
            std::cout << " " << analysisDCnpoints << " steps";
        } else if (analysisDCnpoints != 0) {
            std::cout << " dvalue = " << analysisDCinc;
        } else {
            std::cerr << "Invalid number of points in DC sweep!" << std::endl;
            exit(-1);
        }

        std::cout << std::endl;
        break;
    }

    for (unsigned int elemInd = 0; elemInd < elements.size(); elemInd++) {
        Element elem = elements[elemInd];

        if (elemTypeCount.find(elem.elemType) == elemTypeCount.end()) {
            elemTypeCount[elem.elemType] = 1;
        } else {
            elemTypeCount[elem.elemType]++;
        }

        std::cout << "Element " << elemInd << " \"" << elem.name << "\""<< " :";
        for (unsigned int nodeInd = 0; nodeInd < elem.nodeList.size(); nodeInd++) {
            std::cout << "\"" << elem.nodeList[nodeInd] << "\"/"
                      << nodeList->mapStringNode[elem.nodeList[nodeInd]] << ", ";
        }
        std::cout << elemTypeCount[elem.elemType];
        std::cout << std::endl;
    }

    nodeList->disp();
}

Parser::~Parser() {
    delete nodeList;
    nodeList = 0;
}

#ifdef PARSE_TEST

int
main(int argc, char **argv) {
    cirFile cir(std::string("test.cir"));
    Parser par(cir.statList);
}

#endif
