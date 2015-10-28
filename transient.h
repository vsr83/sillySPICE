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

#ifndef TRANSIENT_H
#define TRANSIENT_H

#include "parser.h"
#include "cirFile.h"
#include "element.h"
#include "nodeList.h"
#include "value.h"
#include "statements.h"
#include "topology.h"
#include "assembly.h"

#include "matrix.h"

/* Transient object performs transient solution of a linear circuit by
 * numerical integration with theta methods. Numerical integration is
 * implemented by replacing the energy storage elements with Norton companion
 * models updated each time step.
 *
 * t1     Initial time
 * t2     End time
 * dt     Time step size
 * theta  Theta parameter (0 ~ Forward Euler, 0.5 ~ Trapezoidal, 1 ~ Backward
 *                         Euler)
 */

class Transient {
public:
    Transient(Parser *_parser, double _dt, double _t2, double _t1=0, double _theta=1);
    ~Transient();

    double dt, t1, t2, theta;

    // The element list, where energy storage elements have been replaced.
    ElementList *elemList;
private:
    Parser      *parser;

    // Indices of the elements of the companion models in the element list elemList.
    std::vector <std::vector <int> > companionInd;
};

#endif // TRANSIENT_H
