#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRCarSelect.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeBusted.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeWorldMapQuickList.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

extern int g_MaximumMaximumTimesBusted;







uint32 UIQRCarSelect::ForceCar = (int)0xFFFFFFFF;
bool QRCarSelectBustedManager::bPlayerJustGotBusted = false;











bool QRCarSelectBustedManager::bIsCross = false;
//

int CheatBustedCount = 0;
int CheatMaxBusted = 0;
bool CheatImpounded = false;
bool CheatCanAddImpoundBox = false;
bool CheatReleaseFromImpoundMarker = false;
bool CheatReleasable = false;
int gPlayerNum;
// gPlayerNum (.bss 0x804AB5AC) solo la usa este fichero. El bloque ocupa las
// mismas cinco lineas que la declaracion extern de antes: los FNEW de abajo
// llevan __LINE__ y sus numeros de linea son parte del codigo que se compara.
//

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
        uint32 hash = ImpoundStampHash;
        eUnloadStreamingTexture(&hash, 1);
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
            return;
        case 0xa0fc39f9:
            WorkingCareerRecord->TheImpoundData.NotifyPlayerPaidToRelease();
            WorkingCareerRecord->SetVehicleHeat(0.0f);
            {
                unsigned int cost = WorkingCarRecord->GetReleaseFromImpoundCost();
                FEDatabase->GetCareerSettings()->SpendCash(static_cast<int>(static_cast<float>(cost)));
            }
            RefreshHeader();
            return;
        case 0xe845bc1c:
            WorkingCareerRecord->TheImpoundData.NotifyPlayerUsedMarkerToRelease();
            WorkingCareerRecord->SetVehicleHeat(0.0f);
            TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);
            RefreshHeader();
            return;
        case 0x406415e3:
            return;
        case 0xe0b38195:
            if (!CalcGameOver())
                return;
            FEDatabase->GetCareerSettings()->SetGameOver();
            DialogInterface::ShowOneButton(ParentPkg, "GameOver.fng", dialog_alert, LANGUAGE_COMMON_OK, 0x3fdc64c1, 0x164bed94);
            return;
        case 0x3fdc64c1:
            FEManager::Get()->SetGarageType(static_cast<eGarageType>(1));
            FEDatabase->ClearGameMode(static_cast<eFEGameModes>(1));
            FEDatabase->SetGameMode(static_cast<eFEGameModes>(0x100));
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            return;
        default:
            return;
    }
}

void QRCarSelectBustedManager::TextureLoadedCallback() {
    if (ShowImpoundedTexture()) {
        FEngSetTextureHash(ParentPkg, 0xce18427d, ImpoundStampHash);
        FEngSetTextureHash(ParentPkg, 0x5b8f2a45, ImpoundStampHash);
        if (Flags == BUSTED_ANIM_SHOW_IMPOUNDED) {
            FEngSetScript(ParentPkg, 0xbc7b91f, FEHASH_ANIMATE, true);
            Flags = BUSTED_ANIM_NOTHING;
        } else {
            FEngSetScript(ParentPkg, 0xbc7b91f, FEHASH_NORMAL, true);
        }
    } else {
        FEngSetScript(ParentPkg, 0xbc7b91f, 0x16a259, true);
    }
}

void QRCarSelectBustedManager::LoadImpoundTexture() {
    switch (GetCurrentLanguage()) {
        case 1:
            ImpoundStampHash = 0xce184740;
            break;
        case 2:
            ImpoundStampHash = 0xce1849e1;
            break;
        case 3:
            ImpoundStampHash = 0xce185441;
            break;
        case 4:
            ImpoundStampHash = 0xce187e47;
            break;
        case 5:
            ImpoundStampHash = 0xce183f30;
            break;
        case 6:
            ImpoundStampHash = 0xce187f32;
            break;
        case 7:
            ImpoundStampHash = 0xce183c96;
            break;
        case 12:
            ImpoundStampHash = 0xce18716e;
            break;
        case 13:
            ImpoundStampHash = 0xce184620;
            break;
        case 8:
            ImpoundStampHash = 0xce185c2f;
            break;
        case 9:
            ImpoundStampHash = 0xce183937;
            break;
        case 10:
            ImpoundStampHash = 0xce18561e;
            break;
        case 11:
            ImpoundStampHash = 0xce188180;
            break;
        default:
            ImpoundStampHash = 0xce18427d;
            break;
    }

    uint32 hash = ImpoundStampHash;
    eLoadStreamingTexture(&hash, 1, TextureLoadedCallbackAccessor, reinterpret_cast<uintptr_t>(this), 0);
}

void QRCarSelectBustedManager::SetSelectedCar(FECarRecord *record) {
    WorkingCarRecord = record;
    WorkingCareerRecord = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(record->CareerHandle);
    if (CheatImpounded != 0) {
        WorkingCareerRecord->TheImpoundData.MaxBusted = static_cast<unsigned char>(CheatMaxBusted);
        WorkingCareerRecord->TheImpoundData.TimesBusted = static_cast<char>(CheatMaxBusted);
        WorkingCareerRecord->TheImpoundData.BecomeImpounded(FEImpoundData::IMPOUND_REASON_STRIKE_LIMIT_REACHED);
        Flags = BUSTED_ANIM_SHOW_IMPOUNDED;
    } else if (CheatBustedCount != 0) {
        WorkingCareerRecord->TheImpoundData.TimesBusted = static_cast<char>(CheatBustedCount);
        WorkingCareerRecord->TheImpoundData.MaxBusted = static_cast<unsigned char>(CheatMaxBusted);
        Flags = BUSTED_ANIM_SHOW_STRIKE;
    }
    if (CheatReleasable != 0) {
        bool released;
        do {
            released = WorkingCareerRecord->TheImpoundData.NotifyWin();
        } while (!released);
    }
    RefreshHeader();
}

