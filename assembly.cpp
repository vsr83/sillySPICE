#include "assembly.h"

Assembly::Assembly(Parser *_parser) {
    parser = _parser;
    systemMNA = 0;
    systemExcitation = 0;

    // Numbers of all permissible elements.
    unsigned int numRes  = parser->elemTypeCount[STAT_RESISTANCE],
                 numVolt = parser->elemTypeCount[STAT_VOLTAGESOURCE],
                 numCur  = parser->elemTypeCount[STAT_CURRENTSOURCE],
                 numVCVS = parser->elemTypeCount[STAT_VCVS],
                 numCCVS = parser->elemTypeCount[STAT_CCVS];

    unsigned int numNodes = parser->nodeList->numNodes;

    numDoF = numNodes - 1 + numVolt + numCur + numVCVS + numCCVS*2 ;

    // When the degrees of freedom are complex-valued, the real and imaginary
    // parts of the DoF are included as separate degrees of freedom in the
    // system matrix and excitation vectors.

    double *systemExcitation;
    if (parser->analysisType == Parser::ANALYSIS_AC) {
        complex = true;

        fullMNA = new Matrix((numDoF+1)*2, (numDoF+1)*2);
        fullExcitation = new double[(numDoF+1)*2];

        for (unsigned indDoF = 0; indDoF < (numDoF+1)*2; indDoF++) {
           fullExcitation[indDoF] = 0;
        }
    } else {
        complex = false;

        fullMNA = new Matrix(numDoF+1, numDoF+1);
        fullExcitation = new double[numNodes+1];

        for (unsigned indDoF = 0; indDoF < numNodes; indDoF++) {
           fullExcitation[indDoF] = 0;
        }
    }

    // indSource is used as the index for additional DoFs due to sources in
    // the MNA formulation.

    unsigned int indRes= 0, indSource = 0;

    for (unsigned int indElem = 0; indElem < parser->elements.size(); indElem++) {
        Element elem = parser->elements[indElem];

        std::string nodeStr1 = elem.nodeList[0],
                    nodeStr2 = elem.nodeList[1];
        unsigned int node1 = parser->nodeList->mapStringNode[nodeStr1],
                     node2 = parser->nodeList->mapStringNode[nodeStr2];

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
            fullMNA->set(node1, numNodes + indSource,  1);
            fullMNA->set(node2, numNodes + indSource, -1);
            indSource++;
        } break;
        case STAT_CURRENTSOURCE: {
            double currValue = elem.valueList[0];
            fullExcitation[node1] += currValue;
            fullExcitation[node2] -= currValue;
        } break;
        case STAT_VCVS: {

        } break;
        default:
            std::cerr << "ASSEMBLY : Unknown Element Type!" << std::endl;
            exit(-1);
            break;
        }
    }
    systemMNA = fullMNA->submatrix(1, numDoF, 1, numDoF);
}

Assembly::~Assembly() {
    delete systemMNA;
    delete [] systemExcitation;
    systemMNA = 0;
    systemExcitation = 0;
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
    Parser parser(cir.statList);
    Assembly ass(&parser);

    ass.fullMNA->disp();
    ass.systemMNA->disp();


    std::cout << std::endl << "DC Analysis of resistive circuit: \""
              << cir.title << "\"" << std::endl;

    unsigned int numNodes = parser.nodeList->numNodes;
}

#endif
