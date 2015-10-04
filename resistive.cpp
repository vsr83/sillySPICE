#include "parser.h"
#include "matrix.h"
#include "statements.h"

#ifdef RESISTIVE_TEST

int
main(int argc, char **argv) {
    std::string fileName;

    if (argc < 2) {
        fileName = "test2.cir";
    } else {
        fileName = argv[1];
    }

    cirFile cir(fileName);
    Parser par(cir.statList);

    std::cout << std::endl << "DC Analysis of resistive circuit: \""
              << cir.title << "\"" << std::endl;

    unsigned int numNodes = par.nodeList->numNodes;

    unsigned int numRes = par.elemTypeCount[STAT_RESISTANCE],
                 numVolt= par.elemTypeCount[STAT_VOLTAGESOURCE];
    unsigned int numDoFs = numNodes - 1 + numVolt;
    unsigned int indRes = 0, indVolt = 0;

    Matrix systemMNA(numDoFs, numDoFs);
    double *systemExcitation = new double[numDoFs];

    for (unsigned indDoF = 0; indDoF < numDoFs; indDoF++) {
        systemExcitation[indDoF] = 0;
    }

    std::cout << numNodes << " nodes, "
              << numDoFs << " DoFs" << std::endl;
    std::cout << numRes << " Resistances, "
              << numVolt << " Voltage Sources" << std::endl;

    for (unsigned int indElem = 0; indElem < par.elements.size(); indElem++) {
        Element elem = par.elements[indElem];

        if (elem.elemType == STAT_RESISTANCE) {
            std::string nodeStr1 = elem.nodeList[0],
                        nodeStr2 = elem.nodeList[1];
            unsigned int node1 = par.nodeList->mapStringNode[nodeStr1],
                         node2 = par.nodeList->mapStringNode[nodeStr2];
            double resValue = elem.valueList[0];

            std::cout << "Conductance " << 1/resValue << " Ohms, nodes: "
                      << nodeStr1 << "-" << nodeStr2 << std::endl;

            // Apply stamp

            if (node1 > 0) {
                systemMNA.addto(node1-1, node1-1, 1/resValue);
                if (node2 > 0) {
                    systemMNA.addto(node1-1, node2-1, -1/resValue);
                }
            }
            if (node2 > 0) {
                systemMNA.addto(node2-1, node2-1, 1/resValue);
                if (node1 > 0) {
                    systemMNA.addto(node2-1, node1-1, -1/resValue);
                }
            }

            indRes++;
        } else if (elem.elemType == STAT_VOLTAGESOURCE) {
            std::string nodeStr1 = elem.nodeList[0],
                        nodeStr2 = elem.nodeList[1];
            unsigned int node1 = par.nodeList->mapStringNode[nodeStr1],
                         node2 = par.nodeList->mapStringNode[nodeStr2];
            double voltValue = elem.valueList[0];

            std::cout << "Voltage Source " << voltValue << " V, nodes: "
                      << nodeStr1 << "-" << nodeStr2 << std::endl;

            if (node1 > 0) {
                systemMNA.set(numNodes-1 + indVolt, node1-1, 1);
                systemMNA.set(node1-1, numNodes-1 + indVolt, 1);
            }
            if (node2 > 0) {
                systemMNA.set(numNodes-1 + indVolt, node2-1, -1);
                systemMNA.set(node2-1, numNodes-1 + indVolt, -1);
            }
            systemExcitation[numNodes-1 + indVolt] = voltValue;
            indVolt++;
        } else {
            std::cerr << "Unknown element" << std::endl;
            exit(-1);
        }
    }

    systemMNA.disp();

    std::cout << std::endl;
    for (unsigned indDoF = 0; indDoF < numDoFs; indDoF++) {
        std::cout << systemExcitation[indDoF] << " ";
    }
    std::cout << std::endl;

    Matrix L(numDoFs, numDoFs);
    Matrix U(numDoFs, numDoFs);
    Matrix P(numDoFs, numDoFs);
    double * sol;
    systemMNA.LU(L, U, P);
    sol = systemMNA.LU_solve(L, U, P, systemExcitation);

    for (unsigned indDoF = 0; indDoF < numDoFs; indDoF++) {
        std::cout << sol[indDoF] << " ";
    }
    std::cout << std::endl;

    delete [] systemExcitation;
    delete [] sol;
}

#endif
