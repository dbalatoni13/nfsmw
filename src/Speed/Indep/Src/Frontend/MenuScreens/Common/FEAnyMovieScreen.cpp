#include "FEAnyMovieScreen.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Event.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEMovies.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

// Defined in other units.
void DismissChyron();

char FEAnyMovieScreen::MovieFilename[64] = "unknown";
char FEAnyMovieScreen::ReturnToPackageName[64];

FEAnyMovieScreen::FEAnyMovieScreen(ScreenConstructorData *sd) : MenuScreen(sd), mSubtitler(), bHidGarage(false) {
    const u32 FEObj_movie = 0x348FF9F;

    bAllowingControllerErrors = FEManager::Get()->IsAllowingControllerError();
    FEManager::Get()->AllowControllerError(false);

    FEngSetMovieName(GetPackageName(), FEObj_movie, MovieFilename);

    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());
    DismissChyron();

    new EFadeScreenOff(FEHASH_15_IN);

    GarageMainScreen *garageMainScreen = GarageMainScreen::GetInstance();
    if (garageMainScreen != nullptr && garageMainScreen->IsVisable()) {
        garageMainScreen->NotificationMessage(0xAD4BBDC, nullptr, 0, 0);
        bHidGarage = true;
    }
}

FEAnyMovieScreen::~FEAnyMovieScreen() {
    if (bHidGarage != 0) {
        GarageMainScreen *garage = GarageMainScreen::GetInstance();
        if (garage != 0) {
            garage->NotificationMessage(0x18883F75, 0, 0, 0);
        }
    }
    FEManager::Get()->SetEATraxSecondButton();
    FEManager::Get()->AllowControllerError(bAllowingControllerErrors);
}

MenuScreen *FEAnyMovieScreen::Create(ScreenConstructorData *sd) {
    return new ("FEAnyMovieScreen", 0) FEAnyMovieScreen(sd);
}

void FEAnyMovieScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    mSubtitler.Update(msg);

    switch (msg) {
        case FEMSG_MOVIE_FINISHED:
            DismissMovie();
            break;
        case __PAD_ACCEPT__:
        case __PAD_START__:
            if (FEDatabase->IsDDay()) {
                if (!MoviePlayer_Bypass())
                    break;
            }
            mSubtitler.Update(0xC3960EB9);
            DismissMovie();
            break;
    }
}

void FEAnyMovieScreen::LaunchMovie(const char *return_to_pkg, const char *filename) {
    bStrNCpy(ReturnToPackageName, return_to_pkg, FEMOVIENAMESIZE);
    SetMovieName(filename);
    cFEng::Get()->QueuePackageSwitch(GetFEngPackageName(), 0, 0, 0);
}

void FEAnyMovieScreen::PlaySafehouseIntroMovie() {
    SetMovieName("storyfmv_saf25");
    bStrNCpy(ReturnToPackageName, "SafeHouseRivalBio.fng", FEMOVIENAMESIZE);
}

void FEAnyMovieScreen::DismissMovie() {
    if (ReturnToPackageName[0] != 0) {
        cFEng::Get()->QueuePackageSwitch(ReturnToPackageName, 0, 0, 0);
        ReturnToPackageName[0] = 0;
    } else if (FEDatabase->IsPostRivalMode()) {
        uiRepSheetRivalFlow::Get()->Next();
    } else {
        cFEng::Get()->QueuePackagePop(1);
    }
}

void FEAnyMovieScreen::SetMovieName(const char *movie_name) {
    bStrNCpy(MovieFilename, movie_name, FEMOVIENAMESIZE);
}

const char *FEAnyMovieScreen::GetFEngPackageName() {
    if (eIsWidescreen()) {
        return "WS_FEAnyMovie.fng";
    }
    return "FEAnyMovie.fng";
}
