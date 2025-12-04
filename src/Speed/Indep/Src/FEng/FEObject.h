#ifndef FENG_FEOBJECT_H
#define FENG_FEOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEList.h"
#include "FEScript.h"

enum FEObjType {
    FE_None = 0,
    FE_Image = 1,
    FE_String = 2,
    FE_Model = 3,
    FE_List = 4,
    FE_Group = 5,
    FE_CodeList = 6,
    FE_Movie = 7,
    FE_Effect = 8,
    FE_ColoredImage = 9,
    FE_AnimImage = 10,
    FE_SimpleImage = 11,
    FE_MultiImage = 12,
    FE_UserMin = 256,
};

// total size: 0x5C
class FEObject : public FEMinNode {
  public:
  private:
    unsigned long GUID;           // offset 0xC, size 0x4
    unsigned long NameHash;       // offset 0x10, size 0x4
    char *pName;                  // offset 0x14, size 0x4
    FEObjType Type;               // offset 0x18, size 0x4
    unsigned long Flags;          // offset 0x1C, size 0x4
    unsigned short RenderContext; // offset 0x20, size 0x2
    unsigned short ResourceIndex; // offset 0x22, size 0x2
    unsigned long Handle;         // offset 0x24, size 0x4
    unsigned long UserParam;      // offset 0x28, size 0x4
    unsigned char *pData;         // offset 0x2C, size 0x4
    unsigned long DataSize;       // offset 0x30, size 0x4
    FEMinList Responses;          // offset 0x34, size 0x10
    FEMinList Scripts;            // offset 0x44, size 0x10
    FEScript *pCurrentScript;     // offset 0x54, size 0x4
    class FERenderObject *Cached; // offset 0x58, size 0x4
};

#endif
