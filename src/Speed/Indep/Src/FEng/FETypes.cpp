#include "Speed/Indep/Src/FEng/FETypes.h"

FEImageData::FEImageData()
    : UpperLeft(),
      LowerRight() {
    Col = FEColor();
    Pivot = FEVector3();
    Pos = FEVector3();
    Rot = FEQuaternion();
    Size = FEVector3();
}

FEColor::FEColor(unsigned long Col) {
    a = Col >> 24;
    r = (Col >> 16) & 0xFF;
    g = (Col >> 8) & 0xFF;
    b = Col & 0xFF;
}

FEColor::operator unsigned long() const {
    int rv, gv, bv, av;

    if (r < 0) {
        rv = 0;
    } else if (r < 256) {
        rv = r;
    } else {
        rv = 255;
    }

    if (g < 0) {
        gv = 0;
    } else if (g < 256) {
        gv = g;
    } else {
        gv = 255;
    }

    if (b < 0) {
        bv = 0;
    } else if (b < 256) {
        bv = b;
    } else {
        bv = 255;
    }

    if (a < 0) {
        av = 0;
    } else if (a < 256) {
        av = a;
    } else {
        av = 255;
    }

    return (av << 24) | (rv << 16) | (gv << 8) | bv;
}

FEColor& FEColor::operator=(const FEColor& rhs) {
    a = rhs.a;
    r = rhs.r;
    g = rhs.g;
    b = rhs.b;
    return *this;
}

FEColor& FEColor::operator+=(const FEColor& rhs) {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    a += rhs.a;
    return *this;
}

FEColor FEColor::operator-(const FEColor& rhs) const {
    FEColor result;
    result.b = b - rhs.b;
    result.g = g - rhs.g;
    result.r = r - rhs.r;
    result.a = a - rhs.a;
    return result;
}
