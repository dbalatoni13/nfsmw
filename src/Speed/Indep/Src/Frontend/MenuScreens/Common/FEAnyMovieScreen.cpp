#include "FEAnyMovieScreen.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern void DismissChyron();
extern bool MoviePlayer_Bypass();

struct FEMovie;
extern FEObject* FEngFindObject(const char* pkg_name, unsigned int obj_hash);
extern void FEngSetMovieName(FEMovie* movie, const char* name);

// GarageMainScreen already defined in uiMain.cpp (earlier in TU)

char FEAnyMovieScreen::MovieFilename[64];
char FEAnyMovieScreen::ReturnToPackageName[64];

FEAnyMovieScreen::FEAnyMovieScreen(ScreenConstructorData* sd)
    : MenuScreen(sd) //
    , mSubtitler() //
    , bHidGarage(false) //
    , bAllowingControllerErrors(false)
{
    const unsigned int FEObj_movie = 0x348FF9F;

    bAllowingControllerErrors = FEManager::Get()->IsAllowingControllerError();
    FEManager::Get()->AllowControllerError(false);

    FEMovie* movie = static_cast<FEMovie*>(FEngFindObject(GetPackageName(), FEObj_movie));
    FEngSetMovieName(movie, MovieFilename);

    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());
    DismissChyron();

    EFadeScreenOff* fadeEvent = new EFadeScreenOff(0x14035FB);

    GarageMainScreen* garageMainScreen = GarageMainScreen::GetInstance();
    if (garageMainScreen != nullptr && !garageMainScreen->IsVisable()) {
        garageMainScreen->NotificationMessage(0xAD4BBDCUL, nullptr, 0, 0);
        bHidGarage = true;
    }
}

FEAnyMovieScreen::~FEAnyMovieScreen() {
    if (bHidGarage) {
        GarageMainScreen* garageMainScreen = GarageMainScreen::GetInstance();
        if (garageMainScreen != nullptr) {
            garageMainScreen->NotificationMessage(0x18883F75UL, nullptr, 0, 0);
        }
    }
    FEManager::Get()->SetEATraxSecondButton();
    FEManager::Get()->AllowControllerError(bAllowingControllerErrors);
}

MenuScreen* FEAnyMovieScreen::Create(ScreenConstructorData* sd) {
    return new ("", 0) FEAnyMovieScreen(sd);
}

void FEAnyMovieScreen::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                                            unsigned long param2) {
    mSubtitler.Update(msg);

    switch (msg) {
    case 0xC3960EB9:
        DismissMovie();
        break;
    case 0x406415E3:
    case 0xB5AF2461:
        if (FEDatabase->IsDDay()) {
            if (!MoviePlayer_Bypass()) break;
        }
        mSubtitler.Update(0xC3960EB9);
        DismissMovie();
        break;
    }
}

void FEAnyMovieScreen::LaunchMovie(const char* return_to_pkg, const char* filename) {
    bStrNCpy(ReturnToPackageName, return_to_pkg, 64);
    SetMovieName(filename);
    cFEng::Get()->QueuePackageSwitch(GetFEngPackageName(), 0, 0, false);
}

void FEAnyMovieScreen::PlaySafehouseIntroMovie() {
    SetMovieName("SafehouseIntroMovie");
    bStrNCpy(ReturnToPackageName, "IG_SafehouseMain.fng", 64);
}

void FEAnyMovieScreen::DismissMovie() {
    if (ReturnToPackageName[0] != '\0') {
        cFEng::Get()->QueuePackageSwitch(ReturnToPackageName, 0, 0, false);
        ReturnToPackageName[0] = '\0';
    } else {
        if (FEDatabase->IsPostRivalMode()) {
            uiRepSheetRivalFlow* flow = uiRepSheetRivalFlow::Get();
            flow->Next();
        } else {
            cFEng::Get()->QueuePackagePop(1);
        }
    }
}

void FEAnyMovieScreen::SetMovieName(const char* filename) {
    bStrNCpy(MovieFilename, filename, 64);
}

extern int eIsWidescreen();

const char* FEAnyMovieScreen::GetFEngPackageName() {
    if (eIsWidescreen()) {
        return "FEAnyMovieScreenW.fng";
    }
    return "FEAnyMovieScreen.fng";
}