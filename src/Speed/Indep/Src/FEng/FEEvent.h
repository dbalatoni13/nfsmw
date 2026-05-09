#ifndef FENG_FEEVENT_H
#define FENG_FEEVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

typedef struct { // 0xc
    /* 0x0 */ u32 EventID;
    /* 0x4 */ u32 Target;
    /* 0x8 */ u32 tTime;
} FEEvent;

// total size: 0x8
class FEEventList {
  public:
    int Count;       // offset 0x0, size 0x4
    FEEvent *pEvent; // offset 0x4, size 0x4

    inline FEEventList() : Count(0), pEvent(nullptr) {}
    void operator=(FEEventList &rhs);
    void SetCount(long NewCount);
    inline unsigned long GetCount() {
        return Count;
    }
    inline FEEvent &operator[](int Index) {
        return pEvent[Index];
    }
};

#endif
