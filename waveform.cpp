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

        if (strList.size() != 5) {
            std::cerr << "SFFM() requires five parameters!" << std::endl;;
            exit(-1);
        }
        Value valVO(strList[0]);  SFFMVO  = valVO.val;
        Value valVA(strList[1]);  SFFMVA  = valVA.val;
        Value valFC(strList[2]);  SFFMFC  = valFC.val;
        Value valMDI(strList[3]); SFFMMDI = valMDI.val;
        Value valMFS(strList[4]); SFFMFS  = valMFS.val;
    } else if (name == "AM") {
        mode = WAVEFORM_AM;

        if (strList.size() != 5) {
            std::cerr << "AM() requires five parameters!" << std::endl;;
            exit(-1);
        }
        Value valVA(strList[0]); AMVA = valVA.val;
        Value valVO(strList[1]); AMVO = valVO.val;
        Value valMF(strList[2]); AMMF = valMF.val;
        Value valFC(strList[3]); AMFC = valFC.val;
        Value valTD(strList[4]); AMTD = valTD.val;
    } else if (name == "PULSE") {
        mode = WAVEFORM_PULSE;
        if (strList.size() < 2) {
            std::cerr << "PULSE() requires at least two parameters!" << std::endl;;
            exit(-1);
        }
        Value valV1(strList[0]); pulseV1 = valV1.val;
        Value valV2(strList[1]); pulseV2 = valV2.val;

        if (strList.size() >= 3) {
            Value valTD(strList[2]); pulseTD = valTD.val;
        } else {
            pulseTD    = 0;
        }
        if (strList.size() >= 4) {
            Value valTR(strList[3]); pulseTR = valTR.val;
            pulseTRset = true;
        } else {
            pulseTRset = false;
        }
        if (strList.size() >= 5) {
            Value valTF(strList[4]); pulseTF = valTF.val;
            pulseTFset = true;
        } else {
            pulseTFset = false;
        }
        if (strList.size() >= 6) {
            Value valPW(strList[5]); pulsePW = valPW.val;
            pulsePWset = true;
        } else {
            pulsePWset = false;
        }
        if (strList.size() >= 7) {
            Value valPER(strList[6]); pulsePER = valPER.val;
            pulsePERset = true;
        } else {
            pulsePERset = false;
        }
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
    case WAVEFORM_PULSE:
        if (!pulseTRset) {
            pulseTR = timestep;
            pulseTRset = true;
        }
        if (!pulseTFset) {
            pulseTF = timestep;
            pulseTFset = true;
        }
        if (!pulsePWset) {
            pulsePW = t2;
            pulsePWset = true;
        }
        if (!pulsePERset) {
            pulsePER = t2;
            pulsePERset = true;
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
    case WAVEFORM_PWL: {
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
      }  break;
    case WAVEFORM_PULSE: {
        double tmod = fmod(t, pulsePER);

        double t1 = pulseTD,
               t2 = pulseTD + pulseTR,
               t3 = pulseTD + pulseTR + pulsePW,
               t4 = pulseTD + pulseTR + pulsePW + pulseTF;

        if (tmod >= 0 && tmod < t1) {
            return pulseV1;
        } else if (tmod >= t1 && tmod < t2) {
            if (t1 != t2) {
                return pulseV1 + (pulseV2 - pulseV1) * (tmod - t1)/(t2 - t1);
            } else {
                return pulseV1;
            }
        } else if (tmod >= t2 && tmod < t3) {
            return pulseV2;
        } else if (tmod >= t3 && tmod < t4) {
            if (t3 != t4) {
                return pulseV2 - (pulseV2 - pulseV1) * (tmod - t3)/(t4 - t3);
            } else {
                return pulseV2;
            }
        } else if (tmod >= t4) {
            return pulseV1;
        } else {
            return pulseV1;
        }
      }  break;
    case WAVEFORM_SFFM:
        return SFFMVO + SFFMVA * sin(2*M_PI*SFFMFC*t + SFFMMDI * sin(2*M_PI*SFFMFS*t));
        break;
    case WAVEFORM_AM:
        return AMVA * (AMVO + sin(2*M_PI*AMMF*t))*sin(2*M_PI*AMFC*t);
        break;
    }
}

Waveform::~Waveform() {

}

#ifdef TEST_WAVEFORM

int
main(int argc, char **argv) {
    std::string sinName("SIN");
    std::vector<std::string> vsin;
    vsin.push_back("2");
    vsin.push_back("1.5");
    vsin.push_back("10");

    std::string pulseName("PULSE");
    std::vector<std::string> vpulse;
    vpulse.push_back("0.5");
    vpulse.push_back("2");
    vpulse.push_back("0.0");
    vpulse.push_back("0.01");
    vpulse.push_back("0.01");
    vpulse.push_back("0.025");
    vpulse.push_back("0.1");


    Waveform wfSin(vsin, sinName);
    Waveform wfPulse(vpulse, pulseName);

    for (double t = 0; t < 1; t+= 0.001) {
        double valSin = wfSin.eval(t),
               valPulse = wfPulse.eval(t);
        std::cout << t << " " << valSin << " " << valPulse << std::endl;
    }
}

#endif
