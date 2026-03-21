#ifndef FENG_FEEVENT_H
#define FENG_FEEVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0xC
struct FEEvent {
    unsigned long EventID; // offset 0x0, size 0x4
    unsigned long Target;  // offset 0x4, size 0x4
    unsigned long tTime;   // offset 0x8, size 0x4
};

// total size: 0x8
class FEEventList {
  public:
    int Count;              // offset 0x0, size 0x4
    FEEvent* pEvent;        // offset 0x4, size 0x4

    inline FEEventList() : Count(0), pEvent(nullptr) {}
    void operator=(FEEventList& rhs);
    void SetCount(long NewCount);
    inline unsigned long GetCount() { return Count; }
    inline FEEvent& operator[](int Index) { return pEvent[Index]; }
};

#endif
