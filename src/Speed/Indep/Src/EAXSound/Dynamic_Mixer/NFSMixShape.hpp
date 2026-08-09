//
//
//
//
#ifndef NFSMIXSHAPE_H
#define NFSMIXSHAPE_H

#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixerDefines.hpp"

namespace NFSMixShape {

int GetCentsFromPitchMult(float ratio);
int GetIntPitchMultFromCents(int cents);
float GetPitchMultFromCents(int cents);
int GetQ15FromHundredthsdB(int ndB);
int GetdBFromQ15(int nQ15);
float GetFloatFromHundredthsdB(int ndB);
int GetQ15FromQ7(int nQ7Value);
int GetCurveOutput(eMIXTABLEID etable, int nQ15Ratio, bool bdBOut);
int GetAzimShapeOutput(eMIXTABLEID etable1, eMIXTABLEID etable2, int *pdistances, int nmixratio);

}; // namespace NFSMixShape

#endif
