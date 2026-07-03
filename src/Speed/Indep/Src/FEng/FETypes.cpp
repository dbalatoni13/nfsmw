#include "Speed/Indep/Src/FEng/FETypes.h"

FEColor::FEColor(u32 Col) {
    a = Col >> 24;
    r = (Col >> 16) & 0xFF;
    g = (Col >> 8) & 0xFF;
    b = Col & 0xFF;
}

FEColor::operator u32() const {
    u8 cr, cg, cb, ca;

    cr = FECLAMP(r);
    cg = FECLAMP(g);
    cb = FECLAMP(b);
    ca = FECLAMP(a);

    return (ca << 24) | (cr << 16) | (cg << 8) | cb;
}

FEColor &FEColor::operator=(const FEColor &rhs) {
    a = rhs.a;
    r = rhs.r;
    g = rhs.g;
    b = rhs.b;
    return *this;
}

FEColor FEColor::operator+(const FEColor &rhs) const {
    FEColor Result;
    Result.r = r + rhs.r;
    Result.g = g + rhs.g;
    Result.b = b + rhs.b;
    Result.a = a + rhs.a;
    return Result;
}

FEColor &FEColor::operator+=(const FEColor &rhs) {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    a += rhs.a;
    return *this;
}

FEColor FEColor::operator-(const FEColor &rhs) const {
    FEColor Result;
    Result.r = r - rhs.r;
    Result.g = g - rhs.g;
    Result.b = b - rhs.b;
    Result.a = a - rhs.a;
    return Result;
}

FEColor &FEColor::operator-=(const FEColor &rhs) {
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    a -= rhs.a;
    return *this;
}
