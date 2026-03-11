#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"

struct TextureInfo;
struct Shape;
struct DECODER;

extern TrackStreamer TheTrackStreamer;
extern bool IsSoundEnabled;
extern int SkipMovies;
extern bool ShutJosieUp;
extern TextureInfo MovieTextureInfo;
extern void* RCMPDecodeBuffer;

extern int bStrNICmp(const char*, const char*, int);

typedef void* (*RCMP_AllocFunc)(const char*, unsigned int, int, int, int);
typedef void (*RCMP_FreeFunc)(void*);

extern RCMP_AllocFunc RCMP_PlayerAllocAlign;
extern RCMP_FreeFunc RCMP_PlayerFree_func;

struct RCMP_System {
    RCMP_AllocFunc AllocMem;
    RCMP_FreeFunc FreeMem;
    int memClass;
};
extern RCMP_System __4RCMP_rcmp_sys;

struct MovieVolumeEntry {
    const char* name;
    unsigned int volume;
};
extern MovieVolumeEntry MovieVolumeArray[];

extern void PlatFinishMovie();
extern void PlatSetFirstMovieFrame(TextureInfo*, Shape*, bool);
extern unsigned int RCMP_GetMaxFramesOutStanding();
extern void RCMP_PlayerFree(void*);
extern unsigned int bGetTicker();
extern float bGetTickerDifference(unsigned int);
extern void* bMalloc(int, int);
extern void bFree(void*);
extern int GetVideoMode();
extern void eWaitUntilRenderingDone();
extern void NotifyFirstFrame_SubTitler();
extern void SoundPause(bool pause, eSNDPAUSE_REASON reason);
extern void SYNCTASK_run();
extern void THREAD_yield(int);

namespace RealShape {
void SetAllocator(EA::Allocator::IAllocator*);
}

extern int GamecubeMaybeAllocateFromCarLoader(int, const char*, int);

// AV_PLAYER member functions not declared in header (using mangled names)
extern "C" {
AV_PLAYER* __Q24RCMP9AV_PLAYERPCciQ34RCMP9AV_PLAYER9LOAD_ENUMQ34RCMP9AV_PLAYER10SOUND_ENUM(
    void*, const char*, int, int, int);
FRAME* GetFrame__Q24RCMP9AV_PLAYERf(AV_PLAYER*, float);
int IsTimeForDecode__Q24RCMP9AV_PLAYER(AV_PLAYER*);
int IsAudioFinished__Q24RCMP9AV_PLAYER(AV_PLAYER*);
void ReleaseFrame__Q24RCMP7DECODERPQ24RCMP5FRAME(DECODER*, FRAME*);
void FillInTextureInfo__11MoviePlayerPUiP11TextureInfoPQ29RealShape5Shape(
    MoviePlayer*, unsigned int*, TextureInfo*, Shape*);
}

MoviePlayer* gMoviePlayer;
unsigned int gMovieStartTime = 0xFFFFFFFF;
ShapeMemoryAllocator gShapeMemoryAllocator;

bool MoviePlayer_Bypass() {
    return bGetTickerDifference(gMovieStartTime) > 30.0f;
}

void MoviePlayer_Play() {
    if (gMoviePlayer != nullptr) {
        gMovieStartTime = bGetTicker();
        gMoviePlayer->Play();
    }
}

void MoviePlayer_StartUp() {
    if (gMoviePlayer == nullptr) {
        gMoviePlayer = new MoviePlayer(0);
    }
}

void MoviePlayer_ShutDown() {
    gMovieStartTime = 0xFFFFFFFF;
    if (gMoviePlayer != nullptr) {
        delete gMoviePlayer;
        gMoviePlayer = nullptr;
    }
    TheTrackStreamer.RefreshLoading();
}

MoviePlayer::MoviePlayer(int memClass) {
    mSettings.filename[0] = '\0';
    mSettings.bufferSize = 0x40000;
    mSettings.activeController = 0;
    mSettings.preload = false;
    mSettings.volume = 0;
    mSettings.sound = IsSoundEnabled != false;
    fLiveStatus = 3;
    fStatus = 3;
    CurFrame = nullptr;
    mSettings.loop = false;
    mSettings.pal = false;
    mSettings.type = 0;
    mSettings.movieId = 0;
    fCurFrameNum = 0;
    fPlayer = nullptr;
    __4RCMP_rcmp_sys.AllocMem = RCMP_PlayerAllocAlign;
    __4RCMP_rcmp_sys.FreeMem = RCMP_PlayerFree_func;
    __4RCMP_rcmp_sys.memClass = memClass;
    RealShape::SetAllocator(&gShapeMemoryAllocator);
    if (TheTrackStreamer.HasMemoryPool()) {
        TheTrackStreamer.MakeSpaceInPool(0x271000, true);
    }
}

