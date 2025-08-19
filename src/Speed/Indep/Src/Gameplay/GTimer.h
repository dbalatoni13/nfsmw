#ifndef GAMEPLAY_GTIMER_H
#define GAMEPLAY_GTIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct GTimer {
    // total size: 0xC
    float mStartTime; // offset 0x0, size 0x4
    float mTotalTime; // offset 0x4, size 0x4
    bool mRunning;    // offset 0x8, size 0x1
};

#endif
