#ifndef __FEGENERICVAL_H__
#define __FEGENERICVAL_H__

#include "FETypes.h"
#include "Speed/Indep/Src/FEng/FEMath.h"

// File: speed/indep/src/feng/FEGenericVal.h
// total size: 0x10
// Decl: speed/indep/src/feng/FEGenericVal.h:74
class FEGenericVal {
  public:
    u32 Data[4]; // offset 0x0, size 0x10, Decl: speed/indep/src/feng/FEGenericVal.h:85

  private:
    operator float() const {}
    operator i32() const {}
    operator FEVector2() const {}
    operator FEVector3() const {}
    operator FEQuaternion() const {}
    operator FEColor() const {}

  public:
    operator u8 *() const {
        return reinterpret_cast<u8 *>(const_cast<u32 *>(Data));
    }
    operator float *() const {}
    operator i32 *() const {
        return reinterpret_cast<i32 *>(const_cast<u32 *>(Data));
    }
    operator FEVector2 *() const {
        return reinterpret_cast<FEVector2 *>(const_cast<u32 *>(Data));
    }
    operator FEVector3 *() const {
        return reinterpret_cast<FEVector3 *>(const_cast<u32 *>(Data));
    }
    operator FEQuaternion *() const {
        return reinterpret_cast<FEQuaternion *>(const_cast<u32 *>(Data));
    }
    operator FEColor *() const {
        return reinterpret_cast<FEColor *>(const_cast<u32 *>(Data));
    }

    FEGenericVal &operator=(const FEGenericVal &Val) {
        Data[0] = Val.Data[0];
        Data[1] = Val.Data[1];
        Data[2] = Val.Data[2];
        Data[3] = Val.Data[3];
        return *this;
    }

    FEGenericVal &operator=(const i32 &Val) {
        *reinterpret_cast<i32 *>(Data) = Val;
        return *this;
    }

    FEGenericVal &operator=(const FEVector2 &Val) {
        *reinterpret_cast<FEVector2 *>(Data) = Val;
        return *this;
    }

    FEGenericVal &operator=(const FEVector3 &Val) {
        *reinterpret_cast<FEVector3 *>(Data) = Val;
        return *this;
    }

    FEGenericVal &operator=(const FEQuaternion &Val) {
        *reinterpret_cast<FEQuaternion *>(Data) = Val;
        return *this;
    }

    FEGenericVal &operator=(const FEColor &Val) {
        *reinterpret_cast<FEColor *>(Data) = Val;
        return *this;
    }
};

#endif
