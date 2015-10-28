/* sillySPICE - A SPICE-like Circuit Solver
   Copyright (C) 2015 Ville Räisänen <vsr at vsr.name>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "parser.h"
#include <string>

Parser::Parser(std::vector<cirStatement> &statList) {
    analysisType = ANALYSIS_EMPTY;

    std::cout << "Parsing Statements:" << std::endl;


//    numRefBranches = 0;

    for (unsigned int indStat = 0; indStat < statList.size(); indStat++) {
        cirStatement stat = statList[indStat];

        std::cout << indStat << ": \"" << stat.strList[0] << "\""
                  << " class=" << stat.statClass
                  << " type=" << stat.type
                  << std::endl;

        // Check the number of arguments given for each statement.
        if (stat.strList.size()-1 < statements[stat.type].minArg
         || stat.strList.size()-1 > statements[stat.type].maxArg) {
            std::cerr << "Invalid number of arguments " << stat.strList.size() << " with"
                      << statements[stat.type].statStr << " "
                      << statements[stat.type].minArg << " - " << statements[stat.type].maxArg
                      << " required!" << std::endl;
            exit(-1);
        }

        // Parse circuit elements and circuit nodes from the statement.
        if (stat.statClass == CLASS_PASSIVE) {
            Element elem(stat);
            elements.push_back(elem);
//            mapNameElem[stat.strList[0]] = elements.size()-1;

            for (unsigned int indNode = 0; indNode < elem.nodeList.size(); indNode++) {
                nodeSet.insert(elem.nodeList[indNode]);
            }
        }
        if (stat.statClass == CLASS_SOURCE) {
            Element elem(stat);
           // mapNameElem[stat.strList[0]] = elements.size();
            elements.push_back(elem);

            // Extract the names of reference branches.
/*            if (stat.type == STAT_CCCS || stat.type == STAT_CCVS) {
                refElements[elem.elemList[0]] = numRefBranches;
                numRefBranches++;
            }*/
            for (unsigned int indNode = 0; indNode < elem.nodeList.size(); indNode++) {
                nodeSet.insert(elem.nodeList[indNode]);
            }
        }

        // Parse analysis statement.
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
    elemList = new ElementList(elements);

    std::cout << std::endl << "Node List:" << std::endl;

    nodeList->disp();

    // Check topology of the circuit:
    topology = new Topology(*nodeList);

    for (unsigned int indElem = 0; indElem < elements.size(); indElem++) {
        Element elem = elements[indElem];
//        assert(elem.nodeList.size() == 2);

        topology->addEdge(elem.nodeList[0], elem.nodeList[1], indElem);
    }

    std::vector<int> dist, parent;
    unsigned int numTraversed;
    topology->BFS(dist, parent, spanningTree, numTraversed, 0);

    std::cout << std::endl << "Topology: " << nodeList->numNodes << " nodes, "
              << numTraversed << " traversed" << std::endl;
    if (nodeList->numNodes != numTraversed && nodeList->numNodes > 1) {
        for (unsigned int indNode = 1; indNode < nodeList->numNodes; indNode++) {
            if (dist[indNode] == -1) {
                std::cerr << "Node " << nodeList->mapNodeString[indNode]
                          << " is floating!" << std::endl;
            }
        }
        exit(-1);
    }


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

}

Parser::~Parser() {
    delete nodeList;
    nodeList = 0;
    delete topology;
    topology = 0;
    delete elemList;
    elemList = 0;
}

#ifdef PARSE_TEST

int
main(int argc, char **argv) {
    cirFile cir(std::string("test.cir"));
    Parser par(cir.statList);
}

#endif
