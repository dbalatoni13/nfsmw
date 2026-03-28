#ifndef MISC_TIMER_H
#define MISC_TIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"

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

    int operator==(const Timer &t) const {
        return this->PackedTime == t.PackedTime;
    }

    int operator!=(const Timer &t) const {
        return this->PackedTime != t.PackedTime;
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

    Timer operator+(const Timer &t) const {
        return Timer(PackedTime + t.PackedTime);
    }

    Timer &operator+=(const Timer &t) {
        PackedTime += t.PackedTime;
        return *this;
    }

    Timer &operator-=(const Timer &t) {
        PackedTime += t.PackedTime;
        return *this;
    }

    // Timer &operator-() {}

    void ResetLow() {
        this->PackedTime = 0;
    }
    void ResetHigh() {
        this->PackedTime = 0x7fffffff;
    }

    void UnSet() {
        this->PackedTime = 0;
    }

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

    void SetPackedTime(int packed_time) {
        this->PackedTime = packed_time;
    }

    void GetHoursMinsSeconds(int *hours, int *minutes, int *seconds, int *thousandths_seconds);
    void PrintToString(char *string, int flags);

  private:
    int PackedTime; // offset 0x0, size 0x4
};

extern int WorldTimeFrames;
extern float WorldTimeElapsed;
extern int RealTimeFrames;
extern Timer WorldTimer;
extern int WorldTime;
extern Timer RealTimer;
extern float RealTimeElapsed;
extern int RealLoopCounter;

extern volatile int FrameCounter;
extern volatile unsigned int LastFrameCounterTick;

void PrepareRealTimestep(float video_time_elapsed);
void PrepareWorldTimestep(float elapsed_time);
void AdvanceRealTime();
void AdvanceWorldTime();
float GetDebugRealTime();
VIDEO_MODE GetVideoMode();

inline float GetVideoFrameTime(VIDEO_MODE video_mode) {
    if (video_mode == MODE_PAL) {
        return 1.0f / 50.0f;
    }
    return 1.0f / 60.0f;
}

inline float VideoFramesToSeconds(int num_frames) {
    return static_cast<float>(num_frames) * GetVideoFrameTime(GetVideoMode());
}

#endif
