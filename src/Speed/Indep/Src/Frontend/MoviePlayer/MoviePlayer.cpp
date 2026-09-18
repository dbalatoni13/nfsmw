#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/GameCube/Src/G.hpp"
#include "realcore/system.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"

// File: speed/indep/src/frontend/MoviePlayer/MoviePlayer.cpp
// total size: 0x8
// Decl: speed/indep/src/frontend/MoviePlayer/MoviePlayer.cpp:141
struct ShapeMemoryAllocator : public EA::Allocator::IAllocator {
  public:
    ShapeMemoryAllocator() : mRefcount(1) {} // Decl: speed/indep/src/frontend/MoviePlayer/MoviePlayer.cpp:143
    ~ShapeMemoryAllocator() override {} // Decl: speed/indep/src/frontend/MoviePlayer/MoviePlayer.cpp:144

    void *Alloc(size_t size, const EA::TagValuePair &flags) override;
    void *Alloc(size_t size);

  private:
    int mRefcount; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/MoviePlayer/MoviePlayer.cpp:152

    void Free(void *pBlock, size_t size) override; // Decl: speed/indep/src/frontend/MoviePlayer/MoviePlayer.cpp:228

    int AddRef() override; // Decl: speed/indep/src/frontend/MoviePlayer/MoviePlayer.cpp:240

    int Release() override; // Decl: speed/indep/src/frontend/MoviePlayer/MoviePlayer.cpp:245
};

#define MOVIE_TEXTUREINFO_COUNT 3 // :258

// TODO D:/env/egami/realgraph/6/source/shape/cmn/allocator.cpp
namespace RealShape {
class GraphObject {
  public:
    static void SetAllocator(EA::Allocator::IAllocator *);
};
} // namespace RealShape

TextureInfo MovieTextureInfo;

void *GamecubeMaybeAllocateFromCarLoader(int size, const char *name, int alloc_params) {
    bool track_stream_pool_exists = TheTrackStreamer.HasMemoryPool();
    if ((track_stream_pool_exists && size <= bLargestMalloc(7)) || size <= bLargestMalloc(0)) {
        return nullptr;
    }
    TheCarLoader.MakeSpaceInPool(size);
    void *ptr = bMalloc(size, name, __LINE__, (CarLoaderMemoryPoolNumber & 0xf) | alloc_params);
    if (ptr != nullptr) {
        return ptr;
    }
    return nullptr;
}

MoviePlayer *gMoviePlayer = nullptr;
int gMovieStartTime = 0xFFFFFFFF;

bool IsMovieTimerPrintf = false;
ShapeMemoryAllocator gShapeMemoryAllocator;

// total size: 0x8
struct MoveVolumeInfo {
    // Members
    const char *MovieBaseName; // offset 0x0, size 0x4
    int Volume;                // offset 0x4, size 0x4
};
// c33dat: el objetivo lo tiene de 304 B = 38 entradas, no 40 (.data:0x8041B7DC).
// El bucle de GetMovieCategoryVolume usa el literal 0x26, no sizeof: cambiar el
// tamano declarado no toca una sola instruccion.
MoveVolumeInfo MovieVolumeArray[] = {
    {"MOVIES\\attract_movie", 65},
    {"MOVIES\\drag_tutorial", 55},
    {"MOVIES\\ealogo", 60},
    {"MOVIES\\intro_movie", 55},
    {"MOVIES\\sprint_tutorial", 55},
    {"MOVIES\\tollbooth_tutorial", 55},
    {"MOVIES\\bounty_tutorial", 55},
    {"MOVIES\\pursuit_tutorial", 55},
    {"MOVIES\\speedtrap_tutorial", 55},
    {"MOVIES\\psa", 50},
    {"MOVIES\\blacklist_01", 45},
    {"MOVIES\\blacklist_02", 45},
    {"MOVIES\\blacklist_03", 45},
    {"MOVIES\\blacklist_04", 45},
    {"MOVIES\\blacklist_05", 45},
    {"MOVIES\\blacklist_06", 45},
    {"MOVIES\\blacklist_07", 45},
    {"MOVIES\\blacklist_08", 45},
    {"MOVIES\\blacklist_09", 45},
    {"MOVIES\\blacklist_10", 45},
    {"MOVIES\\blacklist_11", 45},
    {"MOVIES\\blacklist_12", 45},
    {"MOVIES\\blacklist_13", 45},
    {"MOVIES\\blacklist_14", 45},
    {"MOVIES\\blacklist_15", 45},
    {"MOVIES\\storyfmv_bla134", 80},
    {"MOVIES\\storyfmv_bus12", 80},
    {"MOVIES\\storyfmv_cro06_coh06a", 80},
    {"MOVIES\\storyfmv_dda01", 80},
    {"MOVIES\\storyfmv_epi138", 80},
    {"MOVIES\\storyfmv_her136", 80},
    {"MOVIES\\storyfmv_pin11", 80},
    {"MOVIES\\storyfmv_pol17_mot21", 80},
    {"MOVIES\\storyfmv_rap30", 80},
    {"MOVIES\\storyfmv_raz08", 80},
    {"MOVIES\\storyfmv_roc02", 80},
    {"MOVIES\\storyfmv_saf25", 80},
    {"MOVIES\\storyfmv_rac01", 80},
};



