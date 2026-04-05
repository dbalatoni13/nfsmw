#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"

extern int g_dBToQ15XForm[0x25A];
extern int g_Q15todBXForm[0x200];
extern int g_nArrayCosTable[0x201];
extern float g_fPitchSemitoneTable[12];
extern float g_fPitchCentTable[100];
extern int nDBreturn;
extern int ndBShift;
extern int ndBRem;

int NFSMixShape::GetCentsFromPitchMult(float ratio) {
    int ndB;
    float cents;

    if (ratio > 1.0f) {
        ndB = GetdBFromQ15(static_cast<int>(32767.0f / ratio));
        cents = static_cast<float>(ndB) * -1.9931569f;
    } else {
        ndB = GetdBFromQ15(static_cast<int>(ratio * 32767.0f));
        cents = static_cast<float>(ndB) * 1.9931569f;
    }

    return static_cast<int>(cents);
}

int NFSMixShape::GetIntPitchMultFromCents(int cents) {
    float mult;

    if ((cents & 0x8000U) != 0) {
        cents = cents | 0xFFFF0000;
    }

    GetPitchMultFromCents(cents);
    mult = GetPitchMultFromCents(cents);
    return static_cast<int>(mult * 4096.0f);
}

float NFSMixShape::GetPitchMultFromCents(int cents) {
    int num;
    float octaveMulti;

    octaveMulti = 1.0f;
    num = cents;
    if (cents > 0x4AF) {
        do {
            num = num - 0x4B0;
            octaveMulti = octaveMulti + octaveMulti;
        } while (num > 0x4AF);
    }

    for (; num < -0x4AF; num = num + 0x4B0) {
        octaveMulti = octaveMulti + octaveMulti;
    }

    if (cents > -1) {
        return g_fPitchSemitoneTable[num / 100] * g_fPitchCentTable[num % 100] * octaveMulti;
    }

    return ((1.0f / g_fPitchSemitoneTable[-num / 100]) * (1.0f / g_fPitchCentTable[-num % 100])) / octaveMulti;
}

int NFSMixShape::GetQ15FromHundredthsdB(int ndB) {
    if (ndB == 0) {
        return 0x7FFF;
    }

    ndBShift = ndB / -0x25A;
    ndBRem = -(ndB % 0x25A);
    nDBreturn = g_dBToQ15XForm[(ndB % 0x25A) + 0x259] >> (ndBShift & 0x1FU);
    if (nDBreturn < 0x32) {
        nDBreturn = 0;
    }

    return nDBreturn;
}

int NFSMixShape::GetdBFromQ15(int nQ15) {
    unsigned int n;
    int ndB;

    nDBreturn = -1;
    n = 0;
    ndB = -10000;
    if (nQ15 != 0) {
        if ((nQ15 & 0x4000U) == 0) {
            n = 1;
            do {
                if (((0x4000 >> (n & 0x1FU)) & nQ15) != 0) {
                    goto found;
                }
                n = n + 1;
            } while (n != 0x11);
        } else {
found:
            int index;
            int nRem;

            nRem = nQ15 - (0x4000 >> (n & 0x1FU));
            if (static_cast<int>(n) < 5) {
                index = nRem >> (5 - n);
            } else {
                index = (nRem << (n - 5)) | ((1 << (n - 5)) - 1);
            }
            ndB = ((n + 1) * -0x25A) + g_Q15todBXForm[index];
            if (ndB <= -0x2711) {
                ndB = -10000;
            }
        }
    }

    return ndB;
}

float NFSMixShape::GetFloatFromHundredthsdB(int ndB) {
    return static_cast<float>(GetQ15FromHundredthsdB(ndB)) * 3.051851e-05f;
}

