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

    numNodes = parser->nodeList->numNodes;
    numDoF = numNodes - 1 + numVolt + numVCVS + numCCVS*2 ;

    // When the degrees of freedom are complex-valued, the real and imaginary
    // parts of the DoF are included as separate degrees of freedom in the
    // system matrix and excitation vectors.

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
            // Current sources are "natural" for nodal analysis and thus do
            // not introduce additional degrees of freedom.

            double currValue = elem.valueList[0];
            fullExcitation[node1] += currValue;
            fullExcitation[node2] -= currValue;
        } break;
        case STAT_VCVS: {

        } break;
        case STAT_CCCS: {

        } break;
        case STAT_VCCS: {

        } break;
        case STAT_CCVS: {

        }
        default:
            std::cerr << "ASSEMBLY : Unknown Element Type!" << std::endl;
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
    Assembly ass(&parser);

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
}

#endif
