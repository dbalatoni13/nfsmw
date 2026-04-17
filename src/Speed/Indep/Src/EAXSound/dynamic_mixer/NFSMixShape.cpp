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
    if (ratio <= 1.0f) {
        float lookup;

        lookup = 1.9931569f;
        return static_cast<int>(static_cast<float>(GetdBFromQ15(static_cast<int>(ratio * 32767.0f))) * lookup);
    }

    {
        float lookup;

        lookup = -1.9931569f;
        return static_cast<int>(static_cast<float>(GetdBFromQ15(static_cast<int>(32767.0f / ratio))) * lookup);
    }
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
    float octaveMulti;
    bool Neg;

    octaveMulti = 1.0f;
    Neg = cents < 0;
    if (cents > 0x4AF) {
        do {
            cents = cents - 0x4B0;
            octaveMulti = octaveMulti + octaveMulti;
        } while (cents > 0x4AF);
    }

    for (; cents < -0x4AF; cents = cents + 0x4B0) {
        octaveMulti = octaveMulti + octaveMulti;
    }

    if (!Neg) {
        float semip;
        float centp;

        semip = g_fPitchSemitoneTable[cents / 100];
        centp = g_fPitchCentTable[cents % 100];
        return semip * centp * octaveMulti;
    }

    {
        float semip;
        float centp;

        semip = 1.0f / g_fPitchSemitoneTable[-cents / 100];
        centp = 1.0f / g_fPitchCentTable[-cents % 100];
        return (semip * centp) / octaveMulti;
    }
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
retry:
    switch (etable) {
    case ::SHAPE_DWN_LINEAR:
        if (bdBOut) {
            return GetdBFromQ15(0x7FFF - nQ15Ratio);
        }
        return 0x7FFF - nQ15Ratio;

    case ::SHAPE_DWN_EQPWR: {
        int nrem;
        int nindex;
        int nMain;
        int nDiff;

        nindex = nQ15Ratio >> 6;
        nMain = g_nArrayCosTable[nindex];
        if (nMain != 0) {
            nrem = ((nQ15Ratio & 0x1F) << 9) | 0x3FF;
            nDiff = g_nArrayCosTable[nindex + 1] - nMain;
            nMain = nMain + ((nDiff * nrem) >> 15);
        }
        if (bdBOut) {
            int ndbout;

            ndbout = GetdBFromQ15(nMain);
            if (ndbout < -0x2580) {
                ndbout = -10000;
            }
            return ndbout;
        }
        return nMain;
    }

    case ::SHAPE_DWN_EQPWR_SQ:
        if (bdBOut) {
            int nOut;

            nOut = GetCurveOutput(::SHAPE_DWN_EQPWR, nQ15Ratio, true) << 1;
            if (nOut < -0x2580) {
                nOut = -10000;
            }
            return nOut;
        }
        {
            int nOut;

            nOut = GetCurveOutput(::SHAPE_DWN_EQPWR, nQ15Ratio, false);
            return (nOut * nOut) >> 15;
        }

    case ::SHAPE_DWN_ONE_MIN_EQPWR: {
        int nrem;
        int nindex;
        int nout;
        int nDiff;
        int nScaledDiff;
        int nQOut;

        nindex = nQ15Ratio >> 6;
        nrem = 0x1FF - nindex;
        nindex = 0x200 - nindex;
        nout = g_nArrayCosTable[nrem];
        nDiff = g_nArrayCosTable[nrem] - g_nArrayCosTable[nindex];
        nScaledDiff = (nDiff * (((nQ15Ratio & 0x1F) << 9) | 0x3FF)) >> 15;
        nQOut = (0x7FFF - nout) + nScaledDiff;
        if (bdBOut) {
            int ndbout;

            ndbout = GetdBFromQ15(nQOut);
            if (ndbout < -0x2580) {
                ndbout = -10000;
            }
            return ndbout;
        }
        return nQOut;
    }

    case ::SHAPE_DWN_ONE_MIN_EQPWR_SQ:
        if (bdBOut) {
            int nOut;

            nOut = GetCurveOutput(::SHAPE_DWN_ONE_MIN_EQPWR, nQ15Ratio, true) << 1;
            if (nOut < -0x2580) {
                nOut = -10000;
            }
            return nOut;
        }
        {
            int nOut;

            nOut = GetCurveOutput(::SHAPE_DWN_ONE_MIN_EQPWR, nQ15Ratio, false);
            return (nOut * nOut) >> 15;
        }

    case ::SHAPE_UP_ONE_MIN_EQPWR_SQ:
        if (bdBOut) {
            return -GetCurveOutput(::SHAPE_DWN_ONE_MIN_EQPWR_SQ, nQ15Ratio, true);
        }
        etable = ::SHAPE_DWN_ONE_MIN_EQPWR_SQ;
        nQ15Ratio = 0x7FFF - nQ15Ratio;
        goto retry;

    case ::SHAPE_UP_EQPWR:
        if (bdBOut) {
            return -GetCurveOutput(::SHAPE_DWN_EQPWR, nQ15Ratio, true);
        }
        etable = ::SHAPE_DWN_EQPWR;
        nQ15Ratio = 0x7FFF - nQ15Ratio;
        goto retry;

    case ::SHAPE_UP_ONE_MIN_EQPWR:
        if (bdBOut) {
            return -GetCurveOutput(::SHAPE_DWN_ONE_MIN_EQPWR, nQ15Ratio, true);
        }
        etable = ::SHAPE_DWN_ONE_MIN_EQPWR;
        nQ15Ratio = 0x7FFF - nQ15Ratio;
        goto retry;

    case ::SHAPE_UP_EQPWR_SQ:
        if (bdBOut) {
            return -GetCurveOutput(::SHAPE_DWN_EQPWR_SQ, nQ15Ratio, true);
        }
        etable = ::SHAPE_DWN_EQPWR_SQ;
        nQ15Ratio = 0x7FFF - nQ15Ratio;
        goto retry;

    case ::SHAPE_UP_LINEAR:
        if (bdBOut) {
            return -GetCurveOutput(::SHAPE_DWN_LINEAR, nQ15Ratio, true);
        }
        etable = ::SHAPE_DWN_LINEAR;
        nQ15Ratio = 0x7FFF - nQ15Ratio;
        goto retry;

    default:
        return 0;
    }
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
