#ifndef GAMEPLAY_GTIMER_H
#define GAMEPLAY_GTIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

// total size: 0xC
class GTimer {
  public:
    GTimer();
    ~GTimer();
    void Start();
    void Stop();
    void Reset(float value);
    float GetTime() const;
    void SetTime(float time);
    bool IsRunning() {
        return mRunning;
    }

  private:
    float mStartTime; // offset 0x0, size 0x4
    float mTotalTime; // offset 0x4, size 0x4
    bool mRunning;    // offset 0x8, size 0x1
};

// total size: 0x20
struct SavedTimerInfo {
    float mInterval; // offset 0x0, size 0x4
    bool mRunning;   // offset 0x4, size 0x1
    float mElapsed;  // offset 0x8, size 0x4
    char mName[20];  // offset 0xC, size 0x14
};

// total size: 0x24
class GEventTimer {
  public:
    GEventTimer();
    ~GEventTimer();
    void Start();
    void Stop();
    void Reset();
    bool IsRunning() const {
        return mRunning;
    };
    float GetInterval() const {
        return mInterval;
    };
    uint32 GetNameHash() const {
        return mNameHash;
    };
    const char *GetName() const {
        return mName;
    };
    float GetElapsed() const {
        return mElapsed;
    };
    void SetInterval(float value);
    void SetName(const char *name);
    void Update(float dT);

  protected:
    void Serialize(SavedTimerInfo *saveInfo);
    void Deserialize(SavedTimerInfo *saveInfo);

  private:
    float mInterval;  // offset 0x0, size 0x4
    bool mRunning;    // offset 0x4, size 0x1
    float mElapsed;   // offset 0x8, size 0x4
    uint32 mNameHash; // offset 0xC, size 0x4
    char mName[20];   // offset 0x10, size 0x14
};

#endif
