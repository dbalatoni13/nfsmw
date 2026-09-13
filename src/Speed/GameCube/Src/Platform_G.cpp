#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "dolphin.h"

#include <dolphin/os/OSFastCast.h>

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

    static cFEng *Get() {
        return mInstance;
    }

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
int FILE_setopts(FILESYSOPTS *opts);
int FILE_init(void *allocator, int allowAsync);
void ASYNCFILE_init(int numFiles, int unk);
void SYNCTASK_add(void (*task)(void *, int), int priority, int unk, void *user);
void fn_80311870(int, void *, int);

extern EA::Allocator::IAllocator &gMemoryAllocator;

void *arenaLo;
char g_GC_Disk_GameName[4];
int snProfilerEnable = 0;

void InitPlatform() {
    FILESYSOPTS fso;
    static unsigned long profdata[2048];

    bWareInit();
    OSInit();
    DVDInit();
    VIInit();
    PADInit();
    arenaLo = OSGetArenaLo();
    bMemoryInit();
    THREAD_init();
    TIMER_init(0x64);
    *reinterpret_cast<unsigned int *>(g_GC_Disk_GameName) = *reinterpret_cast<const unsigned int *>(DVDGetCurrentDiskID());

    fso.size = 0x38;
    FILE_getopts(&fso);
    fso.allocator = &gMemoryAllocator;
    fso.DiscType = 1;
    fso.MaxOpenFiles = 0x20;
    fso.MaxFileOps = 0x40;
    FILE_setopts(&fso);

    FILE_init(nullptr, 0);
    ASYNCFILE_init(0x10, 0);
    SYNCTASK_add(DVDErrorTask, 2, 0, 0);

    OSInitFastCast();
    OSSetGQR5(OS_GQR_S16, 11);
    OSSetGQR6(OS_GQR_U8, 7);
    OSSetGQR7(OS_GQR_S8, 6);

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
        int video_mode = MODE_PAL60;

        SetVideoMode(static_cast<VIDEO_MODE>(video_mode));
        eSetDisplaySystem(GetVideoMode());
    } else {
        int video_mode = GetBuildRegionVideoMode();

        SetVideoMode(static_cast<VIDEO_MODE>(video_mode));
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
    int errorstate;

    switch (error) {
        case 5: errorstate = 5; break;
        case 4: errorstate = 4; break;
        case 6: errorstate = 6; break;
        case 11: errorstate = 11; break;
        case -1: errorstate = -1; break;
        default: errorstate = 0; break;
    }

    return errorstate;
}

