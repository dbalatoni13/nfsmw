#include "FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
class FEMovie;
FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
void FEngSetMovieName(FEMovie *movie, const char *name);
void DismissChyron();
void bStrNCpy(char *dst, const char *src, int size);
int bStrCmp(const char *a, const char *b);
bool eIsWidescreen();
unsigned int FEngHashString(const char *s);
void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int lang_hash);
unsigned int bStringHash(const char *s, unsigned int hash);
static const char *FEAnyTutorialScreenName = "FeTutorial.fng";
char FEAnyTutorialScreen::MovieFilename[64] = {};
char FEAnyTutorialScreen::PackageFilename[64] = {};
bool FEAnyTutorialScreen::PackageSet = false;
FEAnyTutorialScreen::FEAnyTutorialScreen(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , LastTime(0) //
    , TimeElapsed(0.0f) //
    , TextToggleTiming(0.0f) //
    , mTimer() //
    , mSubtitler()
{
    DismissChyron();
    FEMovie *movie = static_cast<FEMovie*>(FEngFindObject(GetPackageName(), 0x348FF9F));
    FEngSetMovieName(movie, MovieFilename);
    if (eIsWidescreen()) cFEng::Get()->QueuePackageMessage(0x70D2183B, GetPackageName(), nullptr);
    CareerSettings *career = FEDatabase->GetCareerSettings();
    unsigned int str_hash = 0;
    bool mSkipable = true;
    unsigned int label_hash;
    if (bStrCmp(MovieFilename, "DragRaceTutorial") == 0) {
        if (career && !career->HasDoneDragTutorial()) { mSkipable = false; career->SetHasDoneDragTutorial(); }
        label_hash = FEngHashString("DragTutText");
    } else if (bStrCmp(MovieFilename, "SpeedtrapTutorial") == 0) {
        if (career && !career->HasDoneSpeedTrapTutorial()) { mSkipable = false; career->SetHasDoneSpeedTrapTutorial(); }
        label_hash = FEngHashString("SpeedTrapTutText");
    } else if (bStrCmp(MovieFilename, "TollBoothTutorial") == 0) {
        if (career && !career->HasDoneTollBoothTutorial()) { mSkipable = false; career->SetHasDoneTollBoothTutorial(); }
        label_hash = FEngHashString("TollBoothTutText");
    } else if (bStrCmp(MovieFilename, "BountyTutorial") == 0) {
        if (career && !career->HasDoneBountyTutorial()) { mSkipable = false; career->SetHasDoneBountyTutorial(); }
        label_hash = FEngHashString("BountyTutText");
    } else if (bStrCmp(MovieFilename, "PursuitTutorial") == 0) {
        if (career && !career->HasDonePursuitTutorial()) { mSkipable = false; career->SetHasDonePursuitTutorial(); }
        label_hash = FEngHashString("PursuitTutText");
    } else {
        goto skip_labels;
    }
    str_hash = label_hash;
skip_labels:
    if (mSkipable) cFEng::Get()->QueuePackageMessage(0x59291F95, GetPackageName(), nullptr);
    {
        unsigned int einput = bStringHash("TutText", str_hash);
        FEngSetLanguageHash(GetPackageName(), 0x5A0EE0D9, einput);
        FEngSetLanguageHash(GetPackageName(), 0xF414BF3E, einput);
        FEngSetLanguageHash(GetPackageName(), 0x5A0EE0D8, einput);
        FEngSetLanguageHash(GetPackageName(), 0x07D2EA5D, einput);
    }
    mSubtitler.BeginningMovie(MovieFilename, GetPackageName());
    EFadeScreenOff *evt = new EFadeScreenOff(0x14035FB);
}
MenuScreen *FEAnyTutorialScreen::Create(ScreenConstructorData *sd) { return new(__FILE__, __LINE__) FEAnyTutorialScreen(sd); }
FEAnyTutorialScreen::~FEAnyTutorialScreen() { FEManager::Get()->SetEATraxSecondButton(); }
void FEAnyTutorialScreen::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    mSubtitler.Update(msg);
    if (msg == 0xB5AF2461 || msg == 0x406415E3) { DismissMovie(true); mSubtitler.Update(0xC3960EB9); }
    else if (msg == 0xC3960EB9) { DismissMovie(false); }
}
void FEAnyTutorialScreen::LaunchMovie(const char *filename, const char *packageName) {
    PackageSet = false; SetMovieName(filename);
    if (packageName) SetPackageName(packageName);
    cFEng::Get()->QueuePackagePush(FEAnyTutorialScreenName, 0, 0, false);
}
void FEAnyTutorialScreen::DismissMovie(bool send_message) {
    cFEng::Get()->QueuePackagePop(1);
    if (send_message) cFEng::Get()->QueueGameMessage(0xC3960EB9, PackageFilename, 0xFF);
}
void FEAnyTutorialScreen::SetMovieName(const char *filename) { bStrNCpy(MovieFilename, filename, 64); }
void FEAnyTutorialScreen::SetPackageName(const char *packageName) { PackageSet = true; bStrNCpy(PackageFilename, packageName, 64); }