int NFSMixShape::GetCurveOutput(eMIXTABLEID etable, int nQ15Ratio, bool bdBOut) {
    int nQ15;
    int ndB;
    int nindex;
    eMIXTABLEID opposite;

retry:
    if (etable > ::SHAPE_UP_LINEAR) {
        return 0;
    }

    nindex = nQ15Ratio >> 6;
    switch (etable) {
    case ::SHAPE_DWN_EQPWR:
        nQ15 = g_nArrayCosTable[nindex];
        if (nQ15 != 0) {
            nQ15 = nQ15 + (((g_nArrayCosTable[nindex + 1] - nQ15) * (((nQ15Ratio & 0x1FU) << 9) | 0x3FF)) >> 15);
        }
        if (!bdBOut) {
            return nQ15;
        }
        ndB = GetdBFromQ15(nQ15);
        if (ndB > -0x2581) {
            return ndB;
        }
        return -10000;

    case ::SHAPE_UP_EQPWR:
        if (!bdBOut) {
            etable = ::SHAPE_DWN_EQPWR;
            nQ15Ratio = 0x7FFF - nQ15Ratio;
            goto retry;
        }
        opposite = ::SHAPE_DWN_EQPWR;
        break;

    case ::SHAPE_DWN_EQPWR_SQ:
        if (!bdBOut) {
            nQ15 = GetCurveOutput(::SHAPE_DWN_EQPWR, nQ15Ratio, false);
            return (nQ15 * nQ15) >> 15;
        }
        ndB = GetCurveOutput(::SHAPE_DWN_EQPWR, nQ15Ratio, true);
        if ((ndB << 1) > -0x2581) {
            return ndB << 1;
        }
        return -10000;

    case ::SHAPE_UP_EQPWR_SQ:
        if (!bdBOut) {
            etable = ::SHAPE_DWN_EQPWR_SQ;
            nQ15Ratio = 0x7FFF - nQ15Ratio;
            goto retry;
        }
        opposite = ::SHAPE_DWN_EQPWR_SQ;
        break;

    case ::SHAPE_DWN_ONE_MIN_EQPWR:
        nQ15 = (0x7FFF - g_nArrayCosTable[0x1FF - nindex]) +
               (((g_nArrayCosTable[0x1FF - nindex] - g_nArrayCosTable[0x200 - nindex]) *
                 (((nQ15Ratio & 0x1FU) << 9) | 0x3FF)) >>
                15);
        if (!bdBOut) {
            return nQ15;
        }
        ndB = GetdBFromQ15(nQ15);
        if (ndB > -0x2581) {
            return ndB;
        }
        return -10000;

    case ::SHAPE_UP_ONE_MIN_EQPWR:
        if (!bdBOut) {
            etable = ::SHAPE_DWN_ONE_MIN_EQPWR;
            nQ15Ratio = 0x7FFF - nQ15Ratio;
            goto retry;
        }
        opposite = ::SHAPE_DWN_ONE_MIN_EQPWR;
        break;

    case ::SHAPE_DWN_ONE_MIN_EQPWR_SQ:
        if (!bdBOut) {
            nQ15 = GetCurveOutput(::SHAPE_DWN_ONE_MIN_EQPWR, nQ15Ratio, false);
            return (nQ15 * nQ15) >> 15;
        }
        ndB = GetCurveOutput(::SHAPE_DWN_ONE_MIN_EQPWR, nQ15Ratio, true);
        if ((ndB << 1) > -0x2581) {
            return ndB << 1;
        }
        return -10000;

    case ::SHAPE_UP_ONE_MIN_EQPWR_SQ:
        if (!bdBOut) {
            etable = ::SHAPE_DWN_ONE_MIN_EQPWR_SQ;
            nQ15Ratio = 0x7FFF - nQ15Ratio;
            goto retry;
        }
        opposite = ::SHAPE_DWN_ONE_MIN_EQPWR_SQ;
        break;

    case ::SHAPE_DWN_LINEAR:
        if (!bdBOut) {
            return 0x7FFF - nQ15Ratio;
        }
        return GetdBFromQ15(0x7FFF - nQ15Ratio);

    case ::SHAPE_UP_LINEAR:
        if (!bdBOut) {
            etable = ::SHAPE_DWN_LINEAR;
            nQ15Ratio = 0x7FFF - nQ15Ratio;
            goto retry;
        }
        opposite = ::SHAPE_DWN_LINEAR;
        break;

    default:
        return 0;
    }

    return -GetCurveOutput(opposite, nQ15Ratio, true);
}

int NFSMixShape::GetAzimShapeOutput(eMIXTABLEID etable1, eMIXTABLEID etable2, int *pdistances, int nmixratio) {
    int nQOutTable_00;
    int nQOutTable_01;

    nQOutTable_00 = GetCurveOutput(etable1, pdistances[0], false);
    nQOutTable_01 = 0x7FFF;
    if (nmixratio != 0) {
        nQOutTable_01 = GetCurveOutput(etable2, pdistances[1], false);
    }

    return (nQOutTable_00 * (0x7FFF - (nmixratio * 2)) >> 15) + (nQOutTable_01 * (nmixratio * 2) >> 15);
}
