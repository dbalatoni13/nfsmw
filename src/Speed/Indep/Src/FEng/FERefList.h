#ifndef FENG_FEREFLIST_H
#define FENG_FEREFLIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEList.h"

// total size: 0x10
class FERefList {
  public:
  private:
    bool bIsReference; // offset 0x0, size 0x1
  protected:
    union {
        FERefList *pRef; // offset 0x0, size 0x4
        FEMinNode *head; // offset 0x0, size 0x4
    }; // offset 0x4, size 0x4
    FEMinNode *tail; // offset 0x8, size 0x4
};

#endif
