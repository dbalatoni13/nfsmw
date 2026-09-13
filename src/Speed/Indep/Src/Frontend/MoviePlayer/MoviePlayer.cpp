#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/GameCube/Src/G.hpp"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system/systask.h"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system/threads.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

// total size: 0x8
// Decl: 141
struct ShapeMemoryAllocator : public EA::Allocator::IAllocator {
  public:
    ShapeMemoryAllocator() {}           // Decl: 143
    ~ShapeMemoryAllocator() override {} // Decl: 144

    void *Alloc(size_t size, const EA::TagValuePair &flags) override;
    void *Alloc(size_t size);

  private:
    int mRefcount; // offset 0x4, size 0x4, Decl: 152

    void Free(void *pBlock, size_t size) override; // Decl: 228

    int AddRef() override; // Decl: 240

    int Release() override; // Decl: 245
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
    } else {
        TheCarLoader.MakeSpaceInPool(size);
        void *ptr = bMalloc(size, name, 0, (CarLoaderMemoryPoolNumber & 0xf) | alloc_params);
        if (ptr != nullptr) {
            return ptr;
        }
        return nullptr;
    }
}

MoviePlayer *gMoviePlayer = nullptr;
unsigned int gMovieStartTime = 0xFFFFFFFF;
ShapeMemoryAllocator gShapeMemoryAllocator;

// total size: 0x8
struct MoveVolumeInfo {
    const char *MovieBaseName; // offset 0x0, size 0x4
    int Volume;                // offset 0x4, size 0x4
};

MoveVolumeInfo MovieVolumeArray[38] = {{"MOVIES\\attract_movie", 0x41},
                                       {"MOVIES\\drag_tutorial", 0x37},
                                       {"MOVIES\\ealogo", 0x3c},
                                       {"MOVIES\\intro_movie", 0x37},
                                       {"MOVIES\\sprint_tutorial", 0x37},
                                       {"MOVIES\\tollbooth_tutorial", 0x37},
                                       {"MOVIES\\bounty_tutorial", 0x37},
                                       {"MOVIES\\pursuit_tutorial", 0x37},
                                       {"MOVIES\\speedtrap_tutorial", 0x37},
                                       {"MOVIES\\psa", 0x32},
                                       {"MOVIES\\blacklist_01", 0x2d},
                                       {"MOVIES\\blacklist_02", 0x2d},
                                       {"MOVIES\\blacklist_03", 0x2d},
                                       {"MOVIES\\blacklist_04", 0x2d},
                                       {"MOVIES\\blacklist_05", 0x2d},
                                       {"MOVIES\\blacklist_06", 0x2d},
                                       {"MOVIES\\blacklist_07", 0x2d},
                                       {"MOVIES\\blacklist_08", 0x2d},
                                       {"MOVIES\\blacklist_09", 0x2d},
                                       {"MOVIES\\blacklist_10", 0x2d},
                                       {"MOVIES\\blacklist_11", 0x2d},
                                       {"MOVIES\\blacklist_12", 0x2d},
                                       {"MOVIES\\blacklist_13", 0x2d},
                                       {"MOVIES\\blacklist_14", 0x2d},
                                       {"MOVIES\\blacklist_15", 0x2d},
                                       {"MOVIES\\storyfmv_bla134", 0x50},
                                       {"MOVIES\\storyfmv_bus12", 0x50},
                                       {"MOVIES\\storyfmv_cro06_coh06a", 0x50},
                                       {"MOVIES\\storyfmv_dda01", 0x50},
                                       {"MOVIES\\storyfmv_epi138", 0x50},
                                       {"MOVIES\\storyfmv_her136", 0x50},
                                       {"MOVIES\\storyfmv_pin11", 0x50},
                                       {"MOVIES\\storyfmv_pol17_mot21", 0x50},
                                       {"MOVIES\\storyfmv_rap30", 0x50},
                                       {"MOVIES\\storyfmv_raz08", 0x50},
                                       {"MOVIES\\storyfmv_roc02", 0x50},
                                       {"MOVIES\\storyfmv_saf25", 0x50},
                                       {"MOVIES\\storyfmv_rac01", 0x50}};

bool MoviePlayer_Bypass() {
    return bGetTickerDifference(gMovieStartTime) > 5000.0f;
}

