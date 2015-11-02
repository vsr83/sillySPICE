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

#include "waveform.h"
#include <math.h>
#include <stdlib.h>

Waveform::Waveform(std::vector<std::string> &bracket, std::string &bracketName) {
    strList = bracket;
    name = bracketName;

    if (name == "SIN") {
        mode = WAVEFORM_SIN;

        if (strList.size() < 2) {
            std::cerr << "SIN() requires at least two parameters!" << std::endl;;
            exit(-1);
        }
        Value valV0(strList[0]); sinV0 = valV0.val;
        Value valVA(strList[1]); sinVA = valVA.val;

        if (strList.size() >= 3) {
            Value valFREQ(strList[2]); sinFREQ = valFREQ.val;
            sinFREQset = true;
        } else {
            sinFREQ    = 0;
            sinFREQset = false;
        }
        if (strList.size() >= 4) {
            Value valTD(strList[3]); sinTD = valTD.val;
        } else {
            sinTD = 0;
        }
        if (strList.size() >= 5) {
            Value valTHETA(strList[4]); sinTHETA = valTHETA.val;
        } else {
            sinTHETA = 0;
        }
    } else if (name == "DC") {
        mode = WAVEFORM_DC;
    } else if (name == "EXP") {
        mode = WAVEFORM_EXP;

        if (strList.size() < 2) {
            std::cerr << "EXP() requires at least two parameters!" << std::endl;;
            exit(-1);
        }
        Value valV1(strList[0]); expV1 = valV1.val;
        Value valV2(strList[1]); expV2 = valV2.val;

        if (strList.size() >= 3) {
            Value valTD1(strList[2]); expTD1 = valTD1.val;
        } else {
            expTD1 = 0;
        }
        if (strList.size() >= 4) {
            Value valTAU1(strList[3]); expTAU1 = valTAU1.val;
            expTAU1set = true;
        } else {
            expTAU1set = false;
        }
        if (strList.size() >= 5) {
            Value valTD2(strList[4]); expTD2 = valTD2.val;
            expTD2set = true;
        } else {
            expTD2set = false;
        }
        if (strList.size() >= 6) {
            Value valTAU2(strList[4]); expTAU2 = valTAU2.val;
            expTAU2set = true;
        } else {
            expTAU2set = false;
        }
    } else if (name == "PWL") {
        mode = WAVEFORM_PWL;

        if (strList.size() % 2 != 0) {
            std::cerr << "The number of parameters in PWL() must be even!" << std::endl;
            exit(-1);
        }

        PWLtime.reserve(strList.size()/2);
        PWLampl.reserve(strList.size()/2);
        double t0;

        for (unsigned int indVal = 0; indVal < strList.size(); indVal++) {
            Value val(strList[indVal]);
            if (indVal % 2 == 0) {
                PWLtime.push_back(val.val);

                if (indVal > 0) {
                    if (val.val <= t0) {
                        std::cerr << "Time values in PWL() data must be in increasing order!" << std::endl;
                        exit(-1);
                    }
                }
                t0 = val.val;
            } else {
                PWLampl.push_back(val.val);
            }
        }
    } else if (name == "SFFM") {
        mode = WAVEFORM_SFFM;
    } else if (name == "AM") {
        mode = WAVEFORM_AM;
    } else if (name == "PULSE") {
        mode = WAVEFORM_PULSE;
    } else if (name == "NOISE") {
        mode = WAVEFORM_NOISE;
    } else {
        std::cerr << "Unknown Waveform : " << name << std::endl;
        exit(-1);
    }
}

void
Waveform::setTransientParameters(double timestep, double t1, double t2) {
    switch(mode) {
    case WAVEFORM_SIN:
        if (!sinFREQset) {
            sinFREQ = 1/t2;
            sinFREQset = true;
        }
        break;
    case WAVEFORM_EXP:
        if (!expTAU1set) {
            expTAU1 = timestep;
            expTAU1set = true;
        }
        if (!expTD2set) {
            expTD2 = expTD1 + timestep;
            expTD2set = true;
        }
        if (!expTAU2set) {
            expTAU2 = timestep;
            expTAU2set = true;
        }
        break;
    }
}

double
Waveform::eval(double t) {
    switch (mode) {
    case WAVEFORM_SIN:
        if (t >= 0 && t < sinTD) {
            return sinV0;
        } else if (t >= sinTD) {
            return sinV0 + sinVA * exp(-(t - sinTD)*sinTHETA)
                                 * sin(2*M_PI*sinFREQ*(t-sinTD));
        } else {
            return 0;
        }
        break;
    case WAVEFORM_EXP:
        if (t >= 0 && t < expTD1) {
            return expV1;
        } else if (t >= expTD1 && t < expTD2) {
            return expV1 + (expV2 - expV1)*(1-exp(-(t- expTD1)/expTAU1));
        } else if (t >= expTD2) {
            return expV1 + (expV2 - expV1)*(1-exp(-(t- expTD1)/expTAU1))
                         + (expV1 - expV2)*(1-exp(-(t- expTD2)/expTAU2));
        } else {
            return 0;
        }

        break;
    case WAVEFORM_PWL:
        for (unsigned int indPWL = 0; indPWL < PWLampl.size()-1; indPWL++) {
            double timeCur = PWLtime[indPWL],
                   timeNext= PWLtime[indPWL+1],
                   amplCur = PWLampl[indPWL],
                   amplNext= PWLampl[indPWL+1];
            if (t >= timeCur && t < timeNext) {
                return amplCur + (amplNext-amplCur)*(t - timeCur)/(timeNext - timeCur);
            }
        }
        return 0;
        break;
    }
}

Waveform::~Waveform() {

}
