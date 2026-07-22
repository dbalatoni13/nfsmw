//
//
//
//
//
//
//
//
#ifndef TIMER_HPP
#define TIMER_HPP // Decl: 10

#define TIMER_SHIFT_VALUE_FLOAT 4000.0f // Decl: 49
#define TIMER_SHIFT_VALUE_INT 4000      // Decl: 50

#define TIMER_STRING_SIZE 32 // Decl: 53

#define TIMER_PRINT_FLAG_NEGATIVE 1           // Decl: 56
#define TIMER_PRINT_FLAG_SHOW_PLUS_SIGN 2     // Decl: 57
#define TIMER_PRINT_FLAG_SHOW_UNUSED_DIGITS 4 // Decl: 58
#define TIMER_PRINT_FLAG_KEEP_LAST_DIGIT 8    // Decl: 59
#define TIMER_PRINT_FLAG_DONT_SHOW_MS 16      // Decl: 60

#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"

// total size: 0x4
// Decl: 64
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

    Timer operator-(const Timer &t) const {
        return Timer(this->PackedTime - t.PackedTime);
    }

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
        return static_cast<int>(PackedTime != 0 && PackedTime != 0x7fffffff);
    }

    void SetTime(float seconds) {
        PackedTime = static_cast<int>(seconds * TIMER_SHIFT_VALUE_FLOAT + 0.5f);
    }

    float GetSeconds() {
        return this->PackedTime / TIMER_SHIFT_VALUE_FLOAT;
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
extern float WorldTimeSeconds;
extern Timer RealTimer;
extern float RealTimeElapsed;
extern int RealLoopCounter;
extern int WorldLoopCounter;

extern volatile int FrameCounter;
extern volatile unsigned int LastFrameCounterTick;

void ResetWorldTime();
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
