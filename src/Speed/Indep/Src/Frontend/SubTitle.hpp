#ifndef FRONTEND_SUBTITLE_H
#define FRONTEND_SUBTITLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "Speed/Indep/Src/FEng/FEString.h"

// total size: 0x4
struct SubtitleInfo {
    uint16 startTime;  // offset 0x0
    uint32 stringHash; // offset 0x4
};

// total size: 0x24
class SubTitler {
  public:
    SubTitler();
    ~SubTitler();
    void BeginningMovie(const char *moviename, const char *packagename);
    void Load(const char *movieName, const char *packageName);
    void Unload();
    void Update(uint32 msg);
    void Pause();
    void UnPause();
    bool ShouldShowSubTitles(const char *movie_name);
    static void NotifyFirstFrame();
    float GetElapsedTime();
    void RefreshText();
    void SetIsTutorialMovie(const char *movieName);

  private:
    void Start();

    unsigned int next_;                          // offset 0x0, size 0x4
    struct SubtitleInfo *data_;                  // offset 0x4, size 0x4
    struct FEString *str_;                       // offset 0x8, size 0x4
    struct FEString *str2_;                      // offset 0xC, size 0x4
    struct FEObject *back_;                      // offset 0x10, size 0x4
    float timeElapsed;                           // offset 0x14, size 0x4
    unsigned int lastTime;                       // offset 0x18, size 0x4
    bool mIsTutorial;                            // offset 0x1C, size 0x1
    bool mSubtitlePaused;                        // offset 0x20, size 0x1
    static struct SubTitler *gCurrentSubtitler_; // size: 0x4, address: 0x8041BBB4
};

#endif
