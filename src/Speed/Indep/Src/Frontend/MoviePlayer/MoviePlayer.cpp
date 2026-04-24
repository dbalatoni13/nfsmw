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

typedef void* (*RCMP_AllocFunc)(const char*, int, int, int, int);
typedef void (*RCMP_FreeFunc)(void*);

void* RCMP_PlayerAllocAlign(const char*, int, int, int, int);
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

class CarLoader {
  public:
    void MakeSpaceInPool(int size);
};
extern CarLoader TheCarLoader;
extern int CarLoaderMemoryPoolNumber;

void* GamecubeMaybeAllocateFromCarLoader(int size, const char* name, int alloc_params) {
    bool track_stream_pool_exists = TheTrackStreamer.HasMemoryPool();
    if ((track_stream_pool_exists && size <= bLargestMalloc(7)) || size <= bLargestMalloc(0)) {
        return nullptr;
    }
    TheCarLoader.MakeSpaceInPool(size);
    void* ptr = bMalloc(size, name, __LINE__, (CarLoaderMemoryPoolNumber & 0xf) | alloc_params);
    if (ptr != nullptr) {
        return ptr;
    }
    return nullptr;
}

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
    mSettings.preload = false;
    mSettings.filename[0] = '\0';
    mSettings.bufferSize = 0x40000;
    mSettings.activeController = 0;
    mSettings.sound = IsSoundEnabled != false;
    mSettings.volume = 0;
    fStatus = 3;
    fLiveStatus = 3;
    fPlayer = nullptr;
    CurFrame = nullptr;
    mSettings.loop = false;
    mSettings.pal = false;
    mSettings.type = 0;
    mSettings.movieId = 0;
    fCurFrameNum = 0;
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
    mTicker = 0;
    mMoviePaused = false;
    mTickerFirstTime = true;
    mili_seconds = 0;
    seconds = 0;
    minutes = 0;
    milliseconds = 0.0f;
    prevMilliseconds = 0.0f;
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

void MoviePlayer::Play() {
    if (SkipMovies == 0) {
        int loadType = 0;
        if (mSettings.preload) {
            loadType = 1;
        }
        int soundType = mSettings.sound == false;
        void* mem = __4RCMP_rcmp_sys.AllocMem("avplayer", 0x94, 0, 0, __4RCMP_rcmp_sys.memClass);
        fPlayer = __Q24RCMP9AV_PLAYERPCciQ34RCMP9AV_PLAYER9LOAD_ENUMQ34RCMP9AV_PLAYER10SOUND_ENUM(
            mem, mSettings.filename, mSettings.bufferSize, loadType, soundType);
        HandleFatalError();
        if (fPlayer == nullptr) {
            fStatus = 2;
            fLiveStatus = 2;
            return;
        }
        GetFirstFrame();
        if (mSettings.sound != false) {
            fPlayer->SetVol(mSettings.volume);
        }
        fCurFrameNum = fCurFrameNum + 1;
        fPlayer->Pause();
        if (CurFrame != nullptr) {
            goto curframe_found;
        }
    }
    cFEng::Get()->QueueGameMessage(0xc3960eb9, nullptr, 0xff);
    return;
curframe_found:
    {
        Shape* shape = *reinterpret_cast<Shape**>(reinterpret_cast<char*>(CurFrame) + 4);
        PlatSetFirstMovieFrame(&MovieTextureInfo, shape, mSettings.type == 0);
        NotifyFirstFrame_SubTitler();
        fStatus = 5;
        fLiveStatus = 5;
        fPlayer->UnPause();
    }
}

void MoviePlayer::GetFirstFrame() {
    AV_PLAYER* player = fPlayer;
    unsigned int maxFrames = RCMP_GetMaxFramesOutStanding();
    int msPerFrame = GetMillisecondsPerFrame();
    CurFrame = player->GetFirstFrame(maxFrames, msPerFrame << 1);
}

void MoviePlayer::Update() {
    if (fStatus == 5) {
        UpdateFunction();
        int movie_done = (fLiveStatus != 5);
        if (Joylog::IsReplaying()) {
            int joylog_movie_done = Joylog::GetData(4, static_cast<JoylogChannel>(8));
            if (joylog_movie_done != 0 && movie_done == 0) {
                while (fLiveStatus != 0) {
                    UpdateFunction();
                }
            }
            movie_done = joylog_movie_done;
        }
        Joylog::AddData(movie_done, 4, static_cast<JoylogChannel>(8));
        if (movie_done != 0) {
            fStatus = fLiveStatus;
            eWaitUntilRenderingDone();
            cFEng::Get()->QueueGameMessage(0xc3960eb9, nullptr, 0xff);
            SoundPause(false, static_cast<eSNDPAUSE_REASON>(6));
            SetSoundControlState(false, static_cast<eSNDCTLSTATE>(1), "");
            if (fPlayer != nullptr) {
                delete fPlayer;
            }
            fPlayer = nullptr;
            void* buf = RCMPDecodeBuffer;
            RCMP_PlayerFree(buf);
            RCMPDecodeBuffer = nullptr;
            ResetTimer();
        }
        HandleFatalError();
    }
}

