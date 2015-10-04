#ifndef CIRFILE_H
#define CIRFILE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "statements.h"

class cirStatement {
public:
    cirStatement(const cirStatement &_stat);
    cirStatement(std::string &rawString);
    ~cirStatement();

    unsigned int type, statClass;
    std::vector <std::string> strList;
private:
    std::vector <std::string> strSplit (const std::string &s, char delim);
    std::string toUpper(const std::string &s);
};

class cirFile {
public:
    cirFile(const std::string &fileName);
    ~cirFile();

    void disp();
    std::vector <std::string>  strList;
    std::vector <cirStatement> statList;
    std::string fileName;

    std::string title;
private:
};

#endif // CIRFILE_H
