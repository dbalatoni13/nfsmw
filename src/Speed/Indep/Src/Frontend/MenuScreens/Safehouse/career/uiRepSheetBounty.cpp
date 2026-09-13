#include "uiRepSheetBounty.hpp"

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

Attrib::Key theMarker;
const char *gTUTORIAL_MOVIE_BOUNTY = "bounty_tutorial";

void BountyDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == __BUTTON_PRESSED__) {
        theMarker = GManager::Get().GetBountySpawnMarker(index);
    }
}

uiRepSheetBounty::uiRepSheetBounty(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 3, 3, true) {
    bIsInGame = sd->Arg != 0;
    TrackMapStreamer = nullptr;
    TrackMap = nullptr;
    tutorialPlaying = false;
    TrackMapStreamer = new ("UITrackMapStreamer", 0) UITrackMapStreamer();
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("EVENT_ICON_%d", i + 1));
        if (img != nullptr) {
            AddSlot(new ("ImageArraySlot", 0) ImageArraySlot(img));
        }
    }
    TrackMap = reinterpret_cast<FEMultiImage *>(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    if (bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0x6ddfa694);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xbde82fcc, 0xe451941e);
    }
    Setup();
}

eMenuSoundTriggers uiRepSheetBounty::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == 0x7b6b89d7 && bIsInGame) {
        return UISND_NONE;
    } else {
        BountyDatum *d = static_cast<BountyDatum *>(GetCurrentDatum());
        if (d->IsLocked()) {
            return UISND_COMMON_WRONG;
        }
    }

    return maybe;
}

// UNSOLVED
void uiRepSheetBounty::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    int currentIndex = GetNumDatum() - 1;
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
        case __BUTTON_PRESSED__: {
            BountyDatum *d = static_cast<BountyDatum *>(GetCurrentDatum());
            if (GetNumDatum() < 1) {
                return;
            }
            if (d->IsLocked()) {
                return;
            }
            if (!bIsInGame) {
                int joyPort = FEngMapJoyParamToJoyport(param1);
                FEDatabase->SetPlayersJoystickPort(0, joyPort);
            }
            const char *dialog = "";
            if (bIsInGame) {
                dialog = "InGameDialog.fng";
            }
            DialogInterface ::ShowTwoButtons(GetPackageName(), dialog, dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes,
                                             dialog_message_no, dialog_message_no, first_dialog_button2, 0xcd195d0b);
            return;
        }
        case __PAD_BUTTON4__:
            if (!bIsInGame) {
                tutorialPlaying = true;

                const u32 FEObj_MASTERBLASTER = 0x99344537;
                const u32 FEObj_HIDE = FEObj_HIDE;
                FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_HIDE, true);
                FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_BOUNTY, GetPackageName());
            }
            return;
        case dialog_message_yes: {
            CareerSettings *career = FEDatabase->GetCareerSettings();
            if (!career->HasDoneBountyTutorial()) {
                if (bIsInGame) {
                    if (TrackMapStreamer != nullptr) {
                        delete TrackMapStreamer;
                    }
                    TrackMapStreamer = nullptr;
                    InGameAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_BOUNTY, GetPackageName());
                    const u32 FEObj_BACKGROUND = 0x2716cdbf;
                    FEngSetInvisible("InGameBackground.fng", 0x2716cdbf);
                } else {
                    FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_BOUNTY, GetPackageName());
                }

                const u32 FEObj_MASTERBLASTER = 0x99344537;
                const u32 FEObj_HIDE = FEObj_HIDE;
                FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_HIDE, true);
                FEngSetInvisible(GetPackageName(), FEngHashString("TRACK_MAP"));
                career->SetHasDoneBountyTutorial();
                return;
            }
            cFEng::Get()->QueueGameMessage(0xc3960eb9, GetPackageName(), 0xff);
            return;
        }
        case 0xc3960eb9:
            if (tutorialPlaying) {
                tutorialPlaying = false;

                const u32 FEObj_MASTERBLASTER = 0x99344537;
                const u32 FEObj_Init = FEHASH_INIT;
                FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_Init, true);
                return;
            }
            if (bIsInGame) {
                const u32 FEObj_BACKGROUND = 0x2716cdbf;
                FEngSetVisible("InGameBackground.fng", FEObj_BACKGROUND);
                GManager::Get().WarpToMarker(theMarker, true);
                new ERaceSheetOff();
                return;
            }
            GManager::Get().OverrideFreeRoamStartMarker(theMarker);
            GManager::Get().QueueFreeRoamPursuit(0.0f);
            RaceStarter::StartCareerFreeRoam();
            return;
        case __PAD_BACK__:
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
            }
            return;
        case FEMSG_SCREEN_TICK:
            if (TrackMapStreamer != nullptr) {
                TrackMapStreamer->UpdateAnimation();
            }
            return;
        case __PAD_UP__:
        case __PAD_DOWN__:
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
            break;
        case dialog_message_no:
            return;
        default:
            return;
    }
    int newIndex = GetNumDatum() - 1;
    if (currentIndex != newIndex && GetCurrentDatum() != nullptr) {
        RefreshTrack();
    }
}

