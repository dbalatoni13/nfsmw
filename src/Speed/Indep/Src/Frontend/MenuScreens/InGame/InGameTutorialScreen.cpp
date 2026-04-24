#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"

extern unsigned int FEngHashString(const char *, ...);
extern unsigned int bStringHash(const char *, int);

extern bool gInGameMoviePlaying;

struct MenuScreen;
struct ScreenConstructorData;

extern const char *GetLoadingScreenPackageName();
static const char *InGameTutorialScreenName = "InGameTutorial.fng";

struct InGameAnyTutorialScreen : MenuScreen {
    InGameAnyTutorialScreen(ScreenConstructorData *sd);
    ~InGameAnyTutorialScreen() override;
    static MenuScreen *Create(ScreenConstructorData *sd);
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override;
    static void LaunchMovie(const char *filename, const char *packageName);
    void DismissMovie();
    static void SetMovieName(const char *filename);
    static void SetPackageName(const char *packageName);
    static char MovieFilename[64];
    static char PackageFilename[64];
    static bool PackageSet;
    SubTitler mSubtitler; // offset 0x2C
};

char InGameAnyTutorialScreen::MovieFilename[64];
char InGameAnyTutorialScreen::PackageFilename[64];
bool InGameAnyTutorialScreen::PackageSet;

void InGameAnyTutorialScreen::SetMovieName(const char *filename) {
    bStrNCpy(MovieFilename, filename, 0x40);
}

void InGameAnyTutorialScreen::SetPackageName(const char *packageName) {
    PackageSet = true;
    bStrNCpy(PackageFilename, packageName, 0x40);
}

MenuScreen *InGameAnyTutorialScreen::Create(ScreenConstructorData *sd) {
    return new ("", 0) InGameAnyTutorialScreen(sd);
}

InGameAnyTutorialScreen::InGameAnyTutorialScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    bool mSkipable = true;
    unsigned int str_hash = 0;
    const char *label;

    DismissChyron();
    FEngSetMovieName(GetPackageName(), 0x348ff9f, MovieFilename);

    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(0x70d2183b, GetPackageName(), 0);
    }

    CareerSettings *career = FEDatabase->GetCareerSettings();

    if (bStrCmp(MovieFilename, "drag_tutorial") == 0) {
        if (career && !career->HasDoneDragTutorial()) {
            mSkipable = false;
            career->SetHasDoneDragTutorial();
        }
        label = "TUTORIAL_DRAG";
    } else if (bStrCmp(MovieFilename, "speedtrap_tutorial") == 0) {
        if (career && !career->HasDoneSpeedTrapTutorial()) {
            mSkipable = false;
            career->SetHasDoneSpeedTrapTutorial();
        }
        label = "TUTORIAL_SPEEDTRAPRACE";
    } else if (bStrCmp(MovieFilename, "tollbooth_tutorial") == 0) {
        if (career && !career->HasDoneTollBoothTutorial()) {
            mSkipable = false;
            career->SetHasDoneTollBoothTutorial();
        }
        label = "TUTORIAL_TOLLBOOTH";
    } else if (bStrCmp(MovieFilename, "bounty_tutorial") == 0) {
        if (career && !career->HasDoneBountyTutorial()) {
            mSkipable = false;
            career->SetHasDoneBountyTutorial();
        }
        label = "TUTORIAL_BOUNTY";
    } else if (bStrCmp(MovieFilename, "pursuit_tutorial") == 0) {
        if (career && !career->HasDonePursuitTutorial()) {
            mSkipable = false;
            career->SetHasDonePursuitTutorial();
        }
        label = "TUTORIAL_PURSUIT";
    } else {
        goto skip_hash;
    }

    str_hash = FEngHashString(label);

skip_hash:
    if (mSkipable) {
        cFEng::Get()->QueuePackageMessage(0x59291f95, GetPackageName(), 0);
    }

    unsigned int label_hash = bStringHash("_LABEL", str_hash);
    FEngSetLanguageHash(GetPackageName(), 0x5a0ee0d9, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0xf414bf3e, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0x5a0ee0d8, label_hash);
    FEngSetLanguageHash(GetPackageName(), 0x07d2ea5d, label_hash);

    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());
    new EFadeScreenOff(0x14035fb);
}

InGameAnyTutorialScreen::~InGameAnyTutorialScreen() {
    gInGameMoviePlaying = false;
}

void InGameAnyTutorialScreen::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
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
    DismissMovie();
    mSubtitler.Update(0xc3960eb9);
}

void InGameAnyTutorialScreen::LaunchMovie(const char *filename, const char *packageName) {
    gInGameMoviePlaying = true;
    PackageSet = false;
    SetMovieName(filename);
    if (packageName) {
        SetPackageName(packageName);
    }
    if (cFEng::Get()->IsPackageInControl(GetLoadingScreenPackageName())) {
        cFEng::Get()->QueuePackageSwitch(InGameTutorialScreenName, 0, 0, false);
    } else {
        cFEng::Get()->QueuePackagePush(InGameTutorialScreenName, 0, 0, false);
    }
}

void InGameAnyTutorialScreen::DismissMovie() {
    UCrc32 port(0x20d60dbf);
    gInGameMoviePlaying = false;
    MNotifyMovieFinished msg;
    msg.Post(port);
    cFEng::mInstance->QueuePackagePop(0);
    cFEng::mInstance->QueueGameMessage(0xc3960eb9, PackageFilename, 0xff);
    new EFadeScreenOn(false);
}