void QRCarSelectBustedManager::RefreshHeader() {
    if (!IsImpoundInfoVisible())
        return;

    bool bNotImpounded = false;
    if (ShowImpoundedTexture()) {
        TextureInfo *texInfo = GetTextureInfo(ImpoundStampHash, 0, 0);
        if (texInfo) {
            FEngSetScript(ParentPkg, 0xbc7b91f, FEHASH_NORMAL, true);
            FEngSetScript(ParentPkg, 0x64f3a49c, FEHASH_APPEAR, true);
            FEngSetTextureHash(GetPackageName(), 0xce18427d, ImpoundStampHash);
            FEngSetTextureHash(GetPackageName(), 0x5b8f2a45, ImpoundStampHash);
        }
        unsigned int cost = WorkingCarRecord->GetReleaseFromImpoundCost();
        int playerCash = *reinterpret_cast<int *>(reinterpret_cast<char *>(FEDatabase->GetUserProfile(0)) + 0xf0);
        bool canAffordRelease = playerCash >= static_cast<int>(static_cast<float>(cost));
        bool hasMarkers = TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);
        if (WorkingCareerRecord->TheImpoundData.IsReleasable() && canAffordRelease) {
            FEngSetLanguageHash(ParentPkg, 0xb94139f4, 0x281dee8a);
        } else if (hasMarkers) {
            FEngSetLanguageHash(ParentPkg, 0xb94139f4, 0xf9c73cc2);
        } else {
            FEngSetLanguageHash(ParentPkg, 0xb94139f4, 0x2b65a216);
        }
    } else {
        FEngSetLanguageHash(ParentPkg, 0xb94139f4, 0x2b65a216);
        FEngSetScript(ParentPkg, 0x64f3a49c, 0x16a259, true);
    }
    if ((WorkingCareerRecord->TheImpoundData.TimesBusted & 0x80) == 0) {
        FEngSetVisible(FEngFindObject(ParentPkg, 0x75721326));
        int posIndex = 1;
        unsigned int script1 = 0x16a259;
        unsigned int script2 = 0x16a259;
        switch (WorkingCareerRecord->TheImpoundData.MaxBusted) {
            case 4:
                posIndex = 2;
                script2 = 0x1ca7c0;
                break;
            case 5:
                posIndex = 3;
                script2 = 0x1ca7c0;
                script1 = 0x1ca7c0;
                break;
        }
        FEngSetScript(ParentPkg, 0x5bc78037, script2, true);
        FEngSetScript(ParentPkg, 0x48095518, script1, true);
        FEngSetScript(ParentPkg, 0xf9a5ce86, FEngHashString("POS%d", posIndex), true);
        FEngSetScript(ParentPkg, 0xebf0016e, FEngHashString("POS%d", posIndex), true);
        if (Flags == BUSTED_ANIM_SHOW_STRIKE) {
            FEngSetScript(ParentPkg, FEngHashString("IMPOUND_STATE_%d", static_cast<int>(WorkingCareerRecord->TheImpoundData.TimesBusted)),
                          FEHASH_ANIMATE, true);
            Flags = BUSTED_ANIM_NOTHING;
        }
        for (int i = 1; i <= static_cast<int>(static_cast<unsigned char>(WorkingCareerRecord->TheImpoundData.MaxBusted)); i++) {
            if (i <= WorkingCareerRecord->TheImpoundData.TimesBusted) {
                if (!FEngIsScriptSet(ParentPkg, FEngHashString("IMPOUND_STATE_%d", i), FEHASH_ANIMATE)) {
                    FEngSetScript(ParentPkg, FEngHashString("IMPOUND_STATE_%d", i), 0x1ca7c0, true);
                }
            } else {
                FEngSetScript(ParentPkg, FEngHashString("IMPOUND_STATE_%d", i), 0x16a259, true);
            }
        }
    } else {
        if (WorkingCareerRecord->TheImpoundData.IsReleasable()) {
            FEngSetInvisible(FEngFindObject(ParentPkg, 0x75721326));
        } else if (WorkingCareerRecord->TheImpoundData.IsImpounded()) {
            FEngSetInvisible(FEngFindObject(ParentPkg, 0x75721326));
        } else {
            bNotImpounded = true;
        }
    }
    if (bNotImpounded) {
        FEngSetScript(ParentPkg, 0xbc7b91f, 0x16a259, true);
        FEngSetInvisible(FEngFindObject(ParentPkg, 0x75721326));
        FEngSetLanguageHash(ParentPkg, 0xb94139f4, 0x2b65a216);
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

void QRCarSelectBustedManager::MaybeReleaseCar() {
    FECareerRecord *record = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(WorkingCarRecord->CareerHandle);
    float costf = WorkingCarRecord->GetReleaseFromImpoundCost();
    int cost = static_cast<int>(costf);
    bool has_cash = FEDatabase->GetCareerSettings()->GetCash() >= cost;
    bool has_marker = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0) > 0;

    if (record->TheImpoundData.IsReleasable() && has_cash) {
        if (has_marker) {
            DialogInterface::ShowThreeButtons(ParentPkg, "", dialog_confirmation, 0xf9c73cc2, 0x4eb9591f, LANGUAGE_COMMON_CANCEL, 0xe845bc1c, 0xa0fc39f9,
                                              0x5ee58948, 0x5ee58948, first_dialog_button2, 0xb715ae8f, cost);
        } else {
            DialogInterface::ShowTwoButtons(ParentPkg, "", dialog_confirmation, LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL, 0xa0fc39f9, 0x5ee58948, first_dialog_button1,
                                            0xcad5722e, cost);
        }
    } else if (has_marker) {
        DialogInterface::ShowTwoButtons(ParentPkg, "", dialog_confirmation, LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL, 0xe845bc1c, 0x5ee58948, first_dialog_button1,
                                        0xed4dd591, static_cast<int>(costf));
    } else {
        DialogInterface::ShowOneButton(ParentPkg, "", dialog_alert, LANGUAGE_COMMON_OK, 0x5ee58948, 0xe96fa0c5);
    }





}

void QRCarSelectBustedManager::MaybeAddImpoundBox() {
    bool can_add_impound_box = false;
    if (WorkingCareerRecord->TheImpoundData.CanAddMaxBusted()) {
        can_add_impound_box = true;
    }
    can_add_impound_box = can_add_impound_box && TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0);
    bool showDialog = can_add_impound_box || CheatCanAddImpoundBox != 0;
    if (showDialog) {
        DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_confirmation, LANGUAGE_COMMON_OK, LANGUAGE_COMMON_CANCEL, 0x8defa48b, 0xb4edeb6d,
                                        first_dialog_button2, 0xcebda20);
    } else if (static_cast<int>(WorkingCareerRecord->TheImpoundData.MaxBusted) >= g_MaximumMaximumTimesBusted) {
        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_info, LANGUAGE_COMMON_OK, 0xb4edeb6d, 0xbcae8539);
    }
}

