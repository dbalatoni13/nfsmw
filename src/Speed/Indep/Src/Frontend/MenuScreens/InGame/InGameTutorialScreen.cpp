#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/SubTitle.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"

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
    void LaunchMovie(const char *filename, const char *packageName);
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
    if (cFEng::mInstance->IsPackageInControl(GetLoadingScreenPackageName())) {
        cFEng::mInstance->QueuePackageSwitch(InGameTutorialScreenName, 0, 0, false);
    } else {
        cFEng::mInstance->QueuePackagePush(InGameTutorialScreenName, 0, 0, false);
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
