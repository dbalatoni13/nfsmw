#ifndef MISC_TIMER_H
#define MISC_TIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class Timer {
    // total size: 0x4
    int PackedTime; // offset 0x0, size 0x4

  public:
    Timer() {}

    Timer(float seconds) {
        this->PackedTime = static_cast<int>(seconds * 4000);
    }

    Timer(int packed_time) {
        this->PackedTime = packed_time;
    }

    int operator=(const Timer &t) const {}

    int operator!=(const Timer &t) const {}

    Timer &operator=(const Timer &t) {}

    int operator>(const Timer &t) const {
        return this->PackedTime > t.PackedTime;
    }

    int operator>=(const Timer &t) const {
        return this->PackedTime >= t.PackedTime;
    }

    int operator<(const Timer &t) const {
        return this->PackedTime < t.PackedTime;
    }

    int operator<=(const Timer &t) const {
        return this->PackedTime <= t.PackedTime;
    }

    Timer operator+(const Timer &t) const {}

    Timer operator*(const Timer &t) const {}

    Timer &operator+=(const Timer &t) {}

    Timer &operator-=(const Timer &t) {}

    Timer &operator-() {}

    void ResetLow() {}

    void ResetHigh() {}

    void UnSet() {}

    int IsSet() {}

    void SetTime(float seconds) {}

    float GetSeconds() {
        return this->PackedTime / 4000.0f;
    }

    int GetPackedTime() {}

    void SetPackedTime(int packed_time) {}
};

extern int WorldTimeFrames;
extern float WorldTimeElapsed;
extern int RealTimeFrames;
extern Timer WorldTimer;

#endif
