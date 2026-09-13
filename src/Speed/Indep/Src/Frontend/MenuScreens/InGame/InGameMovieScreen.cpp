
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
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

static bool gInGameMoviePlaying = false;
char InGameAnyMovieScreen::MovieFilename[64] = "unknown";

InGameAnyMovieScreen::InGameAnyMovieScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    bAllowingControllerErrors = FEManager::Get()->IsAllowingControllerError();
    FEManager::Get()->AllowControllerError(false);
    while (TheCarLoader.IsLoadingInProgress() || TheTrackStreamer.IsLoadingInProgress() || g_pEAXSound->AreResourceLoadsPending()) {
        MiniMainLoop();
    }
    DismissChyron();
    const u32 FEObj_movie = 0x348ff9f;
    FEngSetMovieName(GetPackageName(), FEObj_movie, MovieFilename);
    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());
    new EFadeScreenOff(FEHASH_15_IN);
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
    switch (msg) {
        case FEMSG_MOVIE_FINISHED:
            DismissMovie();
            break;
        case __PAD_START__:
        case __PAD_ACCEPT__: {
            extern int SkipMovies;
            if (FEDatabase->IsDDay() || SkipMovies || MoviePlayer_Bypass()) {
                mSubtitler.Update(FEMSG_MOVIE_FINISHED);
                DismissMovie();
            }
        } break;
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
    gInGameMoviePlaying = false;
    MNotifyMovieFinished().Post(0x20d60dbf);
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
