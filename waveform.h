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

    /* SIN(V0 VA FREQ TD THETA)
     *                       Default
     *  V0   Offset             -
     *  VA   Amplitude          -
     * FREQ  Frequency         1/t2
     *  TD   Delay              0
     * THETA Damping factor     0
     */
    double sinV0, sinVA, sinFREQ, sinTD, sinTHETA;
    bool sinFREQset;

    /* PULSE(V1 V2 TD TR TF PW PER)
     *                      Default
     * V1   Initial value      -
     * V2   Pulsed value       -
     * TD   Delay time         0
     * TR   Rise time       time step
     * TF   Fall time       time step
     * PW   Pulse width        t2
     * PER  Period             t2
     */

    double pulseV1, pulseV2, pulseTD, pulseTR, pulseTF, pulsePW, pulsePER;
    bool pulseTRset, pulseTFset, pulsePWset, pulsePERset;

    /* EXP(V1 V2 TD1 TAU1 TD2 TAU2)
     *                            Default
     * V1    Initial value           -
     * V2    Pulsed value            -
     * TD1   Rise delay time         0
     * TAU1  Rise time constant  time step
     * TD2   Fall delay time     t1 + time step
     * TAU2  Fall time constant  time step
     */

    double expV1, expV2, expTD1, expTAU1, expTD2, expTAU2;
    bool expTAU1set, expTD2set, expTAU2set;

    // PWL(T0 A0 T1 A1 T2 A2 ...)

    std::vector <double> PWLtime, PWLampl;

    /* SFFM(VO VA FC MDI FS)
     *                           Default
     *  VO   Offset                 -
     *  VA   Amplitude              -
     *  FC   Carrier frequency      -
     *  MDI  Modulation index       -
     *  FS   Signal frequency       -
     */

    double SFFMVO, SFFMVA, SFFMFC, SFFMMDI, SFFMFS;

    /* AM(VA VO MF DC tD)
     *                           Default
     *  VA   Amplitude              -
     *  VO   Offset                 -
     *  MF   Modulation frequency   -
     *  FC   Carrier frequency      -
     *  TD   Signal delay           -
     */

    double AMVA, AMVO, AMMF, AMFC, AMTD;

    double noiseNA, noiseNT, noiseNALPHA, noiseNAMP, noiseRTSAM, noiseRTSCAPT, noiseRTSEMT;

    unsigned int mode;
private:
    std::vector <std::string> strList;
    std::string name;
};

#endif // WAVEFORM_H
