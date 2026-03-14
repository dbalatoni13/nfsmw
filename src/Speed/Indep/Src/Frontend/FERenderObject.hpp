#ifndef FRONTEND_FERENDEROBJECT_H
#define FRONTEND_FERENDEROBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0xA4
class FERenderEPoly : public bTNode<FERenderEPoly> {
  public:
    void *operator new(unsigned int size);
    void operator delete(void *p);

  private:
    ePoly EPoly;               // offset 0x8, size 0x94
    TextureInfo *pTexture;     // offset 0x9C, size 0x4
    TextureInfo *pTextureMask; // offset 0xA0, size 0x4
};

// total size: 0x64
class FERenderObject : public bTNode<FERenderObject> {
  public:
    FERenderObject(FEObject *obj, TextureInfo *tex);
    ~FERenderObject();
    void SetTransform(bMatrix4 *pMatrix);
    static void Initialize();

  private:
    FEObject *mpobOwner;               // offset 0x8, size 0x4
    unsigned int mulFlags;             // offset 0xC, size 0x4
    unsigned int mulNumTimesRendered;  // offset 0x10, size 0x4
    TextureInfo *mpobTexture;          // offset 0x14, size 0x4
    bTList<FERenderEPoly> mobPolyList; // offset 0x18, size 0x8
    unsigned int mPolyCount;           // offset 0x20, size 0x4
    bMatrix4 mstTransform;             // offset 0x24, size 0x40
};

#endif
