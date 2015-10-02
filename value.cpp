#include "value.h"
#include <assert.h>
#include <cctype>
#include <stdlib.h>

Value::Value(std::string str) {
    unsigned len = str.length();

    std::string strNum, strSuffix;

    assert(len > 0);
    bool modenum = true;
    for (unsigned int indStr = 0; indStr < len; indStr++) {
        char c = str[indStr];

        if (!isdigit(c) && c!='-' && c!='.' && c!='+') {
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
