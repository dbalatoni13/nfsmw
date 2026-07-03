
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEMovies.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_Chyron.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/InGameMovieScreen.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Misc/Main.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"

static bool gInGameMoviePlaying;
char InGameAnyMovieScreen::MovieFilename[64];

InGameAnyMovieScreen::InGameAnyMovieScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    bAllowingControllerErrors = FEManager::Get()->IsAllowingControllerError();
    FEManager::Get()->AllowControllerError(false);
    while (TheCarLoader.IsLoadingInProgress() || TheTrackStreamer.IsLoadingInProgress() || g_pEAXSound->AreResourceLoadsPending()) {
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

MenuScreen *InGameAnyMovieScreen::Create(ScreenConstructorData *sd) {
    return new ("InGameAnyMovieScreen", 0) InGameAnyMovieScreen(sd);
}

void InGameAnyMovieScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
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

bool InGameAnyMovieScreen::IsPlaying() {
    return gInGameMoviePlaying;
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
    cFEng::Get()->QueuePackagePop(0);
    new EFadeScreenOn(false);
}

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
