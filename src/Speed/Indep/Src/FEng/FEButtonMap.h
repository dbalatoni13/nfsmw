#ifndef FENG_FEBUTTONMAP_H
#define FENG_FEBUTTONMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "FEObject.h"
// #include "Speed/Indep/Src/FEng/FEGameInterface.h"

class FEGameInterface;

typedef enum { Wrap_None = 0, Wrap_Horizontal = 1, Wrap_Vertical = 2, Wrap_Both = 3 } FEButtonWrapMode;

// total size: 0x8
class FEButtonMap {
  public:
    FEObject **pList;    // offset 0x0, size 0x4
    unsigned long Count; // offset 0x4, size 0x4

    inline FEButtonMap() : pList(nullptr), Count(0) {}
    inline ~FEButtonMap() {
        if (pList) {
            delete[] pList;
        }
    }
    inline unsigned long GetCount() {
        return Count;
    }
    inline void SetButton(unsigned long Index, FEObject *pButton) {
        pList[Index] = pButton;
    }
    inline FEObject *GetButton(unsigned long Index) {
        return pList[Index];
    }

    void SetCount(unsigned long NewCount);
    FEObject *GetButtonFrom(FEObject *pButton, long Direction, FEGameInterface *pInterface, FEButtonWrapMode WrapMode);
    void ComputeButtonLocation(FEObject *pObj, FEGameInterface *pInterface, FEVector2 &Loc);
};

#endif
