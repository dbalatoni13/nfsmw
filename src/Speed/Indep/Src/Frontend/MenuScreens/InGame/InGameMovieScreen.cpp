static bool gInGameMoviePlaying;

#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"

struct FEMovie;
struct MenuScreen;
struct ScreenConstructorData;

extern int SkipMovies;
extern const char *GetLoadingScreenPackageName();
extern void MiniMainLoop();
extern void DismissChyron();
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
extern void FEngSetMovieName(FEMovie *movie, const char *name);

inline void FEngSetMovieName(const char *pkg_name, unsigned int obj_hash, const char *name) {
    FEngSetMovieName(reinterpret_cast<FEMovie *>(FEngFindObject(pkg_name, obj_hash)), name);
}
extern bool TrackStreamerIsLoadingInProgress() asm("IsLoadingInProgress__13TrackStreamer");

struct CarLoader {
    char _pad[0x14];       //
    int LoadingInProgress; //
    int IsLoadingInProgress() {
        return LoadingInProgress;
    }
};
extern CarLoader TheCarLoader;

struct InGameAnyMovieScreen : MenuScreen {
    InGameAnyMovieScreen(ScreenConstructorData *sd);
    ~InGameAnyMovieScreen() override;
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override;
    static void LaunchMovie(const char *filename);
    void DismissMovie();
    static bool IsPlaying();
    static void SetMovieName(const char *filename);
    static const char *GetFEngPackageName();
    static char MovieFilename[64];
    SubTitler mSubtitler;           // offset 0x2C
    bool bAllowingControllerErrors; // offset 0x50
};

bool InGameAnyMovieScreen::IsPlaying() {
    return gInGameMoviePlaying;
}

char InGameAnyMovieScreen::MovieFilename[64];

void InGameAnyMovieScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, 0x40);
}

const char *InGameAnyMovieScreen::GetFEngPackageName() {
    bool ps2_widescreen = eIsWidescreen();
    if (!ps2_widescreen) {
        return "InGameAnyMovie.fng";
    }
    return "WS_InGameAnyMovie.fng";
}

MenuScreen *InGameAnyMovieScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) InGameAnyMovieScreen(sd);
}

InGameAnyMovieScreen::InGameAnyMovieScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    bAllowingControllerErrors = FEManager::Get()->IsAllowingControllerError();
    FEManager::Get()->AllowControllerError(false);
    while (TheCarLoader.IsLoadingInProgress() || TrackStreamerIsLoadingInProgress() || g_pEAXSound->AreResourceLoadsPending()) {
        MiniMainLoop();
    }
    DismissChyron();
    FEngSetMovieName(GetPackageName(), 0x348ff9f, MovieFilename);
    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());
    new EFadeScreenOff(0x14035fb);
}

InGameAnyMovieScreen::~InGameAnyMovieScreen() {
    FEManager::Get()->AllowControllerError(bAllowingControllerErrors);
    gInGameMoviePlaying = false;
}

void InGameAnyMovieScreen::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    mSubtitler.Update(msg);
    if (msg != 0xb5af2461) {
        if (msg > 0xb5af2461) {
            if (msg != 0xc3960eb9) {
                return;
            }
            DismissMovie();
            return;
        }
        if (msg != 0x406415e3) {
            return;
        }
    }
    if (FEDatabase->GetCareerSettings()->GetCurrentBin() < 0x10 || SkipMovies || MoviePlayer_Bypass()) {
        mSubtitler.Update(0xc3960eb9);
        DismissMovie();
    }
}

void InGameAnyMovieScreen::LaunchMovie(const char *filename) {
    InGameAnyMovieScreen::SetMovieName(filename);
    gInGameMoviePlaying = true;
    if (cFEng::Get()->IsPackageInControl(GetLoadingScreenPackageName())) {
        cFEng::Get()->QueuePackageSwitch(GetFEngPackageName(), 0, 0, false);
    } else {
        cFEng::Get()->QueuePackagePush(GetFEngPackageName(), 0, 0, false);
    }
}

void InGameAnyMovieScreen::DismissMovie() {
    UCrc32 port(0x20d60dbf);
    gInGameMoviePlaying = false;
    MNotifyMovieFinished msg;
    msg.Post(port);
    cFEng::mInstance->QueuePackagePop(0);
    new EFadeScreenOn(false);
}
