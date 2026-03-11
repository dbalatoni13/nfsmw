#include "uiRepSheetRaceEvents.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/Src/Misc/FixedPoint.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct FEObject;

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
FEImage* FEngFindImage(const char* pkg_name, int hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetTextureHash(FEImage* image, unsigned int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
unsigned int FEngHashString(const char* format, ...);
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash, bool);
const char* GetLocalizedString(unsigned int hash);
unsigned int CalcLanguageHash(const char* prefix, GRaceParameters* pRaceParams);
int FEngMapJoyParamToJoyport(int feng_param);
void StartRace();

inline float MPS2KPH(const float mps) {
    return mps * 3.6f;
}

extern unsigned int FEDBGetRaceIconHash(cFrontendDatabase*, GRace::Type) asm("GetRaceIconHash__17cFrontendDatabaseQ25GRace4Type");
extern unsigned int FEDBGetRaceNameHash(cFrontendDatabase*, GRace::Type) asm("GetRaceNameHash__17cFrontendDatabaseQ25GRace4Type");

struct GRaceSaveInfo {
    unsigned int mRaceHash;
    unsigned int mFlags;
    float mHighScores;
    FixedPoint< unsigned short, 10, 2 > mTopSpeed;
    FixedPoint< unsigned short, 10, 2 > mAverageSpeed;
};

extern unsigned int iCurrentViewBin;
extern GRaceParameters* theRace;

void RaceDatum::NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) {
    if (msg == 0xc407210) {
        theRace = race;
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
    switch (msg) {
    case 0xc98356ba:
        TrackMapStreamer.UpdateAnimation();
        break;
    case 0x9120409e:
    case 0x911c0a4b:
    case 0xb5971bf1:
        RefreshHeader();
        break;
    case 0x5073ef13:
    case 0xd9feec59:
        ToggleList();
        break;
    case 0x0c407210: {
        if (theRace == nullptr) {
            break;
        }
        signed char joyPort = static_cast< signed char >(FEngMapJoyParamToJoyport(param2));
        FEDatabase->SetPlayersJoystickPort(0, joyPort);
        const char* dialog = "";
        if (bIsInGame) {
            dialog = "InGameDialog.fng";
        }
        DialogInterface::ShowTwoButtons(GetPackageName(), dialog,
                                        static_cast< eDialogTitle >(1), 0x70E01038,
                                        0x417B25E4, 0xD05FC3A3, 0x34DC1BCF, 0x34DC1BCF,
                                        static_cast< eDialogFirstButtons >(1), 0x77CF03C5);
        break;
    }
    case 0xd05fc3a3:
        if (bIsInGame) {
            new ERaceSheetOff();
            GManager::Get().StartRaceFromInGame(theRace->GetEventHash());
        } else {
            GRaceCustom* race = GRaceDatabase::Get().AllocCustomRace(theRace);
            GRaceDatabase::Get().SetStartupRace(race, kRaceContext_Career);
            GRaceDatabase::Get().FreeCustomRace(race);
            StartRace();
        }
        break;
    case 0x34dc1bcf:
        break;
    case 0x911ab364:
        if (bIsInGame) {
            cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
        } else {
            cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
        }
        break;
    }
}

void UISafehouseRaceSheet::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    FEPrintf(GetPackageName(), 0x5a856a34, "%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", GetNumDatum());
    unsigned int eventsHash = 0x6475236d;
    if (currentEvents) {
        eventsHash = 0xc948ef80;
    }
    FEngSetLanguageHash(GetPackageName(), 0x78008599, eventsHash);
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %",
             GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %",
             GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
    ArrayDatum* datum = GetCurrentDatum();
    if (datum == nullptr) {
        return;
    }
    GRaceParameters* race = static_cast< RaceDatum* >(GetCurrentDatum())->race;
    FEPrintf(GetPackageName(), 0x13c45e, "%.0f", race->GetCashValue());
    const char* distUnits;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569a26a);
    } else {
        distUnits = GetLocalizedString(0x867dcfd9);
    }
    FEPrintf(GetPackageName(), 0x18b36f, "%d", race->GetNumLaps());
    FEPrintf(GetPackageName(), 0x80c9daa, "%ash.1f %s",
             race->GetRaceLengthMeters() * 0.001f, distUnits);
    unsigned int trackNameHash = CalcLanguageHash("TRACKNAME_", race);
    FEngSetLanguageHash(GetPackageName(), 0xf2cd475, trackNameHash);
    unsigned int copsHash = 0x73c615a3;
    if (race->GetCopsEnabled()) {
        copsHash = 0x61d1c5a5;
    }
    FEngSetLanguageHash(GetPackageName(), 0x9b21, copsHash);
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x1c8fc866));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x7af67920));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0xbbf970cd));
    GRaceSaveInfo* info = GRaceDatabase::Get().GetScoreInfo(race->GetEventHash());
    GRace::Type raceType = race->GetRaceType();
    if (raceType == GRace::kRaceType_P2P || raceType == GRace::kRaceType_Circuit ||
        raceType == GRace::kRaceType_Drag || raceType == GRace::kRaceType_Knockout ||
        raceType == GRace::kRaceType_Tollbooth) {
        if (info->mHighScores == 0.0f) {
            FEPrintf(GetPackageName(), 0x8fd41bb4, GetLocalizedString(0x472aa00a));
        } else {
            Timer t(info->mHighScores);
            char buf[64];
            t.PrintToString(buf, 0);
            FEPrintf(GetPackageName(), 0x8fd41bb4, "%s", buf);
        }
    } else {
        FEPrintf(GetPackageName(), 0x8fd41bb4, "%s", GetLocalizedString(0x472aa00a));
    }
    float top_speed;
    float avg_speed;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569a25f);
        top_speed = MPS2KPH(static_cast< float >(info->mTopSpeed));
        avg_speed = MPS2KPH(static_cast< float >(info->mAverageSpeed));
    } else {
        distUnits = GetLocalizedString(0x8569ab44);
        top_speed = MPS2MPH(static_cast< float >(info->mTopSpeed));
        avg_speed = MPS2MPH(static_cast< float >(info->mAverageSpeed));
    }
    FEPrintf(GetPackageName(), 0xebd7f926, "%ash.2f %s", top_speed, distUnits);
    FEPrintf(GetPackageName(), 0xde9145fb, "%ash.2f %s", avg_speed, distUnits);
    FEPrintf(GetPackageName(), 0x763f4b5b, "%ash.0f", race->GetCashValue());
    FEImage* img = FEngFindImage(GetPackageName(), 0xf97ec5d5);
    FEngSetTextureHash(img, FEDBGetRaceIconHash(FEDatabase, race->GetRaceType()));
    for (int i = 0; i < GetNumSlots(); i++) {
        RaceDatum* rdatum = static_cast< RaceDatum* >(GetDatumAt(i + GetStartDatumNum()));
        unsigned int check_hash = FEngHashString("MEDAL_THUMB_%d", i + 1);
        FEngSetInvisible(FEngFindObject(GetPackageName(), check_hash));
        if (rdatum == nullptr) {
            continue;
        }
        if (rdatum->IsLocked()) {
            FEngSetVisible(FEngFindObject(GetPackageName(), check_hash));
            FEngSetTextureHash(FEngFindImage(GetPackageName(), check_hash), 0x18ed48);
        } else if (rdatum->IsChecked()) {
            FEngSetVisible(FEngFindObject(GetPackageName(), check_hash));
            FEngSetTextureHash(FEngFindImage(GetPackageName(), check_hash), 0x28feadd);
        }
    }
    if (currentIndex != GetCurrentDatumNum() - 1 && GetCurrentDatum() != nullptr) {
        TrackMapStreamer.Init(static_cast< RaceDatum* >(GetCurrentDatum())->race,
                             TrackMap, 0, 0);
        currentIndex = GetCurrentDatumNum() - 1;
    }
}

bool UISafehouseRaceSheet::AddRace(GRaceParameters* race) {
    GRace::Type type = race->GetRaceType();
    if (type == GRace::kRaceType_JumpToSpeedTrap || type == GRace::kRaceType_JumpToMilestone) {
        return false;
    }
    RaceDatum* datum = new ("", 0) RaceDatum(
        FEDBGetRaceIconHash(FEDatabase, race->GetRaceType()),
        FEDBGetRaceNameHash(FEDatabase, race->GetRaceType()),
        race);
    AddDatum(datum);
    return true;
}

void UISafehouseRaceSheet::Setup() {
    ClearData();
    GRaceBin* bin = GRaceDatabase::Get().GetBin(iCurrentViewBin);
    if (bin != nullptr) {
        unsigned int count = bin->GetBossRaceCount();
        for (unsigned int i = 0; i < count; i++) {
            unsigned int hash = bin->GetBossRaceHash(i);
            GRaceParameters* race = GRaceDatabase::Get().GetRaceFromHash(hash);
            AddRace(race);
        }
    }
    SetInitialPosition(0);
    RefreshHeader();
}

void UISafehouseRaceSheet::ToggleList() {
}
