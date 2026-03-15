#ifndef MISC_FIXEDPOINT_H
#define MISC_FIXEDPOINT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

template< int BASE >
int RaiseToPower(int power) {
    if (power == 0) return 1;
    return RaiseToPower< BASE >(power - 1) * BASE;
}

template< typename T, int BASE, unsigned int PRECISION >
struct FixedPoint {
    T mWord; // offset 0x0

    static int GetScale() {
        static const unsigned int scale = RaiseToPower< BASE >(PRECISION - 1) * BASE;
        return scale;
    }

    operator float() {
        return static_cast< float >(mWord) / GetScale();
    }

    FixedPoint(float val) {
        mWord = static_cast< T >(val * GetScale());
    }
};

#endif