void *RCMPDecodeBuffer = nullptr;

bool MoviePlayer_Bypass() {
    return bGetTickerDifference(gMovieStartTime) > 5000.0f;
}

void MoviePlayer_Play() {
    if (gMoviePlayer != nullptr) {
        gMovieStartTime = bGetTicker();
        gMoviePlayer->Play();
    }
}

void *ShapeMemoryAllocator::Alloc(size_t size, const EA::TagValuePair &flags) {
    const char *name = "";
    int allocation_params = 0x40;
    int offset = 0;
    const EA::TagValuePair *p = &flags;
    while (p != nullptr) {
        unsigned int tag = p->mTag;
        switch (tag) {
        case 1:
            name = static_cast<const char *>(p->mValue.mPointer);
            break;
        case 2:
            allocation_params |= (p->mValue.mInt & 0x1FFC) << 6;
            break;
        case 3: {
            int value = p->mValue.mInt;
            allocation_params |= (value & 0x1FFC) << 17;
            offset = value;
            break;
        }
        case 4:
            allocation_params &= ~0x40;
            break;
        }
        p = p->mNext;
    }
    void *maybe = GamecubeMaybeAllocateFromCarLoader(size, name, allocation_params);
    if (maybe == nullptr) {
        if (TheTrackStreamer.HasMemoryPool()) {
            maybe = TheTrackStreamer.AllocateUserMemory(size, "shape_mem", offset);
        } else {
            maybe = bMalloc(size, allocation_params);
        }
    }
    return maybe;
}

