#ifndef FEBUTTONMAP_H_
#define FEBUTTONMAP_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "FEObject.h"
// #include "Speed/Indep/Src/FEng/FEGameInterface.h"

class FEGameInterface;

// Decl: 41
typedef enum FEButtonWrapMode {
    Wrap_None = 0,
    Wrap_Horizontal = 1,
    Wrap_Vertical = 2,
    Wrap_Both = 3,
} FEButtonWrapMode;

// total size: 0x8
// Decl: 53
class FEButtonMap {
  private:
    FEObject **pList; // offset 0x0, size 0x4, Decl: 55
    u32 Count;        // offset 0x4, size 0x4, Decl: 56

  public:
    FEButtonMap() : pList(nullptr), Count(0) {} // Decl: 59
    ~FEButtonMap() {
        if (pList) {
            delete[] pList;
        }
    }

    void SetCount(u32 NewCount);
    u32 GetCount() { // Decl: 63
        return Count;
    }

    void SetButton(u32 Index, FEObject *pButton) {
        pList[Index] = pButton;
    }

    FEObject *GetButton(u32 Index) {
        return pList[Index];
    }

    FEObject *GetButtonFrom(FEObject *pButton, i32 Direction, FEGameInterface *pInterface, FEButtonWrapMode WrapMode);

    void ComputeButtonLocation(FEObject *pObj, FEGameInterface *pInterface, FEVector2 &Loc); // Decl: 72
};

extern u32 FEDirection_Message[8];

#endif
