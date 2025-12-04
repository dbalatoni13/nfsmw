#ifndef FENG_FEEVENT_H
#define FENG_FEEVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x8
class FEEventList {
  private:
    int Count;              // offset 0x0, size 0x4
    struct FEEvent *pEvent; // offset 0x4, size 0x4
};

#endif