UIQRCarSelect::UIQRCarSelect(ScreenConstructorData *sd)
    : MenuScreen(sd), pManuLogo(nullptr), pCarBadge(nullptr), pCarName(nullptr), pCarNameShadow(nullptr),
      pFilter(nullptr), originalCar(0xFFFFFFFF),
      TheBustedManager(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), (sd->Arg >> 8) & 0xFF),
      bLoadingBarActive(false), bShowcaseMode(false) {
    iPlayerNum = sd->Arg & 0xFF;
    filter = 0;
    iPrevButtonMsg = 0;
    tLastEventTimer.ResetLow();

    const u32 FEObj_ManufacturerLogo = 0x3e01ad1d;
    pManuLogo = FEngFindImage(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), FEObj_ManufacturerLogo);
    const u32 FEObj_CarBadge = 0xb05dd708;
    pCarBadge = FEngFindImage(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), FEObj_CarBadge);
    const u32 FEObj_CARNAME = __CARNAME__;
    pCarName = FEngFindString(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), FEObj_CARNAME);
    const u32 FEObj_CARNAME_SHADOW = 0x79d6e45c;
    pCarNameShadow = FEngFindString(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), FEObj_CARNAME_SHADOW);
    const u32 FEObj_FILTER = __FILTER__;
    pFilter = FEngFindString(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), FEObj_FILTER);

    if (FEDatabase->IsSplitScreenMode()) {
        const u32 FEObj_PLAYER_TEXT_GROUP = 0xe3fe27fe;
        if (iPlayerNum == 0) {
            gPlayerNum = iPlayerNum;
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), FEObj_PLAYER_TEXT_GROUP, LANGUAGE_COMMON_PLAYER_1);
            FEDatabase->DeleteMultiplayerProfile(1);
        } else {
            gPlayerNum = 1;
            FEDatabase->CreateMultiplayerProfile(1);
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), FEObj_PLAYER_TEXT_GROUP, LANGUAGE_COMMON_PLAYER_2);
        }
    } else {
        gPlayerNum = 0;
    }

    if (!FEDatabase->IsCarLotMode()) {
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x19398802);
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x1930b057);
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x20d113dc);
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x20c83c31);
    }

    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xe9ed0a2);
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x18a4384f);
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
    FECarRecord *car = stable->GetCarRecordByHandle(handle);
    FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);
    if (!career) {
        return false;
    }
    return career->TheImpoundData.IsImpounded();
}

void UIQRCarSelect::CommitChangeStartRace(bool allowError) {
    FEManager::Get()->AllowControllerError(allowError);
    FEDatabase->DeleteMultiplayerProfile(1);
    RaceStarter::StartRace();
}

