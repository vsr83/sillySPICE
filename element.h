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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <iostream>
#include <vector>
#include <map>
#include <assert.h>

#include "cirFile.h"
#include "value.h"
#include "statements.h"

/* Element objects correspond to circuit elements or their models in the
 * circuits.
 *
 * elemType and elemClass are obtained directly from SPICE statement used as
 * parameters to the constructor.
 *
 * nodeList vector contains the list of nodes of the element. When the number
 * of nodes in a element is larger than two, the element is always replaced
 * with a model composed of two-node elements before assembly.
 *
 * elemList vector is used to contain references to other elements such as
 * with current-controlled sources.
 *
 * valueList vector is used to contain the values of the elements such as the
 * values resistances, capacitances or resistances.
 *
 * typeList vector is used to contain additional parameters such as the wave
 * shapes for sources.
 */

class Element {
public:
    Element();
    Element(const cirStatement &stat);
    ~Element();

    unsigned int elemType, elemClass;
    std::string name;
    std::vector <std::string> nodeList;
    std::vector <std::string> elemList;
    std::vector <std::string> typeList;
    std::vector <double> valueList;
private:
    void processWaveform(std::vector<std::string> &bracket, std::string &bracketName);
};

/* ElementList objects are vectors of Element objects with basic functionality
 * such as mapping between the names of the elements and their indices. The
 * typeCount map maps the type of element to the number of such elements in
 * the element list. */

class ElementList {
public:
    ElementList(std::vector <Element> &_elements);
    ~ElementList();
    void disp();

    std::vector <Element> elements;
    std::map <std::string, unsigned int> mapNameElem;
    std::map <unsigned int, unsigned int> typeCount;
};

#endif // ELEMENT_H