// UNSOLVED
void MoviePlayer_Play() {
    if (gMoviePlayer != nullptr) {
        gMovieStartTime = bGetTicker();
        gMoviePlayer->Play();
    }
}

void *ShapeMemoryAllocator::Alloc(size_t size, const EA::TagValuePair &flags) {
    char *name = "";
    int allocation_params = BMEMORY_TOP_BIT;
    const EA::TagValuePair *p = &flags;
    int offset = 0;

    while (p != nullptr) {
        switch (p->mTag) {
            case 1: {
                void *ptr = const_cast<void *>(p->mValue.mPointer);
                name = static_cast<char *>(ptr);
                break;
            }
            case 2:
                allocation_params |= BMEMORY_ALIGNMENT(p->mValue.mInt);
                break;
            case 3:
                allocation_params |= BMEMORY_ALIGNMENT_OFFSET(p->mValue.mInt);
                offset = p->mValue.mInt;
                break;
            case 4:
                allocation_params &= ~BMEMORY_TOP_BIT;
                break;
        }
        p = p->mNext;
    }
    void *maybe = GamecubeMaybeAllocateFromCarLoader(size, name, allocation_params);
    if (maybe == nullptr) {
        if (TheTrackStreamer.HasMemoryPool()) {
            maybe = TheTrackStreamer.AllocateUserMemory(size, "shape_mem", offset);
        } else {
            maybe = bMalloc(size, name, 0, allocation_params);
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
    mRefcount--;
    if (mRefcount < 1) {
        delete this;
        return 0;
    }
    return mRefcount;
}

void *RCMP_PlayerAllocAlign(const char *name, int size, int alignment, int headersize, int type) {
    if (name == nullptr || *name == '\0') {
        name = "RCMP_Mem";
    }
    size = size + headersize;

    void *maybe = GamecubeMaybeAllocateFromCarLoader(size, name, BMEMORY_ALIGNMENT_OFFSET(headersize) | BMEMORY_ALIGNMENT(alignment));
    if (maybe != nullptr) {
        return maybe;
    }

    if (TheTrackStreamer.HasMemoryPool()) {
        void *ptr = TheTrackStreamer.AllocateUserMemory(size, name, headersize);
        return ptr;
    }

    if (alignment == 0) {
        alignment = BMEMORY_BEST_FIT;
    }
    return bMalloc(size, name, 0, BMEMORY_ALIGNMENT_OFFSET(headersize) | BMEMORY_ALIGNMENT(alignment) | BMEMORY_TOP_BIT);
}

// UNSOLVED
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

MoviePlayer::MoviePlayer(int memClass) : mSettings() {
    fStatus = 3;
    fLiveStatus = 3;
    mSettings.loop = false;
    mSettings.pal = false;
    mSettings.type = 0;
    mSettings.movieId = 0;
    fCurFrameNum = 0;
    fPlayer = nullptr;
    CurFrame = nullptr;
    RCMP::rcmp_sys.AllocMemFunc = RCMP_PlayerAllocAlign;
    RCMP::rcmp_sys.FreeMemFunc = RCMP_PlayerFree;
    RCMP::rcmp_sys.m_DefaultMemDir = memClass;
    RealShape::GraphObject::SetAllocator(&gShapeMemoryAllocator);
    if (TheTrackStreamer.HasMemoryPool()) {
        TheTrackStreamer.MakeSpaceInPool(2560000, true);
    }
}

void *RCMPDecodeBuffer = nullptr;

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
    mSettings = newSettings;
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

// TODO: missing dwarf inlines
void MoviePlayer::Play() {
    if (SkipMovies != 0) {
        cFEng::Get()->QueueGameMessage(0xc3960eb9, nullptr, 0xff);
        return;
    }

    RCMP::AV_PLAYER::LOAD_ENUM loadType = RCMP::AV_PLAYER::STREAM;
    if (mSettings.preload) {
        loadType = RCMP::AV_PLAYER::PRELOAD;
    }
    RCMP::AV_PLAYER::SOUND_ENUM soundType = mSettings.sound ? RCMP::AV_PLAYER::SOUND_ON : RCMP::AV_PLAYER::SOUND_OFF;
    fPlayer = new RCMP::AV_PLAYER(mSettings.filename, mSettings.bufferSize, loadType, soundType);
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
    if (CurFrame == nullptr) {
        cFEng::Get()->QueueGameMessage(0xc3960eb9, nullptr, 0xff);
        return;
    }
    PlatSetFirstMovieFrame(&MovieTextureInfo, CurFrame->GetShape(), mSettings.type == 0);
    SubTitler::NotifyFirstFrame();
    fStatus = 5;
    fLiveStatus = 5;
    fPlayer->UnPause();
}

void MoviePlayer::Stop() {
    fStatus = 1;
    fLiveStatus = 1;
    ResetTimer();
}

int MoviePlayer::GetMovieCategoryVolume() {
    int volume = 0x7F; // int8 max
    for (int i = 0; i < NUM_ELEMENTS(MovieVolumeArray); i++) {
        if (bStrNICmp(MovieVolumeArray[i].MovieBaseName, mSettings.filename, bStrLen(MovieVolumeArray[i].MovieBaseName)) == 0) {
            volume = MovieVolumeArray[i].Volume;
            extern bool ShutJosieUp;
            if (ShutJosieUp) {
                if (bStrNICmp(MovieVolumeArray[i].MovieBaseName, "MOVIES\\psa", bStrLen(MovieVolumeArray[i].MovieBaseName)) == 0) {
                    volume = 0;
                }
            }
        }
    }
    return volume;
}

void MoviePlayer::GetFirstFrame() {
    CurFrame = fPlayer->GetFirstFrame(RCMP_GetMaxFramesOutStanding(), GetMillisecondsPerFrame() * 2);
}

void MoviePlayer::Update() {
    if (fStatus != 5) {
        return;
    }

    UpdateFunction();
    int movie_done = static_cast<int>(fLiveStatus != 5);
    if (Joylog::IsReplaying()) {
        int joylog_movie_done = Joylog::GetData(4, JOYLOG_CHANNEL_MOVIE_PLAYER_STATUS);
        if (joylog_movie_done != 0 && movie_done == 0) {
            while (fLiveStatus != 0) {
                UpdateFunction();
            }
        }
        movie_done = joylog_movie_done;
    }
    Joylog::AddData(movie_done, 4, JOYLOG_CHANNEL_MOVIE_PLAYER_STATUS);
    if (movie_done != 0) {
        fStatus = fLiveStatus;
        eWaitUntilRenderingDone();
        cFEng::Get()->QueueGameMessage(0xc3960eb9, nullptr, 0xff);
        SoundPause(false, eSNDPAUSE_MOVIE);
        SetSoundControlState(false, SNDSTATE_PAUSE, "movie_done");
        if (fPlayer != nullptr) {
            delete fPlayer;
        }
        fPlayer = nullptr;
        RCMP_PlayerFree(RCMPDecodeBuffer);
        RCMPDecodeBuffer = nullptr;
        ResetTimer();
    }
    HandleFatalError();
}

void MoviePlayer::UpdateFunction() {
    static int recurse = 0;
    if (recurse != 0) {
        return;
    }
    bool MovieFinished = false;
    recurse = 1;
    bool ReDraw = false;
    SYNCTASK_run();
    THREAD_yield(0);
    if (fPlayer->IsTimeForDecode() && CurFrame != nullptr) {
        fPlayer->GetDecoder()->ReleaseFrame(CurFrame);
        CurFrame = fPlayer->GetFrame(fPlayer->GetGoalFrame());
    } else {
        ReDraw = true;
    }
    if (CurFrame == nullptr) {
        MovieFinished = fPlayer->IsAudioFinished();
    } else {
        RealShape::Shape *shape = CurFrame->GetShape();
        if (!ReDraw) {
            this->FillInTextureInfo(reinterpret_cast<uint32 *>(RCMPDecodeBuffer), &MovieTextureInfo, CurFrame->GetShape());
        }
    }
    HandleFatalError();
    if (MovieFinished) {
        eWaitUntilRenderingDone();
        fLiveStatus = 0;
    }
    recurse = 0;
}

uint32 MoviePlayer::GetMillisecondsPerFrame() {
    if (GetVideoMode() == 0) {
        return 20;
    }
    return 16;
}

void MoviePlayer::HandleFatalError() {}

bool GiveTheMoviePlayerBandwidth() {
    return gMoviePlayer != nullptr && gMoviePlayer->IsMoviePlaying();
}
