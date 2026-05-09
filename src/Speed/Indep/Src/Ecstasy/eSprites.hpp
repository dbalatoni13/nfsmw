#ifndef ECSTASY_ESPRITES_H
#define ECSTASY_ESPRITES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
// total size: 0x30
class ePointSprite3D {
  public:
    static void *operator new(size_t size) {}

    ePointSprite3D() {}

    ~ePointSprite3D() {}

    // Members
    float X;       // offset 0x0, size 0x4
    float Y;       // offset 0x4, size 0x4
    float Z;       // offset 0x8, size 0x4
    float Radius;  // offset 0xC, size 0x4
    float S1;      // offset 0x10, size 0x4
    float T1;      // offset 0x14, size 0x4
    float S0;      // offset 0x18, size 0x4
    float T0;      // offset 0x1C, size 0x4
    uint32 Colour; // offset 0x20, size 0x4
    uint32 pad0;   // offset 0x24, size 0x4
    float Cos;     // offset 0x28, size 0x4
    float Sin;     // offset 0x2C, size 0x4
};
#endif
