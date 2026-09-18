#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "uiRepSheetBounty.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/InGameTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeWorldMapQuickList.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

struct FEObject;
struct FEMultiImage;

int FEPrintf(const char *pkg_name, int hash, const char *fmt, ...);
unsigned int FEngHashString(const char *format, ...);
// [fe3] const char *GetLocalizedString(unsigned int hash);
void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool);
void FEngSetRotationZ(FEObject *obj, float angle);
void FEngSetTextureHash(FEImage *image, unsigned int hash);
int FEngMapJoyParamToJoyport(int feng_param);

extern unsigned int iCurrentViewBin;
// .bss:0x80472B18 del original (4 B, global). Nadie mas lo define: el objeto
// de zFe lo referencia como UND y sin esto la unidad no enlaza.
unsigned int theMarker;
const char *gTUTORIAL_MOVIE_BOUNTY = "bounty_tutorial";

// FEngSetInvisible/FEngSetVisible inlines defined in uiMain.cpp
// FEngSetTextureHash inline already defined in uiOptionsScreen.cpp



void BountyDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == 0xc407210) {
        theMarker = GManager::Get().GetBountySpawnMarker(static_cast<unsigned int>(index));
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
        if (img) {
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
    if (msg == 0x7b6b89d7 && bIsInGame)
        return static_cast<eMenuSoundTriggers>(-1);
    if (currentDatum->IsLocked())
        return static_cast<eMenuSoundTriggers>(7);
    return maybe;
}

void uiRepSheetBounty::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    int currentIndex = data.IsInList(currentDatum) - 1;
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    switch (msg) {
        case 0xc407210: {
            BountyDatum *d = static_cast<BountyDatum *>(GetCurrentDatum());
            if (GetNumDatum() < 1) {
                return;
            }
            if (d->IsLocked()) {
                return;
            }
            if (!bIsInGame) {
                signed char joyPort = static_cast<signed char>(FEngMapJoyParamToJoyport(param1));
                FEDatabase->SetPlayersJoystickPort(0, joyPort);
            }
            const char *dialog = "";
            if (bIsInGame) {
                dialog = "InGameDialog.fng";
            }
            DialogInterface::ShowTwoButtons(GetPackageName(), dialog, static_cast<eDialogTitle>(1), LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0xd05fc3a3, 0x34dc1bcf,
                                            0x34dc1bcf, static_cast<eDialogFirstButtons>(1), 0xcd195d0b);
            return;
        }
        case 0xc519bfc3:
            if (bIsInGame) {
                return;
            }
            tutorialPlaying = true;
            FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
            FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_BOUNTY, GetPackageName());
            return;
        case 0xd05fc3a3: {
            CareerSettings *career = FEDatabase->GetCareerSettings();
            if (((career->SpecialFlags >> 10) & 1) == 0) {
                if (bIsInGame) {
                    if (TrackMapStreamer != nullptr) {
                        delete TrackMapStreamer;
                    }
                    TrackMapStreamer = nullptr;
                    InGameAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_BOUNTY, GetPackageName());
                    FEngSetInvisible("InGameBackground.fng", 0x2716cdbf);
                } else {
                    FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_BOUNTY, GetPackageName());
                }
                FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
                FEngSetInvisible(GetPackageName(), FEngHashString("TRACK_MAP"));
                career->SpecialFlags |= 0x400;
                return;
            }
            cFEng::Get()->QueueGameMessage(0xc3960eb9, GetPackageName(), 0xff);
            return;
        }
        case 0xc3960eb9:
            if (tutorialPlaying) {
                tutorialPlaying = false;
                FEngSetScript(GetPackageName(), 0x99344537, 0x1744b3, true);
                return;
            }
            if (bIsInGame) {
                FEngSetVisible("InGameBackground.fng", 0x2716cdbf);
                GManager::Get().WarpToMarker(theMarker, true);
                new ERaceSheetOff();
                return;
            }
            GManager::Get().OverrideFreeRoamStartMarker(theMarker);
            GManager::Get().QueueFreeRoamPursuit(0.0f);
            GManager::Get().QueueFreeRoamPursuit(0.0f);
            RaceStarter::StartCareerFreeRoam();
            return;
        case 0x911ab364:
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
            }
            return;
        case 0xc98356ba:
            if (TrackMapStreamer != nullptr) {
                TrackMapStreamer->UpdateAnimation();
            }
            return;
        case 0x1265ece9:
            return;
        case 0x34dc1bcf:
            return;
        case 0x72619778:
        case 0x911c0a4b:
        case 0x9120409e:
        case 0xb5971bf1:
            break;
        default:
            return;
    }
    int newIndex = data.IsInList(currentDatum) - 1;
    if (currentIndex != newIndex && currentDatum != nullptr) {
        RefreshTrack();
    }
}

void uiRepSheetBounty::Setup() {
    ClearData();
    unsigned int bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    for (int i = 0; i < 9; i++) {
        unsigned int check_hash = FEngHashString("CHECK_%d", i + 1);
        FEngSetInvisible(GetPackageName(), check_hash);
    }
    for (unsigned int i = 0; i < GManager::Get().GetNumBountySpawnMarkers(); i++) {
        int index = GManager::Get().GetBountySpawnMarkerTag(i);
        orderedList[index - 1] = static_cast<char>(i);
    }
    for (unsigned int i = 0; i < GManager::Get().GetNumBountySpawnMarkers(); i++) {
        if (i < 4 || (i < 8 && bin < 13) || bin < 9) {
            AddDatum(new ("BountyDatum", 0) BountyDatum(FEDatabase->GetBountyIconHash(i + 1),
                                                        FEDatabase->GetBountyHeaderHash(i + 1),
                                                        static_cast<int>(static_cast<signed char>(orderedList[i]))));
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
        unsigned int key = GManager::Get().GetBountySpawnMarker(d->index);
        GManager::Get().CalcMapCoordsForMarker(key, position, rotation);
        if (TrackMapStreamer != nullptr) {
            TrackMapStreamer->PanTo(position);
            bVector2 zoom(0.27f, 0.27f);
            TrackMapStreamer->ZoomTo(zoom);
        }
        FEngSetRotationZ(FEngFindObject(GetPackageName(), 0xaf51dd73), rotation);
    }
}

void uiRepSheetBounty::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    FEPrintf(GetPackageName(), __NUMBER_GROUP__, "%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), __NUMBER_OF_GROUP__, "%d", GetNumDatum());
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
            unsigned int check_hash = FEngHashString("CHECK_%d", i + 1);
            FEngSetTextureHash(GetPackageName(), check_hash, 0x18ed48);
            if (datum != nullptr) {
                FEngSetInvisible(GetPackageName(), check_hash);
            }
        }
    }
}
