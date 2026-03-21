#ifndef FRONTEND_MEMORYCARD_MEMORYCARD_H
#define FRONTEND_MEMORYCARD_MEMORYCARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class MemoryCard {
  public:
    enum SaveType {
        ST_PROFILE = 0,
        ST_THUMBNAIL = 1,
        ST_IMAGE = 2,
        ST_MAX = 3,
    };

    void Tick(int);

    static MemoryCard *GetInstance() {
        return s_pThis;
    }

    static MemoryCard *s_pThis;
    static int IsCardBusy();
};

void InitMemoryCard();

#endif
