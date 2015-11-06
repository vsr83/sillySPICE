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

#ifndef CIRFILE_H
#define CIRFILE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <list>

#include "statements.h"

/* cirStatement class implements the parsing of invidual lines in .cir-files
 * into SPICE statements with a type and a class. Types and classes are listed
 * in statements.h. Since SPICE files are case-insensitive, all input strings
 * s are converted into upper case by toUpper(s). Parameters to the statements
 * obtained with strSplit are assembled into strList.
 */

class cirStatement {
public:
    cirStatement(const cirStatement &_stat);
    cirStatement(std::string &rawString);
    ~cirStatement();

    unsigned int type, statClass;
    std::vector <std::string>               strList, bracketNames;
    std::vector <std::vector<std::string> > bracketList;

    void disp() const;
private:
//    std::vector <std::string> strSplit (const std::string &s, char delim);
//    void extractBrackets(const std::string &s);
};

/* cirFile object reads contents of a .cir-file into a vector of SPICE
 * statements. Multi-line statements are automatically combined into single
 * statements.
 */

class cirFile {
public:
    cirFile(const std::string &fileName);
    ~cirFile();

    void disp();
    std::list   <std::string>               lineList;
    std::vector <cirStatement> statList;
    std::string fileName;

    std::string title;
private:
    std::string toUpper(const std::string &s);
};

#endif // CIRFILE_H
