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
    TrackMap = nullptr;
    TrackMapStreamer = new UITrackMapStreamer();
    if (!bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x216f1b81);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x578b767b);
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
    int currentIndex = GetCurrentDatumNum();
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
    case 0xc407210: {
        if (theMilestone == nullptr) {
            return;
        }
        if (theMilestone->IsChecked()) {
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(7));
            return;
        }
        if (!bIsInGame) {
            int joyPort = FEngMapJoyParamToJoyport(param2);
            FEDatabase->SetPlayersJoystickPort(0, static_cast<signed char>(joyPort));
        }
        const char* dialog;
        if (bIsInGame) {
            dialog = "IG_DIALOG.fng";
        } else {
            dialog = "DIALOG.fng";
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
    case 0xc519bfc3:
        if (bIsInGame) {
            return;
        }
        FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
        FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
        return;
    case 0xc98356ba:
        if (TrackMapStreamer != nullptr) {
            TrackMapStreamer->UpdateAnimation();
        }
        return;
    case 0x911c0a4b:
    case 0xb5971bf1:
        break;
    case 0xc3960eb9: {
        if (bIsInGame) {
            FEngSetVisible("IG_BL_TRACKMAP.fng", 0x2716cdbf);
        }
        FEngSetScript(GetPackageName(), 0x99344537, 0x1744b3, true);
        if (theMilestone == nullptr) {
            return;
        }
        unsigned int marker;
        bool pursuit;
        if (theMilestone->GetType() != 0) {
            SpeedTrapDatum* st = static_cast<SpeedTrapDatum*>(theMilestone);
            GSpeedTrap* pSpeedTrap = st->my_speedtrap;
            marker = pSpeedTrap->GetJumpMarkerKey();
        } else {
            GMilestone* pMilestone = theMilestone->my_milestone;
            marker = pMilestone->GetJumpMarkerKey();
        }
        pursuit = theMilestone->GetType() == 0;
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
    case 0xd05fc3a3: {
        CareerSettings* career = FEDatabase->GetCareerSettings();
        if ((career->SpecialFlags & 0x200) == 0) {
            if (!bIsInGame) {
                FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
            } else {
                if (TrackMapStreamer != nullptr) {
                    delete TrackMapStreamer;
                }
                TrackMapStreamer = nullptr;
                InGameAnyTutorialScreenLaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
                FEngSetInvisible("IG_BL_TRACKMAP.fng", 0x2716cdbf);
            }
            FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
            FEngSetInvisible(GetPackageName(), FEngHashString("MASTERBLASTER"));
            career->SpecialFlags |= 0x200;
            return;
        }
        cFEng::Get()->QueueGameMessage(0xc3960eb9, GetPackageName(), 0xff);
        return;
    }
    case 0x911ab364:
        if (bIsInGame) {
            cFEng::Get()->QueuePackageSwitch("IG_BL_MAIN", 1, 0, false);
        } else {
            cFEng::Get()->QueuePackageSwitch("BL_MAIN", 0, 0, false);
        }
        return;
    case 0x72619778:
    case 0x9120409e:
        break;
    case 0x34dc1bcf:
        return;
    default:
        return;
    }
    int newIndex = GetCurrentDatumNum();
    if (currentIndex != newIndex && GetCurrentDatum() != nullptr) {
        RefreshTrack();
    }
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
    MilestoneDatum* datum = new MilestoneDatum();
    datum->my_milestone = milestone;
    AddDatum(datum);
}

void uiRepSheetMilestones::AddSpeedtrap(GSpeedTrap* trap) {
    SpeedTrapDatum* datum = new SpeedTrapDatum();
    datum->my_speedtrap = trap;
    AddDatum(datum);
}

void uiRepSheetMilestones::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    FEPrintf(GetPackageName(), 0x5a856a34, "%d/%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d/%d", GetNumDatum());
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %d", GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
    ArrayDatum* currentDatum = GetCurrentDatum();
    if (currentDatum != nullptr) {
        MilestoneDatum* d = static_cast<MilestoneDatum*>(currentDatum);
        if (d->GetType() == 0) {
            GMilestone* pMilestone = d->my_milestone;
            FEngSetTextureHash(GetPackageName(), 0xf97ec5d5,
                               FEDatabase->GetMilestoneIconHash(pMilestone->GetTypeKey(), true));
            FEPrintf(GetPackageName(), 0xb21d69bd, "%d", pMilestone->GetBounty());
            float goal = pMilestone->GetRequiredValue();
            if (FEDatabase->IsMilestoneTimeFormat(pMilestone->GetTypeKey())) {
                goal = goal * 0.001f;
            }
            char buf[32];
            bSNPrintf(buf, 32, "%d", static_cast<int>(goal));
            FEPrintf(GetPackageName(), 0x28049d6, "%s %s",
                     GetLocalizedString(FEDatabase->GetMilestoneDescHash(pMilestone->GetLocalizationTag())),
                     buf, buf);
        } else {
            SpeedTrapDatum* p = static_cast<SpeedTrapDatum*>(d);
            GSpeedTrap* pSpeedTrap = p->my_speedtrap;
            FEngSetTextureHash(GetPackageName(), 0xf97ec5d5,
                               FEDatabase->GetRaceIconHash(GRace::kRaceType_SpeedTrap));
            FEPrintf(GetPackageName(), 0xb21d69bd, "%d", pSpeedTrap->GetBounty());
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
            bSNPrintf(buf, 32, "%d %s", static_cast<int>(value), distUnits);
            FEPrintf(GetPackageName(), 0x28049d6, "%s %s",
                     GetLocalizedString(0xb14018bd), buf);
        }
        for (int i = 0; i < GetNumSlots(); i++) {
            ArrayDatum* datum = GetDatumAt(i + GetStartDatumNum());
            unsigned int check_hash = FEngHashString("CHECK_ICON_%d", i + 1);
            FEngSetInvisible(GetPackageName(), check_hash);
            if (datum == nullptr) {
                FEngSetInvisible(GetPackageName(), check_hash);
            } else if (!datum->IsLocked()) {
                if (!datum->IsChecked()) {
                    FEngSetInvisible(GetPackageName(), check_hash);
                } else {
                    FEngSetVisible(GetPackageName(), check_hash);
                    FEngSetTextureHash(GetPackageName(), check_hash, 0x28feadd);
                }
            } else {
                FEngSetVisible(GetPackageName(), check_hash);
                FEngSetTextureHash(GetPackageName(), check_hash, 0x18ed48);
            }
        }
    }
}

bool GSpeedTrap::IsFlagSet(unsigned int mask) const { return (mFlags & mask) != 0; }
bool GSpeedTrap::IsFlagClear(unsigned int mask) const { return (mFlags & mask) == 0; }
