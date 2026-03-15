#ifndef FENG_FEMATH_H
#define FENG_FEMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct FEMatrix4 {
    float m11; // offset 0x0, size 0x4
    float m12; // offset 0x4, size 0x4
    float m13; // offset 0x8, size 0x4
    float m14; // offset 0xC, size 0x4
    float m21; // offset 0x10, size 0x4
    float m22; // offset 0x14, size 0x4
    float m23; // offset 0x18, size 0x4
    float m24; // offset 0x1C, size 0x4
    float m31; // offset 0x20, size 0x4
    float m32; // offset 0x24, size 0x4
    float m33; // offset 0x28, size 0x4
    float m34; // offset 0x2C, size 0x4
    float m41; // offset 0x30, size 0x4
    float m42; // offset 0x34, size 0x4
    float m43; // offset 0x38, size 0x4
    float m44; // offset 0x3C, size 0x4

    void Identify();
};

#endif
