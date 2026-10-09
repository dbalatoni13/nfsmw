#ifndef __MOVIEPLAYER_HPP__
#define __MOVIEPLAYER_HPP__

#include <types.h>
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

#include "realgraph/shape.h"
#include "rcmp/rcmp.h"
#include "rcmp/av/avplayer.h"

#define VIDEO_STREAM_BUFFER_SIZE (640 * 1024) // :59
#define MOVIEPLAYER_FILENAME_LEN 256          // :62

// total size: 0x158
// Decl: 87
class MoviePlayer {
  public:
    // Decl: 95
    class Settings { // Decl: 95
      public:
        Settings() { // Decl: 95
            bufferSize = 0x40000;
            activeController = 0;
            preload = false;
            volume = 0;
            filename[0] = '\0';
            sound = IsSoundEnabled != 0;
            loop = false;
            pal = false;
            type = 0;
            movieId = 0;
        }
        ~Settings() {}                                       // Decl: 95
        void operator=(const struct Settings &newSettings) { // Decl: 95
            activeController = newSettings.activeController;
            bufferSize = newSettings.bufferSize;
            loop = newSettings.loop;
            preload = newSettings.preload;
            volume = newSettings.volume;
            sound = newSettings.sound;
            pal = newSettings.pal;
            type = newSettings.type;
            movieId = newSettings.movieId;
            bStrNCpy(filename, newSettings.filename, 256);
        }
        unsigned int volume;           // offset 0x0, size 0x4, Decl: 95
        unsigned int bufferSize;       // offset 0x4, size 0x4, Decl: 95
        unsigned int activeController; // offset 0x8, size 0x4, Decl: 95
        int type;                      // offset 0xC, size 0x4, Decl: 95
        int movieId;                   // offset 0x10, size 0x4, Decl: 95
        bool preload;                  // offset 0x14, size 0x1, Decl: 95
        bool sound;                    // offset 0x18, size 0x1, Decl: 95
        bool loop;                     // offset 0x1C, size 0x1, Decl: 95
        bool pal;                      // offset 0x20, size 0x1, Decl: 95
        char filename[256];            // offset 0x24, size 0x100, Decl: 95
    };

    MoviePlayer(int memClass);
    ~MoviePlayer();

    void Init(Settings &newSettings);
    void Play();
    void Stop();
    void Pause();
    void UnPause();
    void Update();

    TextureInfo *GetTexture();

    void ResetTimer();

    RCMP::AV_PLAYER *GetPlayer() {
        return fPlayer;
    }

    void DisplayTime();

    bool IsMoviePaused() { // Decl: 204
        return mMoviePaused;
    }

    void FillInTextureInfo(uint32 *frame_address, TextureInfo *texture_info, RealShape::Shape *shape);

    Settings GetSettings() { // Decl: 211
        return mSettings;
    }

    int GetStatus() { // Decl: 214
        return fStatus;
    }
    int GetLiveStatus() { // Decl: 215
        return fLiveStatus;
    }

    bool IsMoviePlaying() { // Decl: 217
        return this->fStatus == 3 || this->fStatus == 4 || this->fStatus == 5;
    }

    const char *GetMovieFilename();

  protected:
    void UpdateFunction();

    void GetFirstFrame(); // Decl: 237

  private:
    uint32 GetMillisecondsPerFrame();

    void HandleFatalError();

    void GetInShape();

    int GetMovieCategoryVolume();

    Settings mSettings;        // offset 0x0, size 0x124, Decl: 250
    unsigned int fCurFrameNum; // offset 0x124, size 0x4, Decl: 253
    int fStatus;               // offset 0x128, size 0x4, Decl: 256
    int fLiveStatus;           // offset 0x12C, size 0x4, Decl: 257
    unsigned int mTicker;      // offset 0x130, size 0x4, Decl: 260
    bool mTickerFirstTime;     // offset 0x134, size 0x1, Decl: 261
    bool mMoviePaused;         // offset 0x138, size 0x1, Decl: 262
    int mili_seconds;          // offset 0x13C, size 0x4, Decl: 263
    int seconds;               // offset 0x140, size 0x4, Decl: 264
    int minutes;               // offset 0x144, size 0x4, Decl: 265
    float milliseconds;        // offset 0x148, size 0x4, Decl: 266
    float prevMilliseconds;    // offset 0x14C, size 0x4, Decl: 267
    RCMP::AV_PLAYER *fPlayer;  // offset 0x150, size 0x4
    RCMP::FRAME *CurFrame;     // offset 0x154, size 0x4
};

extern MoviePlayer *gMoviePlayer;
extern unsigned int gMovieStartTime;

#define MoviePlayer_Init(_a) gMoviePlayer->Init(_a) // :308

bool MoviePlayer_Bypass();
void MoviePlayer_Play();
void MoviePlayer_StartUp();
void MoviePlayer_ShutDown();
bool GiveTheMoviePlayerBandwidth();

#endif
