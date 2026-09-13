#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRCarSelect.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/SoundHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

extern int g_MaximumMaximumTimesBusted;

extern int CheatBustedCount;
extern int CheatMaxBusted;
extern bool CheatImpounded;
extern bool CheatCanAddImpoundBox;
extern bool CheatReleaseFromImpoundMarker;
extern bool CheatReleasable;
extern int gPlayerNum;

uint32 UIQRCarSelect::ForceCar;
bool QRCarSelectBustedManager::bPlayerJustGotBusted;

QRCarSelectBustedManager::QRCarSelectBustedManager(const char *pkg_name, int flags) {
    Flags = static_cast<eBustedAnimationTypes>(flags);
    ParentPkg = pkg_name;
    ImpoundStampHash = 0;
    bWantsImpound = false;
    WorkingCareerRecord = nullptr;
    WorkingCarRecord = nullptr;
}

QRCarSelectBustedManager::~QRCarSelectBustedManager() {
    if (ImpoundStampHash) {
        eUnloadStreamingTexture(ImpoundStampHash);
        ImpoundStampHash = 0;
    }
}

bool QRCarSelectBustedManager::IsImpoundInfoVisible() {
    return FEDatabase->IsCareerMode() && !FEDatabase->IsCarLotMode();
}

bool QRCarSelectBustedManager::ShowImpoundedTexture() {
    return WorkingCareerRecord->TheImpoundData.IsImpounded();
}

void QRCarSelectBustedManager::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x8defa48b:
            TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0);
            WorkingCareerRecord->TheImpoundData.AddMaxBusted();
            RefreshHeader();
            break;
        case 0xa0fc39f9: {
            WorkingCareerRecord->TheImpoundData.NotifyPlayerPaidToRelease();
            WorkingCareerRecord->SetVehicleHeat(0.0f);

            float cost = WorkingCarRecord->GetReleaseFromImpoundCost();
            FEDatabase->GetCareerSettings()->SpendCash(cost);
            RefreshHeader();
            break;
        }
        case 0xe845bc1c:
            WorkingCareerRecord->TheImpoundData.NotifyPlayerUsedMarkerToRelease();
            WorkingCareerRecord->SetVehicleHeat(0.0f);
            TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);
            RefreshHeader();
            break;
        case 0xe0b38195:
            if (CalcGameOver()) {
                FEDatabase->GetCareerSettings()->SetGameOver();
                DialogInterface::ShowOneButton(GetPackageName(), "GameOver.fng", dialog_alert, 0x417b2601, 0x3fdc64c1, 0x164bed94);
            }
            break;
        case 0x3fdc64c1:
            FEManager::Get()->SetGarageType(GARAGETYPE_MAIN_FE);
            FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER);
            FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER_MANAGER);
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            break;
        case 0x44444444: // TODO: some number in range to solve switch
        default:
            break;
    }
}

void QRCarSelectBustedManager::TextureLoadedCallback() {
    if (ShowImpoundedTexture()) {
        FEngSetTextureHash(GetPackageName(), STRINGHASH_IMPOUNDED_ENG, ImpoundStampHash);
        FEngSetTextureHash(GetPackageName(), 0x5b8f2a45, ImpoundStampHash);
        if (ShowImpoundedAnimation()) {
            const u32 FEObj_ANIMATE = FEHASH_ANIMATE;
            FEngSetScript(GetPackageName(), 0xbc7b91f, FEObj_ANIMATE, true);
            Flags = BUSTED_ANIM_NOTHING;
        } else {
            const u32 FEObj_NORMAL = FEHASH_NORMAL;
            FEngSetScript(GetPackageName(), 0xbc7b91f, FEObj_NORMAL, true);
        }
    } else {
        const u32 FEObj_HIDE = FEHASH_HIDE;
        FEngSetScript(GetPackageName(), 0xbc7b91f, FEObj_HIDE, true);
    }
}

void QRCarSelectBustedManager::LoadImpoundTexture() {
    switch (GetCurrentLanguage()) {
        case eLANGUAGE_FRENCH:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_FRE;
            break;
        case eLANGUAGE_GERMAN:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_GER;
            break;
        case eLANGUAGE_ITALIAN:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_ITA;
            break;
        case eLANGUAGE_SPANISH:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_SPA;
            break;
        case eLANGUAGE_DUTCH:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_DUT;
            break;
        case eLANGUAGE_SWEDISH:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_SWE;
            break;
        case eLANGUAGE_DANISH:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_DAN;
            break;
        case eLANGUAGE_POLISH:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_POL;
            break;
        case eLANGUAGE_FINNISH:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_FIN;
            break;
        case eLANGUAGE_KOREAN:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_KOR;
            break;
        case eLANGUAGE_CHINESE:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_CHI;
            break;
        case eLANGUAGE_JAPANESE:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_JAP;
            break;
        case eLANGUAGE_THAI:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_THA;
            break;
        default:
            ImpoundStampHash = STRINGHASH_IMPOUNDED_ENG;
            break;
    }

    eLoadStreamingTexture(ImpoundStampHash, TextureLoadedCallbackAccessor, reinterpret_cast<uint32>(this), 0);
}

void QRCarSelectBustedManager::SetSelectedCar(FECarRecord *record) {
    WorkingCarRecord = record;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    WorkingCareerRecord = stable->GetCareerRecordByHandle(record->CareerHandle);
    if (CheatImpounded) {
        WorkingCareerRecord->TheImpoundData.MaxBusted = CheatMaxBusted;
        WorkingCareerRecord->TheImpoundData.TimesBusted = CheatMaxBusted;
        WorkingCareerRecord->TheImpoundData.BecomeImpounded(FEImpoundData::IMPOUND_REASON_STRIKE_LIMIT_REACHED);
        Flags = BUSTED_ANIM_SHOW_IMPOUNDED;
    } else if (CheatBustedCount != 0) {
        WorkingCareerRecord->TheImpoundData.TimesBusted = CheatBustedCount;
        WorkingCareerRecord->TheImpoundData.MaxBusted = CheatMaxBusted;
        Flags = BUSTED_ANIM_SHOW_STRIKE;
    }
    if (CheatReleasable) {
        while (!WorkingCareerRecord->TheImpoundData.NotifyWin()) {
        }
    }
    RefreshHeader();
}

