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

#include "value.h"
#include <assert.h>
#include <cctype>
#include <stdlib.h>

Value::Value(std::string str) {
    unsigned len = str.length();

    // Each Value string consist of a numeric part strNum and a possible
    // suffix strSuffix. The start of strSuffix is the first character not
    // allowed in the strNum part.

    std::string strNum, strSuffix;

    assert(len > 0);
    bool modenum = true;
    for (unsigned int indStr = 0; indStr < len; indStr++) {
        char c = str[indStr];

        // Values with the form "1e-5" in strNum must be allowed.
        if (!isdigit(c) && c != 'E' && c!='-' && c!='.' && c!='+') {
            modenum = false;
        }
        if (modenum) {
            strNum.push_back(c);
        } else {
            strSuffix.push_back(std::toupper(c));
        }
    }
//    std::cout << "\"" << strNum << "\"" << "/\"" << strSuffix << "\"" << std::endl;
    val = atof(strNum.c_str());

    // Process suffix.
    if (strSuffix.length() == 0) {
    } else if (strSuffix.length() >= 2 &&
                strSuffix[0] == 'M' && strSuffix[1] == 'E' && strSuffix[2] == 'G') {
        val *= 1e6;
    } else if (strSuffix.length() >= 2 &&
                        strSuffix[0] == 'M' && strSuffix[1] == 'I' && strSuffix[2] == 'L') {
                 val *= 25.4e-6;
    } else if (strSuffix[0] == 'F') {
        val *= 1e-15;
    } else if (strSuffix[0] == 'P') {
        val *= 1e-12;
    } else if (strSuffix[0] == 'N') {
        val *= 1e-9;
    } else if (strSuffix[0] == 'U') {
        val *= 1e-6;
    } else if (strSuffix[0] == 'M') {
        val *= 1e-3;
    } else if (strSuffix[0] == 'K') {
        val *= 1e3;
    } else if (strSuffix[0] == 'G') {
        val *= 1e9;
    } else if (strSuffix[0] == 'T') {
        val *= 1e12;
    }
}

Value::~Value() {
}

#ifdef TEST_VALUE

int
main(int argc, char **argv) {
    Value val1(std::string("-1.43MEGF")),
          val2(std::string("1.1.1PC")),
          val3(std::string("-.1MEGA"));

    std::cout << val1.val << std::endl;
    std::cout << val2.val << std::endl;
    std::cout << val3.val << std::endl;
}
#endif
