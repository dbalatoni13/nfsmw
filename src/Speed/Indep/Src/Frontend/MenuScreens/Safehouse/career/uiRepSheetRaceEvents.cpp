#include "uiRepSheetRaceEvents.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
FEImage* FEngFindImage(const char* pkg_name, int hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
void FEPrintf(const char* pkg_name, unsigned int hash, const char* format, ...);
unsigned int FEngHashString(const char* format, ...);
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash, bool);

extern unsigned int iCurrentViewBin;

void RaceDatum::NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) {
    if (msg == 0x406415e3) {
    }
}

UISafehouseRaceSheet::UISafehouseRaceSheet(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 3, 3, true) {
    bIsInGame = sd->Arg != 0;
    currentEvents = true;
    currentIndex = 0;
    TrackMap = nullptr;
    Setup();
}

UISafehouseRaceSheet::~UISafehouseRaceSheet() {
}

eMenuSoundTriggers UISafehouseRaceSheet::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    eMenuSoundTriggers result = ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x7b6b89d7 && bIsInGame) {
        return static_cast<eMenuSoundTriggers>(-1);
    }
    return result;
}

void UISafehouseRaceSheet::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    if (msg == 0x911ab364) {
        if (bIsInGame) {
            cFEng::Get()->QueuePackageSwitch("IG_BL_RIVAL", 1, 0, false);
        } else {
            cFEng::Get()->QueuePackageSwitch("BL_RIVAL", 0, 0, false);
        }
    }
}

void UISafehouseRaceSheet::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
}

bool UISafehouseRaceSheet::AddRace(GRaceParameters* race) {
    if (race == nullptr) {
        return false;
    }
    RaceDatum* datum = new RaceDatum();
    datum->race = race;
    AddDatum(datum);
    return true;
}

void UISafehouseRaceSheet::Setup() {
    ClearData();
    GRaceBin* bin = GRaceDatabase::mObj->GetBinByNumber(iCurrentViewBin);
    if (bin != nullptr) {
        unsigned int count = bin->GetBossRaceCount();
        for (unsigned int i = 0; i < count; i++) {
            unsigned int hash = bin->GetBossRaceHash(i);
            GRaceParameters* race = GRaceDatabase::mObj->GetRaceFromHash(hash);
            AddRace(race);
        }
    }
    SetInitialPosition(0);
    RefreshHeader();
}

void UISafehouseRaceSheet::ToggleList() {
}