void QRCarSelectBustedManager::RefreshHeader() {
    if (!IsImpoundInfoVisible()) {
        return;
    }

    const u32 FEObj_SHOW = 0;
    const u32 FEObj_HIDE = 0;
    const u32 FEObj_ANIMATE = 0;

    bool hide_stuff = false;
    if (ShowImpoundedTexture()) {
        const u32 FEObj_APPEAR = FEHASH_APPEAR;
        const u32 FEObj_NORMAL = FEHASH_NORMAL;

        if (GetTextureInfo(ImpoundStampHash, 0, 0) != nullptr) {
            FEngSetScript(GetPackageName(), 0xbc7b91f, FEObj_NORMAL, true);
            FEngSetScript(GetPackageName(), 0x64f3a49c, FEObj_APPEAR, true);
            FEngSetTextureHash(GetPackageName(), 0xce18427d, ImpoundStampHash);
            FEngSetTextureHash(GetPackageName(), 0x5b8f2a45, ImpoundStampHash);
        }

        // int -> float -> int... okay.
        float cost = WorkingCarRecord->GetReleaseFromImpoundCost();
        bool has_cash = FEDatabase->GetCareerSettings()->GetCash() >= static_cast<int>(cost);
        bool has_marker = TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);

        if (WorkingCareerRecord->TheImpoundData.IsReleasable() && has_cash) {
            FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x281dee8a);
        } else if (has_marker) {
            FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0xf9c73cc2);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x2b65a216);
        }
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x2b65a216);
        FEngSetScript(GetPackageName(), 0x64f3a49c, FEHASH_HIDE, true);
    }
    if (WorkingCareerRecord->GetTimesBusted() >= 0) {
        FEngSetVisible(GetPackageName(), 0x75721326);

        int pos = 1;
        u32 x4state = FEHASH_HIDE;
        u32 x5state = FEHASH_HIDE;

        switch (WorkingCareerRecord->TheImpoundData.MaxBusted) {
            case 4:
                pos = 2;
                x4state = FEHASH_SHOW;
                break;
            case 5:
                pos = 3;
                x4state = FEHASH_SHOW;
                x5state = FEHASH_SHOW;
                break;
        }
        FEngSetScript(GetPackageName(), 0x5bc78037, x4state, true);
        FEngSetScript(GetPackageName(), 0x48095518, x5state, true);
        FEngSetScript(GetPackageName(), 0xf9a5ce86, FEngHashString("POS%d", pos), true);
        FEngSetScript(GetPackageName(), 0xebf0016e, FEngHashString("POS%d", pos), true);
        if (ShowNewStrikeAnimation()) {
            FEngSetScript(GetPackageName(), FEngHashString("IMPOUND_STATE_%d", WorkingCareerRecord->GetTimesBusted()), FEHASH_ANIMATE, true);
            Flags = BUSTED_ANIM_NOTHING;
        }
        for (int i = 1; i <= WorkingCareerRecord->TheImpoundData.MaxBusted; i++) {
            if (i <= WorkingCareerRecord->GetTimesBusted()) {
                if (!FEngIsScriptSet(GetPackageName(), FEngHashString("IMPOUND_STATE_%d", i), FEHASH_ANIMATE)) {
                    FEngSetScript(GetPackageName(), FEngHashString("IMPOUND_STATE_%d", i), FEHASH_SHOW, true);
                }
            } else {
                FEngSetScript(GetPackageName(), FEngHashString("IMPOUND_STATE_%d", i), FEHASH_HIDE, true);
            }
        }
    } else {
        if (WorkingCareerRecord->TheImpoundData.IsReleasable() || WorkingCareerRecord->TheImpoundData.IsImpounded()) {
            FEngSetInvisible(GetPackageName(), 0x75721326);
        } else {
            hide_stuff = true;
        }
    }
    if (hide_stuff) {
        FEngSetScript(GetPackageName(), 0xbc7b91f, FEHASH_HIDE, true);
        FEngSetInvisible(GetPackageName(), 0x75721326);
        FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x2b65a216);
    }
}

bool QRCarSelectBustedManager::CalcGameOver() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    bool game_over = false;
    if (stable->GetNumAvailableCareerCars() < 1) {
        if (!TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0)) {
            game_over = true;
        }
    }
    return game_over;
}

// UNSOLVED
void QRCarSelectBustedManager::MaybeReleaseCar() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECareerRecord *record = stable->GetCareerRecordByHandle(WorkingCarRecord->CareerHandle);

    // int -> float -> int... okay.
    float cost = WorkingCarRecord->GetReleaseFromImpoundCost();
    bool has_cash = static_cast<int>(cost) <= FEDatabase->GetCareerSettings()->GetCash();
    bool has_marker = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0) > 0;

    if (record->TheImpoundData.IsReleasable() && has_cash) {
        if (has_marker) {
            DialogInterface::ShowThreeButtons(GetPackageName(), "", dialog_confirmation, 0xf9c73cc2, 0x4eb9591f, 0x1a294dad, 0xe845bc1c, 0xa0fc39f9,
                                              0x5ee58948, 0x5ee58948, first_dialog_button2, 0xb715ae8f, static_cast<int>(cost));
        } else {
            DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_confirmation, 0x417b2601, 0x1a294dad, 0xa0fc39f9, 0x5ee58948,
                                            first_dialog_button1, 0xcad5722e, static_cast<int>(cost));
        }
    } else {
        if (has_marker) {
            DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_confirmation, 0x417b2601, 0x1a294dad, 0xe845bc1c, 0x5ee58948,
                                            first_dialog_button1, 0xed4dd591, static_cast<int>(cost));
        } else {
            DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417b2601, 0x5ee58948, 0xe96fa0c5);
        }
    }
}

// UNSOLVED
void QRCarSelectBustedManager::MaybeAddImpoundBox() {
    bool can_add_impound_box =
        (WorkingCareerRecord->TheImpoundData.CanAddMaxBusted() && TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0));

    if (can_add_impound_box || static_cast<int>(CheatCanAddImpoundBox) != 0) {
        DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_confirmation, 0x417b2601, 0x1a294dad, 0x8defa48b, dialog_message_cancelled,
                                        first_dialog_button2, 0xcebda20);
    } else if (g_MaximumMaximumTimesBusted <= static_cast<int>(WorkingCareerRecord->TheImpoundData.MaxBusted)) {
        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_info, 0x417b2601, dialog_message_cancelled, 0xbcae8539);
    }
}

UIQRCarSelect::UIQRCarSelect(ScreenConstructorData *sd) // TODO: best way to get third byte from Arg?
    : MenuScreen(sd), TheBustedManager(this->GetPackageName(), ((uint8 *)(&sd->Arg))[2]), FilteredCarsList(), originalCar(INVALID_CAR_HANDLE),
      tLastEventTimer(), pManuLogo(nullptr), pCarBadge(nullptr), pCarName(nullptr), pCarNameShadow(nullptr), pFilter(nullptr),
      bLoadingBarActive(false), bShowcaseMode(false), iPlayerNum(sd->Arg & 0xFF), filter(0), iPrevButtonMsg(0) {
    tLastEventTimer.ResetLow();

    const u32 FEObj_ManufacturerLogo = 0x3e01ad1d;
    const u32 FEObj_CarBadge = 0xb05dd708;
    const u32 FEObj_CARNAME = 0xd6d32016;
    const u32 FEObj_CARNAME_SHADOW = 0x79d6e45c;
    const u32 FEObj_FILTER = 0x5ba2f765;

    pManuLogo = FEngFindImage(this->GetPackageName(), FEObj_ManufacturerLogo);
    pCarBadge = FEngFindImage(this->GetPackageName(), FEObj_CarBadge);
    pCarName = FEngFindString(this->GetPackageName(), FEObj_CARNAME);
    pCarNameShadow = FEngFindString(this->GetPackageName(), FEObj_CARNAME_SHADOW);
    pFilter = FEngFindString(this->GetPackageName(), FEObj_FILTER);

    if (FEDatabase->IsSplitScreenMode()) {
        const u32 FEObj_PLAYER_TEXT_GROUP = 0xe3fe27fe;
        if (iPlayerNum == 0) {
            gPlayerNum = 0;
            FEngSetLanguageHash(this->GetPackageName(), FEObj_PLAYER_TEXT_GROUP, 0x7b070984);
            FEDatabase->DeleteMultiplayerProfile(1);
        } else {
            gPlayerNum = 1;
            FEDatabase->CreateMultiplayerProfile(1);
            FEngSetLanguageHash(this->GetPackageName(), FEObj_PLAYER_TEXT_GROUP, 0x7b070985);
        }
    } else {
        gPlayerNum = 0;
    }

    if (!FEDatabase->IsCarLotMode()) {
        FEngSetInvisible(this->GetPackageName(), 0x19398802);
        FEngSetInvisible(this->GetPackageName(), 0x1930b057);
        FEngSetInvisible(this->GetPackageName(), 0x20d113dc);
        FEngSetInvisible(this->GetPackageName(), 0x20c83c31);
    }

    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        FEngSetInvisible(this->GetPackageName(), 0xe9ed0a2);
        FEngSetInvisible(this->GetPackageName(), 0x18a4384f);
    }

    if (TheBustedManager.IsImpoundInfoVisible()) {
        TheBustedManager.LoadImpoundTexture();
    }

    InitStatsSliders();
    SetAsGarageScreen();
    Setup();
    if (!FEDatabase->IsCareerMode() || !FEDatabase->IsCarLotMode()) {
        GarageMainScreen::GetInstance()->CancelCameraPush();
    }
}

