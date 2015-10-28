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

#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "parser.h"
#include "cirFile.h"
#include "element.h"
#include "nodeList.h"
#include "value.h"
#include "statements.h"
#include "topology.h"

#include "matrix.h"

/*
 * This class implements the assembly of the system matrix and excitation
 * vectors for DC and AC analysis.
 *
 * Modified Nodal Analysis (MNA) is used to formulate the problem :
 * Nodal Analysis (NA) equations are assembled for the non-datum nodes in
 * the form [G]V = J, where [G] is the conductance matrix. Independent and
 * controlled sources introduce additional DoFs and corresponding equations.
 *
 * Note that transient and non-linear solution procedures can be implemented
 * with companion models so that the solution process reduces to the solution
 * of a sequence of DC circuits. Thus, this class implements the basic
 * building block used in all solution processes.
 *
 * In this class, the circuits are assumed to contain only real- or complex-
 * valued conductances, controlled sources and independent sources.
 */

class Assembly {
public:
    Assembly(NodeList *_nodeList, ElementList *_elemList, bool _complex = false);
    ~Assembly();
    double * solve();

    bool complex;              // Are the DoFs complex?
    unsigned int numDoF;
    Matrix *systemMNA;         // The system matrix.
    double *systemExcitation;  // The excitation vector.

    std::map <std::string, unsigned int> sourceDoFmap;

    // The system matrix systemMNA is extracted from the full (with the ground
    // node) matrix fullMNA by removal of the first row and column. By
    // construction of the matrix fullMNA first, the construction process
    // becomes simpler.

    // fullMNA is singular whereas systemMNA is not. Note also that since
    // potential of the ground node is zero, the first column of fullMNA does
    // not contribute to the matrix product.

    Matrix *fullMNA;
    double *fullExcitation;
//    Parser *parser;            // Parser constructed outside.
    NodeList *nodeList;
    ElementList *elemList;

    std::vector <double> currentRe, currentIm,
                         voltageRe, voltageIm;
    void postProc(double *sol);
    void disp();

private:
    void buildReal();
    void buildComplex();

    unsigned int numNodes;
};

#endif // ASSEMBLY_H
