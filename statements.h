#ifndef STATEMENT_H
#define STATEMENT_H

enum {CLASS_EMPTY,
      CLASS_COMMENT,
      CLASS_SUBCKT,
      CLASS_PASSIVE,
      CLASS_SOURCE,
      CLASS_NONLINEAR,
      CLASS_META,
      CLASS_MODEL,
      CLASS_ANALYSIS,
      CLASS_POSTPROC};

enum {STAT_EMPTY,
      STAT_COMMENT,       // '*'
      STAT_CONTLINE,      // '+'
      // Passive linear components:
      STAT_RESISTANCE,    // 'R' +n -n [model] {value}
      STAT_INDUCTANCE,    // 'L' +n -n [model] {value} [IC={initial}]
      STAT_MUTUALIND,     // 'K' L{name} {L{name}}* {coupling coeff}
      STAT_CAPACITANCE,   // 'C' +n -n [model] {value} [IC={initial}]
      STAT_TRANSMISLINE,  // 'T' A+ A- B+ B- Z0=val [TD=val | F=val [NL=val]]
      // Independent sources:
      STAT_VOLTAGESOURCE, // 'V' +n -n [[DC] {value}] [AC {mag} [{phase}]]
      STAT_CURRENTSOURCE, // 'I' +n -n [[DC] {value}] [AC {mag} [{phase}]]
      // Controlled Sources:
      STAT_VCVS,          // 'E' +n -n +c -c gain
      STAT_CCCS,          // 'F' +n -n vsname gain
      STAT_VCCS,          // 'G' +n -n +c -c gain
      STAT_CCVS,          // 'H' +n -n vsname gain
                          //     +n -n POLY({val}) {vsname}* {coeff}*
      // Nonlinear Elements
      STAT_DIODE,         // 'D' +n -n model [area]
      STAT_BJT,           // 'Q' c b e [subs] model [area]
      STAT_MOSFET,        // 'M' d g s {sub} {mdl} [L=val] [W=val]
                          // + [AD=val] [AS=val] [PD=val] [PS=val] [NRD=val] [NRS=val]
      STAT_ANALYSISAC,    // .AC .DC .FOUR .IC .OP .TRAN .SENS
      STAT_ANALYSISDC,    // .AC .DC .FOUR .IC .OP .TRAN .SENS
      STAT_ANALYSISFOUR,  // .AC .DC .FOUR .IC .OP .TRAN .SENS
      STAT_ANALYSISIC,    // .AC .DC .FOUR .IC .OP .TRAN .SENS
      STAT_ANALYSISOP,    // .AC .DC .FOUR .IC .OP .TRAN .SENS
      STAT_ANALYSISTRAN,  // .AC .DC .FOUR .IC .OP .TRAN .SENS
      STAT_ANALYSISSENS,  // .AC .DC .FOUR .IC .OP .TRAN .SENS
      STAT_MODEL,         // .MODEL
      STAT_END,           // .END end file
      // Subcircuits:
      STAT_SUBCKT,        // .SUBCKT {name} {node}*
      STAT_ENDS,          //
      STAT_SUBCKTCALL,    // 'X' [node]* name
      // Post-Processing:
      STAT_NODESET,       //
      STAT_NOISE,         //
      STAT_PLOT,          //
      STAT_PRINT,         //
      STAT_PROBE          //
     };

