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

    std::cout << strList.size() << " - ";
    for (unsigned int indStr = 0; indStr < strList.size(); indStr++) {
        std::cout << "/" << strList[indStr];
    }
    std::cout << "/" << std::endl;

    // The remaining code in this function determines the type of the SPICE
    // statement.

    if (strList.size() == 0) {
        type = STAT_EMPTY;
    } else {
        std::string first = strList[0];
        assert(first.length() > 0);
        char typeChar = first[0];

        switch (typeChar) {
        case '*':
            type = STAT_COMMENT;
            break;
        case '+':
            type = STAT_CONTLINE;
            break;
        case 'R':
            type = STAT_RESISTANCE;
            break;
        case 'L':
            type = STAT_INDUCTANCE;
            break;
        case 'C':
            type = STAT_CAPACITANCE;
            break;
        case 'V':
            type = STAT_VOLTAGESOURCE;
            break;
        case '.': {
            std::string metastr(first);
            metastr.erase(0, 1);
            std::cout << "  META : \"" << metastr << "\"" << std::endl;

            if (metastr == "DC" || metastr == "AC" || metastr == "TRAN" || metastr == "TF" ||
                metastr == "OP") {
                type = STAT_ANALYSIS;
                std::cout << "Analysis Type : " << "\"" << metastr << "\"" << std::endl;
            } else if (metastr == "MODEL") {
                type = STAT_MODEL;
            } else if (metastr == "END"){
                type = STAT_END;
            } else if (metastr == "SUBCKT") {
                type = STAT_SUBCKT;
            } else if (metastr == "ENDS") {
                type = STAT_ENDS;
            } else {
                std::cerr << "Invalid Meta Statement : " << "\"" << metastr << "\"" << std::endl;
                exit(-1);
            }
        }
            break;
        default:
            std::cerr << "Unknown Statement type '" << typeChar << "'"
                      << std::endl;
            exit(-1);
            break;
        }
    }
}

cirStatement::cirStatement(const cirStatement &_stat) {
    strList = _stat.strList;
    type = _stat.type;
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
        if (stat.type == stat.STAT_CONTLINE) {
            cirStatement prev = statList.back();
            statList.pop_back();

            for (unsigned int indStr = 1; indStr < stat.strList.size(); indStr++) {
                prev.strList.push_back(stat.strList[indStr]);
            }
            statList.push_back(prev);
        } else if (stat.type != stat.STAT_EMPTY && stat.type != stat.STAT_COMMENT) {
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
