#include <types.h>
#include "Speed/Indep/Src/Misc/Config.h"

struct SlotPool;

extern SlotPool *pCsisSlotPools[];
extern int nCsisSlotPoolSizes[];

EAXSND8Wrapper::EAXSND8Wrapper() {
    m_pSoundHeap = nullptr;
    m_pStreamBuff = nullptr;
    pCsisSlotPools[0] = nullptr;
    nCsisSlotPoolSizes[0] = 0x88;
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
