#ifndef GAMEPLAY_GTIMER_H
#define GAMEPLAY_GTIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x24
struct SavedTimerInfo {
    float mInterval; // offset 0x0, size 0x4
    bool mRunning;   // offset 0x4, size 0x1
    float mElapsed;  // offset 0x8, size 0x4
    char mName[20];  // offset 0xC, size 0x14
};

// total size: 0xC
class GTimer {
  public:
    GTimer();

    ~GTimer();

    void Start();

    void Stop();

    bool IsRunning() const {
        return mRunning;
    }

    void Reset(float value);

    float GetTime() const;

    void SetTime(float time);

  private:
    float mStartTime; // offset 0x0, size 0x4
    float mTotalTime; // offset 0x4, size 0x4
    bool mRunning;    // offset 0x8, size 0x1
};

// total size: 0x24
class GEventTimer {
  public:
    GEventTimer();

    ~GEventTimer();

    void Reset();

    void Start();

    void Stop();

    void SetInterval(float value);

    void Update(float dT);

    void SetName(const char *name);

    void Serialize(SavedTimerInfo *saveInfo);

    void Deserialize(SavedTimerInfo *saveInfo);

    bool IsRunning() const {
        return mRunning;
    }

    float GetInterval() const {
        return mInterval;
    }

    unsigned int GetNameHash() const {
        return mNameHash;
    }

    const char *GetName() const {
        return mName;
    }

    float GetElapsed() const {
        return mElapsed;
    }

  private:
    float mInterval;        // offset 0x0, size 0x4
    bool mRunning;          // offset 0x4, size 0x1
    float mElapsed;         // offset 0x8, size 0x4
    unsigned int mNameHash; // offset 0xC, size 0x4
    char mName[20];         // offset 0x10, size 0x14
};

#endif
