#include "FEAnyMovieScreen.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
struct FEMovie;
struct GarageMainScreen {
    static GarageMainScreen *GetInstance();
    bool IsVisable();
    void NotificationMessage(unsigned long, unsigned long, unsigned long, unsigned long);
};
FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
void FEngSetMovieName(FEMovie *movie, const char *name);
void DismissChyron();
void bStrNCpy(char *dst, const char *src, int size);
bool eIsWidescreen();
char FEAnyMovieScreen::MovieFilename[64] = {};
char FEAnyMovieScreen::ReturnToPackageName[64] = {};
FEAnyMovieScreen::FEAnyMovieScreen(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , mSubtitler() //
    , bHidGarage(false) //
    , bAllowingControllerErrors(false)
{
    bAllowingControllerErrors = FEManager::Get()->IsAllowingControllerError();
    FEManager::Get()->AllowControllerError(false);
    FEMovie *movie = static_cast<FEMovie*>(FEngFindObject(GetPackageName(), 0x348FF9F));
    FEngSetMovieName(movie, MovieFilename);
    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());
    DismissChyron();
    EFadeScreenOff *evt = new EFadeScreenOff(0x14035FB);
    GarageMainScreen *gs = GarageMainScreen::GetInstance();
    if (gs && !gs->IsVisable()) { gs->NotificationMessage(0xAD4BBDC, 0, 0, 0); bHidGarage = true; }
}
FEAnyMovieScreen::~FEAnyMovieScreen() {
    if (bHidGarage) { GarageMainScreen *gs = GarageMainScreen::GetInstance(); if (gs) gs->NotificationMessage(0x18883F75, 0, 0, 0); }
    FEManager::Get()->SetEATraxSecondButton();
    FEManager::Get()->AllowControllerError(bAllowingControllerErrors);
}
MenuScreen *FEAnyMovieScreen::Create(ScreenConstructorData *sd) { return new(__FILE__, __LINE__) FEAnyMovieScreen(sd); }
void FEAnyMovieScreen::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    mSubtitler.Update(msg);
    if (msg == 0xB5AF2461 || msg == 0x406415E3) {
        if (FEDatabase->IsDDay() || MoviePlayer_Bypass()) { mSubtitler.Update(0xC3960EB9); DismissMovie(); }
    } else if (msg == 0xC3960EB9) { DismissMovie(); }
}
void FEAnyMovieScreen::LaunchMovie(const char *return_to_pkg, const char *filename) {
    bStrNCpy(ReturnToPackageName, return_to_pkg, 64); SetMovieName(filename);
    cFEng::Get()->QueuePackageSwitch(GetFEngPackageName(), 0, 0, false);
}
void FEAnyMovieScreen::PlaySafehouseIntroMovie() {
    SetMovieName("SafehouseIntroMovie"); bStrNCpy(ReturnToPackageName, "FeMainMenu.fng", 64);
}
void FEAnyMovieScreen::DismissMovie() {
    if (ReturnToPackageName[0] != '\0') {
        cFEng::Get()->QueuePackageSwitch(ReturnToPackageName, 0, 0, false);
        ReturnToPackageName[0] = '\0';
    } else {
        if (FEDatabase->IsPostRivalMode()) uiRepSheetRivalFlow::Get()->Next();
        else cFEng::Get()->QueuePackagePop(1);
    }
}
void FEAnyMovieScreen::SetMovieName(const char *filename) { bStrNCpy(MovieFilename, filename, 64); }
const char *FEAnyMovieScreen::GetFEngPackageName() {
    if (eIsWidescreen()) return "FeMovieWide.fng";
    return "FeMovie.fng";
}
