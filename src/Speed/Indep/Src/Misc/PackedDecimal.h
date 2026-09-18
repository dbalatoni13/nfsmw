#ifndef MISC_PACKEDDECIMAL_H
#define MISC_PACKEDDECIMAL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/FixedPoint.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x2
template <typename T, int BASE, int MINEXP, int EXPBITS, int MANTBITS>
struct FloatingPoint {
    static int GetNormalizedLower() {
        static const int lower = RaiseToPower<BASE>(MINEXP - 2) * BASE;
        return lower;
    }

    static int GetNormalizedUpper() {
        static const int upper = RaiseToPower<BASE>(MINEXP - 1) * BASE;
        return upper;
    }
};

class PackedDecimal {
  public:
    PackedDecimal(float value);

    operator float() const {
        int exponent = mExp;
        int count = bAbs(exponent);
        int scale = 1;

        while (count-- > 0) {
            scale *= 10;
        }
        if (mExp < 0) {
            return (float)mMan / (float)scale;
        }
        return (float)mMan * (float)scale;
    }

  private:
    short mExp : 5;  // offset 0x0, size 0x2
    short mMan : 11; // offset 0x0, size 0x2
};

#endif
