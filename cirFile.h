#ifndef CIRFILE_H
#define CIRFILE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class cirStatement {
public:
    cirStatement(const cirStatement &_stat);
    cirStatement(std::string &rawString);
    ~cirStatement();

    unsigned int type, statClass;
    std::vector <std::string> strList;

    enum {CLASS_EMPTY,
          CLASS_COMMENT,
          CLASS_SUBCKT,
          CLASS_PASSIVE,
          CLASS_SOURCE,
          CLASS_NONLINEAR,
          CLASS_META};
    enum {STAT_EMPTY, STAT_COMMENT, STAT_CONTLINE,
          STAT_RESISTANCE, STAT_INDUCTANCE, STAT_MUTUALIND, STAT_CAPACITANCE,
          STAT_VOLTAGESOURCE, STAT_CURRENTSOURCE,
          STAT_DIODE, STAT_BJT, STAT_MOSFET,
          STAT_ANALYSIS, STAT_MODEL, STAT_END, STAT_SUBCKT, STAT_ENDS};
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
