#include "Timer.hpp"
#include "BuildRegion.hpp"
#include "Joylog.hpp"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int GetCurrentLanguage();
extern int CurrentLoopCounter;
extern float TimeDifferenceInSeconds;
extern float TimeDifferenceInMiliseconds;
extern unsigned int TimeDifferenceInMicroseconds;

inline int FloatAsInteger(float x) {
    return *reinterpret_cast<int *>(&x);
}

VIDEO_MODE CurrentVideoMode;
extern float RealTimeElapsed;
int RealTimeElapsedQuantized;
int RealTimeFramesElapsed;
float RealTimeElapsedFrame;
float RealTimeElapsedError;
float MaxTicksPerTimestep;
extern Timer RealTimer;
int RealTime;
int RealTimeFrames;
extern int RealLoopCounter;

int WorldTimeFrames;
float WorldTimeElapsed;
float WorldTimeElapsedFrame;
int WorldTimeFramesElapsed;
float WorldTimeSeconds;
int WorldTime;
int WorldLoopCounter;
Timer WorldTimer;
int NeedToPrepareWorldTimestep;

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

VIDEO_MODE GetVideoMode() {
    return CurrentVideoMode;
}

VIDEO_MODE SetVideoMode(VIDEO_MODE video_mode) {
    VIDEO_MODE old_video_mode = CurrentVideoMode;
    CurrentVideoMode = video_mode;
    return old_video_mode;
}

VIDEO_MODE GetBuildRegionVideoMode() {
    VIDEO_MODE video_mode;
    if (BuildRegion::IsPal()) {
        video_mode = MODE_PAL;
    } else {
        video_mode = MODE_NTSC;
    }
    return video_mode;
}

float GetRealTimeElapsedFromQuantized(int real_time_elapsed_quantized) {
    return static_cast<float>(real_time_elapsed_quantized) * (1.0f / 3600.0f);
}

int GetQuantizedRealTimeElapsed(float real_time_elapsed) {
    return static_cast<int>(real_time_elapsed * 3600.0f + 0.5f);
}

