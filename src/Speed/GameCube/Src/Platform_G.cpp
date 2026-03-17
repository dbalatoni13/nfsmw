#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include <dolphin/DVDPriv.h>
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

struct FILESYSOPTS {
    int size;
    EA::Allocator::IAllocator *allocator;
    int MaxOpenFiles;
    int MaxFileOps;
    int nSearchLocs;
    int nSearchPathLength;
    int MaxDevices;
    int ThreadStackSize;
    int (*decompresssize)(const void *);
    int (*decompress)(const void *, void *);
    unsigned int LargeReadSliceSize;
    unsigned int AllocAlignBoundary;
    int DiscType;
    int mErrorRetryCount;
};

void bWareInit();
void bMemoryInit();
void THREAD_init();
void TIMER_init(int slice);
void FILE_getopts(FILESYSOPTS *opts);
void FILE_setopts(FILESYSOPTS *opts);
void FILE_init(void *allocator, int allowAsync);
void ASYNCFILE_init(int numFiles, int unk);
void SYNCTASK_add(void (*task)(void *, int), int priority, int unk, void *user);
void fn_80311870(int, void *, int);

extern EA::Allocator::IAllocator &gMemoryAllocator;

void *arenaLo;
unsigned int g_GC_Disk_GameName;
int snProfilerEnable;

void InitPlatform() {
    static char profdata[0x2000];
    FILESYSOPTS opts;

    bWareInit();
    OSInit();
    DVDInit();
    VIInit();
    PADInit();
    arenaLo = OSGetArenaLo();
    bMemoryInit();
    THREAD_init();
    TIMER_init(0x64);
    g_GC_Disk_GameName = *reinterpret_cast<const unsigned int *>(DVDGetCurrentDiskID());

    opts.size = 0x38;
    FILE_getopts(&opts);
    opts.allocator = &gMemoryAllocator;
    opts.MaxOpenFiles = 0x20;
    opts.MaxFileOps = 0x40;
    FILE_setopts(&opts);

    FILE_init(0, 0);
    ASYNCFILE_init(0x10, 0);
    SYNCTASK_add(DVDErrorTask, 2, 0, 0);

    asm volatile(
        "li 3, 4\n\t"
        "oris 3, 3, 4\n\t"
        "mtspr 914, 3\n\t"
        "li 3, 5\n\t"
        "oris 3, 3, 5\n\t"
        "mtspr 915, 3\n\t"
        "li 3, 6\n\t"
        "oris 3, 3, 6\n\t"
        "mtspr 916, 3\n\t"
        "li 3, 7\n\t"
        "oris 3, 3, 7\n\t"
        "mtspr 917, 3\n\t"
        "lis 9, 0x0B07\n\t"
        "ori 9, 9, 0x0B07\n\t"
        "mtspr 917, 9\n\t"
        "lis 11, 0x0704\n\t"
        "ori 11, 11, 0x0704\n\t"
        "mtspr 918, 11\n\t"
        "lis 9, 0x0606\n\t"
        "ori 9, 9, 0x0606\n\t"
        "mtspr 919, 9");

    if (snProfilerEnable) {
        fn_80311870(0x278D, profdata, 0x2000);
    }
}

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

struct VMStats {
    unsigned int mNumPageFaults;
    unsigned int mNumWritebacks;
    float mElapsedTime;
    unsigned int mServiceTimeMicroSecs;
    unsigned int mServiceTimeMin;
    unsigned int mServiceTimeMax;
    float mServiceTimeAvg;

    void Init();
};

struct VMStatsManager {
    bool mInitialized;
    unsigned long long mFrameCounter;
    VMStats mFrameStats;
    float mElapsedTime;
    unsigned int mAccumService_us;
    unsigned int mAccumNumFaults;
    float mMinServicePercentPerFrame;
    float mMaxServicePercentPerFrame;
    unsigned int mMinNumServicesPerFrame;
    unsigned int mMaxNumServicesPerFrame;
    float mMinFrameTime;
    float mMaxFrameTime;
    const char *DebugName;

    void Init(const char *name);
};

void VMStats::Init() {
    mServiceTimeMax = 0;
    mServiceTimeAvg = 0.0f;
    mServiceTimeMin = static_cast<unsigned int>(-1);
    mNumPageFaults = 0;
    mNumWritebacks = 0;
    mElapsedTime = 0.0f;
    mServiceTimeMicroSecs = 0;
}

void VMStatsManager::Init(const char *name) {
    mFrameStats.mNumPageFaults = 0;
    mFrameStats.mNumWritebacks = 0;
    DebugName = name;
    mFrameStats.mServiceTimeMin = static_cast<unsigned int>(-1);
    mFrameCounter = 0;
    mElapsedTime = 0.0f;
    mMinNumServicesPerFrame = 9999999;
    mMaxNumServicesPerFrame = 0;
    mMinFrameTime = 9999999.0f;
    mMaxFrameTime = -9999999.0f;
    mFrameStats.mElapsedTime = 0.0f;
    mFrameStats.mServiceTimeMicroSecs = 0;
    mFrameStats.mServiceTimeMax = 0;
    mFrameStats.mServiceTimeAvg = 0.0f;
    mAccumService_us = 0;
    mAccumNumFaults = 0;
    mMinServicePercentPerFrame = 9999999.0f;
    mMaxServicePercentPerFrame = -9999999.0f;
}
