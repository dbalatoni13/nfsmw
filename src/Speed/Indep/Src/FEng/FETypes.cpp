#include "FETypes.h"

FEColor::FEColor(u32 Color) {
    a = Color >> 24;
    r = (Color >> 16) & 0xFF;
    g = (Color >> 8) & 0xFF;
    b = Color & 0xFF;
}

FEColor::operator u32() const {
    unsigned char R = (r < 0) ? 0 : ((r > 255) ? 255 : (unsigned char) r);
    unsigned char G = (g < 0) ? 0 : ((g > 255) ? 255 : (unsigned char) g);
    unsigned char B = (b < 0) ? 0 : ((b > 255) ? 255 : (unsigned char) b);
    unsigned char A = (a < 0) ? 0 : ((a > 255) ? 255 : (unsigned char) a);

    return (A << 24) | (R << 16) | (G << 8) | B;
}

FEColor &FEColor::operator=(const FEColor &rhs) {
    a = rhs.a;
    r = rhs.r;
    g = rhs.g;
    b = rhs.b;

    return *this;
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
