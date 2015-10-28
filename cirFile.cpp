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

#include "cirFile.h"
#include <stdlib.h>
#include <cctype>
#include <cassert>

std::vector<std::string>
cirStatement::strSplit(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;

  std::vector <std::string> elems;

  while (std::getline(ss, item, delim)) {
    if (item.length() > 0) {
        elems.push_back(toUpper(item));
    }
  }
  return elems;
}

std::string
cirStatement::toUpper(const std::string &s) {
    std::string str(s);
    for (unsigned int ind = 0; ind < str.length(); ind++) {
        str[ind] = std::toupper(str[ind]);
    }
    return str;
}

cirStatement::cirStatement(std::string &rawString) {
    std::vector <std::string> tmpList;

    // Normal SPICE statements can contain comments separated with the symbol
    // ';'. The following code removes the comment from the statement.
    tmpList = strSplit(rawString, ';');
    if (tmpList.size() > 0) {
        strList = strSplit(tmpList[0], ' ');
    } else {
        strList = tmpList;
    }

    // The remaining code in this function determines the type of the SPICE
    // statement.
    statClass = CLASS_EMPTY;
    if (strList.size() == 0) {
        type = STAT_EMPTY;
    } else {
        std::string first = strList[0];
        assert(first.length() > 0);

        bool found = false;

        for (unsigned int statType = 0; statType < numStatements; statType++) {
            if (statements[statType].singleChar && first[0] == statements[statType].statStr[0]) {
                statClass = statements[statType].classNum;
                type      = statements[statType].statNum;
                found = true;
                break;
            } else if (!statements[statType].singleChar && first == statements[statType].statStr) {
                statClass = statements[statType].classNum;
                type      = statements[statType].statNum;
                found = true;
                break;
            }
        }
        if (!found) {
            std::cerr << "Unknown statement type \"" << first << "\"!" << std::endl;
            exit(-1);
        }
        if (!statements[type].implemented) {
            std::cerr << "Statement \"" << statements[type].statStr << "\" not implemented!" << std::endl;
            exit(-1);
        }

    }
    std::cout << strList.size() << " - ";
    for (unsigned int indStr = 0; indStr < strList.size(); indStr++) {
        std::cout << "/" << strList[indStr];
    }
    std::cout << "/" << std::endl;
}

cirStatement::cirStatement(const cirStatement &_stat) {
    strList = _stat.strList;
    type = _stat.type;
    statClass = _stat.statClass;
}

cirStatement::~cirStatement() {
}

cirFile::cirFile(const std::string &_fileName) {
    fileName = std::string(_fileName);
    std::ifstream inputFile(fileName.c_str(), std::ios_base::in);

    std::string line;
    strList.clear();
    while(getline(inputFile, line, '\n')) {
        strList.push_back(line);
    }

    std::cout << "File  :\"" << fileName << "\" with " << strList.size() << " lines" << std::endl;
    if (strList.size() == 0) {
        std::cerr << "Input file empty." << std::endl;
        exit(-1);
    }
    if (strList.size() == 1) {
        std::cerr << "Input file contains only the title." << std::endl;
        exit(-1);
    }
    title = strList[0];
    std::cout << "Title :\"" << title << "\"" << std::endl;

    // Assemble statements into a vector.
    for (unsigned int elemInd = 1; elemInd < strList.size(); elemInd++) {
        std::cout << elemInd << ":";
        cirStatement stat(strList[elemInd]);

        // If statement is continued into the new line with the symbol '+',
        // combine previous statement with the new statement. Empty and comment
        // statements are not added the vector.
        if (stat.type == STAT_CONTLINE) {
            cirStatement prev = statList.back();
            statList.pop_back();

            for (unsigned int indStr = 1; indStr < stat.strList.size(); indStr++) {
                prev.strList.push_back(stat.strList[indStr]);
            }
            statList.push_back(prev);
        } else if (stat.type != STAT_EMPTY && stat.type != STAT_COMMENT) {
            statList.push_back(stat);
        }
    }    
}

cirFile::~cirFile() {
}

void
cirFile::disp() {
    for (unsigned int indList = 0; indList < statList.size(); indList++) {
        cirStatement stat = statList[indList];

        std::cout << std::endl;
        std::cout << statements[stat.type].name << std::endl;
        std::cout << indList << " : " << stat.type << " /";
        for (unsigned int indStr = 0; indStr < stat.strList.size(); indStr++) {
            std::cout << stat.strList[indStr] << "/";
        }
        std::cout << std::endl;
    }
}

#ifdef CIRFILE_TEST

int
main(int argc, char **argv) {
    cirFile cir(std::string("test.cir"));
    cir.disp();
}

#endif
