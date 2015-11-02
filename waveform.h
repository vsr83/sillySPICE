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

#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "element.h"

class Waveform {
public:
    enum{WAVEFORM_SIN,
         WAVEFORM_DC,
         WAVEFORM_EXP,
         WAVEFORM_PWL,
         WAVEFORM_SFFM,
         WAVEFORM_AM,
         WAVEFORM_PULSE,
         WAVEFORM_NOISE};

    Waveform(std::vector <std::string> &bracket, std::string &bracketName);
    ~Waveform();

    double eval(double t);
    void setTransientParameters(double timestep, double t1, double t2);

    double sinV0, sinVA, sinFREQ, sinTD, sinTHETA;
    bool sinFREQset;

    double pulseV1, pulseV2, pulseTD, pulseTR, pulsePW, pulsePER;

    double expV1, expV2, expTD1, expTAU1, expTD2, expTAU2;
    bool expTAU1set, expTD2set, expTAU2set;

    std::vector <double> PWLtime, PWLampl;

    double SFFMV0, SFFMVA, SFFMFC, SFFMMDI, SFFMFS;

    double AMVA, AMVO, AMMF, AMFC, AMTD;

    double noiseNA, noiseNT, noiseNALPHA, noiseNAMP, noiseRTSAM, noiseRTSCAPT, noiseRTSEMT;

    unsigned int mode;
private:
    std::vector <std::string> strList;
    std::string name;
};

#endif // WAVEFORM_H
