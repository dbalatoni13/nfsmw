#include "uiRepSheetMilestones.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
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
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash, bool);
void FEngSetRotationZ(FEObject* obj, float angle);
void FEngSetTextureHash(FEImage* image, unsigned int hash);
int FEngMapJoyParamToJoyport(int feng_param);

void RaceStarterStartCareerFreeRoam() asm("StartCareerFreeRoam__11RaceStarter");
void InGameAnyTutorialScreenLaunchMovie(const char*, const char*) asm("LaunchMovie__23InGameAnyTutorialScreenPCcT1");

extern unsigned int iCurrentViewBin;
extern const char* gTUTORIAL_MOVIE_PURSUIT;

// FEngSetInvisible/FEngSetVisible/FEngSetTextureHash inlines defined in uiMain.cpp

MilestoneDatum* theMilestone;

void MilestoneDatum::NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1,
                                         unsigned long param2) {
    if (msg != 0xc407210) {
        return;
    }
    if (!IsChecked()) {
        theMilestone = this;
    } else {
        theMilestone = nullptr;
    }
}

uiRepSheetMilestones::uiRepSheetMilestones(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 3, 3, true) {
    bIsInGame = sd->Arg != 0;
    TrackMapStreamer = nullptr;
    theMilestone = nullptr;
    TrackMapStreamer = new (__FILE__, __LINE__) UITrackMapStreamer();
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage* img = FEngFindImage(GetPackageName(), FEngHashString("EVENT_ICON_%d", i + 1));
        if (img) {
            AddSlot(new (__FILE__, __LINE__) ImageArraySlot(img));
        }
    }
    TrackMap = reinterpret_cast< FEMultiImage* >(
        FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    if (bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x578b767b);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x216f1b81);
    }
    Setup();
}

eMenuSoundTriggers uiRepSheetMilestones::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    eMenuSoundTriggers result = ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x7b6b89d7 && bIsInGame) {
        return static_cast<eMenuSoundTriggers>(-1);
    }
    return result;
}

void uiRepSheetMilestones::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    int currentIndex = data.TraversebList(currentDatum) - 1;
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    if (msg == 0x911c0a4b) {
        goto refresh;
    }
    if (msg <= 0x911c0a4b) {
        if (msg == 0x34dc1bcf) {
            return;
        }
        if (msg <= 0x34dc1bcf) {
            if (msg == 0xc407210) {
                goto handleActivate;
            }
            return;
        }
        if (msg == 0x72619778) {
            goto refresh;
        }
        if (msg == 0x911ab364) {
            goto handlePackageSwitch;
        }
        return;
    }
    if (msg == 0xc3960eb9) {
        goto handleWarp;
    }
    if (msg <= 0xc3960eb9) {
        if (msg == 0x9120409e || msg == 0xb5971bf1) {
            goto refresh;
        }
        return;
    }
    if (msg == 0xc98356ba) {
        goto handleUpdateAnimation;
    }
    if (msg <= 0xc98356ba) {
        if (msg == 0xc519bfc3) {
            goto handleTutorial;
        }
        return;
    }
    if (msg == 0xd05fc3a3) {
        goto handleTutorialAccept;
    }
    return;

handleActivate:
    {
        if (theMilestone == nullptr) {
            return;
        }
        if (theMilestone->IsChecked()) {
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(7));
            return;
        }
        if (!bIsInGame) {
            signed char joyPort = static_cast< signed char >(FEngMapJoyParamToJoyport(param1));
            FEDatabase->SetPlayersJoystickPort(0, joyPort);
        }
        const char* dialog = "";
        if (bIsInGame) {
            dialog = "InGameDialog.fng";
        }
        unsigned int messageHash = 0xa5a8409a;
        if (theMilestone->GetType() != 0) {
            messageHash = 0xbf1dcd38;
        }
        DialogInterface::ShowTwoButtons(GetPackageName(), dialog, static_cast<eDialogTitle>(1),
                                        0x70e01038, 0x417b25e4, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf,
                                        static_cast<eDialogFirstButtons>(1), messageHash);
        return;
    }

handleTutorial:
    {
        if (bIsInGame) {
            return;
        }
        FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
        FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
        return;
    }

