#include "uiRepSheetMilestones.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/InGameTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

extern int iCurrentViewBin;

MilestoneDatum *theMilestone = nullptr;
const char *gTUTORIAL_MOVIE_PURSUIT = "pursuit_tutorial";

void MilestoneDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == __BUTTON_PRESSED__) {
#ifdef EA_BUILD_A124
        theMilestone = this;
#else
        if (!IsChecked()) {
            theMilestone = this;
        } else {
            theMilestone = nullptr;
        }
#endif
    }
}

uiRepSheetMilestones::uiRepSheetMilestones(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 3, 3, true) {
    bIsInGame = sd->Arg != 0;
    TrackMapStreamer = nullptr;
    theMilestone = nullptr;
    TrackMapStreamer = new ("UITrackMapStreamer", 0) UITrackMapStreamer();
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("EVENT_ICON_%d", i + 1));
        if (img != nullptr) {
            AddSlot(new ("ImageArraySlot", 0) ImageArraySlot(img));
        }
    }
    TrackMap = reinterpret_cast<FEMultiImage *>(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    if (bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x578b767b);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x216f1b81);
    }
    Setup();
}

eMenuSoundTriggers uiRepSheetMilestones::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    maybe = ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x7b6b89d7 && bIsInGame) {
        return UISND_NONE;
    }
    return maybe;
}

void uiRepSheetMilestones::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    int currentIndex = GetNumDatum() - 1;
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    // UNSOLVED
    switch (msg) {
        case __BUTTON_PRESSED__: {
            if (theMilestone == nullptr) {
                break;
            }
            if (theMilestone->IsChecked()) {
                g_pEAXSound->PlayUISoundFX(UISND_COMMON_WRONG);
                break;
            }
            if (!bIsInGame) {
                int joyPort = FEngMapJoyParamToJoyport(param1);
                FEDatabase->SetPlayersJoystickPort(0, joyPort);
            }
            const char *dialog = "";
            if (bIsInGame) {
                dialog = "InGameDialog.fng";
            }
            uint32 messageHash = 0xa5a8409a;
            if (theMilestone->GetType() != 0) {
                messageHash = 0xbf1dcd38;
            }
            DialogInterface::ShowTwoButtons(GetPackageName(), dialog, dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes,
                                            dialog_message_no, dialog_message_no, first_dialog_button2, messageHash);
            break;
        }
        case __PAD_BUTTON4__:
            if (!bIsInGame) {
                const u32 FEObj_MASTERBLASTER = 0x99344537;
                const u32 FEObj_HIDE = FEHASH_HIDE;
                FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_HIDE, true);
                FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
            }
            break;
        case 0xc3960eb9: {
            if (bIsInGame) {
                const u32 FEObj_BACKGROUND = 0x2716cdbf;
                FEngSetVisible(FEngFindObject("InGameBackground.fng", FEObj_BACKGROUND));
            }
            const u32 FEObj_MASTERBLASTER = 0x99344537;
            const u32 FEObj_Init = FEHASH_INIT;

            FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_Init, true);
            if (theMilestone == nullptr) {
                return;
            }
            Attrib::Key marker;
            bool pursuit = false;
            if (theMilestone->GetType() == 0) {
                GMilestone *pMilestone = theMilestone->my_milestone;
                marker = pMilestone->GetJumpMarkerKey();
                pursuit = true;
            } else {
                GSpeedTrap *pSpeedTrap = static_cast<SpeedTrapDatum *>(theMilestone)->my_speedtrap;
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
            RaceStarter::StartCareerFreeRoam();
            return;
        }
        case FEMSG_SCREEN_TICK:
            if (TrackMapStreamer != nullptr) {
                TrackMapStreamer->UpdateAnimation();
            }
            break;
        case __PAD_DOWN__:
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
        case __PAD_UP__: {
            if (currentIndex != GetNumDatum() - 1 && GetCurrentDatum() != nullptr) {
                RefreshTrack();
            }
            break;
        }
        case dialog_message_yes: {
            UserProfile *prof = FEDatabase->GetUserProfile(0);
            CareerSettings *career = FEDatabase->GetCareerSettings();
            if (!career->HasDonePursuitTutorial()) {
                if (bIsInGame) {
                    if (TrackMapStreamer != nullptr) {
                        delete TrackMapStreamer;
                    }
                    TrackMapStreamer = nullptr;
                    InGameAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
                    const u32 FEObj_BACKGROUND = 0x2716cdbf;
                    FEngSetInvisible(FEngFindObject("InGameBackground.fng", FEObj_BACKGROUND));
                } else {
                    FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_PURSUIT, GetPackageName());
                }
                const u32 FEObj_MASTERBLASTER = 0x99344537;
                const u32 FEObj_HIDE = FEHASH_HIDE;
                FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_HIDE, true);
                FEngSetInvisible(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
                career->SpecialFlags |= 0x200;
                return;
            }
            cFEng::Get()->QueueGameMessage(0xc3960eb9, GetPackageName(), 0xff);
            break;
        }
        case __PAD_BACK__:
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
            }
            break;
        case dialog_message_no:
            break;
    }
}

