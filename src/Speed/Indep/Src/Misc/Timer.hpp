#ifndef MISC_TIMER_H
#define MISC_TIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x4
class Timer {
  public:
    Timer() {
        this->PackedTime = 0;
    }

    Timer(float seconds) {
        SetTime(seconds);
    }

    Timer(int packed_time) {
        this->PackedTime = packed_time;
    }

    int operator=(const Timer &t) const {}

    int operator!=(const Timer &t) const {
        return PackedTime != t.PackedTime;
    }

    Timer &operator=(const Timer &t) {
        this->PackedTime = t.PackedTime;
        return *this;
    }

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

    Timer operator-(const Timer &t) const {
        return Timer(PackedTime - t.PackedTime);
    }

    Timer operator*(const Timer &t) const {}

    Timer &operator+=(const Timer &t) {}

    Timer &operator-=(const Timer &t) {}

    Timer &operator-() {}

    void ResetLow() {
        this->PackedTime = 0;
    }

    void ResetHigh() {}

    void UnSet() { PackedTime = 0; }

    int IsSet() { return PackedTime != 0 && PackedTime != 0x7fffffff; }

    void SetTime(float seconds);

    float GetSeconds() {
        return this->PackedTime / 4000.0f;
    }

    int GetPackedTime() {
        return this->PackedTime;
    }

    void SetPackedTime(int packed_time) {}

    void PrintToString(char*, int);

  private:
    int PackedTime; // offset 0x0, size 0x4
};

extern int WorldTimeFrames;
extern float WorldTimeElapsed;
extern int RealTimeFrames;
extern Timer WorldTimer;

void PrepareRealTimestep(float video_time_elapsed);
void PrepareWorldTimestep(float elapsed_time);
void AdvanceRealTime();
void AdvanceWorldTime();
float GetDebugRealTime();

#endif