void DVDErrorTask(void *, int) {
    int dvdstatus;
    int dvderrorhappened = 0;
    int dvderrormessagehash = 0;

    static int resetButtonPressed;
    static int queuedSavingResetButtonPressed;
    static int resetMode = -1;
    static int softwareResetCheckStarted;
    static u32 softwareResetStartTick;

    int softwareResetTriggered = 0;
    bool cleanUp = false;
    const int resetButtonCombo = 0x1600;
    eLanguages current_language = static_cast<eLanguages>(0);
    int dvderrorticks = 0;
    int last_anim_tick = 0;
    int current_anim_tick = 0;
    char strip_chars = 0;

    static int num_queued_resets;

    unsigned int nextFrame;

    do {
        IOModule::GetIOModule().Update();

        if (cFEngJoyInput::mInstance != 0) {
            cFEngJoyInput::mInstance->HandleJoy();
        }

        if (!FinishedLoadingGlobalSuccesful) {
            ActualReadJoystickData();
        }

        /* Check for software reset combo (L+R+Start = 0x1600) on pad 0 or pad 1 */
        if ((HardwarePadStatus[0].button & resetButtonCombo) == resetButtonCombo ||
            (HardwarePadStatus[1].button & resetButtonCombo) == resetButtonCombo) {
            if (!softwareResetCheckStarted) {
                softwareResetStartTick = OSGetTick();
                softwareResetCheckStarted = 1;
            } else {
                u32 current_tick = OSGetTick();
                u32 diff_tick = (current_tick - softwareResetStartTick) / (OS_BUS_CLOCK / 4000);

                if (diff_tick > 500) {
                    resetMode = 0;
                    softwareResetTriggered = 1;
                }
            }
        } else {
            softwareResetCheckStarted = 0;
        }

        if (MemoryCard::IsCardBusy()) {
            /* Card is busy - check for reset button press and queue it */
            if (OSGetResetSwitchState() || softwareResetTriggered) {
                queuedSavingResetButtonPressed = 1;
            } else if (queuedSavingResetButtonPressed) {
                resetButtonPressed = 1;
                num_queued_resets = num_queued_resets + 1;
            }

            nextFrame = dvderrorticks + 1;
            if (MemoryCard::GetInstance() != 0) {
                MemoryCard::GetInstance()->Tick(16);
            }
            goto loop_end;
        }

        if (dvderrorhappened != 0) {
            unsigned long MotorRumble[4];

            /* Error state active - run sync tasks and handle input */
            bSyncTaskRun();
            if (MemoryCard::GetInstance() != 0) {
                MemoryCard::GetInstance()->Tick(16);
            }

            {
                int port;

                DVDCheckDisk();

                bMemSet(MotorRumble, 0, 16);
                MotorRumble[0] = 2;
                MotorRumble[1] = 2;
                MotorRumble[2] = 2;
                MotorRumble[3] = 2;
                PADControlAllMotors(MotorRumble);

                LGWheels_ReadAll(plat_lgwheels);
                for (port = 0; port <= 3; port++) {
                    if (LGWheels_IsConnected(plat_lgwheels, port)) {
                        LGWheels_StopConstantForce(plat_lgwheels, port);
                        LGWheels_StopSurfaceEffect(plat_lgwheels, port);
                        LGWheels_StopDamperForce(plat_lgwheels, port);
                        LGWheels_StopCarAirborne(plat_lgwheels, port);
                        LGWheels_StopSlipperyRoadEffect(plat_lgwheels, port);
                        LGWheels_PlaySpringForce(plat_lgwheels, port,
                            *(signed char *)((char *)plat_lgwheels + port * 10 + 3),
                            0xb4, 0xb4);
                    }
                }
            }
        }

        /* Check for hardware reset button */
        if (num_queued_resets == 0 && resetMode == -1 && OSGetResetSwitchState()) {
            resetButtonPressed = 1;
        } else if (num_queued_resets > 0 || resetButtonPressed) {
            resetMode = 0;
        }

        dvdstatus = DVDGetDriveStatus();

        if (dvdstatus != -1 && resetMode != -1) {
            int reset_mode = resetMode;
            resetMode = -1;
            CheckReset(reset_mode);
        }

        /* Map drive status to error index */
        switch (dvdstatus) {
            case 5:
                dvderrormessagehash = 0;
                break;
            case 4:
                dvderrormessagehash = 1;
                break;
            case 6:
                dvderrormessagehash = 2;
                break;
            case 11:
                dvderrormessagehash = 3;
                break;
            case -1:
                dvderrormessagehash = 4;
                break;
        }

        if (MemoryCard::IsCardBusy()) {
            return;
        }

        dvderrorhappened = DVDValidErrorState(dvdstatus);
        if (dvderrorhappened != 0) {
            /* New error detected */
            current_language = GC_GetOSLanguage();
            g_discErrorNumber = dvderrorhappened;
            g_discErrorOccured = 1;
            if (gMoviePlayer != 0) {
                gMoviePlayer->Stop();
            }
            SoundPause(true, -1);
            SetSoundControlState(true, 0x10, "GC Error");
            if (g_pEAXSound != 0) {
                g_pEAXSound->Update(0.1f);
            }

            cFEng *feng = cFEng::Get();
            if (!feng->IsPackagePushed("DiscError.fng")) {
                feng->PushErrorPackage("DiscError.fng", 0, 0xff);
            }

            FEPrintf("DiscError.fng", 0xEEFFD04F,
                s_OpenCover_ErrorText[current_language][dvderrormessagehash]);
            nextFrame = dvderrorticks + 1;
        } else if (g_discErrorOccured == 0) {
            nextFrame = dvderrorticks + 1;
            break;
        } else {
            /* Disc error was active, check if we should service streaming */
            if (!TheTrackStreamer.HasUserMemoryAllocations() && TheTrackStreamer.IsLoadingInProgressNonRepeatable()) {
                ServiceResourceLoading();
                dvdstatus = 1;
                TheTrackStreamer.ServiceNonGameState();
                TheTrackStreamer.ServiceGameState();
            }

            if (dvdstatus != 0) {
                /* Scrolling text display */
                char the_loading_text[16];
                int to_copy;
                char copy_length;
                int anim_frames;

                copy_length = static_cast<char>(bStrLen(
                    s_OpenCover_ErrorText[current_language][5]));
                bMemSet(the_loading_text, 0, 16);

                anim_frames = 0x10;
                if (IsGameFlowInGame()) {
                    anim_frames = 0x40;
                }

                nextFrame = dvderrorticks + 1;
                if ((dvderrorticks & anim_frames) != (last_anim_tick & anim_frames)) {
                    int rem;

                    rem = current_anim_tick;
                    if (current_anim_tick < 0) {
                        rem = current_anim_tick + 3;
                    }
                    rem = rem & ~3;
                    last_anim_tick = dvderrorticks;
                    strip_chars = (signed char)(3 - (current_anim_tick - rem));
                    current_anim_tick = current_anim_tick + 1;
                }

                bStrNCpy(the_loading_text,
                    s_OpenCover_ErrorText[current_language][5],
                    copy_length - strip_chars);

                to_copy = bStrLen(the_loading_text);
                while (to_copy <= copy_length) {
                    to_copy = to_copy + 1;
                    bStrCat(the_loading_text, the_loading_text, " ");
                }

                FEPrintf("DiscError.fng", 0xEEFFD04F, the_loading_text);

                if (MemoryCard::GetInstance() != 0) {
                    MemoryCard::GetInstance()->Tick(16);
                }
            } else {
                /* Error resolved */
                g_discErrorOccured = 0;
                g_discErrorNumber = 0;
                dvderrorhappened = 0;

                SoundPause(false, -1);
                SetSoundControlState(false, 0x10, "GC Error");
                if (g_pEAXSound != 0) {
                    g_pEAXSound->Update(0.1f);
                }

                bool wasMovieActive = false;
                if (gMoviePlayer != 0) {
                    wasMovieActive = true;
                    gMoviePlayer->Stop();
                }

                cFEng *feng;

                cFEng::Get()->MakeLoadedPackagesDirty();
                if (cFEng::Get()->IsPackagePushed("DiscError.fng")) {
                    cFEng::Get()->PopErrorPackage();
                }
                nextFrame = dvderrorticks + 1;
                if (wasMovieActive) {
                    cFEng::Get()->QueueGameMessage(0xC3960EB9, 0, 0xff);
                }
            }
        }

        /* Render error screen if disc error is active */
        if (g_discErrorOccured != 0) {
            FEngTickSinglePackage(FEngDiscErrorPackage, dvderrorticks);
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
                PADStatus LocalHardwarePadStatus[4];

                PADRead(LocalHardwarePadStatus);
                {
                    int pad_state_0 = LocalHardwarePadStatus[0].err;
                    int pad_state_1 = LocalHardwarePadStatus[1].err;

                    if (pad_state_0 == 0) {
                        bMemCpy(&HardwarePadStatus[0], &LocalHardwarePadStatus[0], 0xc);
                    }
                    if (pad_state_1 == 0) {
                        bMemCpy(&HardwarePadStatus[1], &LocalHardwarePadStatus[1], 0xc);
                    }
                }
            }
        }

    loop_end:
        dvderrorticks = nextFrame;
    } while (g_discErrorOccured != 0);
}

void ServicePlatform() {}

void eInitTexture() {}

void eUnSwizzle8bitPalette(unsigned int *palette) {}

void eSwizzle8bitPalette(unsigned int *palette) {}

struct VMStats {
    VMStats() {}

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

inline void VMStats::Init() {
    mNumPageFaults = 0;
    mNumWritebacks = 0;
    mElapsedTime = 0.0f;
    mServiceTimeMicroSecs = 0;
    mServiceTimeMin = static_cast<unsigned int>(-1);
    mServiceTimeMax = 0;
    mServiceTimeAvg = 0.0f;
}

void VMStatsManager::Init(const char *name) {
    mFrameStats.Init();
    mFrameCounter = 0;
    mElapsedTime = 0.0f;
    mAccumService_us = 0;
    mAccumNumFaults = 0;
    mMinServicePercentPerFrame = 9999999.0f;
    mMaxServicePercentPerFrame = -9999999.0f;
    mMinNumServicesPerFrame = 9999999;
    mMaxNumServicesPerFrame = 0;
    mMinFrameTime = 9999999.0f;
    mMaxFrameTime = -9999999.0f;
    DebugName = name;
}