struct {
    unsigned statNum, classNum;
    const char *statStr;
    bool singleChar, implemented;
    unsigned int minArg, maxArg;
    const char *name;
} static const statements[] = {
    STAT_EMPTY,          CLASS_EMPTY,     " ",          false, true,  0, 0,    "Empty Statement",
    STAT_COMMENT,        CLASS_META,      "*",          true,  true,  0, 1024, "Comment",
    STAT_CONTLINE,       CLASS_META,      "+",          true,  true,  0, 0,    "Continue Line",
    STAT_RESISTANCE,     CLASS_PASSIVE,   "R",          true,  true,  3, 4,    "Resistance",
    STAT_INDUCTANCE,     CLASS_PASSIVE,   "L",          true,  true,  3, 5,    "Inductance",
    STAT_MUTUALIND,      CLASS_PASSIVE,   "K",          true,  false, 3, 1024, "Mutual Inductance",
    STAT_CAPACITANCE,    CLASS_PASSIVE,   "C",          true,  true,  3, 5,    "Capacitance",
    STAT_TRANSMISLINE,   CLASS_PASSIVE,   "T",          true,  false, 5, 8,    "Transmission Line",
    STAT_VOLTAGESOURCE,  CLASS_SOURCE,    "V",          true,  true,  3, 5,    "Voltage Source",
    STAT_CURRENTSOURCE,  CLASS_SOURCE,    "I",          true,  true , 3, 5,    "Current Source",
    STAT_VCVS,           CLASS_SOURCE,    "E",          true,  true,  5, 5,    "Voltage-Controlled Voltage Source",
    STAT_CCCS,           CLASS_SOURCE,    "F",          true,  true,  4, 4,    "Current-Controlled Current Source",
    STAT_VCCS,           CLASS_SOURCE,    "G",          true,  true,  5, 5,    "Voltage-Controlled Current Source",
    STAT_CCVS,           CLASS_SOURCE,    "H",          true,  true,  4, 1024, "Current-Controlled Voltage Source",
    STAT_DIODE,          CLASS_NONLINEAR, "D",          true,  false, 3, 4,    "Diode",
    STAT_BJT,            CLASS_NONLINEAR, "Q",          true,  false, 4, 6,    "Bipolar Junction Transistor",
    STAT_MOSFET,         CLASS_NONLINEAR, "Q",          true,  false, 5, 13,   "MOSFET Transistor",
    STAT_ANALYSISAC,     CLASS_ANALYSIS,  ".AC",        false, false, 3, 4,    "AC Analysis",
    STAT_ANALYSISDC,     CLASS_ANALYSIS,  ".DC",        false, true,  4, 5,    "DC Analysis",
    STAT_ANALYSISFOUR,   CLASS_ANALYSIS,  ".FOUR",      false, false, 2, 1024, "Fourier Analysis",
    STAT_ANALYSISIC,     CLASS_ANALYSIS,  ".IC",        false, false, 1, 1024, "Initial Transient Condition Analysis",
    STAT_ANALYSISOP,     CLASS_ANALYSIS,  ".OP",        false, false, 0, 0,    "DC Operating Point Analysis",
    STAT_ANALYSISTRAN,   CLASS_ANALYSIS,  ".TRAN",      false, true,  2, 4,    "Transient Analysis",
    STAT_ANALYSISSENS,   CLASS_ANALYSIS,  ".SENS",      false, false, 1, 1024, "Sensitivity Analysis",
    STAT_MODEL,          CLASS_MODEL,     ".MODEL",     false, false, 2, 1024, "Model Statement",
    STAT_END,            CLASS_META,      ".END",       false, true,  0, 0,    "End-of-File Statement",
    STAT_SUBCKT,         CLASS_SUBCKT,    ".SUBCKT",    false, false, 2, 1024, "Start of Subcircuit",
    STAT_ENDS,           CLASS_SUBCKT,    ".ENDS",      false, false, 0, 0,    "End of Subcircuit",
    STAT_SUBCKTCALL,     CLASS_SUBCKT,    "X",          true,  false, 2, 1024, "Subcircuit Call",
    STAT_NODESET,        CLASS_ANALYSIS,  ".NODESET",   false, false, 1, 1024, "Initial Bias Point Guess",
    STAT_NOISE,          CLASS_ANALYSIS,  ".NOISE",     false, false, 2, 3,    "Noise Analysis",
    STAT_PLOT,           CLASS_POSTPROC,  ".PLOT",      false, false, 2, 1024, "Plot Output",
    STAT_PRINT,          CLASS_POSTPROC,  ".PRINT",     false, false, 2, 1024, "Print Output",
    STAT_PROBE,          CLASS_POSTPROC,  ".PROBE",     false, false, 0, 1024, "Save Output"
};

const unsigned int numStatements = STAT_PROBE;

#endif // STATEMENT_H