void uiRepSheetBounty::Setup() {
    ClearData();
    uint32 bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    for (int i = 0; i < 9; i++) {
        uint32 check_hash = FEngHashString("CHECK_%d", i + 1);
        FEngSetInvisible(GetPackageName(), check_hash);
    }
    for (uint32 i = 0; i < GManager::Get().GetNumBountySpawnMarkers(); i++) {
        int index = GManager::Get().GetBountySpawnMarkerTag(i);
        orderedList[index - 1] = static_cast<char>(i);
    }
    for (uint32 i = 0; i < GManager::Get().GetNumBountySpawnMarkers(); i++) {
        if (i < 4 || (i < 8 && bin < 13) || bin < 9) {
            AddDatum(new ("BountyDatum", 0)
                         BountyDatum(FEDatabase->GetBountyIconHash(i + 1), FEDatabase->GetBountyHeaderHash(i + 1), orderedList[i]));
        }
    }
    SetDescLabel(0xb5117fde);
    SetInitialPosition(0);
    RefreshTrack();
    RefreshHeader();
}

void uiRepSheetBounty::RefreshTrack() {
    if (GetCurrentDatum() != nullptr) {
        bVector2 position;
        float rotation = 0.0f;
        if (TrackMapStreamer != nullptr) {
            TrackMapStreamer->Init(nullptr, TrackMap, 0, 0);
            TrackMapStreamer->ResetZoom(false);
        }
        BountyDatum *d = static_cast<BountyDatum *>(GetCurrentDatum());
        Attrib::Key key = GManager::Get().GetBountySpawnMarker(d->index);
        GManager::Get().CalcMapCoordsForMarker(key, position, rotation);
        if (TrackMapStreamer != nullptr) {
            TrackMapStreamer->PanTo(position);
            TrackMapStreamer->ZoomTo(bVector2(0.27f, 0.27f));
        }
        FEngSetRotationZ(FEngFindObject(GetPackageName(), 0xaf51dd73), rotation);
    }
}

void uiRepSheetBounty::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();

    FEPrintf(GetPackageName(), 0x5a856a34, "%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", GetNumDatum());
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %$d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %$d", GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
    int loc_tag = GManager::Get().GetBountySpawnMarkerTag(GetCurrentDatumNum() - 1);
    FEngSetTextureHash(GetPackageName(), 0xf97ec5d5, FEDatabase->GetBountyIconHash(loc_tag));
    BountyDatum *d = static_cast<BountyDatum *>(GetCurrentDatum());
    if (d != nullptr) {
        if (d->IsLocked()) {
            cFEng::Get()->QueuePackageMessage(0xc5dd9d68, GetPackageName(), nullptr);
        } else {
            cFEng::Get()->QueuePackageMessage(0x38091fa1, GetPackageName(), nullptr);
        }
        FEngSetLanguageHash(GetPackageName(), 0x28049d6, FEDatabase->GetBountyDescHash(GetCurrentDatumNum()));
        for (int i = 0; i < GetNumSlots(); i++) {
            ArrayDatum *datum = GetDatumAt(i + GetStartDatumNum());
            uint32 check_hash = FEngHashString("CHECK_%d", i + 1);
            FEngSetTextureHash(GetPackageName(), check_hash, 0x18ed48);
            if (datum != nullptr) {
                FEngSetInvisible(GetPackageName(), check_hash);
            }
        }
    }
}