void UIQRCarSelect::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (TheBustedManager.IsImpoundInfoVisible()) {
        TheBustedManager.NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
        case 0x1265ece9: {
            if (!FEDatabase->IsCareerMode())
                return;
            if (!FEDatabase->IsCarLotMode())
                return;
            GarageMainScreen::GetInstance()->UpdateCurrentCameraView(false);
            return;
        }
        case 0x35f8620b: {
            if (!FEDatabase->IsSplitScreenMode())
                return;
            cFEng::Get()->QueuePackageMessage(0x841d518a, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
            return;
        }
        case 0xc98356ba: {
            if (GarageMainScreen::GetInstance()->IsCarRendering() && bLoadingBarActive) {
                cFEng::Get()->QueuePackageMessage(0x913fa282, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
                bLoadingBarActive = false;
            }
            if (!tLastEventTimer.IsSet())
                return;
            float elapsed = (RealTimer - tLastEventTimer).GetSeconds();
            if (elapsed < 0.25f)
                return;
            {
                RideInfo ride;
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                stable->BuildRideForPlayer(pSelectedCar->mHandle, iPlayerNum, &ride);
                CarViewer::SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
                tLastEventTimer.UnSet();
            }
            return;
        }
        case 0x9120409e:
            ScrollCars(eSD_PREV);
            return;
        case 0xb5971bf1:
            ScrollCars(eSD_NEXT);
            return;
        case 0x72619778:
            ScrollLists(eSD_PREV);
            return;
        case 0x911c0a4b:
            ScrollLists(eSD_NEXT);
            return;
        case 0xc519bfbf: {
            if (!pSelectedCar)
                return;
            if (pSelectedCar->bLocked != 0)
                return;
            unsigned int flags = FEDatabase->GetGameMode();
            if ((flags & 8) != 0)
                return;
            if ((flags & 0x40) != 0)
                return;
            if ((FEDatabase->GetGameMode() & 1) != 0 && (FEDatabase->GetGameMode() & 0x8000) == 0) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                FECarRecord *car = GetSelectedCarRecord();
                FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);
                if (career) {
                    bool impounded = career->TheImpoundData.IsImpounded();
                    if (impounded || career->TheImpoundData.IsReleasable())
                        return;
                }
            }
            cFEng::Get()->QueuePackageMessage(0x89d0649c, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
            bShowcaseMode = true;
            return;
        }
        case 0xa46253ba: {
            FECarRecord *car = GetSelectedCarRecord();
            CareerSettings *settings = FEDatabase->GetCareerSettings();
            unsigned int cost = car->GetCost();
            settings->AwardCash(static_cast<int>(cost >> 1));
            FEPlayerCarDB *stable2 = FEDatabase->GetPlayerCarStable(iPlayerNum);
            stable2->DeleteCareerCar(pSelectedCar->mHandle, true);
            unsigned int old_handle = pSelectedCar->mHandle;
            RefreshCarList();
            if (old_handle == originalCar) {
                SetupForPlayer(0);
                originalCar = FEDatabase->GetCareerSettings()->GetCurrentCar();
            }
            RefreshHeader();
            return;
        }
        case 0xc519bfc4: {
            if (!FEDatabase->IsCareerMode())
                return;
            if (FEDatabase->IsCarLotMode())
                return;
            if (FEDatabase->GetCareerSettings()->GetCurrentBin() > 15)
                return;
            if (!pSelectedCar)
                return;
            FECarRecord *car = GetSelectedCarRecord();
            bool isCareer = car->CareerHandle != 0xff;
            if (isCareer) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);
                if (career->TheImpoundData.IsImpounded()) {
                    DialogInterface::ShowOneButton(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0x34dc1bcf,
                                                   0x80e4f27c);
                    return;
                }
            }
            FEPlayerCarDB *stable2 = FEDatabase->GetPlayerCarStable(iPlayerNum);
            if (stable2->GetNumAvailableCareerCars() < 2) {
                DialogInterface::ShowOneButton(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0x34dc1bcf,
                                               0x9a772bd6);
                return;
            }
            char buf[512];
            char cost_str[16];
            const char *digits_fmt = "%$d";
            unsigned int cost = car->GetCost();
            bSNPrintf(cost_str, 0x10, digits_fmt, cost >> 1);
            bSNPrintf(buf, 0x200, GetLocalizedString(0xb4a40135), cost_str);
            DialogInterface::ShowTwoButtons(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO,
                                            0xa46253ba, 0x34dc1bcf, 0x34dc1bcf, first_dialog_button2, buf);
            return;
        }
        case 0xc519bfc3: {
            if (FEDatabase->IsSplitScreenMode()) {
                unsigned int op = 0x411;
                if (iPlayerNum == 1) {
                    op = 0x20411;
                }
                MemcardEnter(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), reinterpret_cast<MenuScreen *>(this)->GetPackageName(), op,
                             nullptr, nullptr, 0x7e998e5e, 0x8867412d);
                return;
            }
            if (!TheBustedManager.IsImpoundInfoVisible())
                return;
            TheBustedManager.MaybeAddImpoundBox();
            return;
        }
        case 0xe1fde1d1: {
            if (bShowcaseMode) {
                unsigned int handle = pSelectedCar->mHandle;
                FECarRecord *car = FEDatabase->GetPlayerCarStable(iPlayerNum)->GetCarRecordByHandle(handle);
                ForceCar = pSelectedCar->mHandle;
                Showcase::FromArgs = iPlayerNum;
                Showcase::FromPackage = reinterpret_cast<MenuScreen *>(this)->GetPackageName();
                cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(car), 0, false);
                return;
            }
            ForceCar = 0xffffffff;
            if (iPrevButtonMsg == 0x406415e3) {
                if ((FEDatabase->GetGameMode() & 8) != 0 || (FEDatabase->GetGameMode() & 0x40) != 0) {
                    cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
                    return;
                }
                if ((FEDatabase->GetGameMode() & 1) != 0) {
                    if ((FEDatabase->GetGameMode() & 0x8000) == 0) {
                        cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                        return;
                    }
                } else if ((FEDatabase->GetGameMode() & 0x20) != 0) {
                    unsigned int handle = pSelectedCar->mHandle;
                FECarRecord *car = FEDatabase->GetPlayerCarStable(iPlayerNum)->GetCarRecordByHandle(handle);
                    if (!car->IsCustomized()) {
                        car = FEDatabase->GetPlayerCarStable(iPlayerNum)->CreateNewCustomCar(car->Handle);
                    }
                    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                    settings->SelectedCar[iPlayerNum] = car->Handle;
                    cFEng::Get()->QueuePackageSwitch("MyCarsManager.fng", 0, 0, false);
                    return;
                }
                if (FEDatabase->RaceMode != GRace::kRaceType_Drag) {
                    if (FEDatabase->GetPlayerSettings(iPlayerNum)->TransmissionPromptOn != 0) {
                        goto choose_transmission;
                    }
                }
                if (FEDatabase->IsSplitScreenMode() && iPlayerNum != 1) {
                    return;
                }
                CommitChangeStartRace(true);
                return;
            } else if (iPrevButtonMsg == 0x911ab364) {
                int iVar3 = -1;
                unsigned int flags = FEDatabase->GetGameMode();
                if ((flags & 1) != 0) {
                    if ((flags & 0x8000) != 0) {
                        RaceStarter::StartCareerFreeRoam();
                        return;
                    }
                    if (!IsCarImpounded(originalCar)) {
                        FEDatabase->GetCareerSettings()->SetCurrentCar(originalCar);
                    }
                    cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                } else if ((FEDatabase->GetGameMode() & 8) != 0 || (FEDatabase->GetGameMode() & 0x40) != 0) {
                    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                    settings->SelectedCar[iPlayerNum] = originalCar;
                    cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
                } else {
                    bool allowControllerError = (FEDatabase->GetGameMode() & 0x20) != 0;
                    if (allowControllerError) {
                        cFEng::Get()->QueuePackageSwitch("MyCarsManager.fng", 0, 0, false);
                    } else {
                        bool isSplit = FEDatabase->IsSplitScreenMode();
                        if (isSplit) {
                            if (iPlayerNum == 1) {
                                FEDatabase->SetPlayersJoystickPort(1, -1);
                                cFEng::Get()->QueuePackageSwitch("PressStart.fng", true, 0xff, false);
                            } else {
                                FEManager::Get()->AllowControllerError(allowControllerError);
                                cFEng::Get()->QueuePackageSwitch("PressStart.fng", false, 0xff, false);
                            }
                        } else {
                            RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                            settings->SelectedCar[iPlayerNum] = originalCar;
                            iVar3 = originalCar;
                            FEManager::Get()->AllowControllerError(isSplit);
                            cFEng::Get()->QueuePackageSwitch("Track_Options.fng", 0, 0, false);
                        }
                    }
                }
                if (iVar3 != -1) {
                    RideInfo ride;
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                    stable->BuildRideForPlayer(iVar3, iPlayerNum, &ride);
                    CarViewer::SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
                }
                return;
            }
            return;
        }
        case 0xd05fc3a3: {
            FECarRecord *car = GetSelectedCarRecord();
            FECarRecord *new_car = FEDatabase->GetPlayerCarStable(0)->CreateNewCareerCar(car->Handle);
            if (new_car) {
                FEDatabase->GetCareerSettings()->SpendCash(new_car->GetCost());
                FEDatabase->GetCareerSettings()->SetCurrentCar(new_car->Handle);
            }
            RaceStarter::StartCareerFreeRoam();
            return;
        }
        case 0xb1ee867d: {
            FECarRecord *car = GetSelectedCarRecord();
            FECarRecord *new_car = FEDatabase->GetPlayerCarStable(0)->CreateNewCareerCar(car->Handle);
            if (new_car) {
                FEDatabase->GetCareerSettings()->SpendCash(new_car->GetCost());
            }
            RaceStarter::StartCareerFreeRoam();
            return;
        }
        case 0x0c407210:
        case 0x406415e3: {
            if (!pSelectedCar)
                return;
            if (pSelectedCar->bLocked != 0)
                return;
            float elapsed = (RealTimer - tLastEventTimer).GetSeconds();
            if (elapsed < 0.25f)
                return;
            if ((FEDatabase->GetGameMode() & 8) != 0 || (FEDatabase->GetGameMode() & 0x40) != 0) {
                OnlineActOnSelect();
                iPrevButtonMsg = 0x406415e3;
                ChooseTransmission();
                return;
            }
            if ((FEDatabase->GetGameMode() & 1) != 0) {
                if ((FEDatabase->GetGameMode() & 0x8000) != 0) {
                    if (MemoryCard::GetInstance()->IsListingOldSaveFiles())
                        return;
                    unsigned int cost = GetSelectedCarRecord()->GetCost();
                    if (cost > static_cast<unsigned int>(FEDatabase->GetCareerSettings()->GetCash())) {
                        DialogInterface::ShowOneButton(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK,
                                                       0x34dc1bcf, 0x40fa955d);
                        return;
                    }
                    FEPlayerCarDB *stable2 = FEDatabase->GetPlayerCarStable(iPlayerNum);
                    if (stable2->GetNumPurchasedCars() > 9) {
                        DialogInterface::ShowOneButton(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK,
                                                       0x34dc1bcf, 0x41030a1b);
                        return;
                    }
                    if (FEDatabase->GetCareerSettings()->GetCurrentBin() > 15) {
                        DialogInterface::ShowTwoButtons(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES,
                                                        LANGUAGE_COMMON_NO, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf, first_dialog_button2, 0x74317cbc);
                        return;
                    }
                    DialogInterface::ShowThreeButtons(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_alert, 0x5b9d89d0,
                                                      0x889d822e, LANGUAGE_COMMON_CANCEL, 0xd05fc3a3, 0xb1ee867d, 0x34dc1bcf, 0x34dc1bcf, first_dialog_button3,
                                                      0x8c451eba);
                    return;
                }
                FEPlayerCarDB *stable2 = FEDatabase->GetPlayerCarStable(0);
                FECarRecord *car = stable2->GetCarRecordByHandle(pSelectedCar->mHandle);
                FECareerRecord *career = stable2->GetCareerRecordByHandle(car->CareerHandle);
                bool impounded = career->TheImpoundData.IsImpounded();
                if (impounded || career->TheImpoundData.IsReleasable()) {
                    TheBustedManager.MaybeReleaseCar();
                    return;
                }
                iPrevButtonMsg = 0x406415e3;
                cFEng::Get()->QueuePackageMessage(FEHASH_LEAVE_ACCEPT, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
                return;
            }
            if ((FEDatabase->GetGameMode() & 0x20) != 0) {
                iPrevButtonMsg = 0x406415e3;
                cFEng::Get()->QueuePackageMessage(FEHASH_LEAVE_ACCEPT, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
                return;
            }
            if (FEDatabase->RaceMode != GRace::kRaceType_Drag) {
                if (FEDatabase->GetPlayerSettings(iPlayerNum)->TransmissionPromptOn != 0) {
                    goto choose_transmission;
                }
            }
            char port = FEngMapJoyParamToJoyport(param1);
            FEDatabase->SetPlayersJoystickPort(iPlayerNum, port);
            if (!FEDatabase->IsSplitScreenMode() || iPlayerNum != 0) {
                iPrevButtonMsg = 0x406415e3;
                cFEng::Get()->QueuePackageMessage(FEHASH_LEAVE_ACCEPT, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
                return;
            }
            cFEng::Get()->QueuePackageSwitch("PressStart.fng", true, 0xff, false);
            return;
        }
choose_transmission:
        ChooseTransmission();
        return;
        case 0x911ab364: {
            bool bShouldProceed = true;
            unsigned int flags = FEDatabase->GetGameMode();
            if ((flags & 1) != 0) {
                if ((flags & 0x8000) != 0) {
                    bShouldProceed = FEDatabase->GetCareerSettings()->GetCurrentBin() < 16;
                } else {
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    FECarRecord *car = stable->GetCarRecordByHandle(originalCar);
                    FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);
                    bool impounded = career->TheImpoundData.IsImpounded();
                    if (impounded || career->TheImpoundData.IsReleasable()) {
                        DialogInterface::ShowOk(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_alert, 0x630931b6);
                        bShouldProceed = false;
                    }
                }
            } else if ((FEDatabase->GetGameMode() & 8) != 0 || (FEDatabase->GetGameMode() & 0x40) != 0) {
                RideInfo ride;
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                stable->BuildRideForPlayer(originalCar, iPlayerNum, &ride);
                CarViewer::SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
            }
            if (!bShouldProceed)
                return;
            iPrevButtonMsg = 0x911ab364;
            cFEng::Get()->QueuePackageMessage(FEHASH_LEAVE_BACK, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
            return;
        }
        case 0x1fab5998: {
            if (FEDatabase->IsCareerMode()) {
                cFEng::Get()->QueuePackageSwitch(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), iPlayerNum, 0, false);
                return;
            }
            if (!FEDatabase->IsQuickRaceMode()) {
                return;
            }
            bool isSplit = FEDatabase->IsSplitScreenMode();
            if (isSplit) {
                return;
            }
            FEManager::Get()->AllowControllerError(isSplit);
            return;
        }
        case 0x34dc1bcf:
            break;
        case 0x1a2826e1: {
            char port = FEngMapJoyParamToJoyport(param1);
            FEDatabase->SetPlayersJoystickPort(iPlayerNum, port);
            FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission = 0;
            if (FEDatabase->IsSplitScreenMode() && iPlayerNum == 0) {
                cFEng::Get()->QueuePackageSwitch("PressStart.fng", true, 0xff, false);
                return;
            }
            CommitChangeStartRace(false);
            return;
        }
        case 0x5f5e3886: {
            char port = FEngMapJoyParamToJoyport(param1);
            FEDatabase->SetPlayersJoystickPort(iPlayerNum, port);
            FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission = 1;
            if (FEDatabase->IsSplitScreenMode() && iPlayerNum == 0) {
                cFEng::Get()->QueuePackageSwitch("PressStart.fng", true, 0xff, false);
                return;
            }
            CommitChangeStartRace(false);
            return;
        }
        case 0x7e998e5e:
            filter = 0xf0001;
            RefreshCarList();
            RefreshHeader();
            cFEng::Get()->QueuePackageMessage(FEHashUpper("ENABLE_INPUTS"), reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
            return;
        case 0x8defa48b:
        case 0xa0fc39f9:
        case 0xe845bc1c:
            RefreshHeader();
            return;
    }
    return;
}

eMenuSoundTriggers UIQRCarSelect::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == 0x72619778 || msg == 0x911c0a4b || msg == 0xb205316c || msg == 0x480df13f) {
        if (FEDatabase->IsCustomizeMode() || FEDatabase->IsCarLotMode() || FEDatabase->IsCareerMode()) {
            return static_cast<eMenuSoundTriggers>(-1);
        }
    }
    return maybe;
}