void PrepareRealTimestep(float video_time_elapsed) {
    int current_loop_counter = RealLoopCounter & 0xf;
    int quantized_video_time_elapsed;
    float start_video_time_elapsed;

    if (!Joylog::IsReplaying()) {
        float max_time = MaxTicksPerTimestep * GetVideoFrameTime(GetVideoMode());
        start_video_time_elapsed = bMin(video_time_elapsed + RealTimeElapsedError, max_time);
        quantized_video_time_elapsed = GetQuantizedRealTimeElapsed(start_video_time_elapsed);
        Timer t(GetRealTimeElapsedFromQuantized(quantized_video_time_elapsed));
        RealTimeElapsedError = start_video_time_elapsed - t.GetSeconds();
        Joylog::AddData(quantized_video_time_elapsed, 16, JOYLOG_CHANNEL_TIMESTEP);
        Joylog::AddData(current_loop_counter, 4, JOYLOG_CHANNEL_LOOP_COUNTER);
    } else {
        quantized_video_time_elapsed = Joylog::GetData(16, JOYLOG_CHANNEL_TIMESTEP);
        Joylog::GetData(4, JOYLOG_CHANNEL_LOOP_COUNTER);
        start_video_time_elapsed = video_time_elapsed;
    }
    RealTimeElapsedQuantized = quantized_video_time_elapsed;
    RealTimeElapsed = GetRealTimeElapsedFromQuantized(quantized_video_time_elapsed);

    static int SeenTimerProblem;
    if (TimeDifferenceInMiliseconds > 100.0f || TimeDifferenceInMiliseconds < 0.0f ||
        TimeDifferenceInSeconds > 0.2f || TimeDifferenceInSeconds < 0.0f ||
        video_time_elapsed > 0.2f || video_time_elapsed < 0.0f ||
        start_video_time_elapsed > 0.2f || start_video_time_elapsed < 0.0f) {
        SeenTimerProblem = 1;
    }
    if (SeenTimerProblem != 0) {
        static unsigned int SavedTimeDifferenceInMicroseconds;
        static float SavedTimeDifferenceInMiliseconds;
        static float SavedTimeDifferenceInSeconds;
        static float Saved_start_video_time_elapsed;
        static int Saved_high_nibble;
        int high_nibble = CurrentLoopCounter & 0xf0000000;
        if (high_nibble != Saved_high_nibble) {
            SavedTimeDifferenceInMicroseconds = TimeDifferenceInMicroseconds;
            SavedTimeDifferenceInMiliseconds = TimeDifferenceInMiliseconds;
            SavedTimeDifferenceInSeconds = TimeDifferenceInSeconds;
            Saved_start_video_time_elapsed = video_time_elapsed;
            Saved_high_nibble = CurrentLoopCounter & 0xf0000000;
        }
        static int HadBadTimeDifferenceInMiliseconds;
        static int HadBadTimeDifferenceInSeconds;
        static int HadBad_start_video_time_elapsed;
        static int HadBad_video_time_elapsed;
        static int HadBad_MicrosecondsToMiliseconds;
        static int HadBad_MilisecondsToSeconds;
        if (TimeDifferenceInMiliseconds > 100.0f || TimeDifferenceInMiliseconds < 0.0f) {
            HadBadTimeDifferenceInMiliseconds = FloatAsInteger(TimeDifferenceInMiliseconds);
        }
        if (TimeDifferenceInSeconds > 0.2f || TimeDifferenceInSeconds < 0.0f) {
            HadBadTimeDifferenceInSeconds = FloatAsInteger(TimeDifferenceInSeconds);
        }
        if (video_time_elapsed > 0.2f || video_time_elapsed < 0.0f) {
            HadBad_video_time_elapsed = FloatAsInteger(video_time_elapsed);
        }
        if (start_video_time_elapsed > 0.2f || start_video_time_elapsed < 0.0f) {
            HadBad_start_video_time_elapsed = FloatAsInteger(start_video_time_elapsed);
        }
        if (FloatAsInteger(TimeDifferenceInMiliseconds) != FloatAsInteger(TimeDifferenceInMicroseconds * 0.001f)) {
            HadBad_MicrosecondsToMiliseconds = 1;
        }
        if (FloatAsInteger(TimeDifferenceInSeconds) != FloatAsInteger(TimeDifferenceInMiliseconds * 0.001f)) {
            HadBad_MilisecondsToSeconds = 1;
        }
    }
}

void AdvanceRealTime() {
    int frames_elapsed_60hz = static_cast<int>((RealTimeElapsedFrame + RealTimeElapsed) * 60.0f);
    RealTimeElapsedFrame = (RealTimeElapsedFrame + RealTimeElapsed) - static_cast<float>(frames_elapsed_60hz) * (1.0f / 60.0f);
    if (frames_elapsed_60hz == 0) {
        frames_elapsed_60hz = 1;
    }
    RealTimeFramesElapsed = frames_elapsed_60hz;
    RealTimeFrames = RealTimeFrames + frames_elapsed_60hz;
    RealTime = RealTimeFrames;
    Timer t(RealTimeElapsed);
    RealTimer = RealTimer + t;
    RealLoopCounter = RealLoopCounter + 1;
}

void ResetWorldTime() {
    WorldTimer.SetTime(Sim::GetTime());
    WorldTimeElapsed = 0.0f;
    WorldTimeFramesElapsed = 0;
    WorldTimeSeconds = WorldTimer.GetSeconds();
    WorldTime = 1;
    WorldLoopCounter = 0;
    WorldTimeElapsedFrame = 0.0f;
    WorldTimeFrames = 1;
}

void PrepareWorldTimestep(float elapsed_time) {
    if (!NeedToPrepareWorldTimestep) {
        return;
    }
    NeedToPrepareWorldTimestep = 0;
    WorldTimeElapsed = elapsed_time;
}

