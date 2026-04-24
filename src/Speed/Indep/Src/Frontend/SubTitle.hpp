#ifndef FRONTEND_SUBTITLE_H
#define FRONTEND_SUBTITLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct FEObject;
struct FEString;

// total size: 0x4
struct SubtitleInfo {
    unsigned short startTime; // offset 0x0
    unsigned int stringHash;  // offset 0x4
};

// total size: 0x24
struct SubTitler {
    unsigned int next_;        // offset 0x0
    SubtitleInfo* data_;       // offset 0x4
    FEString* str_;            // offset 0x8
    FEString* str2_;           // offset 0xC
    FEObject* back_;           // offset 0x10
    float timeElapsed;         // offset 0x14
    unsigned int lastTime;     // offset 0x18
    bool mIsTutorial;          // offset 0x1C
    int mSubtitlePaused;      // offset 0x20

    static SubTitler* gCurrentSubtitler_;

    SubTitler();
    ~SubTitler();
    void BeginningMovie(const char*, const char*);
    void Load(const char*, const char*);
    void Unload();
    float GetElapsedTime();
    void Update(unsigned int);
    void Start();
    static void NotifyFirstFrame();
    void RefreshText();
    void SetIsTutorialMovie(const char*);
    bool ShouldShowSubTitles(const char*);
};

#endif