void UIQRCarSelect::Setup() {
    if (FEDatabase->IsCarLotMode()) {
        if (FEDatabase->IsDDay()) {
            const u32 FEObj_firsttime = 0x3a12d2f5;
            cFEng::Get()->QueuePackageMessage(FEObj_firsttime, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
        } else {
            const u32 FEObj_carlot = 0x5415e304;
            cFEng::Get()->QueuePackageMessage(FEObj_carlot, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
        }
    } else if (FEDatabase->IsQuickRaceMode()) {
        if (FEDatabase->IsSplitScreenMode()) {
            const u32 FEObj_2players = 0x2cf6c390;
            cFEng::Get()->QueuePackageMessage(FEObj_2players, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
        } else {
            const u32 FEObj_quickrace = 0xde511657;
            cFEng::Get()->QueuePackageMessage(FEObj_quickrace, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
        }
    } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        const u32 FEObj_online = 0x70fbb1e4;
        cFEng::Get()->QueuePackageMessage(FEObj_online, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
    } else if (FEDatabase->IsCustomizeMode()) {
        const u32 FEObj_customize = FEHASH_CUSTOMIZE;
        cFEng::Get()->QueuePackageMessage(FEObj_customize, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
    } else if (FEDatabase->IsCareerMode()) {
        const u32 FEObj_career = 0x5415c3f1;
        cFEng::Get()->QueuePackageMessage(FEObj_career, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
    }

    FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x64f6d21f);

    if ((FEDatabase->GetGameMode() & 1) != 0) {
        originalCar = FEDatabase->GetCareerSettings()->GetCurrentCar();
        if ((FEDatabase->GetGameMode() & 0x8000) != 0) {
            filter = 0xf0001;
            UserProfile *profile = FEDatabase->GetUserProfile(0);
            if ((profile->GetCareer()->SpecialFlags & 2) == 0) {
                cFEng::Get()->QueuePackageMessage(FEHashUpper("DISABLE_INPUTS"), reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
                MemoryCard::GetInstance()->StartListingOldSaveFiles();
            }
            goto init_list_handles;
        }
        filter = 0xf0002;
    } else {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        originalCar = settings->GetSelectedCar(iPlayerNum);
        if ((FEDatabase->GetGameMode() & 0x20) == 0 && originalCar != 0x12345678) {
            unsigned int m3gtrHash = FEHashUpper("M3GTRCAREERSTART");
            if (originalCar != m3gtrHash) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                FECarRecord *car = stable->GetCarRecordByHandle(originalCar);
                if (car) {
                    filter = (car->FilterBits & 0x3f) | 0xf0000;
                    goto init_list_handles;
                }
            }
        }
        filter = 0xf0001;
    }

init_list_handles:
    int i = 0;
    do {
        ListHandles[i] = 0xFFFFFFFF;
        i++;
    } while (i < 6);

    RefreshCarList();
    RefreshHeader();
}

void UIQRCarSelect::InitStatsSliders() {
    TheHeatMeter.Init(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", 1.0f, 5.0f, 1.0f, 1.0f);
    AccelerationSlider.Init(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "ACCELERATION", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    TopSpeedSlider.Init(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "TOPSPEED", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    HandlingSlider.Init(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "HANDLING", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
}

void UIQRCarSelect::UpdateSliders() {
    Physics::Info::Performance perf1;
    Physics::Info::Performance perf2;

    if (pSelectedCar != nullptr) {
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
        if (stable != nullptr) {
            FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
            if (car != nullptr) {
                Attrib::Gen::pvehicle pveh(car->VehicleKey, 0, nullptr);
                bool hasCustomization = (car->Customization != 0xff);
                if (hasCustomization) {
                    FECustomizationRecord *cust = stable->GetCustomizationRecordByHandle(car->Customization);
                    cust->WriteRecordIntoPhysics(pveh);
                }
                Physics::Info::EstimatePerformance(pveh, perf1);
            }
            if ((FEDatabase->GetGameMode() & 1) != 0) {
                car = stable->GetCarRecordByHandle(originalCar);
            }
            if (car != nullptr) {
                Attrib::Gen::pvehicle pveh2(car->VehicleKey, 0, nullptr);
                bool hasCustomization = (car->Customization != 0xff);
                if (hasCustomization) {
                    FECustomizationRecord *cust = stable->GetCustomizationRecordByHandle(car->Customization);
                    cust->WriteRecordIntoPhysics(pveh2);
                }
                Physics::Info::EstimatePerformance(pveh2, perf2);
            }
        }
    }

    if (FEDatabase->GetCareerSettings()->GetCurrentBin() > 15) {
        perf2 = perf1;
    }

    AccelerationSlider.SetValue(perf1.Acceleration);
    AccelerationSlider.SetPreviewValue(perf2.Acceleration);
    AccelerationSlider.Draw();

    TopSpeedSlider.SetValue(perf1.TopSpeed);
    TopSpeedSlider.SetPreviewValue(perf2.TopSpeed);
    TopSpeedSlider.Draw();

    HandlingSlider.SetValue(perf1.Handling);
    HandlingSlider.SetPreviewValue(perf2.Handling);
    HandlingSlider.Draw();
}

int UIQRCarSelect::GetFilterType() {
    switch (static_cast<unsigned int>(static_cast<unsigned short>(filter))) {
        case 1:
            return 0;
        case 2:
            return 1;
        case 4:
            return 2;
        case 8:
            return 3;
        case 0x10:
            return 4;
        case 0x20:
            return 5;
        default:
            return 0;
    }
}

void UIQRCarSelect::SetupForPlayer(int player) {
    SelectableCar *car = nullptr;
    for (SelectableCar *sc = FilteredCarsList.GetHead(); sc != FilteredCarsList.EndOfList(); sc = static_cast<SelectableCar *>(reinterpret_cast<bTNode<SelectableCar> *>(sc)->GetNext())) {
        if (ForceCar != 0xFFFFFFFF) {
            if (sc->mHandle == ForceCar) {
                car = sc;
                break;
            }
        } else {
            if (sc->mHandle == ListHandles[GetFilterType()]) {
                car = sc;
                break;
            }
            if (FEDatabase->IsCareerMode()) {
                if (sc->mHandle == FEDatabase->GetCareerSettings()->GetCurrentCar()) {
                    car = sc;
                    break;
                }
            } else {
                if (sc->mHandle == FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb))->GetSelectedCar(iPlayerNum)) {
                    car = sc;
                    break;
                }
            }
        }
    }
    if (FEDatabase->IsCarLotMode() && ForceCar == 0xFFFFFFFF) {
        CarViewer::CancelCarLoad(eCARVIEWER_PLAYER1_CAR);
        car = FilteredCarsList.GetHead();
        originalCar = car->mHandle;
    }
    ForceCar = 0xFFFFFFFF;
    if (car == nullptr && FilteredCarsList.GetHead() != FilteredCarsList.EndOfList()) {
        car = FilteredCarsList.GetHead();
    }
    SetSelectedCar(car, iPlayerNum);
    RefreshHeader();
}

int UIQRCarSelect::GetBonusUnlockText(FECarRecord *fe_car) {
    switch (fe_car->Handle) {
        case 0x965fu ... 0x9666u:
        case 0x13624eu ... 0x136253u:
            return 0x4ef2a115;
        case 0x2cf370f0u:
            return 0xbd8bac94;
        case 0x03a94520u:
            return 0xbd8bac93;
        case 0xcb6aaf2fu:
            return 0xbd8bac91;
        case 0x2cf385b2u:
            return 0xbd8bac92;
    }
    return 0;
}

int UIQRCarSelect::GetBonusUnlockBinNumber(FECarRecord *fe_car) {
    unsigned int handle = fe_car->Handle;
    switch (handle) {
        case 0x965F:
            return 2;
        case 0x9660:
            return 3;
        case 0x9661:
            return 4;
        case 0x9662:
            return 5;
        case 0x9663:
            return 6;
        case 0x9664:
            return 7;
        case 0x9665:
            return 8;
        case 0x9666:
            return 9;
        case 0x13624E:
            return 10;
        case 0x13624F:
            return 11;
        case 0x136250:
            return 12;
        case 0x136251:
            return 13;
        case 0x136252:
            return 14;
        case 0x136253:
            return 15;
        default:
            return -1;
    }
}

void UIQRCarSelect::RefreshHeader() {
    UpdateSliders();

    unsigned int langhash;
    unsigned int texhash;
    unsigned int list = static_cast<unsigned short>(filter);
    switch (list) {
        case 1:
            langhash = LANGUAGE_CARLIST_STOCK;
            texhash = 0x3a541f7f;
            break;
        case 2:
            langhash = LANGUAGE_CARLIST_CAREER;
            texhash = 0xf0bddecd;
            break;
        case 4:
            langhash = LANGUAGE_CARLIST_QUICKRACE;
            texhash = 0x9996ca1e;
            break;
        case 8:
            langhash = LANGUAGE_CARLIST_BONUS;
            texhash = 0xbe5ad8a2;
            break;
        case 0x10:
            langhash = LANGUAGE_CARLIST_PRESET;
            texhash = 0x03704f3d;
            break;
        case 0x20:
            langhash = 0x3ec63978;
            texhash = 0x03704f3d;
            break;
        default:
            langhash = 0;
            texhash = 0;
            break;
    }

    if (FEDatabase->IsCarLotMode() || !FEDatabase->IsCareerMode()) {
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x39dc21f9);
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xe998fe99);
    }

    FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xaa9834bc, langhash);
    FEImage *filterImg = FEngFindImage(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xe3b271b8);
    FEngSetTextureHash(filterImg, texhash);
    FEngSetScript(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xd0f7c7cc, 0x16a259, true);

    if (!pSelectedCar) {
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x7379349b);
    } else if (pSelectedCar->bLocked) {
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x7379349b);
    }

    if (!pSelectedCar) {
        FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x2d25b2c4, LANGUAGE_CS_NO_CARS_IN_LIST);
        cFEng::Get()->QueuePackageMessage(0xd9420cd5, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
        if (filter & 4) {
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x36c1e04d, 0x0da87b01);
        } else {
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x36c1e04d, LANGUAGE_CS_NO_CARS_IN_LIST);
        }
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x0e9ed0a2);
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x18a4384f);
        CarViewer::CancelCarLoad(static_cast<eCarViewerWhichCar>(0));
        GarageMainScreen::GetInstance()->DisableCarRendering();
        cFEng::Get()->QueuePackageMessage(0x913fa282, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
        bLoadingBarActive = false;
        tLastEventTimer = 0;
        return;
    }

    if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
        FEngSetVisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x0e9ed0a2);
        FEngSetVisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x18a4384f);
    }

    FEngSetVisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x7379349b);
    cFEng::Get()->QueuePackageMessage(0x7c4583dc, reinterpret_cast<MenuScreen *>(this)->GetPackageName(), nullptr);
    FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), __NUMBER__, "%d", FilteredCarsList.GetNodeNumber(pSelectedCar));
    FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), __NUMBER_OF__, "%d", FilteredCarsList.CountElements());

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);

    if (pSelectedCar->bLocked) {
        FEngSetScript(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xd0f7c7cc, 0x1ca7c0, true);
        FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
        if (car->MatchesFilter(0xf0008)) {
            int unlockText = GetBonusUnlockText(car);
            if (unlockText == 0x4ef2a115) {
                int binNum = GetBonusUnlockBinNumber(car);
                char rival_name_locdb[128];
                FEngSNPrintf(rival_name_locdb, 0x80, "blacklist_rival_%02d_aka", binNum);
                FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x2d25b2c4, GetLocalizedString(0x4ef2a115),
                         GetLocalizedString(FEHashUpper(rival_name_locdb)), binNum);
            } else {
                FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x2d25b2c4, static_cast<unsigned int>(unlockText));
            }
        } else {
            Attrib::Gen::frontend fe_attrib(car->FEKey, 0, nullptr);
            int rival_num = fe_attrib.UnlockedAt() + 1;
            char rival_name_locdb[128];
            FEngSNPrintf(rival_name_locdb, 0x80, "blacklist_rival_%02d_aka", rival_num);
            FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x2d25b2c4, GetLocalizedString(0x4ef2a115),
                     GetLocalizedString(FEHashUpper(rival_name_locdb)), rival_num);
        }
    }

    FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
    FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), __CARNAME__);
    FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x79d6e45c);
    FEngSetTextureHash(pManuLogo, car->GetManuLogoHash());
    FEngSetTextureHash(pCarBadge, car->GetLogoHash());

    if (FEDatabase->IsCarLotMode()) {
        FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x1930b057, "%$d", FEDatabase->GetCareerSettings()->GetCash());
        FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x20c83c31, "%$d", car->GetCost());
        FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xdc18c4d4, 0xa9950b93);
        FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb94139f4, 0x7010bbf2);
    }

    if (FEDatabase->IsCareerMode() && !FEDatabase->IsCarLotMode()) {
        TheHeatMeter.SetVisibility(true);
    } else {
        TheHeatMeter.SetVisibility(false);
    }

    if (car->IsCareer()) {
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x39dc21f9);
        FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xe998fe99);

        FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);

        if (TheFEMarkerManager.HasMarker(static_cast<FEMarkerManager::ePossibleMarker>(0x14), 0) || (CheatCanAddImpoundBox && !career->TheImpoundData.IsImpounded())) {
            int num_markers = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(0x14), 0);
            FEngSetVisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x39dc21f9);
            FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x5b875870, "%2d", num_markers);
            FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xea8aecd9, "%2d", num_markers);
        } else {
            FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x39dc21f9);
        }

        if (career->TheImpoundData.IsReleasable()) {
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x72e7ea88, 0x9db4df7d);
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x9d974df3, 0x073b79e0);
            unsigned int cost = car->GetReleaseFromImpoundCost();
            FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x322b18f9, "%$0.0f", static_cast<float>(cost));
            FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x7044a5a4, "%$d", FEDatabase->GetCareerSettings()->GetCash());
            FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x0e9ed0a2);
        } else if (career->TheImpoundData.IsImpounded()) {
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x72e7ea88, 0x9db4df7d);
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x9d974df3, 0x073b79e0);
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x322b18f9, 0xaefedad9);
            FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x7044a5a4, "%$d", FEDatabase->GetCareerSettings()->GetCash());
            FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x0e9ed0a2);

            if (TheFEMarkerManager.HasMarker(static_cast<FEMarkerManager::ePossibleMarker>(0x15), 0) || CheatReleaseFromImpoundMarker) {
                int num_markers = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(0x15), 0);
                FEngSetVisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xe998fe99);
                FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xcc59b910, "%2d", num_markers);
                FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0xb8f9938a, "%2d", num_markers);
                FEngSetInvisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x39dc21f9);
            }
        } else {
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x72e7ea88, 0x17574b0e);
            FEngSetLanguageHash(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x9d974df3, 0x915f4d26);

            if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
                FEngSetVisible(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x0e9ed0a2);
            }

            FECareerRecord *record = stable->GetCareerRecordByHandle(car->CareerHandle);
            if (record) {
                FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x322b18f9, "%$d", record->GetBounty());
                FEPrintf(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), 0x7044a5a4, "%$d", record->GetInfractions(true).GetFineValue());
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
    DialogInterface::ShowTwoButtons(reinterpret_cast<MenuScreen *>(this)->GetPackageName(), "", dialog_confirmation, LANGUAGE_COMMON_MANUAL, LANGUAGE_COMMON_AUTO,
                                    0x5f5e3886, 0x1a2826e1, 0x34dc1bcf,
                                    (eDialogFirstButtons)(FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission == 0), LANGUAGE_COMMON_TRANSMISSION);
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

