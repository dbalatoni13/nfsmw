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
        if (!IsLocked()) {
            theRace = race;
        }
    }
}

UISafehouseRaceSheet::UISafehouseRaceSheet(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 3, 3, true) {
    bIsInGame = sd->Arg != 0;
    currentEvents = true;
    currentIndex = 0;
    theRace = nullptr;
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage* image = FEngFindImage(GetPackageName(), FEngHashString("EVENT_ICON_%d", i + 1));
        if (image) {
            AddSlot(new ImageArraySlot(image));
        }
    }
    TrackMap = reinterpret_cast<FEMultiImage*>(
        FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    if (bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x2f32a021);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x84e4a54c);
    }
    Setup();
}

UISafehouseRaceSheet::~UISafehouseRaceSheet() {
}

eMenuSoundTriggers UISafehouseRaceSheet::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    eMenuSoundTriggers result = ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x7b6b89d7 && !theRace) {
        return static_cast<eMenuSoundTriggers>(7);
    }
    return result;
}

void UISafehouseRaceSheet::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
    case 0xc98356ba:
        TrackMapStreamer.UpdateAnimation();
        break;
    case 0x72619778:
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
        if (!bIsInGame) {
            signed char joyPort = static_cast< signed char >(FEngMapJoyParamToJoyport(param1));
            FEDatabase->SetPlayersJoystickPort(0, joyPort);
        }
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
    case 0x911ab364:
        if (bIsInGame) {
            cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
        } else {
            cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
        }
        break;
    case 0x34dc1bcf:
        break;
    }
}

void UISafehouseRaceSheet::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    FEPrintf(GetPackageName(), 0x5a856a34, "%d", data.TraversebList(currentDatum));
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", data.TraversebList(nullptr));
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
    ArrayDatum* datum = currentDatum;
    if (datum == nullptr) {
        return;
    }
    GRaceParameters* race = static_cast< RaceDatum* >(datum)->race;
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
    unsigned int copsHash;
    if (race->GetCopsEnabled()) {
        copsHash = 0x61d1c5a5;
    } else {
        copsHash = 0x73c615a3;
    }
    FEngSetLanguageHash(GetPackageName(), 0x9b21, copsHash);
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x1c8fc866));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x7af67920));
    FEngSetInvisible(FEngFindObject(GetPackageName(), 0xbbf970cd));
    GRaceSaveInfo* info = GRaceDatabase::Get().GetScoreInfo(race->GetEventHash());
    if (race->GetRaceType() == GRace::kRaceType_P2P || race->GetRaceType() == GRace::kRaceType_Circuit ||
        race->GetRaceType() == GRace::kRaceType_Drag || race->GetRaceType() == GRace::kRaceType_Knockout ||
        race->GetRaceType() == GRace::kRaceType_Tollbooth) {
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
        avg_speed = MPS2KPH(static_cast< float >(info->mAverageSpeed));
        top_speed = MPS2KPH(static_cast< float >(info->mTopSpeed));
    } else {
        distUnits = GetLocalizedString(0x8569ab44);
        avg_speed = MPS2MPH(static_cast< float >(info->mAverageSpeed));
        top_speed = MPS2MPH(static_cast< float >(info->mTopSpeed));
    }
    FEPrintf(GetPackageName(), 0xebd7f926, "%ash.2f %s", top_speed, distUnits);
    FEPrintf(GetPackageName(), 0xde9145fb, "%ash.2f %s", avg_speed, distUnits);
    FEPrintf(GetPackageName(), 0x763f4b5b, "%ash.0f", race->GetCashValue());
    unsigned int iconHash = FEDBGetRaceIconHash(FEDatabase, race->GetRaceType());
    FEImage* img = FEngFindImage(GetPackageName(), 0xf97ec5d5);
    FEngSetTextureHash(img, iconHash);
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
    if (currentIndex != data.TraversebList(currentDatum) - 1 && currentDatum != nullptr) {
        TrackMapStreamer.Init(static_cast< RaceDatum* >(currentDatum)->race,
                             TrackMap, 0, 0);
        currentIndex = data.TraversebList(currentDatum) - 1;
    }
}

bool UISafehouseRaceSheet::AddRace(GRaceParameters* race) {
    GRace::Type type = race->GetRaceType();
    switch (type) {
    case GRace::kRaceType_JumpToSpeedTrap:
    case GRace::kRaceType_JumpToMilestone:
        return false;
    default:
        break;
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
    if (currentEvents) {
        GRaceBin* bin = GRaceDatabase::Get().GetBinNumber(iCurrentViewBin);
        for (unsigned int i = 0; i < bin->GetWorldRaceCount(); i++) {
            unsigned int raceHash = bin->GetWorldRaceHash(i);
            GRaceParameters* race = GRaceDatabase::Get().GetRaceFromHash(raceHash);
            if (AddRace(race)) {
                GetDatumAt(GetNumDatum() - 1)->SetLocked(false);
                if (GRaceDatabase::Get().IsCareerRaceComplete(raceHash)) {
                    GetDatumAt(GetNumDatum() - 1)->SetChecked(true);
                }
            }
        }
    } else {
        unsigned int bindex = FEDatabase->GetCareerSettings()->GetCurrentBin();
        while (bindex <= GRaceDatabase::Get().GetBinCount()) {
            GRaceBin* bin = GRaceDatabase::Get().GetBinNumber(bindex);
            bindex++;
            if (bin != nullptr) {
                for (unsigned int i = 0; i < bin->GetWorldRaceCount(); i++) {
                    if (AddRace(GRaceDatabase::Get().GetRaceFromHash(bin->GetWorldRaceHash(i)))) {
                        GetDatumAt(GetNumDatum() - 1)->SetLocked(false);
                    }
                }
            }
        }
    }
    SetDescLabel(0x9ba78ba2);
    if (GetCurrentDatum() != nullptr) {
        RaceDatum* datum = static_cast< RaceDatum* >(GetCurrentDatum());
        TrackMapStreamer.Init(datum->race, TrackMap, 0, 0);
    }
    SetInitialPosition(0);
    RefreshHeader();
}

void UISafehouseRaceSheet::ToggleList() {
}
