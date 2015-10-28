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

#include "assembly.h"

Assembly::Assembly(NodeList *_nodeList, ElementList *_elemList, bool _complex) :
                                      currentRe(_elemList->elements.size(), 0),
                                      currentIm(_elemList->elements.size(), 0),
                                      voltageRe(_elemList->elements.size(), 0),
                                      voltageIm(_elemList->elements.size(), 0) {
    nodeList         = _nodeList;
    elemList         = _elemList;
    complex          = _complex;
    systemMNA        = 0;
    systemExcitation = 0;

    // Numbers of elements, which influence the number of DoFs.
    unsigned int numRes  = elemList->typeCount[STAT_RESISTANCE],
                 numVolt = elemList->typeCount[STAT_VOLTAGESOURCE],
                 numCur  = elemList->typeCount[STAT_CURRENTSOURCE],
                 numVCVS = elemList->typeCount[STAT_VCVS],
                 numCCVS = elemList->typeCount[STAT_CCVS];

    numNodes = nodeList->numNodes;
    numDoF = numNodes - 1 + numVolt + numVCVS + numCCVS;

    // When the degrees of freedom are complex-valued, the real and imaginary
    // parts of the DoF are included as separate degrees of freedom in the
    // system matrix and excitation vectors.

    if (complex) {
        fullMNA = new Matrix((numDoF+1)*2, (numDoF+1)*2);
        fullExcitation = new double[(numDoF+1)*2];

        for (unsigned indDoF = 0; indDoF < (numDoF+1)*2; indDoF++) {
           fullExcitation[indDoF] = 0;
        }
    } else {
        fullMNA = new Matrix(numDoF+1, numDoF+1);
        fullExcitation = new double[numDoF+1];

        for (unsigned indDoF = 0; indDoF < numDoF+1; indDoF++) {
           fullExcitation[indDoF] = 0;
        }
    }

    if (complex) {
        buildComplex();
        exit(-1);
    } else {
        buildReal();
    }

    // Extract the system matrix and excitation vector:
    if (complex) {
        std::cerr << "ASSEMBLY : Complex solver not implemented!" << std::endl;
        exit(-1);
    } else {
        systemMNA = fullMNA->submatrix(1, numDoF, 1, numDoF);

        systemExcitation = new double[numDoF];
        for (unsigned indDoF = 0; indDoF < numDoF; indDoF++) {
            systemExcitation[indDoF] = fullExcitation[indDoF + 1];
        }
    }

}

