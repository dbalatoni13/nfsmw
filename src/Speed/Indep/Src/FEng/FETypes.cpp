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

FEQuaternion FEQuaternion::operator*(const FEQuaternion& q1) {
    FEQuaternion qRet;
    qRet.x = (q1.y * z - q1.z * y) + q1.x * w + x * q1.w;
    qRet.y = (q1.z * x - q1.x * z) + q1.y * w + y * q1.w;
    qRet.z = (q1.x * y - q1.y * x) + q1.z * w + z * q1.w;
    qRet.w = q1.w * w - (q1.z * z + q1.x * x + q1.y * y);
    return qRet;
}
