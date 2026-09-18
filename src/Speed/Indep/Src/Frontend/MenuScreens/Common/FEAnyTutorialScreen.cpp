#include "FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEMovies.hpp"
#include "Speed/Indep/Src/FEng/FEMovie.h"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

struct FEMovie;
extern void DismissChyron();
extern unsigned int FEngHashString(const char *, ...);
extern unsigned int bStringHash(const char *, int);

// [fe3] FEngSetMovieName(pkg,hash,name) ya es inline en FEngInterfaces/FEngInterfaceFEMovies.hpp

char FEAnyTutorialScreen::MovieFilename[64] = "unknown";
char FEAnyTutorialScreen::PackageFilename[64] = "unknown";
bool FEAnyTutorialScreen::PackageSet = false;

static const char *FEAnyTutorialScreenName = "FEAnyTutorial.fng";

FEAnyTutorialScreen::FEAnyTutorialScreen(ScreenConstructorData *sd)
    : MenuScreen(sd), //
      mTimer(0) {
    unsigned int str_hash = 0;
    bool mSkipable = true;

    DismissChyron();

    FEngSetMovieName(GetPackageName(), 0x0348FF9F, MovieFilename);

    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(0x70D2183B, GetPackageName(), nullptr);
    }

    CareerSettings *career = FEDatabase->GetCareerSettings();

    if (bStrCmp(MovieFilename, "drag_tutorial") == 0) {
        if (career != nullptr && !(career->SpecialFlags & 0x40)) {
            career->SpecialFlags |= 0x40;
            mSkipable = false;
        }
        str_hash = FEngHashString("TUTORIAL_DRAG");
    } else if (bStrCmp(MovieFilename, "speedtrap_tutorial") == 0) {
        if (career != nullptr && !(career->SpecialFlags & 0x80)) {
            career->SpecialFlags |= 0x80;
            mSkipable = false;
        }
        str_hash = FEngHashString("TUTORIAL_SPEEDTRAPRACE");
    } else if (bStrCmp(MovieFilename, "tollbooth_tutorial") == 0) {
        if (career != nullptr && !(career->SpecialFlags & 0x100)) {
            career->SpecialFlags |= 0x100;
            mSkipable = false;
        }
        str_hash = FEngHashString("TUTORIAL_TOLLBOOTH");
    } else if (bStrCmp(MovieFilename, "bounty_tutorial") == 0) {
        if (career != nullptr && !(career->SpecialFlags & 0x400)) {
            career->SpecialFlags |= 0x400;
            mSkipable = false;
        }
        str_hash = FEngHashString("TUTORIAL_BOUNTY");
    } else if (bStrCmp(MovieFilename, "pursuit_tutorial") == 0) {
        if (career != nullptr && !(career->SpecialFlags & 0x200)) {
            career->SpecialFlags |= 0x200;
            mSkipable = false;
        }
        str_hash = FEngHashString("TUTORIAL_PURSUIT");
    }

    if (mSkipable) {
        cFEng::Get()->QueuePackageMessage(0x59291F95, GetPackageName(), nullptr);
    }

    unsigned int label_hash = bStringHash("_LABEL", str_hash);
    FEngSetLanguageHash(GetPackageName(), 0x5A0EE0D9, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0xF414BF3E, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0x5A0EE0D8, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0x07D2EA5D, label_hash);

    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());

    new EFadeScreenOff(0x14035FB);
}

MenuScreen *FEAnyTutorialScreen::Create(ScreenConstructorData *sd) {
    return new ("FEAnyTutorialScreen", 0) FEAnyTutorialScreen(sd);
}

FEAnyTutorialScreen::~FEAnyTutorialScreen() {
    FEManager::Get()->SetEATraxSecondButton();
}

void FEAnyTutorialScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    mSubtitler.Update(msg);

    switch (msg) {
        case 0xC3960EB9:
            DismissMovie(false);
            break;
        case 0xB5AF2461:
        case 0x406415E3:
            DismissMovie(true);
            mSubtitler.Update(0xC3960EB9);
            break;
    }
}

void FEAnyTutorialScreen::LaunchMovie(const char *filename, const char *packageName) {
    PackageSet = false;
    SetMovieName(filename);
    if (packageName != nullptr) {
        SetPackageName(packageName);
    }
    cFEng::Get()->QueuePackagePush(FEAnyTutorialScreenName, 0, 0, false);
}

void FEAnyTutorialScreen::DismissMovie(bool send_message) {
    cFEng::Get()->QueuePackagePop(1);
    if (send_message) {
        cFEng::Get()->QueueGameMessage(0xC3960EB9, PackageFilename, 0xFF);
    }
}

void FEAnyTutorialScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, 64);
}

void FEAnyTutorialScreen::SetPackageName(const char *packageName) {
    PackageSet = true;
    bStrNCpy(PackageFilename, packageName, 64);
}
