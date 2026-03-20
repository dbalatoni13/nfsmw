#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include <dolphin/DVDPriv.h>
#include "dolphin.h"

/* LGWheels is defined later in the unity build, so forward-declare wrappers with asm labels */
class LGWheels;
extern LGWheels *plat_lgwheels;

void LGWheels_ReadAll(LGWheels *) asm("ReadAll__8LGWheels");
int LGWheels_IsConnected(LGWheels *, long) asm("IsConnected__8LGWheelsl");
void LGWheels_StopConstantForce(LGWheels *, long) asm("StopConstantForce__8LGWheelsl");
void LGWheels_StopSurfaceEffect(LGWheels *, long) asm("StopSurfaceEffect__8LGWheelsl");
void LGWheels_StopDamperForce(LGWheels *, long) asm("StopDamperForce__8LGWheelsl");
void LGWheels_StopCarAirborne(LGWheels *, long) asm("StopCarAirborne__8LGWheelsl");
void LGWheels_StopSlipperyRoadEffect(LGWheels *, long) asm("StopSlipperyRoadEffect__8LGWheelsl");
void LGWheels_PlaySpringForce(LGWheels *, long, signed char, unsigned char, short) asm("PlaySpringForce__8LGWheelslScUcs");

class IOModule {
public:
    static IOModule &GetIOModule();
    void Update();
};

class cFEngJoyInput {
public:
    static cFEngJoyInput *mInstance;
    void HandleJoy();
};

class FEObject;

class cFEng {
public:
    static cFEng *mInstance;
    void MakeLoadedPackagesDirty();
    int IsPackagePushed(const char *);
    void PushErrorPackage(const char *, int, unsigned long);
    void PopErrorPackage();
    void QueueGameMessage(unsigned int, const char *, unsigned int);
    void QueuePackageMessage(unsigned int, const char *, FEObject *);
};

class EAXSound {
public:
    void Update(float);
};

class TextureInfo;
namespace RealShape { class Shape; }

struct MoviePlayer {
    void Stop();
    void FillInTextureInfo(unsigned int *, TextureInfo *, RealShape::Shape *);
};

class FEManager {
public:
    static FEManager *Get();
    void Render();
};

void bSyncTaskRun();
int DVDCheckDisk();
void SoundPause(bool, int);
void SetSoundControlState(bool, int, const char *);
void FEPrintf(const char *, int, const char *, ...);
void FEngTickSinglePackage(const char *, unsigned int);
void eBeginScene();
void eEndScene();
int ServiceResourceLoading();
int bStrLen(const char *);
char *bStrNCpy(char *, const char *, int);
char *bStrCat(char *, char const *, char const *);
int ActualReadJoystickData();
extern "C" {
void bMemSet(void *, unsigned char, unsigned int);
void bMemCpy(void *, const void *, unsigned int);
}

extern int FinishedLoadingGlobalSuccesful;
extern int g_discErrorOccured;
extern int g_discErrorNumber;
extern EAXSound *g_pEAXSound;
extern MoviePlayer *gMoviePlayer;
extern const char *s_OpenCover_ErrorText[][6];
extern const char FEngDiscErrorPackage[];
extern PADStatus HardwarePadStatus[4];

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

extern char bEURGB60;
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
int snProfilerEnable = 0;

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
    opts.DiscType = 1;
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

extern "C" int bDoWithStack(void *function, void *stack_pointer, int arg1, int arg2) {
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
            return 5;
        case 4:
            return 4;
        case 6:
            return 6;
        case 11:
            return 11;
        case -1:
            return -1;
        default:
            return 0;
    }
}

