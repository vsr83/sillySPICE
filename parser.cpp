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

        if (stat.statClass == cirStatement::CLASS_PASSIVE
                || stat.statClass == cirStatement::CLASS_SOURCE) {
            Element elem(stat);
            elements.push_back(elem);

            for (unsigned int indNode = 0; indNode < elem.nodeList.size(); indNode++) {
                nodeSet.insert(elem.nodeList[indNode]);
            }
        }

        if (stat.statClass == cirStatement::CLASS_META) {
            // Parse analysis statements
            switch (stat.type) {
                case cirStatement::STAT_ANALYSIS: {
                    std::string anTypeStr(stat.strList[0]);

                    if (anTypeStr == ".DC") {
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
                            Value startFreq(stat.strList[3]),
                                  endFreq  (stat.strList[4]),
                                  stepFreq (stat.strList[5]);
                            analysisDCstartFreq = startFreq.val;
                            analysisDCendFreq   = endFreq.val;
                            if (analysisTypeDC == ANALYSIS_DC_LIN) {
                                analysisDCnpoints = atoi(stat.strList[5].c_str());
                            } else {
                                analysisDCinc   = stepFreq.val;
                            }
                        } else if (stat.strList.size() == 5) {
                            analysisTypeDC = ANALYSIS_DC_LIN;
                            analysisDCvar = stat.strList[1];
                            Value startFreq(stat.strList[2]),
                                  endFreq  (stat.strList[3]),
                                  stepFreq (stat.strList[4]);
                            analysisDCstartFreq = startFreq.val;
                            analysisDCendFreq   = endFreq.val;
                        } else {
                            std::cerr << "Unknown number of parameters" << std::endl;
                            exit(-1);
                        }

                    } else if (anTypeStr == ".AC" ) {
                        analysisType = ANALYSIS_AC;
                        analysisACnpoints = atoi(stat.strList[2].c_str());
                        Value startFreq(stat.strList[3]),
                              endFreq(stat.strList[4]);
                        analysisACstartFreq = startFreq.val;
                        analysisACendFreq   = endFreq.val;

                        if (stat.strList[1] == "LIN") {
                            analysisTypeAC = ANALYSIS_AC_LIN;
                        } else if (stat.strList[1] == "DEC") {
                            analysisTypeAC = ANALYSIS_AC_DEC;
                        } else if (stat.strList[1] == "OCT") {
                            analysisTypeAC = ANALYSIS_AC_OCT;
                        }
                    } else if (anTypeStr == ".TRAN") {
                        analysisType = ANALYSIS_TRAN;
                    } else if (anTypeStr == ".TF") {
                        analysisType = ANALYSIS_TF;
                    } else if (anTypeStr == ".OP") {
                        analysisType = ANALYSIS_OP;
                    } else {
                        std::cerr << "Unknown analysis type :" << anTypeStr;
                    }
                }
                break;
            }
        }
    }
    assert(analysisType != ANALYSIS_EMPTY);
    nodeList = new NodeList(nodeSet);

    for (unsigned int elemInd = 0; elemInd < elements.size(); elemInd++) {
        Element elem = elements[elemInd];
        std::cout << "Element " << elemInd << " :";
        for (unsigned int nodeInd = 0; nodeInd < elem.nodeList.size(); nodeInd++) {
            std::cout << "\"" << elem.nodeList[nodeInd] << "\"/"
                      << nodeList->mapStringNode[elem.nodeList[nodeInd]] << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << "Analysis Type: " << analysisType << std::endl;
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
