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

#include "types.h"

#define TIMER_SHIFT_VALUE_FLOAT 4000.0f // Decl: 49
#define TIMER_SHIFT_VALUE_INT 4000      // Decl: 50

#define TIMER_STRING_SIZE 32 // Decl: 53

#define TIMER_PRINT_FLAG_NEGATIVE 1           // Decl: 56
#define TIMER_PRINT_FLAG_SHOW_PLUS_SIGN 2     // Decl: 57
#define TIMER_PRINT_FLAG_SHOW_UNUSED_DIGITS 4 // Decl: 58
#define TIMER_PRINT_FLAG_KEEP_LAST_DIGIT 8    // Decl: 59
#define TIMER_PRINT_FLAG_DONT_SHOW_MS 16      // Decl: 60

// Decl: 18
enum VIDEO_MODE {
    MODE_PAL = 0,
    MODE_PAL60 = 1,
    MODE_NTSC = 2,
    NUM_VIDEO_MODES = 3,
};

// total size: 0x4
// Decl: 64
class Timer {
  public:
    Timer() {
        this->PackedTime = 0;
    }

    Timer(float seconds) {
        this->SetTime(seconds);
    }

    Timer(int packed_time) {
        this->PackedTime = packed_time;
    }

    int operator==(const Timer &t) const {
        return static_cast<int>(this->PackedTime == t.PackedTime);
    }

    int operator!=(const Timer &t) const {
        return static_cast<int>(this->PackedTime != t.PackedTime);
    }

    Timer &operator=(const Timer &t) {
        this->PackedTime = t.PackedTime;
        return *this;
    }

    int operator>(const Timer &t) const {
        return static_cast<int>(this->PackedTime > t.PackedTime);
    }

    int operator>=(const Timer &t) const {
        return static_cast<int>(this->PackedTime >= t.PackedTime);
    }

    int operator<(const Timer &t) const {
        return static_cast<int>(this->PackedTime < t.PackedTime);
    }

    int operator<=(const Timer &t) const {
        return static_cast<int>(this->PackedTime <= t.PackedTime);
    }

    Timer operator+(const Timer &t) const {
        return Timer(this->PackedTime + t.PackedTime);
    }

    Timer &operator+=(const Timer &t) {
        this->PackedTime += t.PackedTime;
        return *this;
    }

    Timer &operator-=(const Timer &t) {
        this->PackedTime += t.PackedTime;
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
        return static_cast<int>(this->PackedTime != 0 && this->PackedTime != 0x7fffffff);
    }

    void SetTime(float seconds) {
        this->PackedTime = static_cast<int>(seconds * TIMER_SHIFT_VALUE_FLOAT + 0.5f);
    }

    void GetHoursMinsSeconds(int *hours, int *minutes, int *seconds, int *thousandths_seconds);

    float GetSeconds() {
        return this->PackedTime / TIMER_SHIFT_VALUE_FLOAT;
    }

    float GetSecondsRounded(float fIotaSeconds);
    void RoundTime(float fIotaSeconds);

    void PrintToString(char *string, int flags);
    void PrintToString(char *string, float fIota, int flags);

    int GetPackedTime() {
        return this->PackedTime;
    }

    void SetPackedTime(int packed_time) {
        this->PackedTime = packed_time;
    }

    int CountDown(float fSeconds);

  private:
    int PackedTime; // offset 0x0, size 0x4
};

extern Timer WorldTimer;       // Decl: 145
extern float WorldTimeSeconds; // Decl: 146
extern int32 WorldLoopCounter; // Decl: 147
extern int32 WorldTimeFrames;  // Decl: 148

extern int32 WorldTime;        // Decl: 150
extern float WorldTimeElapsed; // Decl: 151

extern Timer RealTimer;       // Decl: 154
extern int32 RealLoopCounter; // Decl: 155
extern int32 RealTimeFrames;  // Decl: 156

extern float RealTimeElapsed; // Decl: 159

// TODO are these here?
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