void DVDErrorTask(void *, int) {
    static int resetButtonPressed;
    static int queuedSavingResetButtonPressed;
    static int resetMode = -1;
    static int softwareResetCheckStarted;
    static u32 softwareResetStartTick;
    static int num_queued_resets;

    int errorIndex = 0;
    unsigned int frame = 0;
    int scrollIndex = 0;
    int resetButtonPressedLocal = 0;
    int language = 0;
    unsigned int prevButtons = 0;
    int scrollOffset = 0;
    int errorState = 0;
    unsigned int nextFrame;
    int driveStatus;
    int movieWasPlaying;
    long ch;
    int textLen;
    int scrollLen;
    int buttonMask;
    cFEng *feng;
    const char *pkgName;
    char textBuf[16];
    PADStatus padBuf[4];

    do {
        IOModule::GetIOModule().Update();

        if (cFEngJoyInput::mInstance != 0) {
            cFEngJoyInput::mInstance->HandleJoy();
        }

        if (!FinishedLoadingGlobalSuccesful) {
            ActualReadJoystickData();
        }

        /* Check for software reset combo (L+R+Start = 0x1600) on pad 0 or pad 1 */
        if ((HardwarePadStatus[0].button & 0x1600) == 0x1600 ||
            (HardwarePadStatus[1].button & 0x1600) == 0x1600) {
            if (!softwareResetCheckStarted) {
                softwareResetStartTick = OSGetTick();
                softwareResetCheckStarted = 1;
            } else {
                u32 currentTick = OSGetTick();
                u32 ticksPerMs = OS_BUS_CLOCK / 4000;
                u32 elapsed = currentTick - softwareResetStartTick;
                u32 msElapsed = elapsed / ticksPerMs;
                if (msElapsed > 500) {
                    resetMode = 0;
                    resetButtonPressedLocal = 1;
                }
            }
        } else {
            softwareResetCheckStarted = 0;
        }

        if (MemoryCard::IsCardBusy()) {
            /* Card is busy - check for reset button press and queue it */
            if (OSGetResetSwitchState() || resetButtonPressedLocal) {
                queuedSavingResetButtonPressed = 1;
            } else if (queuedSavingResetButtonPressed) {
                resetButtonPressed = 1;
                num_queued_resets = num_queued_resets + 1;
            }

            nextFrame = frame + 1;
            if (MemoryCard::s_pThis != 0) {
                MemoryCard::s_pThis->Tick(16);
            }
            goto loop_end;
        }

        if (errorState != 0) {
            /* Error state active - run sync tasks and handle input */
            bSyncTaskRun();
            if (MemoryCard::s_pThis != 0) {
                MemoryCard::s_pThis->Tick(16);
            }
            DVDCheckDisk();

            bMemSet(textBuf, 0, 16);
            *(u32 *)&textBuf[0] = 2;
            *(u32 *)&textBuf[4] = 2;
            *(u32 *)&textBuf[8] = 2;
            *(u32 *)&textBuf[12] = 2;
            PADControlAllMotors((const u32 *)textBuf);

            LGWheels_ReadAll(plat_lgwheels);
            for (ch = 0; ch <= 3; ch++) {
                if (LGWheels_IsConnected(plat_lgwheels, ch)) {
                    LGWheels_StopConstantForce(plat_lgwheels, ch);
                    LGWheels_StopSurfaceEffect(plat_lgwheels, ch);
                    LGWheels_StopDamperForce(plat_lgwheels, ch);
                    LGWheels_StopCarAirborne(plat_lgwheels, ch);
                    LGWheels_StopSlipperyRoadEffect(plat_lgwheels, ch);
                    LGWheels_PlaySpringForce(plat_lgwheels, ch,
                        *(signed char *)((char *)plat_lgwheels + ch * 10 + 3),
                        0xb4, 0xb4);
                }
            }
        }

        /* Check for hardware reset button */
        if (num_queued_resets == 0 && resetMode == -1) {
            if (OSGetResetSwitchState()) {
                resetButtonPressed = 1;
            }
        } else if (num_queued_resets > 0 || resetButtonPressed) {
            resetMode = 0;
        }

        driveStatus = DVDGetDriveStatus();

        if (driveStatus != -1 && resetMode != -1) {
            int mode = resetMode;
            resetMode = -1;
            CheckReset(mode);
        }

        /* Map drive status to error index */
        switch (driveStatus) {
            case 5:
                errorIndex = 0;
                break;
            case 4:
                errorIndex = 1;
                break;
            case 6:
                errorIndex = 2;
                break;
            case 11:
                errorIndex = 3;
                break;
            case -1:
                errorIndex = 4;
                break;
        }

        if (MemoryCard::IsCardBusy()) {
            return;
        }

        errorState = DVDValidErrorState(driveStatus);
        if (errorState != 0) {
            /* New error detected */
            language = GC_GetOSLanguage();
            g_discErrorNumber = errorState;
            g_discErrorOccured = 1;
            if (gMoviePlayer != 0) {
                gMoviePlayer->Stop();
            }
            SoundPause(true, -1);
            SetSoundControlState(true, 0x10, "GC Error");
            if (g_pEAXSound != 0) {
                g_pEAXSound->Update(0.1f);
            }

            pkgName = "DiscError.fng";
            if (!cFEng::mInstance->IsPackagePushed(pkgName)) {
                cFEng::mInstance->PushErrorPackage(pkgName, 0, 0xff);
            }

            nextFrame = frame + 1;
            FEPrintf(pkgName, 0xEEFFD04F,
                s_OpenCover_ErrorText[language][errorIndex]);
        } else if (g_discErrorOccured == 0) {
            nextFrame = frame + 1;
            goto loop_end;
        } else {
            /* Disc error was active, check if we should service streaming */
            nextFrame = frame + 1;

            if (!(TheTrackStreamer.UserMemoryAllocationSize > 0) &&
                (TheTrackStreamer.LoadingPhase != TrackStreamer::LOADING_IDLE)) {
                ServiceResourceLoading();
                driveStatus = 1;
                TheTrackStreamer.ServiceNonGameState();
                TheTrackStreamer.ServiceGameState();
            }

            if (driveStatus != 0) {
                /* Scrolling text display */
                scrollLen = (signed char)bStrLen(
                    s_OpenCover_ErrorText[language][errorIndex]);

                bMemSet(textBuf, 0, 16);

                buttonMask = 0x10;
                if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING) {
                    buttonMask = 0x40;
                }

                if ((frame & buttonMask) != (prevButtons & buttonMask)) {
                    int rem;
                    prevButtons = frame;
                    rem = scrollIndex;
                    if (scrollIndex < 0) {
                        rem = scrollIndex + 3;
                    }
                    rem = rem & ~3;
                    scrollOffset = (signed char)(3 - (scrollIndex - rem));
                    scrollIndex = scrollIndex + 1;
                }

                bStrNCpy(textBuf,
                    s_OpenCover_ErrorText[language][errorIndex],
                    scrollLen - scrollOffset);

                nextFrame = frame + 1;
                textLen = bStrLen(textBuf);
                while (textLen <= scrollLen) {
                    bStrCat(textBuf, textBuf, " ");
                    textLen = textLen + 1;
                }

                FEPrintf("DiscError.fng", 0xEEFFD04F, textBuf);

                if (MemoryCard::s_pThis != 0) {
                    MemoryCard::s_pThis->Tick(16);
                }
            } else {
                /* Error resolved */
                g_discErrorNumber = 0;
                g_discErrorOccured = 0;
                errorState = 0;

                SoundPause(false, -1);
                SetSoundControlState(false, 0x10, "GC Error");
                if (g_pEAXSound != 0) {
                    g_pEAXSound->Update(0.1f);
                }

                movieWasPlaying = 0;
                if (gMoviePlayer != 0) {
                    movieWasPlaying = 1;
                    gMoviePlayer->Stop();
                }

                cFEng::mInstance->MakeLoadedPackagesDirty();
                if (cFEng::mInstance->IsPackagePushed("DiscError.fng")) {
                    cFEng::mInstance->PopErrorPackage();
                }
                nextFrame = frame + 1;
                if (movieWasPlaying) {
                    cFEng::mInstance->QueueGameMessage(0xC3960EB9, 0, 0xff);
                }
            }
        }

        /* Render error screen if disc error is active */
        if (g_discErrorOccured != 0) {
            FEngTickSinglePackage(FEngDiscErrorPackage, frame);
            eBeginScene();
            FEManager::Get()->Render();
            eEndScene();

            /* Read Logitech wheel data for pad input */
            if (LGWheels_IsConnected(plat_lgwheels, 0) ||
                LGWheels_IsConnected(plat_lgwheels, 1)) {
                LGWheels_ReadAll(plat_lgwheels);
                HardwarePadStatus[0].button = *(u16 *)((char *)plat_lgwheels);
                HardwarePadStatus[1].button = *(u16 *)((char *)plat_lgwheels + 10);
            } else {
                PADRead(padBuf);
                if (padBuf[0].err == 0) {
                    bMemCpy(&HardwarePadStatus[0], &padBuf[0], 0xc);
                }
                if (padBuf[1].err == 0) {
                    bMemCpy(&HardwarePadStatus[1], &padBuf[1], 0xc);
                }
            }
        }

    loop_end:
        frame = nextFrame;
    } while (g_discErrorOccured != 0);
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

    VMStatsManager(const char *name) {
        mFrameStats.Init();
        Init(name);
        mInitialized = false;
    }

    void Init(const char *name);
};

VMStatsManager gVMStatsManager_FE("Frontend");
VMStatsManager gVMStatsManager_LS("LoadScreen Streamer");
VMStatsManager gVMStatsManager_IG("InGame");

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