MoviePlayer::~MoviePlayer() {
    if (fPlayer != nullptr) {
        delete fPlayer;
    }
    fPlayer = nullptr;
    RCMP_PlayerFree(RCMPDecodeBuffer);
    RCMPDecodeBuffer = nullptr;
    PlatFinishMovie();
}

void MoviePlayer::Init(Settings& newSettings) {
    mSettings.activeController = newSettings.activeController;
    mSettings.bufferSize = newSettings.bufferSize;
    mSettings.loop = newSettings.loop;
    mSettings.preload = newSettings.preload;
    mSettings.volume = newSettings.volume;
    mSettings.sound = newSettings.sound;
    mSettings.pal = newSettings.pal;
    mSettings.type = newSettings.type;
    mSettings.movieId = newSettings.movieId;
    bStrNCpy(mSettings.filename, newSettings.filename, 256);
    mSettings.volume = GetMovieCategoryVolume();
    ResetTimer();
    HandleFatalError();
}

void MoviePlayer::ResetTimer() {
    mili_seconds = 0;
    seconds = 0;
    mTickerFirstTime = true;
    milliseconds = 0.0f;
    prevMilliseconds = 0.0f;
    minutes = 0;
    mMoviePaused = false;
    mTicker = 0;
}

void MoviePlayer::Stop() {
    fStatus = 1;
    fLiveStatus = 1;
    ResetTimer();
}

unsigned int MoviePlayer::GetMillisecondsPerFrame() {
    if (GetVideoMode() == 0) {
        return 20;
    }
    return 16;
}

int MoviePlayer::GetMovieCategoryVolume() {
    unsigned int vol = 0x7F;
    for (int i = 0; i < 0x26; i++) {
        const char* name = MovieVolumeArray[i].name;
        int len = bStrLen(name);
        if (bStrNICmp(name, mSettings.filename, len) == 0) {
            vol = MovieVolumeArray[i].volume;
            if (ShutJosieUp) {
                const char* name2 = MovieVolumeArray[i].name;
                int len2 = bStrLen(name2);
                if (bStrNICmp(name2, "josie", len2) == 0) {
                    vol = 0;
                }
            }
        }
    }
    return vol;
}

void MoviePlayer::HandleFatalError() {}

bool GiveTheMoviePlayerBandwidth() {
    if (gMoviePlayer == nullptr) {
        return false;
    }
    return static_cast<unsigned int>(gMoviePlayer->fStatus - 3) < 3;
}

void* ShapeMemoryAllocator::Alloc(unsigned int size, const EA::TagValuePair& flags) {
    const char* name = "shapes";
    int allocation_params = 0x40;
    int offset = 0;
    const EA::TagValuePair* p = &flags;
    while (p != nullptr) {
        unsigned int tag = p->mTag;
        if (tag == 2) {
            allocation_params = allocation_params | (p->mValue.mInt & 0x1FFC) << 6;
        } else if (tag < 3) {
            if (tag == 1) {
                name = static_cast<const char*>(p->mValue.mPointer);
            }
        } else if (tag == 3) {
            offset = p->mValue.mInt;
            allocation_params = allocation_params | (offset & 0x1FFC) << 17;
        } else if (tag == 4) {
            allocation_params = allocation_params & ~0x40;
        }
        p = p->mNext;
    }
    void* maybe = reinterpret_cast<void*>(GamecubeMaybeAllocateFromCarLoader(size, name, allocation_params));
    if (maybe == nullptr) {
        if (!TheTrackStreamer.HasMemoryPool()) {
            maybe = bMalloc(size, allocation_params);
        } else {
            maybe = TheTrackStreamer.AllocateUserMemory(size, "shapes", offset);
        }
    }
    return maybe;
}

void ShapeMemoryAllocator::Free(void* pBlock, unsigned int size) {
    if (!TheTrackStreamer.HasMemoryPool() ||
        (pBlock != nullptr && !TheTrackStreamer.IsUserMemory(pBlock))) {
        bFree(pBlock);
    } else {
        TheTrackStreamer.FreeUserMemory(pBlock);
    }
}

int ShapeMemoryAllocator::AddRef() {
    return ++mRefcount;
}

int ShapeMemoryAllocator::Release() {
    int ref = mRefcount - 1;
    mRefcount = ref;
    if (ref < 1) {
        delete this;
        ref = 0;
    }
    return ref;
}