UIQRCarSelect::~UIQRCarSelect() {}

bool UIQRCarSelect::IsCarImpounded(uint32 handle) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *carRecord = stable->GetCarRecordByHandle(handle);
    FECareerRecord *career = stable->GetCareerRecordByHandle(carRecord->CareerHandle);
    if (career == nullptr) {
        return false;
    }
    return career->TheImpoundData.IsImpounded();
}

void UIQRCarSelect::CommitChangeStartRace(bool allowError) {
    FEManager::Get()->AllowControllerError(allowError);
    FEDatabase->DeleteMultiplayerProfile(1);
    RaceStarter::StartRace();
}

// UNSOLVED
void UIQRCarSelect::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (TheBustedManager.IsImpoundInfoVisible()) {
        TheBustedManager.NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
        case FEMSG_INIT_STARTED:
            if (!FEDatabase->IsCareerMode())
                return;
            if (!FEDatabase->IsCarLotMode())
                return;
            GarageMainScreen::GetInstance()->UpdateCurrentCameraView(false);
            return;

        case FEHASH_INITCOMPLETE:
            if (!FEDatabase->IsSplitScreenMode())
                return;
            cFEng::Get()->QueuePackageMessage(0x841d518a, this->GetPackageName(), nullptr);
            return;

        case FEMSG_SCREEN_TICK:
            if (GarageMainScreen::GetInstance()->IsCarRendering() && bLoadingBarActive) {
                const u32 FEObj_LOADERLEAVE = 0x913fa282;
                cFEng::Get()->QueuePackageMessage(FEObj_LOADERLEAVE, this->GetPackageName(), nullptr);
                bLoadingBarActive = false;
            }
            if (!tLastEventTimer.IsSet())
                return;
            if ((RealTimer - tLastEventTimer).GetSeconds() >= 0.5f) {
                RideInfo ride;
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                stable->BuildRideForPlayer(pSelectedCar->mHandle, iPlayerNum, &ride);
                CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
                tLastEventTimer.UnSet();
            }
            return;
        case __PAD_LEFT__:
            ScrollCars(eSD_PREV);
            return;
        case __PAD_RIGHT__:
            ScrollCars(eSD_NEXT);
            return;
        case __PAD_UP__:
            ScrollLists(eSD_PREV);
            return;
        case __PAD_DOWN__:
            ScrollLists(eSD_NEXT);
            return;
        case __PAD_BUTTON0__: {
            if (pSelectedCar == nullptr)
                return;
            if (pSelectedCar->bLocked)
                return;
            if (FEDatabase->IsOnlineMode())
                return;
            if (FEDatabase->IsLANMode())
                return;
            if (FEDatabase->IsCareerMode() && !FEDatabase->IsCarLotMode()) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                FECarRecord *car = GetSelectedCarRecord();
                FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);
                if ((career != nullptr) && (career->TheImpoundData.IsImpounded() || career->TheImpoundData.IsReleasable()))
                    return;
            }
            const unsigned long FEObj_showcase = 0;
            cFEng::Get()->QueuePackageMessage(0x89d0649c, this->GetPackageName(), nullptr);
            bShowcaseMode = true;
            return;
        }
        case 0xa46253ba: {
            FECarRecord *car = GetSelectedCarRecord();
            FEDatabase->GetCareerSettings()->AwardCash(car->GetCost() / 2);
            FEDatabase->GetPlayerCarStable(iPlayerNum)->DeleteCareerCar(pSelectedCar->mHandle, true);
            uint32 handle = pSelectedCar->mHandle;
            RefreshCarList();
            if (handle == originalCar) {
                SetupForPlayer(0);
                originalCar = FEDatabase->GetCareerSettings()->GetCurrentCar();
            }
            RefreshHeader();
            return;
        }
        case __PAD_BUTTON5__: {
            if (!FEDatabase->IsCareerMode())
                return;
            if (FEDatabase->IsCarLotMode())
                return;
            if (FEDatabase->IsDDay())
                return;
            if (pSelectedCar == nullptr)
                return;
            FECarRecord *record = GetSelectedCarRecord();
            if (record->IsCareer()) {
                FECareerRecord *career = FEDatabase->GetPlayerCarStable(iPlayerNum)->GetCareerRecordByHandle(record->CareerHandle);
                if (career->TheImpoundData.IsImpounded()) {
                    DialogInterface::ShowOneButton(this->GetPackageName(), "", dialog_alert, 0x417b2601, dialog_message_no, 0x80e4f27c);
                    return;
                }
            }
            if (FEDatabase->GetPlayerCarStable(iPlayerNum)->GetNumAvailableCareerCars() < 2) {
                DialogInterface::ShowOneButton(this->GetPackageName(), "", dialog_alert, 0x417b2601, dialog_message_no, 0x9a772bd6);
                return;
            } else {
                char buf[512];
                char cost[16];
                bSNPrintf(cost, sizeof(cost), "%d", record->GetCost() / 2);
                bSNPrintf(buf, sizeof(buf), GetLocalizedString(0xb4a40135), cost);
                DialogInterface::ShowTwoButtons(this->GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0xa46253ba,
                                                dialog_message_no, dialog_message_no, first_dialog_button2, buf);
            }
            return;
        }
        case __PAD_BUTTON4__: {
            if (FEDatabase->IsSplitScreenMode()) {
                uint32 params = 0x411;
                if (iPlayerNum == 1) {
                    params = 0x20411;
                }
                MemcardEnter(this->GetPackageName(), this->GetPackageName(), params, nullptr, nullptr, 0x7e998e5e, 0x8867412d);
                return;
            }
            if (!TheBustedManager.IsImpoundInfoVisible())
                return;
            TheBustedManager.MaybeAddImpoundBox();
            return;
        }
        case FEHASH_EXITCOMPLETE:
            if (bShowcaseMode) {
                FECarRecord *pCar = FEDatabase->GetPlayerCarRecordByHandle(iPlayerNum, pSelectedCar->mHandle);
                ForceCar = pSelectedCar->mHandle;
                Showcase::FromArgs = iPlayerNum;
                Showcase::FromPackage = this->GetPackageName();
                cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(pCar), 0, false);
                return;
            }
            ForceCar = INVALID_CAR_HANDLE;
            if (iPrevButtonMsg == __PAD_ACCEPT__) {
                if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                    cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
                    return;
                }
                if (FEDatabase->IsCareerMode()) {
                    if (!FEDatabase->IsCarLotMode()) {
                        cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                        return;
                    }
                } else if (FEDatabase->IsCustomizeMode()) {
                    FECarRecord *carRecord = FEDatabase->GetPlayerCarRecordByHandle(iPlayerNum, pSelectedCar->mHandle);
                    if (!carRecord->IsCustomized()) {
                        carRecord = FEDatabase->GetPlayerCarStable(iPlayerNum)->CreateNewCustomCar(carRecord->Handle);
                    }
                    FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(carRecord->Handle, iPlayerNum);
                    cFEng::Get()->QueuePackageSwitch("MyCarsManager.fng", 0, 0, false);
                    return;
                }
                if (FEDatabase->RaceMode == GRace::kRaceType_Drag || !FEDatabase->GetPlayerSettings(iPlayerNum)->TransmissionPromptOn) {
                    if (FEDatabase->IsSplitScreenMode() && iPlayerNum != 1) {
                        return;
                    }
                    CommitChangeStartRace(true);
                    return;
                }
                ChooseTransmission();
                return;

            } else if (iPrevButtonMsg == __PAD_BACK__) {
                uint32 reload_old_car = INVALID_CAR_HANDLE;
                if (FEDatabase->IsCareerMode()) {
                    if (FEDatabase->IsCarLotMode()) {
                        RaceStarter::StartCareerFreeRoam();
                        return;
                    }
                    if (!IsCarImpounded(originalCar)) {
                        FEDatabase->GetCareerSettings()->SetCurrentCar(originalCar);
                    }
                    cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                    FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(originalCar, iPlayerNum);
                    cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
                } else if (FEDatabase->IsCustomizeMode()) {
                    cFEng::Get()->QueuePackageSwitch("MyCarsManager.fng", 0, 0, false);
                } else if (FEDatabase->IsSplitScreenMode()) {
                    if (iPlayerNum == 1) {
                        FEDatabase->SetPlayersJoystickPort(1, -1);
                        cFEng::Get()->QueuePackageSwitch("PressStart.fng", 1, 0xff, false);

                    } else {
                        FEManager::Get()->AllowControllerError(false);
                        cFEng::Get()->QueuePackageSwitch("PressStart.fng", 0, 0xff, false);
                    }
                } else {
                    FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(originalCar, iPlayerNum);
                    reload_old_car = originalCar;
                    FEManager::Get()->AllowControllerError(false);
                    cFEng::Get()->QueuePackageSwitch("Track_Options.fng", 0, 0, false);
                }

                if (reload_old_car != -1) {
                    RideInfo ride;
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                    stable->BuildRideForPlayer(reload_old_car, iPlayerNum, &ride);
                    CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
                }
                return;
            }
            return;

        case dialog_message_yes: {
            FECarRecord *selected_record = GetSelectedCarRecord();
            FECarRecord *new_record = FEDatabase->GetPlayerCarStable(0)->CreateNewCareerCar(selected_record->Handle);
            if (new_record != nullptr) {
                FEDatabase->GetCareerSettings()->SpendCash(new_record->GetCost());
                FEDatabase->GetCareerSettings()->SetCurrentCar(new_record->Handle);
            }
            RaceStarter::StartCareerFreeRoam();
            return;
        }
        case 0xb1ee867d: {
            FECarRecord *selected_record = GetSelectedCarRecord();
            FECarRecord *new_record = FEDatabase->GetPlayerCarStable(0)->CreateNewCareerCar(selected_record->Handle);
            if (new_record != nullptr) {
                FEDatabase->GetCareerSettings()->SpendCash(new_record->GetCost());
            }
            RaceStarter::StartCareerFreeRoam();
            return;
        }
        case __BUTTON_PRESSED__:
        case __PAD_ACCEPT__: {
            if (pSelectedCar == nullptr)
                return;
            if (pSelectedCar->bLocked)
                return;

            if ((RealTimer - tLastEventTimer).GetSeconds() < 0.5f)
                return;
            if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                OnlineActOnSelect();
                iPrevButtonMsg = __PAD_ACCEPT__;
                ChooseTransmission();
                return;
            }
            if (FEDatabase->IsCareerMode()) {
                if (FEDatabase->IsCarLotMode()) {
                    if (MemoryCard::GetInstance()->IsListingOldSaveFiles())
                        return;
                    FECarRecord *carRecord = GetSelectedCarRecord();
                    if (carRecord->GetCost() > static_cast<unsigned int>(FEDatabase->GetCareerSettings()->GetCash())) {
                        DialogInterface::ShowOneButton(this->GetPackageName(), "", dialog_alert, 0x417b2601, dialog_message_no, 0x40fa955d);
                        return;
                    }
                    if (FEDatabase->GetPlayerCarStable(iPlayerNum)->GetNumPurchasedCars() > 9) {
                        DialogInterface::ShowOneButton(this->GetPackageName(), "", dialog_alert, 0x417b2601, dialog_message_no, 0x41030a1b);
                        return;
                    }
                    if (FEDatabase->IsDDay()) {
                        DialogInterface::ShowTwoButtons(this->GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO,
                                                        dialog_message_yes, dialog_message_no, dialog_message_no, first_dialog_button2, 0x74317cbc);
                        return;
                    }
                    DialogInterface::ShowThreeButtons(this->GetPackageName(), "", dialog_alert, 0x5b9d89d0, 0x889d822e, 0x1a294dad,
                                                      dialog_message_yes, 0xb1ee867d, dialog_message_no, dialog_message_no, first_dialog_button3,
                                                      0x8c451eba);
                    return;
                } else {
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    FECarRecord *carRecord = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
                    FECareerRecord *career = stable->GetCareerRecordByHandle(carRecord->CareerHandle);
                    if (career->TheImpoundData.IsImpounded() || career->TheImpoundData.IsReleasable()) {
                        TheBustedManager.MaybeReleaseCar();
                        return;
                    }
                    iPrevButtonMsg = __PAD_ACCEPT__;
                    cFEng::Get()->QueuePackageMessage(0x2e76edfb, this->GetPackageName(), nullptr);
                    return;
                }
            }
            if (FEDatabase->IsCustomizeMode()) {
                iPrevButtonMsg = __PAD_ACCEPT__;
                cFEng::Get()->QueuePackageMessage(0x2e76edfb, this->GetPackageName(), nullptr);
                return;
            }
            if (FEDatabase->RaceMode == GRace::kRaceType_Drag || !FEDatabase->GetPlayerSettings(iPlayerNum)->TransmissionPromptOn) {
                int joyPort = FEngMapJoyParamToJoyport(param2);
                FEDatabase->SetPlayersJoystickPort(iPlayerNum, joyPort);
                if (FEDatabase->IsSplitScreenMode() && iPlayerNum == 0) {
                    cFEng::Get()->QueuePackageSwitch("PressStart.fng", 1, 0xff, false);
                } else {
                    iPrevButtonMsg = __PAD_ACCEPT__;
                    cFEng::Get()->QueuePackageMessage(0x2e76edfb, this->GetPackageName(), nullptr);
                }
            } else {
                ChooseTransmission();
                return;
            }
            return;
        }
        case __PAD_BACK__: {
            bool goback = true;

            if (FEDatabase->IsCareerMode()) {
                if (FEDatabase->IsCarLotMode()) {
                    goback = !FEDatabase->IsDDay();
                } else {
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    FECarRecord *carRecord = stable->GetCarRecordByHandle(originalCar);
                    FECareerRecord *career = stable->GetCareerRecordByHandle(carRecord->CareerHandle);
                    if (career->TheImpoundData.IsImpounded() || career->TheImpoundData.IsReleasable()) {
                        DialogInterface::ShowOk(this->GetPackageName(), "", dialog_alert, 0x630931b6);
                        goback = false;
                    }
                }
            } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                RideInfo ride;
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                stable->BuildRideForPlayer(originalCar, iPlayerNum, &ride);
                CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
            }

            if (goback) {
                iPrevButtonMsg = __PAD_BACK__;
                cFEng::Get()->QueuePackageMessage(0x93e8a57c, this->GetPackageName(), nullptr);
            }
            return;
        }
        case 0x1fab5998: {
            if (FEDatabase->IsCareerMode()) {
                cFEng::Get()->QueuePackageSwitch(GetPackageName(), iPlayerNum, 0, false);
            } else if (FEDatabase->IsQuickRaceMode() && !FEDatabase->IsSplitScreenMode()) {
                FEManager::Get()->AllowControllerError(false);
            }

            return;
        }
        case 0x1a2826e1: {
            int joyPort = FEngMapJoyParamToJoyport(param2);
            FEDatabase->SetPlayersJoystickPort(iPlayerNum, joyPort);
            FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission = 0;
            if (FEDatabase->IsSplitScreenMode() && iPlayerNum == 0) {
                cFEng::Get()->QueuePackageSwitch("PressStart.fng", 1, 0xff, false);
                return;
            }
            CommitChangeStartRace(false);
            return;
        }
        case 0x5f5e3886: {
            int joyPort = FEngMapJoyParamToJoyport(param2);
            FEDatabase->SetPlayersJoystickPort(iPlayerNum, joyPort);
            FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission = 1;
            if (FEDatabase->IsSplitScreenMode() && iPlayerNum == 0) {
                cFEng::Get()->QueuePackageSwitch("PressStart.fng", 1, 0xff, false);
                return;
            }
            CommitChangeStartRace(false);
            return;
        }
        case 0x7e998e5e:
            filter = 0xf0001;
            RefreshCarList();
            RefreshHeader();
            cFEng::Get()->QueuePackageMessage(FEHashUpper("ENABLE_INPUTS"), this->GetPackageName(), nullptr);
            return;
        case 0x8defa48b:
        case 0xa0fc39f9:
        case 0xe845bc1c:
            RefreshHeader();
            return;
    }
}

