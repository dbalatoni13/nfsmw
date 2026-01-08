#ifndef GAMEPLAY_GTIMER_H
#define GAMEPLAY_GTIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0xC
class GTimer {
  public:
  private:
    float mStartTime; // offset 0x0, size 0x4
    float mTotalTime; // offset 0x4, size 0x4
    bool mRunning;    // offset 0x8, size 0x1
};

// total size: 0x24
class GEventTimer {
  public:
    // bool IsRunning() const {}

    // float GetInterval() const {}

    // unsigned int GetNameHash() const {}

    // const char *GetName() const {}

    // float GetElapsed() const {}

  private:
    float mInterval;        // offset 0x0, size 0x4
    bool mRunning;          // offset 0x4, size 0x1
    float mElapsed;         // offset 0x8, size 0x4
    unsigned int mNameHash; // offset 0xC, size 0x4
    char mName[20];         // offset 0x10, size 0x14
};

#endif