void uiRepSheetMilestones::Setup() {
    ClearData();
    GMilestone *pMilestone = GManager::Get().GetFirstMilestone(false, iCurrentViewBin);
    while (pMilestone != nullptr) {
        AddMilestone(pMilestone);
        if (pMilestone->GetIsLocked()) {
            GetDatumAt(GetNumDatum() - 1)->SetLocked(true);
        }
        if (pMilestone->GetIsAwarded()) {
            GetDatumAt(GetNumDatum() - 1)->SetChecked(true);
        }
        pMilestone = GManager::Get().GetNextMilestone(pMilestone, false, iCurrentViewBin);
    }
    GSpeedTrap *pSpeedTrap = GManager::Get().GetFirstSpeedTrap(false, iCurrentViewBin);
    while (pSpeedTrap != nullptr) {
        AddSpeedtrap(pSpeedTrap);
        if (pSpeedTrap->GetIsLocked()) {
            GetDatumAt(GetNumDatum() - 1)->SetLocked(true);
        }
        if (pSpeedTrap->GetIsCompleted()) {
            GetDatumAt(GetNumDatum() - 1)->SetChecked(true);
        }
        pSpeedTrap = GManager::Get().GetNextSpeedTrap(pSpeedTrap, false, iCurrentViewBin);
    }
    SetDescLabel(0xB5117FDE);
    SetInitialPosition(0);
    RefreshTrack();
    RefreshHeader();
}

void uiRepSheetMilestones::RefreshTrack() {
    if (GetCurrentDatum() != nullptr) {
        Attrib::Key key;
        bVector2 position;
        float rotation = 0.0f;

        if (TrackMapStreamer != nullptr) {
            TrackMapStreamer->Init(nullptr, TrackMap, 0, 0);
            TrackMapStreamer->ResetZoom(false);
        }

        MilestoneDatum *d = static_cast<MilestoneDatum *>(GetCurrentDatum());
        if (d->GetType() == eTYPE_MILESTONE) {
            GMilestone *pMilestone = static_cast<MilestoneDatum *>(GetCurrentDatum())->my_milestone;
            key = pMilestone->GetJumpMarkerKey();
        } else {
            GSpeedTrap *pSpeedTrap = static_cast<SpeedTrapDatum *>(GetCurrentDatum())->my_speedtrap;
            key = pSpeedTrap->GetJumpMarkerKey();
        }

        GManager::Get().CalcMapCoordsForMarker(key, position, rotation);
        if (TrackMapStreamer != nullptr) {
            TrackMapStreamer->PanTo(position);
            TrackMapStreamer->ZoomTo(bVector2(0.27f, 0.27f));
        }
        FEngSetRotationZ(FEngFindObject(GetPackageName(), 0xaf51dd73), rotation);
    }
}

void uiRepSheetMilestones::AddMilestone(GMilestone *pMilestone) {
    AddDatum(new ("MilestoneDatum", 0) MilestoneDatum(FEDatabase->GetMilestoneIconHash(pMilestone->GetTypeKey(), true),
                                                      FEDatabase->GetMilestoneHeaderHash(pMilestone->GetLocalizationTag()), pMilestone));
}

void uiRepSheetMilestones::AddSpeedtrap(GSpeedTrap *pSpeedTrap) {
    AddDatum(new ("SpeedTrapDatum", 0) SpeedTrapDatum(FEDatabase->GetRaceIconHash(GRace::kRaceType_SpeedTrap), 0xF3B3D8DC, pSpeedTrap));
}

// UNSOLVED
void uiRepSheetMilestones::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    FEPrintf(GetPackageName(), 0x5a856a34, "%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", GetNumDatum());
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %$d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %$d", GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
    MilestoneDatum *d = static_cast<MilestoneDatum *>(GetCurrentDatum());
    if (d != nullptr) {
        if (d->GetType() == eTYPE_MILESTONE) {
            GMilestone *pMilestone = d->my_milestone;
            FEngSetTextureHash(GetPackageName(), 0xf97ec5d5, FEDatabase->GetMilestoneIconHash(pMilestone->GetTypeKey(), true));
            FEPrintf(GetPackageName(), 0xb21d69bd, "%$0.0f", pMilestone->GetBounty());
            float goal = pMilestone->GetRequiredValue();
            if (FEDatabase->IsMilestoneTimeFormat(pMilestone->GetTypeKey())) {
                goal = goal / 60;
            }
            char buf[32];
            bSNPrintf(buf, 32, "%$0.0f", goal);
            FEPrintf(GetPackageName(), 0x28049d6, "%s %s", GetLocalizedString(FEDatabase->GetMilestoneDescHash(pMilestone->GetLocalizationTag())),
                     buf, buf);
        } else {
            SpeedTrapDatum *p = static_cast<SpeedTrapDatum *>(d);
            GSpeedTrap *pSpeedTrap = p->my_speedtrap;
            FEngSetTextureHash(GetPackageName(), 0xf97ec5d5, FEDatabase->GetRaceIconHash(GRace::kRaceType_SpeedTrap));
            FEPrintf(GetPackageName(), 0xb21d69bd, "%$0.0f", pSpeedTrap->GetBounty());
            const char *distUnits;
            float value;
            if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
                value = MPS2KPH(pSpeedTrap->GetTriggerSpeed());
                distUnits = GetLocalizedString(0x8569ab44);
            } else {
                value = MPS2MPH(pSpeedTrap->GetTriggerSpeed());
                distUnits = GetLocalizedString(0x8569a25f);
            }
            char buf[32];
            bSNPrintf(buf, 32, "%$0.0f %s", value, distUnits);
            FEPrintf(GetPackageName(), 0x28049d6, "%s %s", GetLocalizedString(0xb14018bd), buf);
        }

        for (int i = 0; i < GetNumSlots(); i++) {
            ArrayDatum *datum = GetDatumAt(i + GetStartDatumNum());
            uint32 check_hash = FEngHashString("MEDAL_THUMB_%d", i + 1);
            FEngSetInvisible(GetPackageName(), check_hash);
            if (datum != nullptr) {
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
