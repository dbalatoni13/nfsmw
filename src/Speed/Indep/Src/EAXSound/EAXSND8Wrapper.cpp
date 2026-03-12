#include <types.h>
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

struct SlotPool;
class EAXSound;

extern SlotPool *pCsisSlotPools[];
extern int nCsisSlotPoolSizes[];
extern void bDeleteSlotPool(SlotPool *slotpool);
extern bool IsAudioStreamingEnabled;
extern bool IsNISAudioEnabled;
extern bool IsSpeechEnabled;
extern EAXSound *g_pEAXSound;
extern "C" void SndSystem_ReInit() asm("ReInit__Q23Snd6System");

EAXSND8Wrapper::EAXSND8Wrapper() {
    m_pSoundHeap = nullptr;
    m_pStreamBuff = nullptr;
    pCsisSlotPools[0] = nullptr;
    nCsisSlotPoolSizes[0] = 0x88;
}

EAXSND8Wrapper::~EAXSND8Wrapper() {
    bDeleteSlotPool(pCsisSlotPools[0]);
    pCsisSlotPools[0] = nullptr;
}

void EAXSND8Wrapper::ReInit() {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
    } else {
        AudioSettings *settings = *reinterpret_cast<AudioSettings **>(reinterpret_cast<char *>(g_pEAXSound) + 0x34);
        eSndAudioMode mode = static_cast<eSndAudioMode>(settings->AudioMode);
        m_eCurrentAudioMode = mode;
        SetSnd8RenderMode(mode);
        SndSystem_ReInit();
    }
}

eSndAudioMode EAXSND8Wrapper::SetAudioModeFromMemoryCard(eSndAudioMode mode) {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
    } else {
        eSndAudioMode DefaultMode = GetDefaultPlatformAudioMode();
        m_eCurrentAudioMode = DefaultMode;
        if (DefaultMode == AUDIO_MODE_MIN) {
            mode = AUDIO_MODE_MIN;
        } else if (DefaultMode == AUDIO_MODE_STEREO) {
            if (mode == AUDIO_MODE_MIN) {
                mode = AUDIO_MODE_STEREO;
            }
        } else {
            mode = m_eCurrentAudioMode;
        }

        if (mode == AUDIO_MODE_STEREO) {
            OSSetSoundMode(1);
        } else if (static_cast<int>(mode) < 2) {
            if (mode == AUDIO_MODE_MIN) {
                OSSetSoundMode(0);
            }
        } else if (mode == AUDIO_MODE_MAX) {
            OSSetSoundMode(1);
        }

        m_eCurrentAudioMode = mode;
    }
    return mode;
}

void EAXSND8Wrapper::Update() {
    STUPID();
}

eSndAudioMode EAXSND8Wrapper::GetDefaultPlatformAudioMode() {
    int mode;
    if (IsSoundEnabled == 0) {
        mode = AUDIO_MODE_STEREO;
        goto ReturnMode;
    }
    {
        int sndmode = OSGetSoundMode();
        mode = 1;
        if (sndmode == 0) {
            mode = 0;
        }
    }

ReturnMode:
    return static_cast<eSndAudioMode>(mode);
}

void EAXSND8Wrapper::STUPID() {}