eMenuSoundTriggers UIQRCarSelect::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == __PAD_UP__ || msg == __PAD_DOWN__ || msg == FEHASH_SOUND_UP || msg == FEHASH_SOUND_DOWN) {
        if (FEDatabase->IsCustomizeMode() || FEDatabase->IsCarLotMode() || FEDatabase->IsCareerMode()) {
            return UISND_NONE;
        }
    }
    return maybe;
}

void UIQRCarSelect::Setup() {
    if (FEDatabase->IsCarLotMode()) {
        if (FEDatabase->IsDDay()) {
            const u32 FEObj_firsttime = 0x3a12d2f5;
            cFEng::Get()->QueuePackageMessage(FEObj_firsttime, this->GetPackageName(), nullptr);
        } else {
            const u32 FEObj_carlot = 0x5415e304;
            cFEng::Get()->QueuePackageMessage(FEObj_carlot, this->GetPackageName(), nullptr);
        }
    } else if (FEDatabase->IsQuickRaceMode()) {
        if (FEDatabase->IsSplitScreenMode()) {
            const u32 FEObj_2players = 0x2cf6c390;
            cFEng::Get()->QueuePackageMessage(FEObj_2players, this->GetPackageName(), nullptr);
        } else {
            const u32 FEObj_quickrace = 0xde511657;
            cFEng::Get()->QueuePackageMessage(FEObj_quickrace, this->GetPackageName(), nullptr);
        }
    } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        const u32 FEObj_online = 0x70fbb1e4;
        cFEng::Get()->QueuePackageMessage(FEObj_online, this->GetPackageName(), nullptr);
    } else if (FEDatabase->IsCustomizeMode()) {
        const u32 FEObj_customize = 0xa936c3a2;
        cFEng::Get()->QueuePackageMessage(FEObj_customize, this->GetPackageName(), nullptr);
    } else if (FEDatabase->IsCareerMode()) {
        const u32 FEObj_career = 0x5415c3f1;
        cFEng::Get()->QueuePackageMessage(FEObj_career, this->GetPackageName(), nullptr);
    }

    FEngSetInvisible(this->GetPackageName(), 0x64f6d21f);

    if (FEDatabase->IsCareerMode()) {
        originalCar = FEDatabase->GetCareerSettings()->GetCurrentCar();
        if (FEDatabase->IsCarLotMode()) {
            filter = 0xf0001;
            if (!FEDatabase->GetCareerSettings()->HasCashBonusBeenAwarded()) {
                cFEng::Get()->QueuePackageMessage(FEHashUpper("DISABLE_INPUTS"), this->GetPackageName(), nullptr);
                MemoryCard::GetInstance()->StartListingOldSaveFiles();
            }
        } else {
            filter = 0xf0002;
        }
    } else {
        originalCar = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(iPlayerNum);
        if (!FEDatabase->IsCustomizeMode() && originalCar != 0x12345678) {
            if (originalCar != FEHashUpper("M3GTRCAREERSTART")) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                FECarRecord *car = stable->GetCarRecordByHandle(originalCar);
                if (car != nullptr) {
                    filter = (car->FilterBits & 0x3f) | 0xf0000;
                } else {
                    filter = 0xf0001;
                }
            } else {
                filter = 0xf0001;
            }
        } else {
            filter = 0xf0001;
        }
    }

    for (int i = 0; i < NUM_LISTS; i++) {
        ListHandles[i] = 0xFFFFFFFF;
    }

    RefreshCarList();
    RefreshHeader();
}

