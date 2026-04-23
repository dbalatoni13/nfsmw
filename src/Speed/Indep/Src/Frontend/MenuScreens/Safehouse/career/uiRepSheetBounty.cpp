#include "uiRepSheetBounty.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;
struct FEMultiImage;

FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
FEImage *FEngFindImage(const char *pkg_name, int hash);
void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int lang_hash);
int FEPrintf(const char *pkg_name, int hash, const char *fmt, ...);
unsigned int FEngHashString(const char *format, ...);
const char *GetLocalizedString(unsigned int hash);
void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool);
void FEngSetRotationZ(FEObject *obj, float angle);
void FEngSetTextureHash(FEImage *image, unsigned int hash);
int FEngMapJoyParamToJoyport(int feng_param);

void RaceStarterStartCareerFreeRoam() asm("StartCareerFreeRoam__11RaceStarter");
void InGameAnyTutorialScreenLaunchMovie(const char *, const char *) asm("LaunchMovie__23InGameAnyTutorialScreenPCcT1");

extern unsigned int iCurrentViewBin;
extern unsigned int theMarker;
extern const char *gTUTORIAL_MOVIE_BOUNTY;

// FEngSetInvisible/FEngSetVisible inlines defined in uiMain.cpp
// FEngSetTextureHash inline already defined in uiOptionsScreen.cpp

uiRepSheetBounty::uiRepSheetBounty(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 3, 3, true) {
    bIsInGame = sd->Arg != 0;
    TrackMapStreamer = nullptr;
    TrackMap = nullptr;
    tutorialPlaying = false;
    TrackMapStreamer = new (__FILE__, __LINE__) UITrackMapStreamer();
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("EVENT_ICON_%d", i + 1));
        if (img) {
            AddSlot(new (__FILE__, __LINE__) ImageArraySlot(img));
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

eMenuSoundTriggers uiRepSheetBounty::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (msg == 0x7b6b89d7 && bIsInGame)
        return static_cast<eMenuSoundTriggers>(-1);
    if (currentDatum->IsLocked())
        return static_cast<eMenuSoundTriggers>(7);
    return maybe;
}

void uiRepSheetBounty::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    int currentIndex = GetNumDatum() - 1;
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
            DialogInterface::ShowTwoButtons(GetPackageName(), dialog, static_cast<eDialogTitle>(1), 0x70e01038, 0x417b25e4, 0xd05fc3a3, 0x34dc1bcf,
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
                    InGameAnyTutorialScreenLaunchMovie(gTUTORIAL_MOVIE_BOUNTY, GetPackageName());
                    FEngSetInvisible("IG_BL_TRACKMAP.fng", 0x2716cdbf);
                } else {
                    FEAnyTutorialScreen::LaunchMovie(gTUTORIAL_MOVIE_BOUNTY, GetPackageName());
                }
                FEngSetScript(GetPackageName(), 0x99344537, 0x16a259, true);
                FEngSetInvisible(GetPackageName(), FEngHashString("MASTERBLASTER"));
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
                FEngSetVisible("IG_BL_TRACKMAP.fng", 0x2716cdbf);
                GManager::Get().WarpToMarker(theMarker, true);
                new ERaceSheetOff();
                return;
            }
            GManager::Get().OverrideFreeRoamStartMarker(theMarker);
            GManager::Get().QueueFreeRoamPursuit(0.0f);
            GManager::Get().QueueFreeRoamPursuit(0.0f);
            RaceStarterStartCareerFreeRoam();
            return;
        case 0x911ab364:
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("IG_BL_MAIN", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("BL_MAIN", 0, 0, false);
            }
            return;
        case 0xc98356ba:
            if (TrackMapStreamer != nullptr) {
                TrackMapStreamer->UpdateAnimation();
            }
            return;
        case 0x72619778:
        case 0x911c0a4b:
        case 0x9120409e:
        case 0xb5971bf1:
            break;
        case 0x34dc1bcf:
            return;
        default:
            return;
    }
    int newIndex = GetNumDatum() - 1;
    if (currentIndex != newIndex && currentDatum != nullptr) {
        RefreshTrack();
    }
}

void uiRepSheetBounty::Setup() {
    ClearData();
    unsigned int bin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    for (int i = 0; i < 9; i++) {
        unsigned int check_hash = FEngHashString("CHECK_ICON_%d", i + 1);
        FEngSetInvisible(GetPackageName(), check_hash);
    }
    for (unsigned int i = 0; i < GManager::Get().GetNumBountySpawnMarkers(); i++) {
        int index = GManager::Get().GetBountySpawnMarkerTag(i);
        orderedList[index - 1] = static_cast<char>(i);
    }
    for (unsigned int i = 0; i < GManager::Get().GetNumBountySpawnMarkers(); i++) {
        if (i < 4 || (i < 8 && bin < 13) || bin < 9) {
            BountyDatum *datum = new BountyDatum(FEDatabase->GetBountyIconHash(i + 1), FEDatabase->GetBountyHeaderHash(i + 1), i);
            datum->index = static_cast<int>(static_cast<signed char>(orderedList[i]));
            AddDatum(datum);
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
            bVector2 zoom(0.5f, 0.5f);
            TrackMapStreamer->ZoomTo(zoom);
        }
        FEngSetRotationZ(FEngFindObject(GetPackageName(), 0xaf51dd73), rotation);
    }
}

void uiRepSheetBounty::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    ArrayDatum *currentDatum = GetCurrentDatum();
    FEPrintf(GetPackageName(), 0x5a856a34, "%d", data.GetNodeNumber(currentDatum));
    FEPrintf(GetPackageName(), 0x2d4d22c8, "%d", GetNumDatum());
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %d", GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
    int loc_tag = GManager::Get().GetBountySpawnMarkerTag(data.GetNodeNumber(currentDatum) - 1);
    FEngSetTextureHash(GetPackageName(), 0xf97ec5d5, FEDatabase->GetBountyIconHash(loc_tag));
    BountyDatum *d = static_cast<BountyDatum *>(currentDatum);
    if (d != nullptr) {
        if (d->IsLocked()) {
            cFEng::Get()->QueuePackageMessage(0xc5dd9d68, GetPackageName(), nullptr);
        } else {
            cFEng::Get()->QueuePackageMessage(0x38091fa1, GetPackageName(), nullptr);
        }
        FEngSetLanguageHash(GetPackageName(), 0x28049d6, FEDatabase->GetBountyDescHash(data.GetNodeNumber(currentDatum)));
        for (int i = 0; i < GetNumSlots(); i++) {
            ArrayDatum *datum = GetDatumAt(i + GetStartDatumNum());
            unsigned int check_hash = FEngHashString("CHECK_ICON_%d", i + 1);
            FEngSetTextureHash(GetPackageName(), check_hash, 0x18ed48);
            if (datum != nullptr) {
                FEngSetInvisible(GetPackageName(), check_hash);
            }
        }
    }
}

void BountyDatum::NotificationMessage(unsigned long msg, FEObject *pObj, unsigned long param1, unsigned long param2) {
    if (msg == 0xc407210) {
        theMarker = GManager::Get().GetBountySpawnMarker(static_cast<unsigned int>(index));
    }
}
