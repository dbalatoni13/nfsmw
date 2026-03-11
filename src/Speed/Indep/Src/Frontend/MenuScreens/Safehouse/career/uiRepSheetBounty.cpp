#include "uiRepSheetBounty.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;
struct FEMultiImage;

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
FEImage* FEngFindImage(const char* pkg_name, int hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);

extern unsigned int iCurrentViewBin;

uiRepSheetBounty::uiRepSheetBounty(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 3, 3, true) {
    bIsInGame = sd->Arg != 0;
    TrackMapStreamer = nullptr;
    TrackMap = nullptr;
    tutorialPlaying = false;
    TrackMapStreamer = new UITrackMapStreamer();
    if (!bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x216f1b81);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x578b767b);
    }
    Setup();
}

eMenuSoundTriggers uiRepSheetBounty::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
}

void uiRepSheetBounty::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    if (msg == 0x911ab364) {
        if (bIsInGame) {
            cFEng::Get()->QueuePackageSwitch("IG_BL_MAIN", 1, 0, false);
        } else {
            cFEng::Get()->QueuePackageSwitch("BL_MAIN", 0, 0, false);
        }
    }
}

void uiRepSheetBounty::Setup() {
    ClearData();
    SetInitialPosition(0);
    RefreshTrack();
    RefreshHeader();
}

void uiRepSheetBounty::RefreshTrack() {
}

void uiRepSheetBounty::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
}