void UIQRCarSelect::InitStatsSliders() {
    TheHeatMeter.Init(this->GetPackageName(), "", 1.0f, 5.0f, 1.0f, 1.0f);
    AccelerationSlider.Init(this->GetPackageName(), "ACCELERATION", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    TopSpeedSlider.Init(this->GetPackageName(), "TOPSPEED", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    HandlingSlider.Init(this->GetPackageName(), "HANDLING", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
}

void UIQRCarSelect::UpdateSliders() {
    Physics::Info::Performance performance;
    Physics::Info::Performance current_performance;

    if (pSelectedCar != nullptr) {
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
        if (stable != nullptr) {
            FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
            if (car != nullptr) {
                Attrib::Gen::pvehicle pvehicle(car->VehicleKey, 0, nullptr);
                if (car->IsCustomized()) {
                    stable->GetCustomizationRecordByHandle(car->Customization)->WriteRecordIntoPhysics(pvehicle);
                }
                Physics::Info::EstimatePerformance(pvehicle, performance);
            }
            if (FEDatabase->IsCareerMode()) {
                car = stable->GetCarRecordByHandle(originalCar);
            }
            if (car != nullptr) {
                Attrib::Gen::pvehicle pvehicle(car->VehicleKey, 0, nullptr);
                if (car->IsCustomized()) {
                    stable->GetCustomizationRecordByHandle(car->Customization)->WriteRecordIntoPhysics(pvehicle);
                }
                Physics::Info::EstimatePerformance(pvehicle, current_performance);
            }
        }
    }

    if (FEDatabase->IsDDay()) {
        current_performance = performance;
    }

    AccelerationSlider.SetValue(performance.Acceleration);
    AccelerationSlider.SetPreviewValue(current_performance.Acceleration);
    AccelerationSlider.Draw();

    TopSpeedSlider.SetValue(performance.TopSpeed);
    TopSpeedSlider.SetPreviewValue(current_performance.TopSpeed);
    TopSpeedSlider.Draw();

    HandlingSlider.SetValue(performance.Handling);
    HandlingSlider.SetPreviewValue(current_performance.Handling);
    HandlingSlider.Draw();
}

// UNSOLVED
int UIQRCarSelect::GetFilterType() {
    switch (static_cast<unsigned short>(filter)) {
        case FE_CAR_FILTER_LIST_STOCK:
            return LIST_STOCK;
        case FE_CAR_FILTER_LIST_CAREER:
            return LIST_CAREER;
        case FE_CAR_FILTER_LIST_QUICK_RACE:
            return LIST_QUICK_RACE;
        case FE_CAR_FILTER_LIST_BONUS:
            return LIST_BONUS;
        case FE_CAR_FILTER_LIST_PRESET:
            return LIST_PRESET;
        case FE_CAR_FILTER_LIST_DEBUG:
            return LIST_DEBUG;
        default:
            return LIST_STOCK;
    }
}

// UNSOLVED
void UIQRCarSelect::SetupForPlayer(int player) {
    SelectableCar *car = nullptr;

    for (SelectableCar *sc = FilteredCarsList.GetHead(); sc != FilteredCarsList.EndOfList(); sc = sc->GetNext()) {
        car = sc;
        if (ForceCar != INVALID_CAR_HANDLE) {
            if (sc->mHandle == ForceCar) {
                break;
            }
        } else {
            if (sc->mHandle == ListHandles[GetFilterType()]) {
                break;
            }
            if (FEDatabase->IsCareerMode()) {
                if (sc->mHandle == FEDatabase->GetCareerSettings()->GetCurrentCar()) {
                    break;
                }
            } else if (sc->mHandle == FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(iPlayerNum)) {
                break;
            }
        }
    }

    if (FEDatabase->IsCarLotMode() && ForceCar == INVALID_CAR_HANDLE) {
        CarViewer::CancelCarLoad(eCARVIEWER_PLAYER1_CAR);
        car = FilteredCarsList.GetHead();
        originalCar = car->mHandle;
    }
    ForceCar = INVALID_CAR_HANDLE;
    if (car == nullptr && !FilteredCarsList.IsEmpty()) {
        car = FilteredCarsList.GetHead();
    }
    SetSelectedCar(car, iPlayerNum);
    RefreshHeader();
}

int UIQRCarSelect::GetBonusUnlockText(FECarRecord *fe_car) {
    switch (fe_car->Handle) {
        case STRINGHASH_BL2:
        case STRINGHASH_BL3:
        case STRINGHASH_BL4:
        case STRINGHASH_BL5:
        case STRINGHASH_BL6:
        case STRINGHASH_BL7:
        case STRINGHASH_BL8:
        case STRINGHASH_BL9:
        case STRINGHASH_BL10:
        case STRINGHASH_BL11:
        case STRINGHASH_BL12:
        case STRINGHASH_BL13:
        case STRINGHASH_BL14:
        case STRINGHASH_BL15:
            return 0x4ef2a115;
        case STRINGHASH_BONUS_C6R:
            return 0xbd8bac94;
        case STRINGHASH_E3_DEMO_BMW:
            return 0xbd8bac93;
        case STRINGHASH_BONUS_SL65:
            return 0xbd8bac91;
        case STRINGHASH_BONUS_GT2:
            return 0xbd8bac92;
        default:
            return 0;
    }
}

int UIQRCarSelect::GetBonusUnlockBinNumber(FECarRecord *fe_car) {
    switch (fe_car->Handle) {
        case STRINGHASH_BL2:
            return 2;
        case STRINGHASH_BL3:
            return 3;
        case STRINGHASH_BL4:
            return 4;
        case STRINGHASH_BL5:
            return 5;
        case STRINGHASH_BL6:
            return 6;
        case STRINGHASH_BL7:
            return 7;
        case STRINGHASH_BL8:
            return 8;
        case STRINGHASH_BL9:
            return 9;
        case STRINGHASH_BL10:
            return 10;
        case STRINGHASH_BL11:
            return 11;
        case STRINGHASH_BL12:
            return 12;
        case STRINGHASH_BL13:
            return 13;
        case STRINGHASH_BL14:
            return 14;
        case STRINGHASH_BL15:
            return 15;
        default:
            return -1;
    }
}

void UIQRCarSelect::RefreshHeader() {
    UpdateSliders();

    uint32 list = filter;
    uint32 langhash;
    uint32 texhash;

    switch (list) {
        case FE_CAR_FILTER_LIST_STOCK:
            langhash = 0xd9d6b954;
            texhash = 0x3a541f7f;
            break;
        case FE_CAR_FILTER_LIST_CAREER:
            langhash = 0xee053562;
            texhash = 0xf0bddecd;
            break;
        case FE_CAR_FILTER_LIST_QUICK_RACE:
            langhash = 0x2414de28;
            texhash = 0x9996ca1e;
            break;
        case FE_CAR_FILTER_LIST_BONUS:
            langhash = 0xd8a058f7;
            texhash = 0xbe5ad8a2;
            break;
        case FE_CAR_FILTER_LIST_PRESET:
            langhash = 0x0d8500c3;
            texhash = 0x03704f3d;
            break;
        case FE_CAR_FILTER_LIST_DEBUG:
            langhash = 0x3ec63978;
            texhash = 0x03704f3d;
            break;
        default:
            langhash = 0;
            texhash = 0;
            break;
    }

    const u32 FEObj_NUMBER = 0;
    const u32 FEObj_NUMBEROF = 0;
    const u32 FEObj_SHOWCASEREP = 0;
    const u32 FEObj_PC_SHOWCASE = 0;
    const u32 FEObj_ACCEPTGROUP = 0;

    if (FEDatabase->IsCarLotMode() || !FEDatabase->IsCareerMode()) {
        FEngSetInvisible(this->GetPackageName(), 0x39dc21f9);
        FEngSetInvisible(this->GetPackageName(), 0xe998fe99);
    }

    FEngSetLanguageHash(this->GetPackageName(), 0xaa9834bc, langhash);
    FEngSetTextureHash(this->GetPackageName(), 0xe3b271b8, texhash);
    FEngSetScript(this->GetPackageName(), 0xd0f7c7cc, FEHASH_HIDE, true);

    if (pSelectedCar == nullptr) {
        FEngSetInvisible(this->GetPackageName(), 0x7379349b);
    } else if (pSelectedCar->bLocked) {
        FEngSetInvisible(this->GetPackageName(), 0x7379349b);
    }

    if (pSelectedCar == nullptr) {
        FEngSetLanguageHash(this->GetPackageName(), 0x2d25b2c4, 0x58bbed2a);
        cFEng::Get()->QueuePackageMessage(0xd9420cd5, this->GetPackageName(), nullptr);
        if (filter & 4) {
            FEngSetLanguageHash(this->GetPackageName(), 0x36c1e04d, 0x0da87b01);
        } else {
            FEngSetLanguageHash(this->GetPackageName(), 0x36c1e04d, 0x58bbed2a);
        }
        FEngSetInvisible(this->GetPackageName(), 0x0e9ed0a2);
        FEngSetInvisible(this->GetPackageName(), 0x18a4384f);
        CarViewer::CancelCarLoad(eCARVIEWER_PLAYER1_CAR);
        GarageMainScreen::GetInstance()->DisableCarRendering();

        const u32 FEObj_LOADERLEAVE = 0x913fa282;
        cFEng::Get()->QueuePackageMessage(FEObj_LOADERLEAVE, this->GetPackageName(), nullptr);

        bLoadingBarActive = false;
        tLastEventTimer.UnSet();
        return;
    }

    if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
        FEngSetVisible(this->GetPackageName(), 0x0e9ed0a2);
        FEngSetVisible(this->GetPackageName(), 0x18a4384f);
    }

    FEngSetVisible(this->GetPackageName(), 0x7379349b);
    cFEng::Get()->QueuePackageMessage(0x7c4583dc, this->GetPackageName(), nullptr);

    FEPrintf(this->GetPackageName(), 0x6f25a248, "%d", FilteredCarsList.GetNodeNumber(pSelectedCar));
    FEPrintf(this->GetPackageName(), 0xb2037bdc, "%d", FilteredCarsList.CountElements());

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);

    if (pSelectedCar->bLocked) {
        FEngSetScript(this->GetPackageName(), 0xd0f7c7cc, FEHASH_SHOW, true);
        FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
        if (car->MatchesFilter(0xf0008)) {
            uint32 unlock_hash = GetBonusUnlockText(car);
            if (unlock_hash == 0x4ef2a115) {
                int rival_num = GetBonusUnlockBinNumber(car);
                char rival_name_locdb[128];
                FEngSNPrintf(rival_name_locdb, sizeof(rival_name_locdb), "blacklist_rival_%02d_aka", rival_num);
                FEPrintf(this->GetPackageName(), 0x2d25b2c4, GetLocalizedString(0x4ef2a115), GetLocalizedString(FEHashUpper(rival_name_locdb)),
                         rival_num);
            } else {
                FEngSetLanguageHash(this->GetPackageName(), 0x2d25b2c4, unlock_hash);
            }
        } else {
            Attrib::Gen::frontend fe_attrib(car->FEKey, 0, nullptr);
            int rival_num = fe_attrib.UnlockedAt() + 1;
            char rival_name_locdb[128];
            FEngSNPrintf(rival_name_locdb, sizeof(rival_name_locdb), "blacklist_rival_%02d_aka", rival_num);
            FEPrintf(this->GetPackageName(), 0x2d25b2c4, GetLocalizedString(0x4ef2a115), GetLocalizedString(FEHashUpper(rival_name_locdb)),
                     rival_num);
        }
    }

    FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
    const u32 FEObj_CARNAME = 0xd6d32016;
    const u32 FEObj_CARNAME_SHADOW = 0x79d6e45c;
    FEngSetInvisible(this->GetPackageName(), FEObj_CARNAME);
    FEngSetInvisible(this->GetPackageName(), FEObj_CARNAME_SHADOW);
    FEngSetTextureHash(pManuLogo, car->GetManuLogoHash());
    FEngSetTextureHash(pCarBadge, car->GetLogoHash());

    if (FEDatabase->IsCarLotMode()) {
        const u32 FEObj_BACKTEXT = 0;
        FEPrintf(this->GetPackageName(), 0x1930b057, "%$d", FEDatabase->GetCareerSettings()->GetCash());
        FEPrintf(this->GetPackageName(), 0x20c83c31, "%$d", car->GetCost());
        FEngSetLanguageHash(this->GetPackageName(), 0xdc18c4d4, 0xa9950b93);
        FEngSetLanguageHash(this->GetPackageName(), 0xb94139f4, 0x7010bbf2);
    }

    if (FEDatabase->IsCareerMode() && !FEDatabase->IsCarLotMode()) {
        TheHeatMeter.SetVisibility(true);
    } else {
        TheHeatMeter.SetVisibility(false);
    }

    if (car->IsCareer()) {
        const u32 FEObj_BOUNTYHEADER = 0;
        const u32 FEObj_UNSERVEDHEADER = 0;
        FEngSetInvisible(this->GetPackageName(), 0x39dc21f9);
        FEngSetInvisible(this->GetPackageName(), 0xe998fe99);

        FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);

        if (TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0) ||
            (CheatCanAddImpoundBox && !career->TheImpoundData.IsImpounded())) {
            int num_markers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0);
            FEngSetVisible(this->GetPackageName(), 0x39dc21f9);
            FEPrintf(this->GetPackageName(), 0x5b875870, "%2d", num_markers);
            FEPrintf(this->GetPackageName(), 0xea8aecd9, "%2d", num_markers);
        } else {
            FEngSetInvisible(this->GetPackageName(), 0x39dc21f9);
        }

        if (career->TheImpoundData.IsReleasable()) {
            FEngSetLanguageHash(this->GetPackageName(), 0x72e7ea88, 0x9db4df7d);
            FEngSetLanguageHash(this->GetPackageName(), 0x9d974df3, 0x073b79e0);
            float cost = car->GetReleaseFromImpoundCost();
            FEPrintf(this->GetPackageName(), 0x322b18f9, "%$0.0f", cost);
            FEPrintf(this->GetPackageName(), 0x7044a5a4, "%$d", FEDatabase->GetCareerSettings()->GetCash());
            FEngSetInvisible(this->GetPackageName(), 0x0e9ed0a2);
        } else if (career->TheImpoundData.IsImpounded()) {
            FEngSetLanguageHash(this->GetPackageName(), 0x72e7ea88, 0x9db4df7d);
            FEngSetLanguageHash(this->GetPackageName(), 0x9d974df3, 0x073b79e0);
            FEngSetLanguageHash(this->GetPackageName(), 0x322b18f9, 0xaefedad9);
            FEPrintf(this->GetPackageName(), 0x7044a5a4, "%$d", FEDatabase->GetCareerSettings()->GetCash());
            FEngSetInvisible(this->GetPackageName(), 0x0e9ed0a2);

            if (TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0) || CheatReleaseFromImpoundMarker) {
                int num_markers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);
                FEngSetVisible(this->GetPackageName(), 0xe998fe99);
                FEPrintf(this->GetPackageName(), 0xcc59b910, "%2d", num_markers);
                FEPrintf(this->GetPackageName(), 0xb8f9938a, "%2d", num_markers);
                FEngSetInvisible(this->GetPackageName(), 0x39dc21f9);
            }
        } else {
            FEngSetLanguageHash(this->GetPackageName(), 0x72e7ea88, 0x17574b0e);
            FEngSetLanguageHash(this->GetPackageName(), 0x9d974df3, 0x915f4d26);
            if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
                FEngSetVisible(this->GetPackageName(), 0x0e9ed0a2);
            }
            FECareerRecord *record = stable->GetCareerRecordByHandle(car->CareerHandle);
            if (record != nullptr) {
                FEPrintf(this->GetPackageName(), 0x322b18f9, "%$d", record->GetBounty());
                FEPrintf(this->GetPackageName(), 0x7044a5a4, "%$d", record->GetInfractions(true).GetFineValue());
            }
        }

        TheHeatMeter.SetCurrent(career->GetVehicleHeat());
        TheHeatMeter.SetPreview(career->GetVehicleHeat());
        TheHeatMeter.Draw();
    } else {
        TheHeatMeter.SetVisibility(false);
    }
}