int SortCarsByUnlock(SelectableCar *a, SelectableCar *b) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(gPlayerNum);
    FECarRecord *carA = stable->GetCarRecordByHandle(a->mHandle);
    Attrib::Gen::frontend fe_a(carA->FEKey, 0, nullptr);
    FECarRecord *carB = stable->GetCarRecordByHandle(b->mHandle);
    Attrib::Gen::frontend fe_b(carB->FEKey, 0, nullptr);
    int binA = fe_a.UnlockedAt();
    int binB = fe_b.UnlockedAt();
    return static_cast<int>(binA > binB);
}

bool IsValidMikeMannCar(FECarRecord *fe_car, unsigned int filter) {
    if (GetMikeMannBuild() == 1) {
        return fe_car->GetType() != CARTYPE_CAYMANS;
    }
    if (GetMikeMannBuild() != 2) {
        return true;
    }
    unsigned short lowFilter = static_cast<unsigned short>(filter);
    if (lowFilter == 1) {
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
    if (lowFilter != 0x10) {
        return true;
    }
    return fe_car->Handle == bStringHash("M3GTRCAREERSTART");
}

void UIQRCarSelect::RefreshBonusCarList() {
    bool bCarUnlocked;
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x136253, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x136253, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x136252, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x136252, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x136251, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x136251, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x136250, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x136250, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x13624f, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x13624f, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x13624e, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x13624e, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9666, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x9666, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9665, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x9665, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9664, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x9664, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9663, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x9663, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9662, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x9662, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9661, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x9661, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9660, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x9660, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x965f, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x965f, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x3a94520, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x3a94520, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0xcb6aaf2f, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0xcb6aaf2f, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x2cf370f0, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x2cf370f0, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x2cf385b2, iPlayerNum);
    FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x2cf385b2, !bCarUnlocked));
    if (UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x34498eb2, iPlayerNum)) {
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x34498eb2, false));
    }
    if (GetIsCollectorsEdition()) {
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x634d1bd2, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0xe1075862, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x02d642b8, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x03d3401a, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x03d8a6d1, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x54653c71, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0xe115ead0, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x54655133, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x582f21d9, false));
        FilteredCarsList.AddTail(new ("SelectableCar", 0) SelectableCar(0x363a1fea, false));
    }
    SetupForPlayer(iPlayerNum);
}

