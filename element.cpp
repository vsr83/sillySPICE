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

// Elements can have more than two nodes, whereas branches in a graph have
// two nodes.

#include "element.h"
#include <stdlib.h>

Element::Element() {
    elemType = STAT_EMPTY;
    elemClass = CLASS_EMPTY;
}

Element::Element(const cirStatement &stat) {
    elemType  = stat.type;
    elemClass = stat.statClass;
    name  = stat.strList[0];

    std::vector <std::string> strList = stat.strList;

    assert(elemClass == CLASS_SOURCE || elemClass == CLASS_PASSIVE
           || elemClass == CLASS_NONLINEAR);

    switch (elemClass) {
    case CLASS_PASSIVE:
    {
        assert(strList.size() == 4);

        nodeList.push_back(strList[1]);
        nodeList.push_back(strList[2]);
        Value val(strList[3]);
        valueList.push_back(val.val);
        assert(val.val != 0);
    }
        break;
    case CLASS_SOURCE:
    {
        switch (elemType) {
        case STAT_VOLTAGESOURCE: {
            assert(strList.size() > 2);

            nodeList.push_back(strList[1]);
            nodeList.push_back(strList[2]);

            if (strList.size() == 4) {
                Value val(strList[3]);
                valueList.push_back(val.val);
                typeList.push_back("DC");
            } else if (strList.size() == 5){
                typeList.push_back(strList[3]);
                Value val(strList[4]);
                valueList.push_back(val.val);
            } else if (strList.size() == 3) {
                std::cout << stat.bracketList.size() << std::endl;
                if (stat.bracketList.size() == 1) {
                    assert(stat.bracketNames.size() == 1);

                    std::string bracketName = stat.bracketNames[0];
                    std::vector <std::string> bracket = stat.bracketList[0];

                    typeList.push_back(bracketName);

                    processWaveform(bracket, bracketName);
                } else {
                    std::cerr << "Incorrect number of parameters for voltage source" << std::endl;
                    exit(-1);
                }
            } else {
                std::cerr << "Incorrect number of arguments for voltage source!" << std::endl;
                exit(-1);
            }
        } break;
        case STAT_CURRENTSOURCE: {
            assert(strList.size() == 4 || strList.size() == 5);

            nodeList.push_back(strList[1]);
            nodeList.push_back(strList[2]);

            if (strList.size() == 4) {
                Value val(strList[3]);
                valueList.push_back(val.val);
            } else {
                typeList.push_back(strList[3]);
                Value val(strList[4]);
                valueList.push_back(val.val);
            }
        } break;
        case STAT_CCCS: {
            assert(strList.size() == 5);
            nodeList.push_back(strList[1]); // +n
            nodeList.push_back(strList[2]); // -n
            elemList.push_back(strList[3]); // ref elem
            Value val(strList[4]);
            valueList.push_back(val.val);   // gain
        } break;
        case STAT_CCVS: {
            assert(strList.size() == 5);
            nodeList.push_back(strList[1]); // +n
            nodeList.push_back(strList[2]); // -n
            elemList.push_back(strList[3]); // ref elem
            Value val(strList[4]);
            valueList.push_back(val.val);   // gain
        } break;
        case STAT_VCVS: {
            assert(strList.size() == 6);
            nodeList.push_back(strList[1]); // +n
            nodeList.push_back(strList[2]); // -n
            nodeList.push_back(strList[3]); // +c
            nodeList.push_back(strList[4]); // -c
            Value val(strList[5]);
            valueList.push_back(val.val);   // gain
        } break;
        case STAT_VCCS: {
            assert(strList.size() == 6);
            nodeList.push_back(strList[1]); // +n
            nodeList.push_back(strList[2]); // -n
            nodeList.push_back(strList[3]); // +c
            nodeList.push_back(strList[4]); // -c
            Value val(strList[5]);
            valueList.push_back(val.val);   // gain
        } break;
        default:
            break;
        }
    }
        break;
    case CLASS_NONLINEAR:
        break;
    }
}

Element::~Element() {
}

void
Element::processWaveform(std::vector<std::string> &bracket, std::string &bracketName) {
    if (bracketName == "SIN") {
        // SIN(VO VA FREQ TD THETA) Offset, Amplitude, Frequency, Delay, Damping factor
        if (bracket.size() < 2) {
            std::cerr << "SIN() requires 2 parameters" << std::endl;
            exit(-1);
        }
    } else if (bracketName == "EXP") {
        // EXP(V1, V2, TD1, TAU1, TD2, TAU2)
        // V1    Initial value,
        // V2    Pulsed value,
        // TD1   Rise delay time,
        // TAU1  Rise time constant,
        // TD2   Fall delay time,
        // TAU2  Fall time constant.
        if (bracket.size() < 2) {
            std::cerr << "EXP() requires 2 parameters" << std::endl;
            exit(-1);
        }

    } else {
        std::cerr << "Unknown transient source : " << bracketName << std::endl;
        exit(-1);
    }

    for (unsigned int indVal = 0; indVal < bracket.size(); indVal++) {
        Value val(bracket[indVal]);
        valueList.push_back(val.val);
    }
}

ElementList::ElementList(std::vector <Element> &_elements) {
    elements = _elements;

    // Create a node list and count the number of elements with each type.
    for (unsigned int elemInd = 0; elemInd < elements.size(); elemInd++) {
        Element elem = elements[elemInd];

        if (typeCount.find(elem.elemType) == typeCount.end()) {
            typeCount[elem.elemType] = 1;
        } else {
            typeCount[elem.elemType]++;
        }

        mapNameElem[elem.name] = elemInd;
    }
}

void
ElementList::disp() {
    std::cout << std::endl << "Element List:" << std::endl;
    // Create a node list and count the number of elements with each type.
    for (unsigned int elemInd = 0; elemInd < elements.size(); elemInd++) {
        Element elem = elements[elemInd];

        std::cout << "Element " << elemInd << " \"" << elem.name << "\""<< " :";
        for (unsigned int nodeInd = 0; nodeInd < elem.nodeList.size(); nodeInd++) {
            std::cout << "\"" << elem.nodeList[nodeInd] << "\", ";
        }
        std::cout << typeCount[elem.elemType];
        std::cout << std::endl;
    }

    std::cout << std::endl << "Element Mapping:" << std::endl;
    // Element list:
    for (std::map<std::string, unsigned int>::iterator it=mapNameElem.begin();
         it != mapNameElem.end(); ++it) {
        std::string  name    = it->first;
        unsigned int elemInd = it->second;
        std::cout << "  " << name << "->" << elemInd << std::endl;

    }
}

ElementList::~ElementList() {

}
