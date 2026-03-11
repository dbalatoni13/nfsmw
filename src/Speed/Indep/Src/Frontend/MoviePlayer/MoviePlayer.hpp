#ifndef FRONTEND_MOVIEPLAYER_MOVIEPLAYER_H
#define FRONTEND_MOVIEPLAYER_MOVIEPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bMemory.hpp"

struct AV_PLAYER;
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

    bool IsMoviePaused() { return mMoviePaused; }

    void Stop();
    void HandleFatalError();
    void Update();
    bool IsMoviePlaying();
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

    ShapeMemoryAllocator() {}
    ~ShapeMemoryAllocator() override {}
    void* Alloc(unsigned int size, const EA::TagValuePair& flags) override;
    void* Alloc(unsigned int size);
    void Free(void* pBlock, unsigned int size) override;
    int AddRef() override;
    int Release() override;
};

#endif
