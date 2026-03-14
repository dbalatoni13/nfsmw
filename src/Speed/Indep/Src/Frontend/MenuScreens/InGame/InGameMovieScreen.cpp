static bool gInGameMoviePlaying;

#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"

struct MenuScreen;
struct ScreenConstructorData;

extern int SkipMovies;
extern const char *GetLoadingScreenPackageName();

struct InGameAnyMovieScreen : MenuScreen {
    InGameAnyMovieScreen(ScreenConstructorData *sd);
    ~InGameAnyMovieScreen() override;
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override;
    void LaunchMovie(const char *filename);
    void DismissMovie();
    static bool IsPlaying();
    static void SetMovieName(const char *filename);
    static const char *GetFEngPackageName();
    static char MovieFilename[64];
    SubTitler mSubtitler;                  // offset 0x2C
    bool bAllowingControllerErrors;        // offset 0x50
};

extern bool eIsWidescreen();

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
    SetMovieName(filename);
    gInGameMoviePlaying = true;
    if (cFEng::mInstance->IsPackageInControl(GetLoadingScreenPackageName())) {
        cFEng::mInstance->QueuePackageSwitch(GetFEngPackageName(), 0, 0, false);
    } else {
        cFEng::mInstance->QueuePackagePush(GetFEngPackageName(), 0, 0, false);
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