handleWarp:
    {
        if (bIsInGame) {
            FEngSetVisible(FEngFindObject("InGameBackground.fng", 0x2716cdbf));
        }
        FEngSetScript(GetPackageName(), 0x99344537, 0x1744b3, true);
        if (theMilestone == nullptr) {
            return;
        }
        unsigned int marker;
        bool pursuit = false;
        if (theMilestone->GetType() == 0) {
            GMilestone* pMilestone = theMilestone->my_milestone;
            marker = pMilestone->GetJumpMarkerKey();
            pursuit = true;
        } else {
            SpeedTrapDatum* st = static_cast<SpeedTrapDatum*>(theMilestone);
            GSpeedTrap* pSpeedTrap = st->my_speedtrap;
            marker = pSpeedTrap->GetJumpMarkerKey();
        }
        if (bIsInGame) {
            new ERaceSheetOff();
            GManager::Get().WarpToMarker(marker, pursuit);
            return;
        }
        GManager::Get().OverrideFreeRoamStartMarker(marker);
        if (pursuit) {
            GManager::Get().QueueFreeRoamPursuit(0.0f);
        }
        RaceStarterStartCareerFreeRoam();
        return;
    }

handleUpdateAnimation:
    if (TrackMapStreamer != nullptr) {
        TrackMapStreamer->UpdateAnimation();
    }
    return;

refresh:
    {
        int newIndex = data.TraversebList(currentDatum) - 1;
        if (currentIndex != newIndex && currentDatum != nullptr) {
            RefreshTrack();
        }
        return;
    }

handleTutorialAccept:
    {
        CareerSettings* career = FEDatabase->GetCareerSettings();
        if (((career->SpecialFlags >> 9) & 1) == 0) {
            if (bIsInGame) {
                if (TrackMapStreamer != nullptr) {
                    delete TrackMapStreamer;
                }
                TrackMapStreamer = nullptr;
                InGameAnyTutorialScreenLaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
                FEngSetInvisible(FEngFindObject("InGameBackground.fng", 0x2716cdbf));
            } else {
                FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
            }
            FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
            FEngSetInvisible(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
            career->SpecialFlags |= 0x200;
            return;
        }
        cFEng::Get()->QueueGameMessage(0xc3960eb9, GetPackageName(), 0xff);
        return;
    }

handlePackageSwitch:
    if (bIsInGame) {
        cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
    } else {
        cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
    }
    return;
}

void uiRepSheetMilestones::Setup() {
    ClearData();
    GMilestone* ms = GManager::mObj->GetFirstMilestone(false, iCurrentViewBin);
    while (ms != nullptr) {
        AddMilestone(ms);
        if (ms->GetIsLocked()) {
            GetDatumAt(GetNumDatum() - 1)->SetLocked(true);
        }
        if (ms->GetIsAwarded()) {
            GetDatumAt(GetNumDatum() - 1)->SetChecked(true);
        }
        ms = GManager::mObj->GetNextMilestone(ms, false, iCurrentViewBin);
    }
    GSpeedTrap* st = GManager::mObj->GetFirstSpeedTrap(false, iCurrentViewBin);
    while (st != nullptr) {
        AddSpeedtrap(st);
        if (st->IsFlagClear(GSpeedTrap::kFlag_Unlocked)) {
            GetDatumAt(GetNumDatum() - 1)->SetLocked(true);
        }
        if (st->IsFlagSet(GSpeedTrap::kFlag_Completed)) {
            GetDatumAt(GetNumDatum() - 1)->SetChecked(true);
        }
        st = GManager::mObj->GetNextSpeedTrap(st, false, iCurrentViewBin);
    }
    SetDescLabel(0xB5117FDE);
    SetInitialPosition(0);
    RefreshTrack();
    RefreshHeader();
}

void uiRepSheetMilestones::RefreshTrack() {
    if (GetCurrentDatum() != nullptr) {
        bVector2 position;
        float rotation = 0.0f;
        if (TrackMapStreamer != nullptr) {
            TrackMapStreamer->Init(nullptr, TrackMap, 0, 0);
            TrackMapStreamer->ResetZoom(false);
        }
        MilestoneDatum* d = static_cast<MilestoneDatum*>(GetCurrentDatum());
        unsigned int key;
        if (d->GetType() == 0) {
            GMilestone* pMilestone = d->my_milestone;
            key = pMilestone->GetJumpMarkerKey();
        } else {
            SpeedTrapDatum* sdt = static_cast<SpeedTrapDatum*>(d);
            GSpeedTrap* pSpeedTrap = sdt->my_speedtrap;
            key = pSpeedTrap->GetJumpMarkerKey();
        }
        GManager::Get().CalcMapCoordsForMarker(key, position, rotation);
        if (TrackMapStreamer != nullptr) {
            TrackMapStreamer->PanTo(position);
            bVector2 zoom(0.5f, 0.5f);
            TrackMapStreamer->ZoomTo(zoom);
        }
        FEngSetRotationZ(FEngFindObject(GetPackageName(), 0xaf51dd73), rotation);
    }
}