void UIQRCarSelect::ChooseTransmission() {
    eDialogFirstButtons first_button = FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission ? first_dialog_button1 : first_dialog_button2;
    DialogInterface::ShowTwoButtons(this->GetPackageName(), "", dialog_confirmation, 0x317d3005, 0x8cd532a0, 0x5f5e3886, 0x1a2826e1,
                                    dialog_message_no, first_button, 0x6f5401d1);
}

FECarRecord *UIQRCarSelect::GetSelectedCarRecord() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
    FECarRecord *selected_car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
    return selected_car;
}

void UIQRCarSelect::SetSelectedCar(SelectableCar *newCar, int player_num) {
    pSelectedCar = newCar;
    if (TheBustedManager.IsImpoundInfoVisible() && pSelectedCar != nullptr) {
        TheBustedManager.SetSelectedCar(GetSelectedCarRecord());
    }
    if (newCar != nullptr) {
        ListHandles[GetFilterType()] = newCar->mHandle;
        GarageMainScreen::GetInstance()->DisableCarRendering();

        const u32 FEObj_LOADERAPPEAR = 0xa05a328e;
        const u32 FEObj_FADE_IN_TEXT = 0x9c0a27eb;
        cFEng::Get()->QueuePackageMessage(FEObj_LOADERAPPEAR, nullptr, nullptr);
        cFEng::Get()->QueuePackageMessage(FEObj_FADE_IN_TEXT, this->GetPackageName(), nullptr);

        bLoadingBarActive = true;
        if (FEDatabase->IsCareerMode()) {
            if (!FEDatabase->IsCarLotMode()) {
                FEDatabase->GetCareerSettings()->SetCurrentCar(newCar->mHandle);
            }
        } else if (!FEDatabase->IsCustomizeMode()) {
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(newCar->mHandle, player_num);
        }
        tLastEventTimer = RealTimer;
    }
}

