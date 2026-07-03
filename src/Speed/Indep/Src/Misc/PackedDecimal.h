#ifndef PACKEDDECIMAL_H__
#define PACKEDDECIMAL_H__

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "types.h"

template <int b> inline int RaiseToPower(int power) {
    if (power == 0) {
        return 1;
    }
    return RaiseToPower<b>(power - 1) * b;
}

// total size: 0x2
template <typename N, int a, int b, int c, int d> class FloatingPoint {
  public:
    // Functions
    FloatingPoint<N, a, b, c, d>(float val) {}

    operator float() {
        int factor = mMan;
        int exp = bAbs(mExp);

        // UNSOLVED
    }

    operator int() {
        return static_cast<int>(float());
    }

  private:
    static int GetNormalizedLower() {
        const int lower = RaiseToPower<a>(b - 1);
        return lower;
    }

    static int GetNormalizedUpper() {
        const int upper = RaiseToPower<a>(b);
        return upper;
    }

    // Members
    N mExp : c; // offset 0x0, size 0x2
    N mMan : d; // offset 0x0, size 0x2
};

// total size: 0x2
template <typename N, int a, int b> class FixedPoint {
  public:
    FixedPoint<N, a, b>() {}
    FixedPoint<N, a, b>(float val) {
        mWord = val * GetScale();
    }

    operator float() {
        return static_cast<float>(mWord) / GetScale();
    }

  private:
    static int GetScale() {
        const unsigned int scale = RaiseToPower<a>(b);

        return scale;
    }

    N mWord; // offset 0x0, size 0x2
};

typedef FloatingPoint<int8, 10, 1, 3, 5> decimal8;
typedef FloatingPoint<int16, 10, 3, 5, 11> decimal16; // Decl: speed/indep/src/Misc/PackedDecimal.h:107

typedef FixedPoint<int16, 10, 2> decfix16;
typedef FixedPoint<uint16, 10, 2u> udecfix16; // Decl: speed/indep/src/Misc/PackedDecimal.h:110

#endif
