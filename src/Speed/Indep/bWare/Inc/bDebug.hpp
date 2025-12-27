#ifndef BWARE_BDEBUG_H
#define BWARE_BDEBUG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Libs/realcore/include/common/realcore/system.h"

struct bMutex {
    // total size: 0x20
    unsigned int MutexMemory[8]; // offset 0x0, size 0x20

    void Create();
    void Destroy();
    void Lock();
    void Unlock();
};

float bGetTickerDifference(unsigned int start_ticks);
int bGetFixTickerDifference(unsigned int start_ticks, unsigned int end_ticks);
float bGetTickerDifference(unsigned int start_ticks, unsigned int end_ticks);
unsigned int bGetTicker();
void bInitTicker(float min_wraparound_time);
void bBreak();
int bDisableInterrupts();
void bRestoreInterrupts(int previous_state);

extern int EnableReleasePrintf;

#endif
