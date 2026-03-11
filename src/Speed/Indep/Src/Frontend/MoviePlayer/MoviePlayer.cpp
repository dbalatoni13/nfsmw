#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"

extern TrackStreamer TheTrackStreamer;
extern bool IsSoundEnabled;
extern int SkipMovies;
extern bool ShutJosieUp;
extern TextureInfo MovieTextureInfo;
extern void* RCMPDecodeBuffer;

extern int bStrNICmp(const char*, const char*, int);

void* RCMP_PlayerAllocAlign(const char*, int, int, int, int);
void RCMP_PlayerFree(void*);

extern void PlatFinishMovie();
extern void PlatSetFirstMovieFrame(TextureInfo*, RealShape::Shape*, bool);
extern unsigned int RCMP_GetMaxFramesOutStanding();
extern unsigned int bGetTicker();
extern float bGetTickerDifference(unsigned int);
extern void* bMalloc(int, int);
extern int bLargestMalloc(int);
extern void bFree(void*);
extern int GetVideoMode();
extern void eWaitUntilRenderingDone();
extern void NotifyFirstFrame_SubTitler();
extern void SoundPause(bool, eSNDPAUSE_REASON);
extern void SYNCTASK_run();
extern void THREAD_yield(int);

extern int CarLoaderMemoryPoolNumber;

struct MovieVolumeEntry {
    const char* name;
    unsigned int volume;
};
extern MovieVolumeEntry MovieVolumeArray[];

MoviePlayer* gMoviePlayer;
unsigned int gMovieStartTime = 0xFFFFFFFF;
ShapeMemoryAllocator gShapeMemoryAllocator;

void* GamecubeMaybeAllocateFromCarLoader(int size, const char* name, int alloc_params) {
    if ((!TheTrackStreamer.HasMemoryPool() || bLargestMalloc(7) < size) &&
        bLargestMalloc(0) < size) {
        TheCarLoader.MakeSpaceInPool(size);
        void* ptr = bMalloc(size, (CarLoaderMemoryPoolNumber & 0xf) | alloc_params);
        if (ptr != nullptr) {
            return ptr;
        }
    }
    return nullptr;
}

bool MoviePlayer_Bypass() {
    return bGetTickerDifference(gMovieStartTime) > 30.0f;
}

void MoviePlayer_Play() {
    if (gMoviePlayer != nullptr) {
        gMovieStartTime = bGetTicker();
        gMoviePlayer->Play();
    }
}

void* RCMP::AV_PLAYER::operator new(unsigned int size) {
    return __4RCMP_rcmp_sys.AllocMem("rcmp", size, 0, 0, __4RCMP_rcmp_sys.memClass);
}
void RCMP::AV_PLAYER::operator delete(void* ptr) {
    __4RCMP_rcmp_sys.FreeMem(ptr);
}

void* RCMP_PlayerAllocAlign(const char* name, int size, int alignment, int headersize, int type) {
    if (name == nullptr || *name == '\0') {
        name = "RCMP_Mem";
    }
    size = size + headersize;
    unsigned int alloc_params = (static_cast<unsigned int>(headersize) & 0x1ffc) << 17;
    void* maybe = reinterpret_cast<void*>(GamecubeMaybeAllocateFromCarLoader(
        size, name, alloc_params | (static_cast<unsigned int>(alignment) & 0x1ffc) << 6));
    if (maybe == nullptr) {
        if (!TheTrackStreamer.HasMemoryPool()) {
            if (alignment == 0) {
                alignment = 0x80;
            }
            maybe = bMalloc(size, alloc_params | (static_cast<unsigned int>(alignment) & 0x1ffc) << 6 | 0x40);
        } else {
            maybe = TheTrackStreamer.AllocateUserMemory(size, name, headersize);
        }
    }
    return maybe;
}

void RCMP_PlayerFree(void* ptr) {
    if (TheTrackStreamer.HasMemoryPool()) {
        if (ptr == nullptr) {
            return;
        }
        if (TheTrackStreamer.IsUserMemory(ptr)) {
            TheTrackStreamer.FreeUserMemory(ptr);
            return;
        }
    }
    bFree(ptr);
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
    mSettings.bufferSize = 0x40000;
    mSettings.activeController = 0;
    mSettings.movieId = 0;
    mSettings.volume = 0;
    mSettings.sound = IsSoundEnabled != false;
    fStatus = 3;
    fLiveStatus = 3;
    CurFrame = nullptr;
    mSettings.loop = false;
    mSettings.pal = false;
    mSettings.type = 0;
    mSettings.preload = false;
    fCurFrameNum = 0;
    fPlayer = nullptr;
    __4RCMP_rcmp_sys.AllocMem = RCMP_PlayerAllocAlign;
    __4RCMP_rcmp_sys.FreeMem = RCMP_PlayerFree;
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
    mSettings = newSettings;
    mSettings.volume = GetMovieCategoryVolume();
    ResetTimer();
    HandleFatalError();
}

