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

/* In SPICE statements, suffixes MEG, MIL, F, P, N, U, M, K, G, T can be
 * attached to any value. These suffixes are case-insensitive and can be
 * extended without any effect to the value (e.g. 1M ~1m ~ 1mF ~ 1mFarads).
 *
 * This class implements the conversion of such value statements into
 * numeric form (std::string -> double).
 */

#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>

class Value {
public:
    Value(std::string str);
    ~Value();

    double val;
};

#endif // VALUE_H
