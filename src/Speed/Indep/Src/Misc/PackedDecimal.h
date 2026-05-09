#ifndef MISC_PACKEDDECIMAL_H
#define MISC_PACKEDDECIMAL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

template <int b> inline int RaiseToPower(int power) {
    if (power == 0) {
        return 1;
    }
    return RaiseToPower<b>(power - 1) * 10;
}

// struct FloatingPoint<short int, 10, 3, 5, 11>

// total size: 0x2
template <typename N, int a, int b, int c, int d> class FloatingPoint {
  public:
    // Functions
    inline FloatingPoint<N, a, b, c, d>(float val) {}

    inline operator float() {}

    inline operator int() {}

  private:
    static inline int GetNormalizedLower() {}

    static inline int GetNormalizedUpper() {}

    // Members
    int16 mExp : 5;  // offset 0x0, size 0x2
    int16 mMan : 11; // offset 0x0, size 0x2
};

// struct FixedPoint<short unsigned int, 10, 2>

// total size: 0x2
template <typename N, int a, int b> class FixedPoint {
  public:
    // Functions
    inline FixedPoint<N, a, b>(float val) {}

    inline operator float() {
        return static_cast<float>(mWord) / GetScale();
    }

  private:
    static inline int GetScale() {
        const unsigned int scale = RaiseToPower<b>(1) * 10;

        return scale;
    }

    // Members
    uint16 mWord; // offset 0x0, size 0x2
};

#endif