void MoviePlayer::ResetTimer() {
    minutes = 0;
    prevMilliseconds = 0.0f;
    mTickerFirstTime = true;
    mTicker = 0;
    mMoviePaused = false;
    mili_seconds = 0;
    seconds = 0;
    milliseconds = 0.0f;
}

void MoviePlayer::Play() {
    if (SkipMovies == 0) {
        RCMP::AV_PLAYER::LOAD_ENUM loadType = static_cast<RCMP::AV_PLAYER::LOAD_ENUM>(mSettings.preload != false);
        RCMP::AV_PLAYER::SOUND_ENUM soundType = static_cast<RCMP::AV_PLAYER::SOUND_ENUM>(mSettings.sound == false);
        fPlayer = new RCMP::AV_PLAYER(mSettings.filename, mSettings.bufferSize, loadType, soundType);
        HandleFatalError();
        if (fPlayer == nullptr) {
            fLiveStatus = 2;
            fStatus = 2;
            return;
        }
        GetFirstFrame();
        if (mSettings.sound) {
            fPlayer->SetVol(mSettings.volume);
        }
        fCurFrameNum++;
        fPlayer->Pause();
        if (CurFrame != nullptr) {
            PlatSetFirstMovieFrame(&MovieTextureInfo, CurFrame->GetShape(), mSettings.type == 0);
            NotifyFirstFrame_SubTitler();
            fLiveStatus = 5;
            fStatus = 5;
            fPlayer->UnPause();
            return;
        }
    }
    cFEng::Get()->QueueGameMessage(0xc3960eb9, 0, 0xff);
}

void MoviePlayer::Stop() {
    fStatus = 1;
    fLiveStatus = 1;
    ResetTimer();
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

void MoviePlayer::GetFirstFrame() {
    CurFrame = fPlayer->GetFirstFrame(RCMP_GetMaxFramesOutStanding(), GetMillisecondsPerFrame() * 2);
}

void MoviePlayer::Update() {
    if (fStatus == 5) {
        UpdateFunction();
        int movie_done = (fLiveStatus != 5);
        {
            int joylog_movie_done = movie_done;
            if (Joylog::IsReplaying()) {
                joylog_movie_done = Joylog::GetData(4, JOYLOG_CHANNEL_MOVIE_PLAYER_STATUS);
                if (joylog_movie_done != 0 && movie_done == 0) {
                    while (fLiveStatus != 0) {
                        UpdateFunction();
                    }
                }
            }
            Joylog::AddData(joylog_movie_done, 4, JOYLOG_CHANNEL_MOVIE_PLAYER_STATUS);
            if (joylog_movie_done != 0) {
                fStatus = fLiveStatus;
                eWaitUntilRenderingDone();
                cFEng::Get()->QueueGameMessage(0xc3960eb9, 0, 0xff);
                SoundPause(false, eSNDPAUSE_MOVIE);
                SetSoundControlState(false, SNDSTATE_PAUSE, "movie done");
                if (fPlayer != nullptr) {
                    delete fPlayer;
                }
                void* buf = RCMPDecodeBuffer;
                fPlayer = nullptr;
                RCMP_PlayerFree(buf);
                RCMPDecodeBuffer = nullptr;
                ResetTimer();
            }
        }
        HandleFatalError();
    }
}

void MoviePlayer::UpdateFunction() {
    static int recurse;
    if (recurse == 0) {
        int MovieFinished = 0;
        recurse = 1;
        bool ReDraw = false;
        SYNCTASK_run();
        THREAD_yield(0);
        if (!fPlayer->IsTimeForDecode() || CurFrame == nullptr) {
            ReDraw = true;
        } else {
            fPlayer->GetDecoder()->ReleaseFrame(CurFrame);
            CurFrame = fPlayer->GetFrame(fPlayer->GetGoalFrame());
        }
        if (CurFrame == nullptr) {
            MovieFinished = fPlayer->IsAudioFinished();
        } else if (!ReDraw) {
            FillInTextureInfo(static_cast<unsigned int*>(RCMPDecodeBuffer), &MovieTextureInfo, CurFrame->GetShape());
        }
        HandleFatalError();
        if (MovieFinished != 0) {
            eWaitUntilRenderingDone();
            fLiveStatus = 0;
        }
        recurse = 0;
    }
}

unsigned int MoviePlayer::GetMillisecondsPerFrame() {
    if (GetVideoMode() == 0) {
        return 20;
    }
    return 16;
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
