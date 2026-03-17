#ifndef FRONTEND_MEMORYCARD_MEMORYCARD_H
#define FRONTEND_MEMORYCARD_MEMORYCARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class MemoryCard {
  public:
    static int IsCardBusy();
};

void InitMemoryCard();

#endif
