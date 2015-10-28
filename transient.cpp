#include "transient.h"

Transient::Transient(Parser *_parser, double _dt, double _t2, double _t1, double _theta) {
    dt = _dt;
    t1 = _t1;
    t2 = _t2;
    theta = _theta;
    parser = _parser;

    // Construct new element list by replacing energy storage elements with
    // Norton companion models.
    unsigned int indNew = 0;
    std::vector <Element> elements;
    std::vector <unsigned int> currentInds;
    std::vector <double> propConstVolt, propConstCur;

    for (unsigned int indElem = 0; indElem < parser->elemList->elements.size(); indElem++) {
        Element elem = parser->elemList->elements[indElem];

        std::vector <int> modelList;
        if (elem.elemType == STAT_CAPACITANCE) {
            // Norton companion model for the capacitance is parallel combination
            // of a conductance G=C/dt and current source with J=C/h times voltage
            // from previous time step.

            assert(elem.valueList.size() >= 1);
            assert(elem.nodeList.size() >= 2);
            double capValue = elem.valueList[0];

            std::stringstream ssCurrent, ssRes;
            ssRes << "R_dummy_" << elem.name << " "
                  << elem.nodeList[0] << " " << elem.nodeList[1] << " " << dt/capValue;
            ssCurrent << "I_dummy_" << elem.name << " "
                      << elem.nodeList[0] << " " << elem.nodeList[1] << " "
                      << capValue*0/dt;
            std::string strCurrent = ssCurrent.str(),
                        strRes     = ssRes.str();

            Element elemRes(strRes), elemCur(strCurrent);
            elements.push_back(elemRes);
            modelList.push_back(indNew);
            indNew++;
            elements.push_back(elemCur);
            modelList.push_back(indNew);
            currentInds.push_back(indNew);
            indNew++;

            propConstVolt.push_back(capValue/dt);
            propConstCur.push_back(0);

        } else if (elem.elemType == STAT_INDUCTANCE) {
            // Norton companion model for the inductance is the parallel combination
            // of a conductance G=dt*theta/L and current source
            // J = -I(prev) - (dt/L)*(1-theta)*V(prev).

            assert(elem.valueList.size() >= 1);
            double indValue = elem.valueList[0];

            std::stringstream ssCurrent, ssRes;
            ssRes << "R_dummy_" << elem.name << " "
                  << elem.nodeList[0] << " " << elem.nodeList[1] << " " << indValue/(dt*theta);
            ssCurrent << "I_dummy_" << elem.name << " "
                      << elem.nodeList[0] << " " << elem.nodeList[1] << " "
                      << -(1-theta)*0*dt/indValue;

            std::string strCurrent = ssCurrent.str(),
                        strRes     = ssRes.str();

            // With Forward Euler, theta = 0 and conductance becomes zero.
            if (theta != 0) {
                Element elemRes(strRes);
                elements.push_back(elemRes);
                modelList.push_back(indNew);
                indNew++;
            }
            Element elemCur(strCurrent);
            elements.push_back(elemCur);
            modelList.push_back(indNew);
            currentInds.push_back(indNew);
            indNew++;

            // Current from previous time step included in the source current is the sum
            // of currents through both branches in the companion model. The part of the
            // current due to the conductance is included in the proportionality constant
            // for the voltage. The proportionality constant due to current corresponds
            // to the part of current due to the current source.

            propConstVolt.push_back(-(1-theta)*dt/indValue - (dt*theta)/indValue);
            propConstCur.push_back(-1);
        } else {
            elements.push_back(elem);
            modelList.push_back(indNew);
            indNew++;
        }
        companionInd.push_back(modelList);
    }

    elemList = new ElementList(elements);

    std::vector <double> voltages(elements.size(), 0);

    for (double t=t1; t < t2; t+=dt) {
        // Assemble the MNA equations for the modified circuit.
        Assembly ass(parser->nodeList, elemList, false);

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

        unsigned int numNodes = parser->nodeList->numNodes;
        ass.postProc(sol);
        ass.disp();

        // Update current source values in Norton equivalents for energy
        // storage elements.
        delete [] sol;
        for (unsigned int indElem = 0; indElem < currentInds.size(); indElem++) {
            unsigned int elemInd = currentInds[indElem];

            elemList->elements[elemInd].valueList[0] = -propConstVolt[indElem] * ass.voltageRe[elemInd]
                                                       -propConstCur[indElem] * ass.currentRe[elemInd];
            std::cout << "UPDATE " << elemList->elements[elemInd].name << "->" <<
                         elemList->elements[elemInd].valueList[0] << " (" <<
                      ass.voltageRe[elemInd] << ")"
                      << propConstVolt[indElem] << std::endl;
        }
    }
}

Transient::~Transient() {

}

#ifdef TEST_TRANSIENT

int
main(int argc, char **argv) {
    std::string fileName;

    if (argc < 2) {
        fileName = "test2.cir";
    } else {
        fileName = argv[1];
    }

    cirFile cir(fileName);
    std::cout << std::endl << "Transient Analysis of linear circuit: \""
              << cir.title << "\"" << std::endl;
    Parser parser(cir.statList);
    Transient tran(&parser, 0.0001, 0.1, 0, 0.5);
    tran.elemList->disp();
}


#endif
