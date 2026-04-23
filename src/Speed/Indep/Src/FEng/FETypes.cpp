#include "Speed/Indep/Src/FEng/FETypes.h"

FEImageData::FEImageData() {
    Rot.w = 1.0f;
}

FEColor::FEColor(unsigned long Col) {
    a = Col >> 24;
    r = (Col >> 16) & 0xFF;
    g = (Col >> 8) & 0xFF;
    b = Col & 0xFF;
}

FEColor::operator unsigned long() const {
    unsigned char rv, gv, bv, av;

    if (r >= 0) {
        if (r > 255) {
            rv = 255;
        } else {
            rv = r;
        }
    } else {
        rv = 0;
    }

    if (g >= 0) {
        if (g > 255) {
            gv = 255;
        } else {
            gv = g;
        }
    } else {
        gv = 0;
    }

    if (b >= 0) {
        if (b > 255) {
            bv = 255;
        } else {
            bv = b;
        }
    } else {
        bv = 0;
    }

    if (a >= 0) {
        av = 255;
        if (a < 256) {
            av = a;
        }
    } else {
        av = 0;
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
    FEColor c;
    c.r = r - rhs.r;
    c.g = g - rhs.g;
    c.b = b - rhs.b;
    c.a = a - rhs.a;
    return c;
}

void FEQuaternion::GetMatrix(FEMatrix4* pMatrix) {
    float xx2 = x * (x + x);
    float yy2 = y * (y + y);
    float zz2 = z * (z + z);
    float xy2 = x * (y + y);
    float xz2 = x * (z + z);
    float yz2 = y * (z + z);
    float wx2 = w * (x + x);
    float wy2 = w * (y + y);
    float wz2 = w * (z + z);

    pMatrix->m11 = 1.0f - (yy2 + zz2);
    pMatrix->m12 = xy2 + wz2;
    pMatrix->m13 = xz2 - wy2;
    pMatrix->m14 = 0.0f;
    pMatrix->m21 = xy2 - wz2;
    pMatrix->m22 = 1.0f - (xx2 + zz2);
    pMatrix->m23 = yz2 + wx2;
    pMatrix->m24 = 0.0f;
    pMatrix->m31 = xz2 + wy2;
    pMatrix->m32 = yz2 - wx2;
    pMatrix->m33 = 1.0f - (xx2 + yy2);
    pMatrix->m34 = 0.0f;
    pMatrix->m41 = 0.0f;
    pMatrix->m42 = 0.0f;
    pMatrix->m43 = 0.0f;
    pMatrix->m44 = 1.0f;
}
