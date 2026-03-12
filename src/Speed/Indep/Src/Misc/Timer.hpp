#ifndef MISC_TIMER_H
#define MISC_TIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum VIDEO_MODE {
    MODE_PAL = 0,
    MODE_PAL60 = 1,
    MODE_NTSC = 2,
    NUM_VIDEO_MODES = 3,
};

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

    int operator!=(const Timer &t) const {}

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

    Timer operator+(const Timer &t) const {
        return Timer(PackedTime + t.PackedTime);
    }

    Timer operator*(const Timer &t) const {}

    Timer &operator+=(const Timer &t) {
        PackedTime += t.PackedTime;
        return *this;
    }

    Timer &operator-=(const Timer &t) {}

    Timer &operator-() {}

    void ResetLow() {
        this->PackedTime = 0;
    }

    void ResetHigh() {}

    void UnSet() {}

    int IsSet() {
        return PackedTime != 0 && PackedTime != 0x7fffffff;
    }

    void SetTime(float seconds) {
        PackedTime = static_cast<int>(seconds * 4000.0f + 0.5f);
    }

    float GetSeconds() {
        return this->PackedTime / 4000.0f;
    }

    int GetPackedTime() {
        return this->PackedTime;
    }

    void SetPackedTime(int packed_time) {}

    void GetHoursMinsSeconds(int *hours, int *minutes, int *seconds, int *thousandths_seconds);
    void PrintToString(char *string, int flags);

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
