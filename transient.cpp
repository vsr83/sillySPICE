#include "transient.h"

Transient::Transient(Parser *_parser, double _dt, double _t2, double _t1, double _theta) {
    dt = _dt;
    t1 = _t1;
    t2 = _t2;
    theta = _theta;
    parser = _parser;

    // Construct new element list by replacing energy storage elements with
    // Norton equivalents.
    unsigned int indNew = 0;
    std::vector <Element> elements;

    for (unsigned int indElem = 0; indElem < parser->elemList->elements.size(); indElem++) {
        Element elem = parser->elemList->elements[indElem];

        std::vector <int> modelList;
        if (elem.elemType == STAT_CAPACITANCE) {
            assert(elem.valueList.size() >= 1);
            assert(elem.nodeList.size() >= 2);
            double capValue = elem.valueList[0];

            std::stringstream ssCurrent, ssRes;
            ssRes << "Rd" << elem.name << " "
                  << elem.nodeList[0] << " " << elem.nodeList[1] << " " << dt/capValue;
            ssCurrent << "Id" << elem.name << " "
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
            indNew++;
        } else if (elem.elemType == STAT_INDUCTANCE) {
            assert(elem.valueList.size() >= 1);
            double indValue = elem.valueList[0];

            std::stringstream ssCurrent, ssRes;
            ssRes << "Rd" << elem.name << " "
                  << elem.nodeList[0] << " " << elem.nodeList[1] << " " << indValue/(dt*theta);
            ssCurrent << "Id" << elem.name << " "
                      << elem.nodeList[0] << " " << elem.nodeList[1] << " "
                      << -(1-theta)*0*dt/indValue;

            std::string strCurrent = ssCurrent.str(),
                        strRes     = ssRes.str();

            Element elemRes(strRes), elemCur(strCurrent);
            elements.push_back(elemRes);
            modelList.push_back(indNew);
            indNew++;
            elements.push_back(elemCur);
            modelList.push_back(indNew);
            indNew++;
        } else {
            elements.push_back(elem);
            modelList.push_back(indNew);
            indNew++;
        }
        companionInd.push_back(modelList);
    }


    std::vector <double> voltages;

    for (double t=t1; t < t2; t+=dt) {
        // Update current source values in Norton equivalents for energy
        // storage elements.
        if (t == t1) {

        } else {

        }
        // Assemble the MNA equations for the modified circuit.

        // Extract required currents and voltages required by the companion
        // models.
    }
}

Transient::~Transient() {

}