void
Assembly::buildReal() {
    // indSource is used as the index for additional DoFs due to sources in
    // the MNA formulation.

    unsigned int indRes= 0, indSource = 0;

    std::cout << std::endl << "Assembly:" << std::endl;

    for (unsigned int indElem = 0; indElem < elemList->elements.size(); indElem++) {
        Element elem = elemList->elements[indElem];

        std::string nodeStr1 = elem.nodeList[0],
                    nodeStr2 = elem.nodeList[1];
        unsigned int node1 = nodeList->mapStringNode[nodeStr1],
                     node2 = nodeList->mapStringNode[nodeStr2];

        switch (elem.elemType) {
        case STAT_RESISTANCE: {
            double resValue = elem.valueList[0];

            std::cout << "Conductance " << 1/resValue << " Ohms, nodes: "
                      << nodeStr1 << "-" << nodeStr2 << std::endl;

            // Current out of node 1 through the conductance G.
            // i_1  = G(v_1 - v_2)
            fullMNA->addto(node1, node1, 1/resValue);
            fullMNA->addto(node1, node2, -1/resValue);
            // i_2 = G(v_2 - v_1)
            fullMNA->addto(node2, node2, 1/resValue);
            fullMNA->addto(node2, node1, -1/resValue);
            indRes++;
        } break;
        case STAT_VOLTAGESOURCE: {
            double voltValue = elem.valueList[0];

            std::cout << "Voltage Source " << voltValue << " V, nodes: "
                      << nodeStr1 << "-" << nodeStr2 << std::endl;

            // Additional variable x[numNodes + indSource] is associated to
            // the current through the voltage source.

            // v_1 - v_2 = voltValue
            fullMNA->set(numNodes + indSource, node1,  1);
            fullMNA->set(numNodes + indSource, node2, -1);
            fullExcitation[numNodes + indSource] = voltValue;

            // Additional current into the node from the voltage source.
            fullMNA->set(node1, numNodes + indSource, -1);
            fullMNA->set(node2, numNodes + indSource, 1);

            sourceDoFmap[elem.name] = numNodes + indSource;
            indSource++;
        } break;
        case STAT_CURRENTSOURCE: {
            // Current sources are "natural" for nodal analysis and thus do
            // not introduce additional degrees of freedom.

            double currValue = elem.valueList[0];
            fullExcitation[node1] -= currValue;
            fullExcitation[node2] += currValue;
        } break;
        case STAT_VCVS: {
        } break;
        case STAT_CCCS: {
        } break;
        case STAT_VCCS: {
        } break;
        case STAT_CCVS: {
        } break;
        default:
            std::cerr << "ASSEMBLY : Unknown Element Type! " << elem.elemType << std::endl;
            exit(-1);
            break;
        }
    }

    for (unsigned int indElem = 0; indElem < elemList->elements.size(); indElem++) {
        Element elem = elemList->elements[indElem];

        std::string nodeStr1 = elem.nodeList[0],
                    nodeStr2 = elem.nodeList[1];
        unsigned int node1 = nodeList->mapStringNode[nodeStr1],
                     node2 = nodeList->mapStringNode[nodeStr2];

        switch (elem.elemType) {
        case STAT_RESISTANCE: {
        } break;
        case STAT_VOLTAGESOURCE: {
        } break;
        case STAT_CURRENTSOURCE: {
        } break;
        case STAT_VCVS: {
            assert(elem.nodeList.size() >= 4);

            std::string nodeStr3 = elem.nodeList[2],
                        nodeStr4 = elem.nodeList[3];
            unsigned int node3 = nodeList->mapStringNode[nodeStr3],
                         node4 = nodeList->mapStringNode[nodeStr4];

            std::cout << "VCVS " << nodeStr1 << " " << nodeStr2 << " "
                      << nodeStr3 << " " << nodeStr4 << std::endl;


            double gainValue = elem.valueList[0];

            // v_1 - v_2 = gain*(v_3 - v_4)
            fullMNA->set(numNodes + indSource, node1,  1);
            fullMNA->set(numNodes + indSource, node2, -1);
            fullMNA->addto(numNodes + indSource, node3, -gainValue);
            fullMNA->addto(numNodes + indSource, node4, gainValue);

            // Additional current into the node from the voltage source.
            fullMNA->set(node1, numNodes + indSource, -1);
            fullMNA->set(node2, numNodes + indSource, 1);

            sourceDoFmap[elem.name] = numNodes + indSource;
            indSource++;
        } break;
        case STAT_CCCS: {
            double gainValue = elem.valueList[0];
            assert(elem.elemList.size() > 0);
            // assert(parser->mapElemDummy.find(elem.elemList[0]) != parser->mapElemDummy.end());
            //std::string dummyVSname = parser->mapElemDummy[elem.elemList[0]];

            std::string dummyVSname  = elem.elemList[0];
            if (sourceDoFmap.find(dummyVSname) == sourceDoFmap.end()) {
                std::cerr << "Unknown source: \"" << dummyVSname << "\"" << std::endl;
                exit(-1);
            }
            unsigned int refCurDoF = sourceDoFmap[dummyVSname];
            fullMNA->addto(node1, refCurDoF, -gainValue);
            fullMNA->addto(node2, refCurDoF, gainValue);
        } break;
        case STAT_VCCS: {
            assert(elem.nodeList.size() >= 4);
            std::string nodeStr3 = elem.nodeList[2],
                        nodeStr4 = elem.nodeList[3];
            unsigned int node3 = nodeList->mapStringNode[nodeStr3],
                         node4 = nodeList->mapStringNode[nodeStr4];

            double gainValue = elem.valueList[0];

            // v_1 - v_2 = gain*(v_3 - v_4)
            fullMNA->addto(node1, node3, gainValue);
            fullMNA->addto(node1, node4, -gainValue);
            fullMNA->addto(node2, node3, -gainValue);
            fullMNA->addto(node2, node4, gainValue);
        } break;
        case STAT_CCVS: {
            double gainValue = elem.valueList[0];
            assert(elem.elemList.size() > 0);

            std::string dummyVSname  = elem.elemList[0];
            if (sourceDoFmap.find(dummyVSname) == sourceDoFmap.end()) {
                std::cerr << "Unknown source: \"" << dummyVSname << "\"" << std::endl;
                exit(-1);
            }
            unsigned int refCurDoF = sourceDoFmap[dummyVSname];

            // v_1 - v_2 = voltValue
            fullMNA->set(numNodes + indSource, node1, -1);
            fullMNA->set(numNodes + indSource, node2, 1);
            fullMNA->set(numNodes + indSource, refCurDoF, -gainValue);

            // Additional current into the node from the voltage source.
            fullMNA->set(node1, numNodes + indSource, -1);
            fullMNA->set(node2, numNodes + indSource, 1);

            sourceDoFmap[elem.name] = numNodes + indSource;
            indSource++;
        }break;
        default:
            std::cerr << "ASSEMBLY : Unknown Element Type! " << elem.elemType << std::endl;
            exit(-1);
            break;
        }
    }



}

void
Assembly::buildComplex() {

}

Assembly::~Assembly() {
    delete systemMNA;
    delete [] systemExcitation;
    delete fullMNA;
    delete [] fullExcitation;

    systemMNA = 0;
    systemExcitation = 0;
    fullMNA = 0;
    fullExcitation = 0;
}

double *
Assembly::solve() {
    Matrix L(numDoF, numDoF);
    Matrix U(numDoF, numDoF);
    Matrix P(numDoF, numDoF);

    double * sol;
    systemMNA->LU(L, U, P);
    sol = systemMNA->LU_solve(L, U, P, systemExcitation);

    return sol;
}

