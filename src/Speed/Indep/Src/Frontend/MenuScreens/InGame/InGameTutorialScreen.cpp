#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/InGameTutorialScreen.hpp"

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEMovies.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_Chyron.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"

extern bool gInGameMoviePlaying;

static const char *InGameTutorialScreenName = "InGameAnyTutorial.fng";

char InGameAnyTutorialScreen::MovieFilename[64] = "unknown";
char InGameAnyTutorialScreen::PackageFilename[64] = "unknown";
bool InGameAnyTutorialScreen::PackageSet = false;

InGameAnyTutorialScreen::InGameAnyTutorialScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    uint32 str_hash = 0;
    bool mSkipable = true;

    DismissChyron();
    FEngSetMovieName(GetPackageName(), 0x348ff9f, MovieFilename);

    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(0x70d2183b, GetPackageName(), nullptr);
    }

    CareerSettings *career = FEDatabase->GetCareerSettings();

    if (bStrCmp(MovieFilename, "drag_tutorial") == 0) {
        if ((career != nullptr) && !career->HasDoneDragTutorial()) {
            mSkipable = false;
            career->SetHasDoneDragTutorial();
        }
        str_hash = FEngHashString("TUTORIAL_DRAG");
    } else if (bStrCmp(MovieFilename, "speedtrap_tutorial") == 0) {
        if ((career != nullptr) && !career->HasDoneSpeedTrapTutorial()) {
            mSkipable = false;
            career->SetHasDoneSpeedTrapTutorial();
        }
        str_hash = FEngHashString("TUTORIAL_SPEEDTRAPRACE");
    } else if (bStrCmp(MovieFilename, "tollbooth_tutorial") == 0) {
        if ((career != nullptr) && !career->HasDoneTollBoothTutorial()) {
            mSkipable = false;
            career->SetHasDoneTollBoothTutorial();
        }
        str_hash = FEngHashString("TUTORIAL_TOLLBOOTH");
    } else if (bStrCmp(MovieFilename, "bounty_tutorial") == 0) {
        if ((career != nullptr) && !career->HasDoneBountyTutorial()) {
            mSkipable = false;
            career->SetHasDoneBountyTutorial();
        }
        str_hash = FEngHashString("TUTORIAL_BOUNTY");
    } else if (bStrCmp(MovieFilename, "pursuit_tutorial") == 0) {
        if ((career != nullptr) && !career->HasDonePursuitTutorial()) {
            mSkipable = false;
            career->SetHasDonePursuitTutorial();
        }
        str_hash = FEngHashString("TUTORIAL_PURSUIT");
    }

    if (mSkipable) {
        uint32 einput = 0x59291f95;
        cFEng::Get()->QueuePackageMessage(einput, GetPackageName(), nullptr);
    }

    uint32 label_hash = bStringHash("_LABEL", str_hash);
    FEngSetLanguageHash(GetPackageName(), 0x5a0ee0d9, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0xf414bf3e, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0x5a0ee0d8, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0x07d2ea5d, label_hash);

    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());
    new EFadeScreenOff(FEHASH_15_IN);
}

MenuScreen *InGameAnyTutorialScreen::Create(ScreenConstructorData *sd) {
    return new ("InGameAnyTutorialScreen", 0) InGameAnyTutorialScreen(sd);
}

void InGameAnyTutorialScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    mSubtitler.Update(msg);
    switch (msg) {
        case 0xc3960eb9:
            DismissMovie();
            break;

        case __PAD_START__:
        case __PAD_ACCEPT__:
            DismissMovie();
            mSubtitler.Update(0xc3960eb9);
            break;
    }
}

InGameAnyTutorialScreen::~InGameAnyTutorialScreen() {
    gInGameMoviePlaying = false;
}

void InGameAnyTutorialScreen::LaunchMovie(const char *filename, const char *packageName) {
    gInGameMoviePlaying = true;
    PackageSet = false;
    SetMovieName(filename);
    if (packageName != nullptr) {
        SetPackageName(packageName);
    }
    if (cFEng::Get()->IsPackageInControl(GetLoadingScreenPackageName())) {
        cFEng::Get()->QueuePackageSwitch(InGameTutorialScreenName, 0, 0, false);
    } else {
        cFEng::Get()->QueuePackagePush(InGameTutorialScreenName, 0, 0, false);
    }
}

void InGameAnyTutorialScreen::DismissMovie() {
    gInGameMoviePlaying = false;
    MNotifyMovieFinished().Post(0x20d60dbf);
    cFEng::Get()->QueuePackagePop(0);
    cFEng::Get()->QueueGameMessage(0xc3960eb9, PackageFilename, 0xff);
    new EFadeScreenOn(false);
}

void InGameAnyTutorialScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, sizeof(MovieFilename));
}

void InGameAnyTutorialScreen::SetPackageName(const char *packageName) {
    PackageSet = true;
    bStrNCpy(PackageFilename, packageName, sizeof(PackageFilename));
}
