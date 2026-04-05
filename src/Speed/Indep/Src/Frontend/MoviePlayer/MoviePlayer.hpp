#ifndef FRONTEND_MOVIEPLAYER_MOVIEPLAYER_H
#define FRONTEND_MOVIEPLAYER_MOVIEPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class AV_PLAYER;
class FRAME;

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"

// total size: 0x158
class MoviePlayer {
  public:
    // total size: 0x124
    struct Settings {
        unsigned int volume;           // offset 0x0, size 0x4
        unsigned int bufferSize;       // offset 0x4, size 0x4
        unsigned int activeController; // offset 0x8, size 0x4
        int type;                      // offset 0xC, size 0x4
        int movieId;                   // offset 0x10, size 0x4
        bool preload;                  // offset 0x14, size 0x1
        bool sound;                    // offset 0x18, size 0x1
        bool loop;                     // offset 0x1C, size 0x1
        bool pal;                      // offset 0x20, size 0x1
        char filename[256];            // offset 0x24, size 0x100

        Settings() {}
        ~Settings() {}
        void operator=(const Settings &newSettings) {
            (void)newSettings;
        }
    };

    MoviePlayer(int memClass);
    ~MoviePlayer();

    void Init(Settings &newSettings);
    void ResetTimer();
    void Play();
    void Stop();
    void Pause();
    void UnPause();
    char *const GetMovieFilename();
    int GetMovieCategoryVolume();
    void GetFirstFrame();
    void DisplayTime();
    void Update();
    void UpdateFunction();
    unsigned int GetMillisecondsPerFrame();
    void HandleFatalError();
    TextureInfo *GetTexture();

    int GetStatus() {
        return fStatus;
    }

  private:
    Settings mSettings;         // offset 0x0, size 0x124
    unsigned int fCurFrameNum;  // offset 0x124, size 0x4
    int fStatus;                // offset 0x128, size 0x4
    int fLiveStatus;            // offset 0x12C, size 0x4
    unsigned int mTicker;       // offset 0x130, size 0x4
    bool mTickerFirstTime;      // offset 0x134, size 0x1
    bool mMoviePaused;          // offset 0x138, size 0x1
    int mili_seconds;           // offset 0x13C, size 0x4
    int seconds;                // offset 0x140, size 0x4
    int minutes;                // offset 0x144, size 0x4
    float milliseconds;         // offset 0x148, size 0x4
    float prevMilliseconds;     // offset 0x14C, size 0x4
    AV_PLAYER *fPlayer;         // offset 0x150, size 0x4
    FRAME *CurFrame;            // offset 0x154, size 0x4
};

#endif