void AdvanceWorldTime() {
    if (NeedToPrepareWorldTimestep == 0 && (NeedToPrepareWorldTimestep = 1, WorldTimeElapsed != 0.0f)) {
        int frames_elapsed_60hz = static_cast<int>((WorldTimeElapsedFrame + WorldTimeElapsed) * 60.0f);
        WorldTimeFramesElapsed = frames_elapsed_60hz;
        WorldTimeFrames = WorldTimeFrames + frames_elapsed_60hz;
        WorldTime = WorldTimeFrames;
        WorldTimeElapsedFrame = (WorldTimeElapsedFrame + WorldTimeElapsed) - static_cast<float>(frames_elapsed_60hz) * (1.0f / 60.0f);
        WorldTimer.SetTime(Sim::GetTime());
        WorldLoopCounter = WorldLoopCounter + 1;
        WorldTimeElapsed = 0.0f;
        WorldTimeSeconds = WorldTimer.GetSeconds();
    }
}

float GetDebugRealTime() {
    extern volatile int FrameCounter;
    extern volatile unsigned int LastFrameCounterTick;
    float frame_counter_seconds = VideoFramesToSeconds(FrameCounter);
    if (LastFrameCounterTick != 0) {
        float fractional_time = bGetTickerDifference(LastFrameCounterTick, bGetTicker()) * 0.001f;
        float max_fractional_time = GetVideoFrameTime(GetVideoMode());
        if (fractional_time > max_fractional_time) {
            fractional_time = max_fractional_time;
        }
        frame_counter_seconds = frame_counter_seconds + fractional_time;
    }
    return frame_counter_seconds;
}

static void IntToString2(char *dest, int value, int num_digits, bool bLeadZero) {
    for (int n = 0; n < num_digits; n++) {
        int digit = value % 10;
        if (!bLeadZero && digit == 0 && value < 10) {
            dest[num_digits - n - 1] = ' ';
        } else {
            dest[num_digits - n - 1] = static_cast<char>(digit) + '0';
        }
        value = value / 10;
    }
}