int SortCarsByUnlock(SelectableCar *before, SelectableCar *after) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(gPlayerNum);
    Attrib::Gen::frontend before_fe(stable->GetCarRecordByHandle(before->mHandle)->FEKey, 0, nullptr);
    Attrib::Gen::frontend after_fe(stable->GetCarRecordByHandle(after->mHandle)->FEKey, 0, nullptr);
    int before_num = before_fe.UnlockedAt();
    int after_num = after_fe.UnlockedAt();
    return static_cast<int>(before_num > after_num);
}

bool IsValidMikeMannCar(FECarRecord *fe_car, uint32 filter) {
    if (GetMikeMannBuild() == 1) {
        return fe_car->GetType() != CARTYPE_CAYMANS;
    }
    if (GetMikeMannBuild() != 2) {
        return true;
    }
    if ((filter & FE_CAR_FILTER_LIST_MASK) == FE_CAR_FILTER_LIST_STOCK) {
        switch (fe_car->GetType()) {
            case CARTYPE_RX8:
            case CARTYPE_SLR:
            case CARTYPE_BMWM3GTR:
            case CARTYPE_CAYMANS:
            case CARTYPE_GALLARDO:
            case CARTYPE_PUNTO:
                return true;
            default:
                return false;
        }
    }
    if ((filter & FE_CAR_FILTER_LIST_MASK) != FE_CAR_FILTER_LIST_PRESET) {
        return true;
    }
    return fe_car->Handle == bStringHash("M3GTRCAREERSTART");
}