void
Assembly::postProc(double *sol) {
    for (unsigned int indElem = 0; indElem < elemList->elements.size(); indElem++) {
        Element elem = elemList->elements[indElem];

        assert(elem.nodeList.size() >= 2);
        std::string nodeStr1 = elem.nodeList[0],
                    nodeStr2 = elem.nodeList[1];
        unsigned int node1 = nodeList->mapStringNode[nodeStr1],
                     node2 = nodeList->mapStringNode[nodeStr2];

        assert(node1 <= numNodes);
        assert(node2 <= numNodes);

        double val1 = 0, val2 = 0;
        if (node1 > 0) {
            val1 = sol[node1 - 1];
        }
        if (node2 > 0) {
            val2 = sol[node2 - 1];
        }
        voltageRe[indElem] = val1 - val2;

        switch(elem.elemType) {
        case STAT_RESISTANCE:
            currentRe[indElem] = voltageRe[indElem]/elem.valueList[0];
            break;
        case STAT_VOLTAGESOURCE: {
            if (sourceDoFmap.find(elem.name) == sourceDoFmap.end()) {
                std::cerr << "Unknown source: \"" << elem.name << "\"" << std::endl;
                exit(-1);
            }
            unsigned int dof = sourceDoFmap[elem.name];
            assert(dof > 0);
            std::cout << elem.name << "->" << dof-1 << std::endl;
            currentRe[indElem] = -sol[dof-1];
        }
            break;
        case STAT_CURRENTSOURCE:
            currentRe[indElem] = elem.valueList[0];
            break;
        case STAT_VCVS: {
            if (sourceDoFmap.find(elem.name) == sourceDoFmap.end()) {
                std::cerr << "Unknown source: \"" << elem.name << "\"" << std::endl;
                exit(-1);
            }
            unsigned int dof = sourceDoFmap[elem.name];
            assert(dof > 0);
            currentRe[indElem] = -sol[dof-1];
        }
            break;
        case STAT_VCCS: {
            assert(elem.valueList.size() >= 1);
            double gainValue = elem.valueList[0];
            assert(elem.nodeList.size() >= 4);

            std::string nodeStr3 = elem.nodeList[2],
                        nodeStr4 = elem.nodeList[3];
            unsigned int node3 = nodeList->mapStringNode[nodeStr3],
                         node4 = nodeList->mapStringNode[nodeStr4];

            currentRe[indElem] = gainValue*(sol[node3-1] - sol[node4-1]);
        }
            break;
        case STAT_CCVS: {
            if (sourceDoFmap.find(elem.name) == sourceDoFmap.end()) {
                std::cerr << "Unknown source: \"" << elem.name << "\"" << std::endl;
                exit(-1);
            }
            unsigned int dof = sourceDoFmap[elem.name];
            std::cout << dof << std::endl;
            assert(dof > 0);
            currentRe[indElem] = -sol[dof-1];
        }
            break;
        case STAT_CCCS:
            double gainValue = elem.valueList[0];
            assert(elem.elemList.size() > 0);

            std::string dummyVSname  = elem.elemList[0];
            if (sourceDoFmap.find(dummyVSname) == sourceDoFmap.end()) {
                std::cerr << "Unknown source: \"" << dummyVSname << "\"" << std::endl;
                exit(-1);
            }
            unsigned int refCurDoF = sourceDoFmap[dummyVSname];
            currentRe[indElem] = gainValue * sol[refCurDoF];
            break;
        }
    }
}

void
Assembly::disp() {
    std::cout << std::endl << "Branch voltages and currents:" << std::endl;
    for (unsigned int indElem = 0; indElem < elemList->elements.size(); indElem++) {
        Element elem = elemList->elements[indElem];
        std::cout << elem.name << " " << voltageRe[indElem]
                  << " " << currentRe[indElem] << std::endl;
    }
}

#ifdef ASSEMBLY_TEST

int
main(int argc, char **argv) {
    std::string fileName;

    if (argc < 2) {
        fileName = "test2.cir";
    } else {
        fileName = argv[1];
    }

    cirFile cir(fileName);
    std::cout << std::endl << "DC Analysis of resistive circuit: \""
              << cir.title << "\"" << std::endl;
    Parser parser(cir.statList);
    Assembly ass(parser.nodeList, parser.elemList, false);

    std::cout << std::endl << "Full Matrix:" << std::endl;
    ass.fullMNA->disp();
    std::cout << std::endl << "System Matrix:" << std::endl;
    ass.systemMNA->disp();

    std::cout << std::endl << "Excitation Vector:" << std::endl;
    for (unsigned int ind = 0; ind < ass.numDoF; ind++) {
        std::cout << ass.systemExcitation[ind] << " ";
    }
    std::cout << std::endl;

    std::cout << std::endl << "Solution:" << std::endl;
    double *sol = ass.solve();
    for (unsigned int ind = 0; ind < ass.numDoF; ind++) {
        std::cout << sol[ind] << " ";
    }
    std::cout << std::endl;

    unsigned int numNodes = parser.nodeList->numNodes;
    ass.postProc(sol);
    ass.disp();
}

#endif
