#ifndef GAMECUBE_ECSTASY_EMATRIXE_H
#define GAMECUBE_ECSTASY_EMATRIXE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "dolphin/gx.h"

void eConvertToGX34(float (&mGX)[3][4], bMatrix4 &mBW);
float (*eLoadPosMtxImm(bMatrix4 &mL2V, _GXPosNrmMtx stage))[3][4];

#endif