void uiRepSheetMilestones::AddMilestone(GMilestone* milestone) {
    ArrayScroller* scroller = this;
    MilestoneDatum* datum = new MilestoneDatum(
        FEDatabase->GetMilestoneIconHash(milestone->GetTypeKey(), true),
        FEDatabase->GetMilestoneHeaderHash(milestone->GetLocalizationTag()),
        milestone);
    scroller->AddDatum(datum);
}

void uiRepSheetMilestones::AddSpeedtrap(GSpeedTrap* trap) {
    ArrayScroller* scroller = this;
    SpeedTrapDatum* datum = new SpeedTrapDatum(
        FEDatabase->GetRaceIconHash(static_cast<GRace::Type>(5)),
        0xF3B3D8DC,
        trap);
    scroller->AddDatum(datum);
}

void uiRepSheetMilestones::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    ArrayDatum* currentDatum = GetCurrentDatum();
    FEPrintf(GetPackageName(), 0x5a856a34, "%d", data.GetNodeNumber(currentDatum));
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", data.CountElements());
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %$d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %$d", GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
    if (currentDatum != nullptr) {
        MilestoneDatum* d = static_cast<MilestoneDatum*>(currentDatum);
        if (d->GetType() == 0) {
            GMilestone* pMilestone = d->my_milestone;
            FEngSetTextureHash(GetPackageName(), 0xf97ec5d5,
                               FEDatabase->GetMilestoneIconHash(pMilestone->GetTypeKey(), true));
            FEPrintf(GetPackageName(), 0xb21d69bd, "%$0.0f", pMilestone->GetBounty());
            float goal = pMilestone->GetRequiredValue();
            if (FEDatabase->IsMilestoneTimeFormat(pMilestone->GetTypeKey())) {
                goal = goal * 0.001f;
            }
            char buf[32];
            bSNPrintf(buf, 32, "%$0.0f", goal);
            FEPrintf(GetPackageName(), 0x28049d6, "%s %s",
                     GetLocalizedString(FEDatabase->GetMilestoneDescHash(pMilestone->GetLocalizationTag())),
                     buf, buf);
        } else {
            SpeedTrapDatum* p = static_cast<SpeedTrapDatum*>(d);
            GSpeedTrap* pSpeedTrap = p->my_speedtrap;
            FEngSetTextureHash(GetPackageName(), 0xf97ec5d5,
                               FEDatabase->GetRaceIconHash(GRace::kRaceType_SpeedTrap));
            FEPrintf(GetPackageName(), 0xb21d69bd, "%$0.0f", pSpeedTrap->GetBounty());
            float value;
            const char* distUnits;
            if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
                value = static_cast<float>(static_cast<int>(pSpeedTrap->GetTriggerSpeed() * 2.23699f));
                distUnits = GetLocalizedString(0x8569a25f);
            } else {
                value = static_cast<float>(static_cast<int>(pSpeedTrap->GetTriggerSpeed() * 3.6f));
                distUnits = GetLocalizedString(0x8569ab44);
            }
            char buf[32];
            bSNPrintf(buf, 32, "%$0.0f %s", value, distUnits);
            FEPrintf(GetPackageName(), 0x28049d6, "%s %s",
                     GetLocalizedString(0xb14018bd), buf);
        }
        for (int i = 0; i < GetNumSlots(); i++) {
            ArrayDatum* datum = GetDatumAt(i + GetStartDatumNum());
            unsigned int check_hash = FEngHashString("MEDAL_THUMB_%d", i + 1);
            FEngSetInvisible(GetPackageName(), check_hash);
            if (datum) {
                if (datum->IsLocked()) {
                    FEngSetVisible(GetPackageName(), check_hash);
                    FEngSetTextureHash(GetPackageName(), check_hash, 0x18ed48);
                } else if (datum->IsChecked()) {
                    FEngSetVisible(GetPackageName(), check_hash);
                    FEngSetTextureHash(GetPackageName(), check_hash, 0x28feadd);
                } else {
                    FEngSetInvisible(GetPackageName(), check_hash);
                }
            } else {
                FEngSetInvisible(GetPackageName(), check_hash);
            }
        }
    }
}

bool GSpeedTrap::IsFlagSet(unsigned int mask) const { return (mFlags & mask) != 0; }
bool GSpeedTrap::IsFlagClear(unsigned int mask) const { return (mFlags & mask) == 0; }
