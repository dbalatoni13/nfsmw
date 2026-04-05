#ifndef EAXSOUND_EAXSND8WRAPPER_H
#define EAXSOUND_EAXSND8WRAPPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

// total size: 0x1C
struct EAXSND8Wrapper : public AudioMemBase {
    char *m_pSoundHeap;                 // offset 0x4, size 0x4
    char *m_pStreamBuff;                // offset 0x8, size 0x4
    int m_nHeapSize;                    // offset 0xC, size 0x4
    int m_nStreamSize;                  // offset 0x10, size 0x4
    eSndAudioMode m_eCurrentAudioMode;  // offset 0x14, size 0x4
    eSndAudioMode m_eLastAudioMode;     // offset 0x18, size 0x4

    void *operator new(unsigned int size, const char *debug_name) {
        return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
    }

    void *operator new(size_t, void *p) { return p; }

    EAXSND8Wrapper();
    virtual ~EAXSND8Wrapper();
    bool Initialize();
    eSndAudioMode GetDefaultPlatformAudioMode();
    eSndAudioMode SetAudioModeFromMemoryCard(eSndAudioMode mode);
    eSndAudioMode SetAudioRenderMode(eSndAudioMode mode);
    void ReInit();
    eSndAudioMode SetSnd8RenderMode(eSndAudioMode mode);
    void Update();
    void STUPID();
};

#endif