static void PrintToString(char *string, int flags, int hours, int minutes, int seconds, int hundredths) {
    bool keep_last_digit = (flags >> 3) & 1;
    if (hours != 0) {
        int lang = GetCurrentLanguage();
        const char *format;
        if (lang == 1) {
            format = (flags & 4) ? "%d.%d.%d.%d" : " %d.%d.%d.%d";
        } else if (lang == 2) {
            format = (flags & 4) ? "%d.%d.%d.%d" : " %d.%d.%d.%d";
        } else if (lang >= 5 && (lang < 8 || (lang > 0xb && lang < 0xe))) {
            format = (flags & 4) ? "%d.%d.%d.%d" : " %d.%d.%d.%d";
        } else {
            format = (flags & 4) ? "%d:%d:%d.%d" : " %d:%d:%d.%d";
        }
        bStrCpy(string, format);
        IntToString2(string, hours, 2, false);
        IntToString2(string + 3, minutes, 2, true);
        IntToString2(string + 6, seconds, 2, true);
        IntToString2(string + 9, hundredths, 2, true);
        if ((flags & 3) != 0) {
            char temp[32];
            unsigned int numCharacters = bStrLen(string);
            bStrCpy(temp, string);
            if (keep_last_digit == 0) {
                temp[numCharacters] = '\0';
            }
            bStrCpy(string + 1, temp);
        }
    } else if (minutes != 0) {
        int lang = GetCurrentLanguage();
        const char *format;
        if (lang == 1) {
            format = (flags & 4) ? "%d.%d.%d" : " %d.%d.%d";
        } else if (lang == 2) {
            format = (flags & 4) ? "%d.%d.%d" : " %d.%d.%d";
        } else if (lang >= 5 && (lang < 8 || (lang > 0xb && lang < 0xe))) {
            format = (flags & 4) ? "%d.%d.%d" : " %d.%d.%d";
        } else {
            format = (flags & 4) ? "%d:%d.%d" : " %d:%d.%d";
        }
        bStrCpy(string, format);
        IntToString2(string, minutes, 2, false);
        IntToString2(string + 3, seconds, 2, true);
        IntToString2(string + 6, hundredths, 2, true);
        if ((flags & 3) != 0) {
            char temp[32];
            unsigned int numCharacters = bStrLen(string);
            bStrCpy(temp, string);
            if (keep_last_digit == 0) {
                temp[numCharacters] = '\0';
            }
            bStrCpy(string + 1, temp);
        }
    } else if (seconds != 0) {
        int lang = GetCurrentLanguage();
        const char *format;
        if (lang == 1) {
            format = (flags & 4) ? "%d.%d" : " %d.%d";
        } else if (lang == 2) {
            format = (flags & 4) ? "%d,%d" : " %d,%d";
        } else if (lang >= 5 && (lang < 8 || (lang > 0xb && lang < 0xe))) {
            format = (flags & 4) ? "%d,%d" : " %d,%d";
        } else {
            format = (flags & 4) ? "%d.%d" : " %d.%d";
        }
        bStrCpy(string, format);
        IntToString2(string, seconds, 2, false);
        IntToString2(string + 3, hundredths, 2, true);
        if ((flags & 3) != 0) {
            char temp[32];
            unsigned int numCharacters = bStrLen(string);
            bStrCpy(temp, string);
            if (keep_last_digit == 0) {
                temp[numCharacters] = '\0';
            }
            bStrCpy(string + 1, temp);
        }
    } else {
        int lang = GetCurrentLanguage();
        const char *format;
        if (lang == 1) {
            format = (flags & 4) ? "%d" : " %d";
        } else if (lang == 2) {
            format = (flags & 4) ? "%d" : " %d";
        } else if (lang >= 5 && (lang < 8 || (lang > 0xb && lang < 0xe))) {
            format = (flags & 4) ? "%d" : " %d";
        } else {
            format = (flags & 4) ? "%d" : " %d";
        }
        bStrCpy(string, format);
        IntToString2(string + 2, hundredths, 2, true);
        if ((flags & 3) != 0) {
            unsigned int numCharacters = bStrLen(string);
            char temp[32];
            bStrCpy(temp, string);
            if (keep_last_digit == 0) {
                temp[numCharacters] = '\0';
            }
            bStrCpy(string + 1, temp);
        }
    }
}

void Timer::GetHoursMinsSeconds(int *hours, int *minutes, int *seconds, int *thousandths_seconds) {
    int int_time = PackedTime / 4000;
    int fractional_time = PackedTime % 4000;
    *hours = int_time / 3600;
    int_time = int_time % 3600;
    *minutes = int_time / 60;
    *seconds = int_time % 60;
    if (fractional_time < 0) {
        fractional_time = fractional_time + 3;
    }
    *thousandths_seconds = fractional_time >> 2;
}

void Timer::PrintToString(char *string, int flags) {
    bool valid = false;
    if (PackedTime != 0 && PackedTime != 0x7fffffff) {
        valid = true;
    }
    if (valid) {
        int hours, minutes, seconds, thousandths_seconds;
        if (PackedTime < 0) {
            PackedTime = -PackedTime;
            GetHoursMinsSeconds(&hours, &minutes, &seconds, &thousandths_seconds);
            PackedTime = -PackedTime;
        } else {
            GetHoursMinsSeconds(&hours, &minutes, &seconds, &thousandths_seconds);
        }
        if (PackedTime < 0) {
            flags = flags | 1;
        }
        ::PrintToString(string, flags, hours, minutes, seconds, thousandths_seconds / 10);
    } else {
        ::PrintToString(string, flags, 0, 0, 0, 0);
    }
}
