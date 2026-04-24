#ifndef FENG_FEGENERICVAL_H
#define FENG_FEGENERICVAL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FETypes.h"

// total size: 0x10
class FEGenericVal {
  public:
    inline operator unsigned char*() const { return reinterpret_cast<unsigned char*>(const_cast<unsigned long*>(Data)); }
    inline operator long*() const { return reinterpret_cast<long*>(const_cast<unsigned long*>(Data)); }
    inline operator FEQuaternion*() const { return reinterpret_cast<FEQuaternion*>(const_cast<unsigned long*>(Data)); }
    inline FEGenericVal& operator=(const FEGenericVal& Val) {
        Data[0] = Val.Data[0]; Data[1] = Val.Data[1]; Data[2] = Val.Data[2]; Data[3] = Val.Data[3];
        return *this;
    }
    inline FEGenericVal& operator=(const FEVector3& Val) {
        *reinterpret_cast<FEVector3*>(Data) = Val;
        return *this;
    }
    inline FEGenericVal& operator=(const FEVector2& Val) {
        *reinterpret_cast<FEVector2*>(Data) = Val;
        return *this;
    }
    inline FEGenericVal& operator=(const FEQuaternion& Val) {
        *reinterpret_cast<FEQuaternion*>(Data) = Val;
        return *this;
    }
    inline FEGenericVal& operator=(const FEColor& Val) {
        *reinterpret_cast<FEColor*>(Data) = Val;
        return *this;
    }
  private:
    unsigned long Data[4]; // offset 0x0, size 0x10
};

#endif