void UIQRCarSelect::RefreshCarList() {
    ClearCarList();
    if ((filter & 8) != 0) {
        RefreshBonusCarList();
        return;
    }
    FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(iPlayerNum);
    unsigned int unlockFilter;
    if (FEDatabase->IsCareerMode()) {
        unlockFilter = 2;
    } else if (FEDatabase->IsQuickRaceMode()) {
        unlockFilter = 1;
    } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        unlockFilter = 7;
    } else {
        unlockFilter = 1;
    }
    int i = 0;
    do {
        FECarRecord *car = carDB->GetCarByIndex(i);
        if (car->IsValid() && car->MatchesFilter(filter)) {
            bool unlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlockFilter), car->Handle, iPlayerNum);
            if (!GetMikeMannBuild() || IsValidMikeMannCar(car, filter)) {
                SelectableCar *newCar = new ("SelectableCar", 0) SelectableCar(car->Handle, !unlocked);
                FilteredCarsList.AddTail(newCar);
            }
        }
        i++;
    } while (i < 200);
    unsigned int f = static_cast<unsigned short>(filter);
    switch (f) {
        case 1:
        case 2:
        case 8:
        case 0x10:
            FilteredCarsList.Sort(SortCarsByUnlock);
            break;
    }
    SetupForPlayer(iPlayerNum);
}

