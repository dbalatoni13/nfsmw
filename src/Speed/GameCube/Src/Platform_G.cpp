#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "dolphin.h"

enum VIDEO_MODE {
    MODE_PAL = 0,
    MODE_PAL60 = 1,
    MODE_NTSC = 2,
    NUM_VIDEO_MODES = 3,
};

enum eLanguages {
    eLANGUAGE_NONE = -1,
    eLANGUAGE_FIRST = 0,
    eLANGUAGE_ENGLISH = 0,
    eLANGUAGE_FRENCH = 1,
    eLANGUAGE_GERMAN = 2,
    eLANGUAGE_ITALIAN = 3,
    eLANGUAGE_SPANISH = 4,
    eLANGUAGE_DUTCH = 5,
    eLANGUAGE_SWEDISH = 6,
    eLANGUAGE_DANISH = 7,
    eLANGUAGE_KOREAN = 8,
    eLANGUAGE_CHINESE = 9,
    eLANGUAGE_JAPANESE = 10,
    eLANGUAGE_THAI = 11,
    eLANGUAGE_POLISH = 12,
    eLANGUAGE_FINNISH = 13,
    eLANGUAGE_LARGEST = 14,
    eLANGUAGE_LABELS = 15,
    eLANGUAGE_MAX = 16,
};

extern bool bEURGB60;
extern "C" void OSResetSystem(BOOL reset, u32 resetCode, BOOL forceMenu);

void FlushCaches() {
    PPCSync();
}

void EnableInterrupts() {
    OSEnableInterrupts();
}

VIDEO_MODE GetVideoMode();
void SetVideoMode(VIDEO_MODE mode);
VIDEO_MODE GetBuildRegionVideoMode();
int eSetDisplaySystem(int video_mode);

void InitDisplaySystem() {
    if (bEURGB60) {
        SetVideoMode(MODE_PAL60);
        eSetDisplaySystem(GetVideoMode());
    } else {
        SetVideoMode(GetBuildRegionVideoMode());
        eSetDisplaySystem(GetVideoMode());
    }
}

void FinishedRenderingFEngLayer() {}

int bDoWithStack(void *function, void *stack_pointer, int arg1, int arg2) {
    return 0;
}

enum eLanguages GC_GetOSLanguage() {
    if (BuildRegion::IsEuropeFr()) {
        return eLANGUAGE_FRENCH;
    }
    if (BuildRegion::IsEuropeGer()) {
        return eLANGUAGE_GERMAN;
    }
    if (BuildRegion::IsJapan()) {
        return eLANGUAGE_JAPANESE;
    }
    return eLANGUAGE_ENGLISH;
}

void CheckReset(int resetMode) {
    if (!MemoryCard::IsCardBusy()) {
        VISetBlack(1);
        VIFlush();
        VIWaitForRetrace();
        VISetBlack(1);
        VIFlush();
        VIWaitForRetrace();
        OSResetSystem(resetMode, 1, 0);
    }
}

int DVDValidErrorState(int error) {
    switch (error) {
        case 5:
        case 4:
        case 6:
        case 11:
        case -1:
            return error;
        default:
            return 0;
    }
}

void ServicePlatform() {}

void eInitTexture() {}

void eUnSwizzle8bitPalette(unsigned int *palette) {}

void eSwizzle8bitPalette(unsigned int *palette) {}