void MoviePlayer::UpdateFunction() {
    static int recurse = 0;
    if (recurse == 0) {
        int finished = 0;
        recurse = 1;
        bool MovieFinished = false;
        SYNCTASK_run();
        THREAD_yield(0);
        if (IsTimeForDecode__Q24RCMP9AV_PLAYER(fPlayer) && CurFrame != nullptr) {
            DECODER* decoder = *reinterpret_cast<DECODER**>(reinterpret_cast<char*>(fPlayer) + 0x64);
            ReleaseFrame__Q24RCMP7DECODERPQ24RCMP5FRAME(decoder, CurFrame);
            float goalFrame = *reinterpret_cast<float*>(reinterpret_cast<char*>(fPlayer) + 0x48);
            CurFrame = GetFrame__Q24RCMP9AV_PLAYERf(fPlayer, goalFrame);
        } else {
            MovieFinished = true;
        }
        if (CurFrame == nullptr) {
            finished = IsAudioFinished__Q24RCMP9AV_PLAYER(fPlayer);
        } else {
            Shape* shape = *reinterpret_cast<Shape**>(reinterpret_cast<char*>(CurFrame) + 4);
            if (!MovieFinished) {
                FillInTextureInfo__11MoviePlayerPUiP11TextureInfoPQ29RealShape5Shape(
                    this, reinterpret_cast<unsigned int*>(RCMPDecodeBuffer), &MovieTextureInfo, shape);
            }
        }
        HandleFatalError();
        if (finished != 0) {
            eWaitUntilRenderingDone();
            fLiveStatus = 0;
        }
        recurse = 0;
    }
}

bool GiveTheMoviePlayerBandwidth() {
    bool result = false;
    if (gMoviePlayer != nullptr) {
        int status = gMoviePlayer->fStatus;
        status -= 3;
        result = static_cast<unsigned int>(status) < 3u;
    }
    return result;
}

void* ShapeMemoryAllocator::Alloc(unsigned int size, const EA::TagValuePair& flags) {
    const char* name = "";
    int allocation_params = 0x40;
    int offset = 0;
    const EA::TagValuePair* p = &flags;
    while (p != nullptr) {
        unsigned int tag = p->mTag;
        if (tag == 2) {
            goto tag_2;
        }
        if (tag > 2) {
            goto tag_gt_2;
        }
        if (tag == 1) {
            name = static_cast<const char*>(p->mValue.mPointer);
            goto next;
        }
next:
        p = p->mNext;
        continue;

    tag_gt_2:
        if (tag == 3) {
            int value = p->mValue.mInt;

            offset = value;
            allocation_params |= (value & 0x1FFC) << 17;
            goto next;
        }
        if (tag == 4) {
            allocation_params &= ~0x40;
        }
        goto next;

    tag_2:
        allocation_params |= (p->mValue.mInt & 0x1FFC) << 6;
        goto next;
    }
    void* maybe = GamecubeMaybeAllocateFromCarLoader(size, name, allocation_params);
    if (maybe == nullptr) {
        if (TheTrackStreamer.HasMemoryPool()) {
            maybe = TheTrackStreamer.AllocateUserMemory(size, "shape_mem", offset);
        } else {
            maybe = bMalloc(size, allocation_params);
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

void* RCMP_PlayerAllocAlign(const char* name, int size, int alignment, int headersize, int type) {
    if (name == nullptr || *name == '\0') {
        name = "RCMP_Mem";
    }
    size = size + headersize;
    int alloc_params = (headersize & 0x1FFC) << 17;
    void* maybe = GamecubeMaybeAllocateFromCarLoader(size, name, alloc_params | (alignment & 0x1FFC) << 6);
    if (maybe == nullptr) {
        if (TheTrackStreamer.HasMemoryPool()) {
            return TheTrackStreamer.AllocateUserMemory(size, name, headersize);
        } else {
            if (alignment == 0) {
                alignment = 0x80;
            }
            void* ptr = bMalloc(size, name, __LINE__, alloc_params | (alignment & 0x1FFC) << 6 | 0x40);
            return ptr;
        }
    }
    return maybe;
}

void RCMP_PlayerFree(void* ptr) {
    if (!TheTrackStreamer.HasMemoryPool()) {
        goto bfree_label;
    }
    if (ptr == nullptr) {
        return;
    }
    if (TheTrackStreamer.IsUserMemory(ptr)) {
        goto freeuser_label;
    }
bfree_label:
    bFree(ptr);
    return;
freeuser_label:
    TheTrackStreamer.FreeUserMemory(ptr);
}
