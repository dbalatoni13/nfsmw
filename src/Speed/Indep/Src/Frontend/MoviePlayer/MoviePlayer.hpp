#ifndef FRONTEND_MOVIEPLAYER_MOVIEPLAYER_H
#define FRONTEND_MOVIEPLAYER_MOVIEPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bMemory.hpp"

struct FRAME;
struct AV_PLAYER {
    ~AV_PLAYER();
    FRAME* GetFirstFrame(unsigned int MaxFramesOutstanding, int VideoLatencyInMs);
    int Pause();
    int UnPause();
    int SetVol(unsigned int Vol);
};
struct FRAME;

// total size: 0x158
struct MoviePlayer {
    // total size: 0x124
    struct Settings {
        unsigned int volume;           // offset 0x0
        unsigned int bufferSize;       // offset 0x4
        unsigned int activeController; // offset 0x8
        int type;                      // offset 0xC
        int movieId;                   // offset 0x10
        bool preload;                  // offset 0x14
        bool sound;                    // offset 0x18
        bool loop;                     // offset 0x1C
        bool pal;                      // offset 0x20
        char filename[256];            // offset 0x24
    };

    Settings mSettings;                // offset 0x0
    unsigned int fCurFrameNum;         // offset 0x124
    int fStatus;                       // offset 0x128
    int fLiveStatus;                   // offset 0x12C
    unsigned int mTicker;              // offset 0x130
    bool mTickerFirstTime;             // offset 0x134
    bool mMoviePaused;                 // offset 0x138
    int mili_seconds;                  // offset 0x13C
    int seconds;                       // offset 0x140
    int minutes;                       // offset 0x144
    float milliseconds;                // offset 0x148
    float prevMilliseconds;            // offset 0x14C
    AV_PLAYER* fPlayer;               // offset 0x150
    FRAME* CurFrame;                   // offset 0x154

    AV_PLAYER* GetPlayer() { return fPlayer; }
    bool IsMoviePaused() { return mMoviePaused; }
    Settings GetSettings() { return mSettings; }
    int GetStatus() { return fStatus; }
    int GetLiveStatus() { return fLiveStatus; }
    bool IsMoviePlaying();

    MoviePlayer(int memClass);
    ~MoviePlayer();
    void Init(Settings& newSettings);
    void ResetTimer();
    void Play();
    void Stop();
    void Pause();
    void UnPause();
    char* const GetMovieFilename();
    int GetMovieCategoryVolume();
    void GetFirstFrame();
    void DisplayTime();
    void Update();
    void UpdateFunction();
    unsigned int GetMillisecondsPerFrame();
    void HandleFatalError();
};

extern MoviePlayer* gMoviePlayer;
extern unsigned int gMovieStartTime;

bool MoviePlayer_Bypass();
void MoviePlayer_Play();
void MoviePlayer_StartUp();
void MoviePlayer_ShutDown();
bool GiveTheMoviePlayerBandwidth();

struct ShapeMemoryAllocator : public EA::Allocator::IAllocator {
    int mRefcount; // offset 0x4, size 0x4

    ShapeMemoryAllocator() : mRefcount(1) {}
    ~ShapeMemoryAllocator() override {}
    void* Alloc(unsigned int size, const EA::TagValuePair& flags) override;
    void* Alloc(unsigned int size);
    void Free(void* pBlock, unsigned int size) override;
    int AddRef() override;
    int Release() override;
};

#endif
