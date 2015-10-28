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

#ifndef PARSER_H
#define PARSER_H

#include "cirFile.h"
#include "element.h"
#include "nodeList.h"
#include "value.h"
#include "statements.h"
#include "topology.h"

#include <stdlib.h>

/* Parser objects parse a list of SPICE statements into the objects necessary
 * for description of nodes, elements, analysis and post-processing. The list
 * of statements used as a parameter to the constructor is typically obtained
 * from a .cir-file with a cirFile object. */

class Parser {
public:
    Parser(std::vector<cirStatement> &statList);
    ~Parser();

    NodeList *nodeList;
    ElementList *elemList;

    // Topology object is used to check whether the circuit has floating
    // nodes.
    Topology *topology;
    std::list<Link> spanningTree;

    // Analysis parameters:
    enum{ANALYSIS_EMPTY, ANALYSIS_DC, ANALYSIS_AC, ANALYSIS_TRAN, ANALYSIS_TF,
         ANALYSIS_OP};
    unsigned int analysisType;

    enum{ANALYSIS_DC_LIN, ANALYSIS_DC_DEC, ANALYSIS_DC_OCT};
    unsigned int analysisTypeDC, analysisDCnpoints;
    std::string  analysisDCvar;
    double analysisDCstartValue, analysisDCendValue, analysisDCinc;

    enum{ANALYSIS_AC_LIN, ANALYSIS_AC_DEC, ANALYSIS_AC_OCT};
    unsigned int analysisTypeAC;
    unsigned int analysisACnpoints, analysisACstartFreq, analysisACendFreq;

private:
    // Circuit elements.
    std::vector <Element> elements;
    std::set <std::string> nodeSet;
};

#endif // PARSER_H