void UIQRCarSelect::ClearCarList() {
    FilteredCarsList.DeleteAllElements();
}

void UIQRCarSelect::ScrollCars(eScrollDir dir) {
    SelectableCar *cur = pSelectedCar;
    if (!cur)
        return;
    SelectableCar *newCar = static_cast<SelectableCar *>(reinterpret_cast<bTNode<SelectableCar> *>(cur)->GetPrev());
    if (newCar == FilteredCarsList.EndOfList()) {
        newCar = FilteredCarsList.GetTail();
    }
    if (dir == eSD_PREV) {
    } else if (dir == eSD_NEXT) {
        newCar = static_cast<SelectableCar *>(reinterpret_cast<bTNode<SelectableCar> *>(cur)->GetNext());
        if (newCar == FilteredCarsList.EndOfList()) {
            newCar = FilteredCarsList.GetHead();
        }
    }
    if (newCar != cur) {
        SetSelectedCar(newCar, iPlayerNum);
        RefreshHeader();
    }
}

// UNSOLVED
void UIQRCarSelect::ScrollLists(eScrollDir dir) {
    if (FEDatabase->IsCareerMode()) {
        return;
    }
    if (FEDatabase->IsCustomizeMode()) {
        return;
    }

    uint32 region = this->filter & 0xffff0000;
    uint32 list = this->filter & 0xffff;

    if (GetMikeMannBuild() == 2) {
        if (list == 1) {
            list = 4;
        } else if (list == 4) {
            list = 16;
        } else if (list == 16) {
            list = 1;
        }
    } else if (dir == eSD_PREV) {
        if (list == 1) {
            list = 8;
        } else {
            list = list / 2;
        }
    } else if (dir == eSD_NEXT) {
        if (list == 8) {
            list = 1;
        } else {
            list = list << 1;
        }
    }

    filter = region | list;

    RefreshCarList();
    RefreshHeader();
}

void UIQRCarSelect::OnlineActOnSelect() {
    FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(1))->SetSelectedCar(pSelectedCar->mHandle, 0);
    FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0))->SetSelectedCar(pSelectedCar->mHandle, 0);
    FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(2))->SetSelectedCar(pSelectedCar->mHandle, 0);
}