void ShapeMemoryAllocator::Free(void *pBlock, size_t size) {
    if (!TheTrackStreamer.HasMemoryPool() || (pBlock != nullptr && !TheTrackStreamer.IsUserMemory(pBlock))) {
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

void *RCMP_PlayerAllocAlign(const char *name, int size, int alignment, int headersize, int type) {
    if (name == nullptr || *name == '\0') {
        name = "RCMP_Mem";
    }
    size = size + headersize;
    int alloc_params = (headersize & 0x1FFC) << 17;
    void *maybe = GamecubeMaybeAllocateFromCarLoader(size, name, alloc_params | (alignment & 0x1FFC) << 6);
    if (maybe == nullptr) {
        if (TheTrackStreamer.HasMemoryPool()) {
            return TheTrackStreamer.AllocateUserMemory(size, name, headersize);
        } else {
            if (alignment == 0) {
                alignment = 0x80;
            }
            void *ptr = bMalloc(size, name, __LINE__, alloc_params | (alignment & 0x1FFC) << 6 | 0x40);
            return ptr;
        }
    }
    return maybe;
}

void RCMP_PlayerFree(void *ptr) {
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

void MoviePlayer_StartUp() {
    if (gMoviePlayer == nullptr) {
        gMoviePlayer = new ("MoviePlayer", 0) MoviePlayer(0);
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
    fStatus = 3;
    fLiveStatus = 3;
    fCurFrameNum = 0;
    fPlayer = nullptr;
    CurFrame = nullptr;
    RCMP::rcmp_sys.AllocMemFunc = RCMP_PlayerAllocAlign;
    RCMP::rcmp_sys.FreeMemFunc = RCMP_PlayerFree;
    RCMP::rcmp_sys.m_DefaultMemDir = memClass;
    RealShape::GraphObject::SetAllocator(&gShapeMemoryAllocator);
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

void MoviePlayer::Init(Settings &newSettings) {
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

void MoviePlayer::Play() {
    if (SkipMovies == 0) {
        RCMP::AV_PLAYER::LOAD_ENUM loadType = RCMP::AV_PLAYER::STREAM;
        if (mSettings.preload) {
            loadType = RCMP::AV_PLAYER::PRELOAD;
        }
        RCMP::AV_PLAYER::SOUND_ENUM soundType = mSettings.sound ? RCMP::AV_PLAYER::SOUND_ON : RCMP::AV_PLAYER::SOUND_OFF;
        void *mem = fPlayer = new RCMP::AV_PLAYER(mSettings.filename, mSettings.bufferSize, loadType, soundType);
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
curframe_found: {
    RealShape::Shape *shape = *reinterpret_cast<RealShape::Shape **>(reinterpret_cast<char *>(CurFrame) + 4);
    PlatSetFirstMovieFrame(&MovieTextureInfo, shape, mSettings.type == 0);
    SubTitler::NotifyFirstFrame();
    fStatus = 5;
    fLiveStatus = 5;
    fPlayer->UnPause();
}
}

void MoviePlayer::Stop() {
    fStatus = 1;
    fLiveStatus = 1;
    ResetTimer();
}

int MoviePlayer::GetMovieCategoryVolume() {
    unsigned int vol = 0x7F;
    for (int i = 0; i < 0x26; i++) {
        const char *name = MovieVolumeArray[i].MovieBaseName;
        int len = bStrLen(name);
        if (bStrNICmp(name, mSettings.filename, len) == 0) {
            vol = MovieVolumeArray[i].Volume;
            if (ShutJosieUp) {
                const char *name2 = MovieVolumeArray[i].MovieBaseName;
                int len2 = bStrLen(name2);
                if (bStrNICmp(name2, "MOVIES\\psa", len2) == 0) {
                    vol = 0;
                }
            }
        }
    }
    return vol;
}

void MoviePlayer::GetFirstFrame() {
    RCMP::AV_PLAYER *player = fPlayer;
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
            SetSoundControlState(false, static_cast<eSNDCTLSTATE>(1), "movie_done");
            if (fPlayer != nullptr) {
                delete fPlayer;
            }
            fPlayer = nullptr;
            void *buf = RCMPDecodeBuffer;
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
        if (fPlayer->IsTimeForDecode() && CurFrame != nullptr) {
            fPlayer->GetDecoder()->ReleaseFrame(CurFrame);
            float goalFrame = fPlayer->GetGoalFrame();
            CurFrame = fPlayer->GetFrame(goalFrame);
        } else {
            MovieFinished = true;
        }
        if (CurFrame == nullptr) {
            finished = fPlayer->IsAudioFinished();
        } else {
            RealShape::Shape *shape = CurFrame->GetShape();
            if (!MovieFinished) {
                this->FillInTextureInfo(reinterpret_cast<unsigned int *>(RCMPDecodeBuffer), &MovieTextureInfo, shape);
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

uint32 MoviePlayer::GetMillisecondsPerFrame() {
    if (GetVideoMode() == 0) {
        return 20;
    }
    return 16;
}

void MoviePlayer::HandleFatalError() {}

bool GiveTheMoviePlayerBandwidth() {
    bool result = false;
    if (gMoviePlayer != nullptr) {
        int status = gMoviePlayer->GetStatus();
        status -= 3;
        result = static_cast<unsigned int>(status) < 3u;
    }
    return result;
}