void UIQRCarSelect::RefreshBonusCarList() {
    uint32 unlock_filter = UNLOCK_QUICK_RACE;
    bool bCarUnlocked;
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL15, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL15, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL14, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL14, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL13, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL13, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL12, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL12, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL11, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL11, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL10, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL10, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL9, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL9, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL8, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL8, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL7, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL7, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL6, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL6, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL5, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL5, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL4, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL4, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL3, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL3, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BL2, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BL2, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_E3_DEMO_BMW, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_E3_DEMO_BMW, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BONUS_SL65, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BONUS_SL65, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BONUS_C6R, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BONUS_C6R, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_BONUS_GT2, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_BONUS_GT2, !bCarUnlocked));
    if (UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), STRINGHASH_CASTROLGT, iPlayerNum)) {
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CASTROLGT, false));
    }
    if (GetIsCollectorsEdition()) {
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_CORVETTE, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_997S, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_ELISE, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_SL500, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_SUPRA, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_C6R, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_SL65, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_GT2, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_CAMARO, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(STRINGHASH_CE_GTRSTREET, false));
    }
    SetupForPlayer(iPlayerNum);
}

void UIQRCarSelect::RefreshCarList() {
    ClearCarList();

    if ((filter & FE_CAR_FILTER_LIST_BONUS) != 0) {
        RefreshBonusCarList();
        return;
    }

    FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(iPlayerNum);
    uint32 unlock_filter;

    if (FEDatabase->IsCareerMode()) {
        unlock_filter = FE_CAR_FILTER_LIST_CAREER;
    } else {
        if (!FEDatabase->IsQuickRaceMode() && (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode())) {
            unlock_filter = FE_CAR_FILTER_LIST_STOCK | FE_CAR_FILTER_LIST_CAREER | FE_CAR_FILTER_LIST_QUICK_RACE;
        } else {
            unlock_filter = FE_CAR_FILTER_LIST_STOCK;
        }
    }

    for (int i = 0; i < 200; i++) {
        FECarRecord *fe_car = carDB->GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(filter)) {
            bool bCarUnlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlock_filter), fe_car->Handle, iPlayerNum);
            if (!GetMikeMannBuild() || IsValidMikeMannCar(fe_car, filter)) {
                FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(fe_car->Handle, !bCarUnlocked));
            }
        }
    }

    switch (filter & FE_CAR_FILTER_LIST_MASK) {
        case FE_CAR_FILTER_LIST_STOCK:
        case FE_CAR_FILTER_LIST_CAREER:
        case FE_CAR_FILTER_LIST_BONUS:
        case FE_CAR_FILTER_LIST_PRESET:
            FilteredCarsList.Sort(SortCarsByUnlock);
    }

    SetupForPlayer(iPlayerNum);
}

void UIQRCarSelect::ClearCarList() {
    FilteredCarsList.DeleteAllElements();
}

// UNSOLVED
void UIQRCarSelect::ScrollCars(eScrollDir dir) {
    SelectableCar *old_car = pSelectedCar;
    if (old_car == nullptr)
        return;

    SelectableCar *new_car = FilteredCarsList.GetPrevCircular(old_car);

    if (dir == eSD_PREV) {
    } else if (dir == eSD_NEXT) {
        new_car = FilteredCarsList.GetNextCircular(old_car);
    }
    if (new_car != old_car) {
        SetSelectedCar(new_car, iPlayerNum);
        RefreshHeader();
    }
}

void UIQRCarSelect::ScrollLists(eScrollDir dir) {
    if (FEDatabase->IsCareerMode()) {
        return;
    }
    if (FEDatabase->IsCustomizeMode()) {
        return;
    }

    uint32 region = this->filter & FE_CAR_FILTER_REGION_MASK;
    uint32 list = this->filter & FE_CAR_FILTER_LIST_MASK;

    if (GetMikeMannBuild() == 2) {
        if (list == FE_CAR_FILTER_LIST_STOCK) {
            list = FE_CAR_FILTER_LIST_QUICK_RACE;
        } else if (list == FE_CAR_FILTER_LIST_QUICK_RACE) {
            list = FE_CAR_FILTER_LIST_PRESET;
        } else if (list == FE_CAR_FILTER_LIST_PRESET) {
            list = FE_CAR_FILTER_LIST_STOCK;
        }
    } else if (dir == eSD_PREV) {
        if (list == FE_CAR_FILTER_LIST_STOCK) {
            list = FE_CAR_FILTER_LIST_BONUS;
        } else {
            list = list >> 1;
        }
    } else if (dir == eSD_NEXT) {
        if (list == FE_CAR_FILTER_LIST_BONUS) {
            list = FE_CAR_FILTER_LIST_STOCK;
        } else {
            list = list << 1;
        }
    }

    filter = region | list;

    RefreshCarList();
    RefreshHeader();
}

void UIQRCarSelect::OnlineActOnSelect() {
    FEDatabase->GetQuickRaceSettings(GRace::kRaceType_Circuit)->SetSelectedCar(pSelectedCar->mHandle, 0);
    FEDatabase->GetQuickRaceSettings(GRace::kRaceType_P2P)->SetSelectedCar(pSelectedCar->mHandle, 0);
    FEDatabase->GetQuickRaceSettings(GRace::kRaceType_Drag)->SetSelectedCar(pSelectedCar->mHandle, 0);
}
