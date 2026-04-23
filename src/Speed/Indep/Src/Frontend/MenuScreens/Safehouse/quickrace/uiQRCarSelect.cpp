#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRCarSelect.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"

extern void eLoadStreamingTexture(unsigned int *textures, int count, void (*callback)(unsigned int), void *user, int priority);
extern void eUnloadStreamingTexture(unsigned int *textures, int count);
extern int GetMikeMannBuild();
extern FEMarkerManager TheFEMarkerManager;
extern int CheatImpounded;
extern int CheatBustedCount;
extern int CheatMaxBusted;
extern int CheatReleasable;
extern int CheatCanAddImpoundBox;
extern int CheatReleaseFromImpoundMarker;
extern int g_MaximumMaximumTimesBusted;
extern int gPlayerNum;

extern void LoadOneTexture(const char *pkg_name, unsigned int hash, void (*callback)(unsigned int), unsigned int param);
extern bool GetIsCollectorsEdition();
extern FEImage *FEngFindImage(const char *pkg, int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern void FEngSetScript(const char *pkg, unsigned int obj_hash, unsigned int script_hash, bool p);
extern bool FEngIsScriptSet(const char *pkg, unsigned int obj_hash, unsigned int script_hash);
extern void FEngSetLanguageHash(const char *pkg, unsigned int obj_hash, unsigned int lang_hash);
extern unsigned int FEngHashString(const char *fmt, ...);

extern Timer RealTimer;
extern int Showcase_FromIndex;
extern const char *Showcase_FromPackage;
extern unsigned int Showcase_FromArgs;
extern int Showcase_FromFilter;

extern int bSNPrintf(char *buf, int size, const char *fmt, ...);
extern const char *GetLocalizedString(unsigned int hash);
extern unsigned long FEHashUpper(const char *str);
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
extern void FEngSetInvisible(FEObject *obj);

void MemcardEnter(const char *from, const char *to, unsigned int op, void (*pTermFunc)(void *), void *pTermFuncParam, unsigned int msgSuccess,
                  unsigned int msgFailed);

void RaceStarterStartCareerFreeRoam() asm("StartCareerFreeRoam__11RaceStarter");
void RaceStarterStartRace() asm("StartRace__11RaceStarter");

unsigned int UIQRCarSelect::ForceCar;
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
        unsigned int hash = ImpoundStampHash;
        eUnloadStreamingTexture(&hash, 1);
        ImpoundStampHash = 0;
    }
}

bool QRCarSelectBustedManager::IsImpoundInfoVisible() {
    unsigned int mode = FEDatabase->GetGameMode();
    if (!(mode & 1))
        return false;
    return !(mode & 0x8000);
}

bool QRCarSelectBustedManager::ShowImpoundedTexture() {
    return WorkingCareerRecord->TheImpoundData.IsImpounded();
}

void QRCarSelectBustedManager::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
        case 0x8defa48b:
            TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0);
            WorkingCareerRecord->TheImpoundData.AddMaxBusted();
            break;
        case 0xa0fc39f9:
            WorkingCareerRecord->TheImpoundData.NotifyPlayerPaidToRelease();
            WorkingCareerRecord->SetVehicleHeat(0.0f);
            {
                unsigned int cost = WorkingCarRecord->GetReleaseFromImpoundCost();
                FEDatabase->GetCareerSettings()->SpendCash(static_cast<int>(static_cast<float>(cost)));
            }
            break;
        case 0xe845bc1c:
            WorkingCareerRecord->TheImpoundData.NotifyPlayerUsedMarkerToRelease();
            WorkingCareerRecord->SetVehicleHeat(0.0f);
            TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);
            break;
        case 0x3fdc64c1:
            FEManager::Get()->SetGarageType(static_cast<eGarageType>(1));
            FEDatabase->ClearGameMode(static_cast<eFEGameModes>(1));
            FEDatabase->SetGameMode(static_cast<eFEGameModes>(0x100));
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            return;
        case 0xe0b38195:
            if (!CalcGameOver())
                return;
            FEDatabase->GetCareerSettings()->SetGameOver();
            DialogInterface::ShowOneButton(ParentPkg, "GameOver.fng", static_cast<eDialogTitle>(1), 0x417b2601, 0x3fdc64c1, 0x164bed94);
            return;
        default:
            return;
    }
    RefreshHeader();
}

void QRCarSelectBustedManager::TextureLoadedCallback() {
    if (!ShowImpoundedTexture()) {
        FEngSetScript(ParentPkg, 0xbc7b91f, 0x16a259, true);
    } else {
        FEImage *img1 = FEngFindImage(ParentPkg, 0xce18427d);
        FEngSetTextureHash(img1, ImpoundStampHash);
        FEImage *img2 = FEngFindImage(ParentPkg, 0x5b8f2a45);
        FEngSetTextureHash(img2, ImpoundStampHash);
        if (Flags == BUSTED_ANIM_SHOW_IMPOUNDED) {
            FEngSetScript(ParentPkg, 0xbc7b91f, 0x5a8e4ebe, true);
            Flags = BUSTED_ANIM_NOTHING;
        } else {
            FEngSetScript(ParentPkg, 0xbc7b91f, 0x6ebbfb68, true);
        }
    }
}

void QRCarSelectBustedManager::LoadImpoundTexture() {
    int lang = GetCurrentLanguage();
    unsigned int hash;
    switch (lang) {
        case 1:
            hash = 0xce184740;
            break;
        case 2:
            hash = 0xce1849e1;
            break;
        case 3:
            hash = 0xce185441;
            break;
        case 4:
            hash = 0xce187e47;
            break;
        case 5:
            hash = 0xce183f30;
            break;
        case 6:
            hash = 0xce187f32;
            break;
        case 7:
            hash = 0xce183c96;
            break;
        case 8:
            hash = 0xce185c2f;
            break;
        case 9:
            hash = 0xce183937;
            break;
        case 10:
            hash = 0xce18561e;
            break;
        case 11:
            hash = 0xce188180;
            break;
        case 12:
            hash = 0xce18716e;
            break;
        case 13:
            hash = 0xce184620;
            break;
        default:
            hash = 0xce18427d;
            break;
    }
    ImpoundStampHash = hash;
    unsigned int texArray[1];
    texArray[0] = ImpoundStampHash;
    eLoadStreamingTexture(texArray, 1, TextureLoadedCallbackAccessor, reinterpret_cast<void *>(this), 0);
}

void QRCarSelectBustedManager::SetSelectedCar(FECarRecord *record) {
    WorkingCarRecord = record;
    FECareerRecord *career = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(record->CareerHandle);
    WorkingCareerRecord = career;
    if (CheatImpounded != 0) {
        career->TheImpoundData.MaxBusted = static_cast<unsigned char>(CheatMaxBusted);
        career->TheImpoundData.TimesBusted = static_cast<char>(CheatMaxBusted);
        career->TheImpoundData.BecomeImpounded(FEImpoundData::IMPOUND_REASON_STRIKE_LIMIT_REACHED);
        Flags = BUSTED_ANIM_SHOW_IMPOUNDED;
    } else if (CheatBustedCount != 0) {
        career->TheImpoundData.TimesBusted = static_cast<char>(CheatBustedCount);
        career->TheImpoundData.MaxBusted = static_cast<unsigned char>(CheatMaxBusted);
        Flags = BUSTED_ANIM_SHOW_STRIKE;
    }
    if (CheatReleasable != 0) {
        bool released;
        do {
            released = career->TheImpoundData.NotifyWin();
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
            FEngSetScript(ParentPkg, 0xbc7b91f, 0x6ebbfb68, true);
            FEngSetScript(ParentPkg, 0x64f3a49c, 0x5079c8f8, true);
            FEImage *img1 = FEngFindImage(ParentPkg, 0xce18427d);
            FEngSetTextureHash(img1, ImpoundStampHash);
            FEImage *img2 = FEngFindImage(ParentPkg, 0x5b8f2a45);
            FEngSetTextureHash(img2, ImpoundStampHash);
        }
        unsigned int cost = WorkingCarRecord->GetReleaseFromImpoundCost();
        int playerCash = *reinterpret_cast<int *>(reinterpret_cast<char *>(FEDatabase->GetUserProfile(0)) + 0xf0);
        bool canAffordRelease = playerCash >= static_cast<int>(static_cast<float>(cost));
        bool hasMarkers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0) > 0;
        if (WorkingCareerRecord->TheImpoundData.ImpoundedState == 4 && canAffordRelease) {
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
        if (WorkingCareerRecord->TheImpoundData.MaxBusted == 4) {
            posIndex = 2;
            script2 = 0x1ca7c0;
        } else if (WorkingCareerRecord->TheImpoundData.MaxBusted == 5) {
            posIndex = 3;
            script2 = 0x1ca7c0;
            script1 = 0x1ca7c0;
        }
        FEngSetScript(ParentPkg, 0x5bc78037, script2, true);
        FEngSetScript(ParentPkg, 0x48095518, script1, true);
        FEngSetScript(ParentPkg, 0xf9a5ce86, FEngHashString("POS%d", posIndex), true);
        FEngSetScript(ParentPkg, 0xebf0016e, FEngHashString("POS%d", posIndex), true);
        if (Flags == BUSTED_ANIM_SHOW_STRIKE) {
            FEngSetScript(ParentPkg, FEngHashString("IMPOUND_STATE_%d", static_cast<int>(WorkingCareerRecord->TheImpoundData.TimesBusted)),
                          0x5a8e4ebe, true);
            Flags = BUSTED_ANIM_NOTHING;
        }
        for (int i = 1; i <= static_cast<int>(static_cast<unsigned char>(WorkingCareerRecord->TheImpoundData.MaxBusted)); i++) {
            if (WorkingCareerRecord->TheImpoundData.TimesBusted >= i) {
                if (!FEngIsScriptSet(ParentPkg, FEngHashString("IMPOUND_STATE_%d", i), 0x5a8e4ebe)) {
                    FEngSetScript(ParentPkg, FEngHashString("IMPOUND_STATE_%d", i), 0x1ca7c0, true);
                }
            } else {
                FEngSetScript(ParentPkg, FEngHashString("IMPOUND_STATE_%d", i), 0x16a259, true);
            }
        }
    } else {
        signed char impState = WorkingCareerRecord->TheImpoundData.ImpoundedState;
        if (impState == 4) {
            FEngSetInvisible(FEngFindObject(ParentPkg, 0x75721326));
        } else if (impState != 0) {
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
    FECareerRecord *career = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(WorkingCarRecord->CareerHandle);
    unsigned int cost = WorkingCarRecord->GetReleaseFromImpoundCost();
    int cash = FEDatabase->GetCareerSettings()->GetCash();
    int numMarkers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);

    if (career->TheImpoundData.ImpoundedState == 4 && static_cast<int>(cost) <= cash) {
        if (numMarkers > 0) {
            DialogInterface::ShowThreeButtons(ParentPkg, "", static_cast<eDialogTitle>(3), 0xf9c73cc2, 0x4eb9591f, 0x1a294dad, 0xe845bc1c, 0xa0fc39f9,
                                              0x5ee58948, 0x5ee58948, static_cast<eDialogFirstButtons>(1), 0xb715ae8f, cost);
        } else {
            DialogInterface::ShowTwoButtons(ParentPkg, "", static_cast<eDialogTitle>(3), 0xf9c73cc2, 0x4eb9591f, 0x1a294dad, 0xa0fc39f9, 0xcad5722e,
                                            static_cast<eDialogFirstButtons>(1), 0xb715ae8f, cost);
        }
    } else {
        if (numMarkers > 0) {
            DialogInterface::ShowTwoButtons(ParentPkg, "", static_cast<eDialogTitle>(3), 0x417b2601, 0x1a294dad, 0xe845bc1c, 0xcad5722e, 0x5ee58948,
                                            static_cast<eDialogFirstButtons>(1), 0xb715ae8f, cost);
        } else {
            DialogInterface::ShowOneButton(ParentPkg, "", static_cast<eDialogTitle>(1), 0x417b2601, 0x5ee58948, 0xe96fa0c5);
        }
    }
}

void QRCarSelectBustedManager::MaybeAddImpoundBox() {
    bool canAdd = false;
    if (WorkingCareerRecord->TheImpoundData.CanAddMaxBusted()) {
        int numMarkers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0);
        if (numMarkers > 0)
            canAdd = true;
    }
    bool showDialog = canAdd || CheatCanAddImpoundBox != 0;
    if (showDialog) {
        DialogInterface::ShowTwoButtons(ParentPkg, "", static_cast<eDialogTitle>(3), 0x417b2601, 0x1a294dad, 0x8defa48b, 0xb4edeb6d, 0xb4edeb6d,
                                        static_cast<eDialogFirstButtons>(1), 0xcebda20);
    } else if (g_MaximumMaximumTimesBusted <= static_cast<int>(WorkingCareerRecord->TheImpoundData.MaxBusted)) {
        DialogInterface::ShowOneButton(ParentPkg, "", static_cast<eDialogTitle>(2), 0x417b2601, 0xb4edeb6d, 0xbcae8539);
    }
}

UIQRCarSelect::UIQRCarSelect(ScreenConstructorData *sd)
    : MenuScreen(sd) //
      ,
      TheBustedManager(GetPackageName(), sd->Arg >> 8) {
    FilteredCarsList.InitList();
    originalCar = 0xFFFFFFFF;
    tLastEventTimer = 0;
    pManuLogo = nullptr;
    pCarBadge = nullptr;
    pCarName = nullptr;
    pCarNameShadow = nullptr;
    pFilter = nullptr;
    tLastEventTimer = 0;
    bLoadingBarActive = false;
    bShowcaseMode = false;
    iPlayerNum = sd->Arg & 0xFF;
    filter = 0;
    iPrevButtonMsg = 0;

    pManuLogo = FEngFindImage(GetPackageName(), 0x3e01ad1d);
    pCarBadge = FEngFindImage(GetPackageName(), 0xb05dd708);
    pCarName = FEngFindString(GetPackageName(), 0xd6d32016);
    pCarNameShadow = FEngFindString(GetPackageName(), 0x79d6e45c);
    pFilter = FEngFindString(GetPackageName(), 0x5ba2f765);

    bool isSplit = false;
    if (FEDatabase->IsSplitScreenMode()) {
        isSplit = FEDatabase->GetGameMode() == 2;
    }
    if (isSplit) {
        gPlayerNum = iPlayerNum;
        if (gPlayerNum == 0) {
            FEngSetLanguageHash(GetPackageName(), 0xe3fe27fe, 0x7b070984);
            FEDatabase->DeleteMultiplayerProfile(1);
        } else {
            gPlayerNum = 1;
            FEDatabase->CreateMultiplayerProfile(1);
            FEngSetLanguageHash(GetPackageName(), 0xe3fe27fe, 0x7b070985);
        }
    } else {
        gPlayerNum = 0;
    }

    if (!FEDatabase->IsCarLotMode()) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x19398802));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x1930b057));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x20d113dc));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x20c83c31));
    }

    if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xe9ed0a2));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x18a4384f));
    }

    if (TheBustedManager.IsImpoundInfoVisible()) {
        TheBustedManager.LoadImpoundTexture();
    }

    InitStatsSliders();
    Setup();
}

UIQRCarSelect::~UIQRCarSelect() {}

bool UIQRCarSelect::IsCarImpounded(unsigned int handle) {
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(handle);
    if (!car)
        return false;
    FECareerRecord *career = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(car->CareerHandle);
    if (!career)
        return false;
    return career->TheImpoundData.IsImpounded();
}

void UIQRCarSelect::CommitChangeStartRace(bool allowError) {
    FEManager::Get()->AllowControllerError(allowError);
    FEDatabase->DeleteMultiplayerProfile(1);
    RaceStarterStartRace();
}

void UIQRCarSelect::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
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
            bool isSplitScreen = false;
            if ((FEDatabase->GetGameMode() & 4) != 0) {
                isSplitScreen = FEDatabase->iNumPlayers == 2;
            }
            if (!isSplitScreen)
                return;
            cFEng::Get()->QueuePackageMessage(0x841d518a, GetPackageName(), nullptr);
            RefreshHeader();
            return;
        }
        case 0xc98356ba: {
            GarageMainScreen::GetInstance();
            if (GarageMainScreen::GetInstance()->IsCarRendering() && bLoadingBarActive) {
                cFEng::Get()->QueuePackageMessage(0x913fa282, GetPackageName(), nullptr);
                bLoadingBarActive = false;
            }
            if (!tLastEventTimer.IsSet())
                return;
            float elapsed = (RealTimer - tLastEventTimer).GetSeconds();
            if (elapsed < 0.5f)
                return;
            {
                RideInfo ride;
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                stable->BuildRideForPlayer(pSelectedCar->mHandle, iPlayerNum, &ride);
                SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
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
            if ((flags & 1) != 0 && (flags & 0x8000) == 0) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                FECarRecord *car = GetSelectedCarRecord();
                FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);
                if (career && career->TheImpoundData.IsImpounded())
                    return;
            }
            cFEng::Get()->QueuePackageMessage(0x89d0649c, GetPackageName(), nullptr);
            bShowcaseMode = true;
            return;
        }
        case 0xa46253ba: {
            FECarRecord *car = GetSelectedCarRecord();
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            unsigned int cost = car->GetCost();
            FEDatabase->GetCareerSettings()->AddCash(static_cast<int>(cost >> 1));
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
                    DialogInterface::ShowOneButton(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x417b2601, 0x34dc1bcf, 0x80e4f27c);
                    return;
                }
            }
            FEPlayerCarDB *stable2 = FEDatabase->GetPlayerCarStable(iPlayerNum);
            if (stable2->GetNumAvailableCareerCars() < 2) {
                DialogInterface::ShowOneButton(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x417b2601, 0x34dc1bcf, 0x9a772bd6);
                return;
            }
            unsigned int cost = car->GetCost();
            char buf[512];
            char cost_str[16];
            bSNPrintf(cost_str, 0x10, "%d", cost >> 1);
            bSNPrintf(buf, 0x200, GetLocalizedString(0xb4a40135), cost_str);
            DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x70e01038, 0x417b25e4, 0xa46253ba, 0x34dc1bcf,
                                            0x34dc1bcf, static_cast<eDialogFirstButtons>(1), buf);
            return;
        }
        case 0xc519bfc3: {
            bool isSplitScreen = false;
            if ((FEDatabase->GetGameMode() & 4) != 0) {
                isSplitScreen = FEDatabase->iNumPlayers == 2;
            }
            if (isSplitScreen) {
                unsigned int op = 0x411;
                if (iPlayerNum == 1) {
                    op = 0x20411;
                }
                MemcardEnter(GetPackageName(), GetPackageName(), op, nullptr, nullptr, 0x7e998e5e, 0x8867412d);
                return;
            }
            if (!TheBustedManager.IsImpoundInfoVisible())
                return;
            TheBustedManager.MaybeAddImpoundBox();
            return;
        }
        case 0xe1fde1d1: {
            if (bShowcaseMode) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
                Showcase_FromPackage = GetPackageName();
                ForceCar = pSelectedCar->mHandle;
                Showcase_FromArgs = iPlayerNum;
                cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(car), 0, false);
                return;
            }
            ForceCar = 0xffffffff;
            if (iPrevButtonMsg == 0x406415e3) {
                unsigned int flags = FEDatabase->GetGameMode();
                if ((flags & 8) != 0 || (flags & 0x40) != 0) {
                    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                    settings->SelectedCar[iPlayerNum] = originalCar;
                    cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
                    return;
                }
                if ((flags & 1) != 0) {
                    if ((flags & 0x8000) == 0) {
                        cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                        return;
                    }
                    if (FEDatabase->iNumPlayers != 2) {
                        if (FEDatabase->GetPlayerSettings(iPlayerNum)->TransmissionPromptOn != 0) {
                            ChooseTransmission();
                            return;
                        }
                    }
                    CommitChangeStartRace(true);
                    return;
                } else {
                    if ((flags & 0x20) != 0) {
                        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                        FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
                        if (!car->IsCustomized()) {
                            FECarRecord *new_car = FEDatabase->GetPlayerCarStable(iPlayerNum)->CreateNewCustomCar(car->Handle);
                            car = new_car;
                        }
                        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                        settings->SelectedCar[iPlayerNum] = car->Handle;
                        cFEng::Get()->QueuePackageSwitch("MyCarsManager.fng", 0, 0, false);
                        return;
                    }
                    if (FEDatabase->iNumPlayers == 2 || FEDatabase->GetPlayerSettings(iPlayerNum)->TransmissionPromptOn == 0) {
                        bool isSplitScreen = false;
                        if ((FEDatabase->GetGameMode() & 4) != 0) {
                            isSplitScreen = FEDatabase->iNumPlayers == 2;
                        }
                        if (isSplitScreen && iPlayerNum != 1) {
                            return;
                        }
                        CommitChangeStartRace(true);
                        return;
                    }
                    ChooseTransmission();
                    return;
                }
            } else if (iPrevButtonMsg == 0x911ab364) {
                int iVar3 = -1;
                unsigned int flags = FEDatabase->GetGameMode();
                if ((flags & 1) != 0) {
                    if ((flags & 0x8000) != 0) {
                        RaceStarterStartCareerFreeRoam();
                        return;
                    }
                    if (IsCarImpounded(originalCar) == 0) {
                        FEDatabase->GetCareerSettings()->SetCurrentCar(originalCar);
                    }
                    cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                } else if ((flags & 8) != 0 || (flags & 0x40) != 0) {
                    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                    settings->SelectedCar[iPlayerNum] = originalCar;
                    cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
                } else if ((flags & 0x20) != 0) {
                    cFEng::Get()->QueuePackageSwitch("MyCarsManager.fng", 0, 0, false);
                } else {
                    bool isSplitScreen = false;
                    if ((flags & 4) != 0) {
                        isSplitScreen = FEDatabase->iNumPlayers == 2;
                    }
                    if (isSplitScreen) {
                        bool returnToPressStart = iPlayerNum != 1;
                        if (returnToPressStart) {
                            FEManager::Get()->SetGarageType(GARAGETYPE_NONE);
                        } else {
                            FEDatabase->SetPlayersJoystickPort(1, -1);
                        }
                        returnToPressStart = !returnToPressStart;
                        cFEng::Get()->QueuePackageSwitch("PressStart.fng", returnToPressStart, 0xff, false);
                    } else {
                        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                        settings->SelectedCar[iPlayerNum] = originalCar;
                        iVar3 = originalCar;
                        FEManager::Get()->SetGarageType(GARAGETYPE_NONE);
                        cFEng::Get()->QueuePackageSwitch("Track_Options.fng", 0, 0, false);
                    }
                }
                if (iVar3 != -1) {
                    RideInfo ride;
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                    stable->BuildRideForPlayer(iVar3, iPlayerNum, &ride);
                    SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
                }
                return;
            }
            ForceCar = 0xffffffff;
            return;
        }
        case 0xd05fc3a3: {
            FECarRecord *car = GetSelectedCarRecord();
            FECarRecord *new_car = FEDatabase->GetPlayerCarStable(0)->CreateNewCareerCar(car->Handle);
            if (new_car) {
                FEDatabase->GetCareerSettings()->SpendCash(new_car->GetCost());
                FEDatabase->GetCareerSettings()->SetCurrentCar(new_car->Handle);
            }
            RaceStarterStartCareerFreeRoam();
            return;
        }
        case 0xb1ee867d: {
            FECarRecord *car = GetSelectedCarRecord();
            FECarRecord *new_car = FEDatabase->GetPlayerCarStable(0)->CreateNewCareerCar(car->Handle);
            if (new_car) {
                FEDatabase->GetCareerSettings()->SpendCash(new_car->GetCost());
            }
            RaceStarterStartCareerFreeRoam();
            return;
        }
        case 0x0c407210:
        case 0x406415e3: {
            if (!pSelectedCar)
                return;
            if (pSelectedCar->bLocked != 0)
                return;

            float elapsed = (RealTimer - tLastEventTimer).GetSeconds();
            if (elapsed < 0.5f)
                return;

            unsigned int flags = FEDatabase->GetGameMode();
            if ((flags & 8) != 0 || (flags & 0x40) != 0) {
                OnlineActOnSelect();
                iPrevButtonMsg = 0x406415e3;
                ChooseTransmission();
                return;
            }
            if ((flags & 1) != 0) {
                if ((flags & 0x8000) != 0) {
                    if (MemoryCard::GetInstance()->m_bListingOldSaveFiles)
                        return;
                    unsigned int cost = GetSelectedCarRecord()->GetCost();
                    if (cost > static_cast<unsigned int>(FEDatabase->GetCareerSettings()->GetCash())) {
                        DialogInterface::ShowOneButton(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x417b2601, 0x34dc1bcf, 0x40fa955d);
                        return;
                    }
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                    if (stable->GetNumPurchasedCars() > 9) {
                        DialogInterface::ShowOneButton(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x417b2601, 0x34dc1bcf, 0x41030a1b);
                        return;
                    }
                    if (FEDatabase->GetCareerSettings()->GetCurrentBin() > 15) {
                        DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x70e01038, 0x417b25e4, 0xd05fc3a3,
                                                        0x34dc1bcf, 0x34dc1bcf, static_cast<eDialogFirstButtons>(1), 0x74317cbc);
                        return;
                    }
                    DialogInterface::ShowThreeButtons(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x5b9d89d0, 0x889d822e, 0x1a294dad,
                                                      0xd05fc3a3, 0xb1ee867d, 0x34dc1bcf, 0x34dc1bcf, static_cast<eDialogFirstButtons>(2),
                                                      0x8c451eba);
                    return;
                }
                FEPlayerCarDB *stable2 = FEDatabase->GetPlayerCarStable(0);
                FECarRecord *car = stable2->GetCarRecordByHandle(pSelectedCar->mHandle);
                FECareerRecord *career = stable2->GetCareerRecordByHandle(car->CareerHandle);
                if (career->TheImpoundData.IsImpounded() || career->TheImpoundData.ImpoundedState == FEImpoundData::IMPOUND_RELEASED) {
                    TheBustedManager.MaybeReleaseCar();
                    return;
                }
                iPrevButtonMsg = 0x406415e3;
                cFEng::Get()->QueuePackageMessage(0x2e76edfb, GetPackageName(), nullptr);
                return;
            }
            if ((flags & 0x20) != 0) {
                iPrevButtonMsg = 0x406415e3;
                cFEng::Get()->QueuePackageMessage(0x2e76edfb, GetPackageName(), nullptr);
                return;
            }
            if (FEDatabase->iNumPlayers != 2) {
                if (FEDatabase->GetPlayerSettings(iPlayerNum)->TransmissionPromptOn != 0) {
                    ChooseTransmission();
                    return;
                }
            }
            char port = FEngMapJoyParamToJoyport(param2);
            FEDatabase->SetPlayersJoystickPort(iPlayerNum, port);
            bool isSplitScreen = false;
            if ((flags & 4) != 0) {
                isSplitScreen = FEDatabase->iNumPlayers == 2;
            }
            if (!isSplitScreen || iPlayerNum != 0) {
                iPrevButtonMsg = 0x406415e3;
                cFEng::Get()->QueuePackageMessage(0x2e76edfb, GetPackageName(), nullptr);
                return;
            }
            cFEng::Get()->QueuePackageSwitch("PressStart.fng", true, 0xff, false);
            return;
        }
        case 0x911ab364: {
            bool bShouldProceed = true;
            unsigned int flags = FEDatabase->GetGameMode();
            if ((flags & 1) == 0) {
                if ((flags & 8) != 0 || (flags & 0x40) != 0) {
                    RideInfo ride;
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                    stable->BuildRideForPlayer(originalCar, iPlayerNum, &ride);
                    SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
                }
            } else if ((flags & 0x8000) == 0) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                FECarRecord *car = stable->GetCarRecordByHandle(originalCar);
                FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);
                if (career->TheImpoundData.IsImpounded() || career->TheImpoundData.ImpoundedState == FEImpoundData::IMPOUND_RELEASED) {
                    DialogInterface::ShowOk(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x630931b6);
                    bShouldProceed = false;
                }
            } else {
                bShouldProceed = FEDatabase->GetCareerSettings()->GetCurrentBin() < 16;
            }
            if (!bShouldProceed)
                return;
            iPrevButtonMsg = 0x911ab364;
            cFEng::Get()->QueuePackageMessage(0x93e8a57c, GetPackageName(), nullptr);
            return;
        }
        case 0x1fab5998: {
            if (FEDatabase->IsCareerMode()) {
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
                unsigned int handle = pSelectedCar->mHandle;
                FECarRecord *car = stable->GetCarRecordByHandle(handle);
                FEDatabase->GetCareerSettings()->SetCurrentCar(car->Handle);
                FEManager::Get()->SetGarageType(GARAGETYPE_NONE);
                return;
            }
            bool isSplitScreen = false;
            if ((FEDatabase->GetGameMode() & 4) != 0) {
                isSplitScreen = FEDatabase->iNumPlayers == 2;
            }
            if (isSplitScreen)
                return;
            FEManager::Get()->SetGarageType(GARAGETYPE_NONE);
            return;
        }
        case 0x1a2826e1: {
            char port = FEngMapJoyParamToJoyport(param2);
            FEDatabase->SetPlayersJoystickPort(iPlayerNum, port);
            FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission = 0;
            bool isSplitScreen = false;
            if ((FEDatabase->GetGameMode() & 4) != 0) {
                isSplitScreen = FEDatabase->iNumPlayers == 2;
            }
            if (isSplitScreen && iPlayerNum == 0) {
                cFEng::Get()->QueuePackageSwitch("PressStart.fng", true, 0xff, false);
                return;
            }
            CommitChangeStartRace(false);
            return;
        }
        case 0x5f5e3886: {
            char port = FEngMapJoyParamToJoyport(param2);
            FEDatabase->SetPlayersJoystickPort(iPlayerNum, port);
            FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission = 1;
            bool isSplitScreen = false;
            if ((FEDatabase->GetGameMode() & 4) != 0) {
                isSplitScreen = FEDatabase->iNumPlayers == 2;
            }
            if (isSplitScreen && iPlayerNum == 0) {
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
            cFEng::Get()->QueuePackageMessage(FEHashUpper("ENABLE_INPUTS"), GetPackageName(), nullptr);
            return;
        case 0x8defa48b:
        case 0xa0fc39f9:
        case 0xe845bc1c:
            RefreshHeader();
            return;
    }
}

eMenuSoundTriggers UIQRCarSelect::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (msg == 0x72619778 || msg == 0x911c0a4b || msg == 0xb205316c || msg == 0x480df13f) {
        unsigned int mode = FEDatabase->GetGameMode();
        if ((mode & 0x20) || (mode & 0x8000) || (mode & 1)) {
            return static_cast<eMenuSoundTriggers>(-1);
        }
    }
    return MenuScreen::NotifySoundMessage(msg, maybe);
}

void UIQRCarSelect::Setup() {
    unsigned int mode = FEDatabase->GetGameMode();
    unsigned int pkgMsg;

    if ((mode & 0x8000) != 0) {
        if (FEDatabase->GetCareerSettings()->GetCurrentBin() > 15) {
            pkgMsg = 0x3a12d2f5;
        } else {
            pkgMsg = 0x5415e304;
        }
        goto queue_msg;
    }

    if ((mode & 4) != 0) {
        bool isTwoPlayer = false;
        if ((mode & 4) != 0) {
            isTwoPlayer = FEDatabase->iNumPlayers == 2;
        }
        if (isTwoPlayer) {
            pkgMsg = 0x2cf6c390;
        } else {
            pkgMsg = 0xde511657;
        }
        goto queue_msg;
    }
    if ((mode & 8) != 0) {
    online_lan:
        pkgMsg = 0x70fbb1e4;
        goto queue_msg;
    }
    if ((mode & 0x40) != 0)
        goto online_lan;
    if ((mode & 0x20) != 0) {
        pkgMsg = 0xa936c3a2;
        goto queue_msg;
    }
    if ((mode & 1) != 0) {
        cFEng::Get()->QueuePackageMessage(0x5415c3f1, GetPackageName(), nullptr);
    }
    goto after_queue;

queue_msg:
    cFEng::Get()->QueuePackageMessage(pkgMsg, GetPackageName(), nullptr);

after_queue:

    FEngSetInvisible(FEngFindObject(GetPackageName(), 0x64f6d21f));

    if ((FEDatabase->GetGameMode() & 1) != 0) {
        originalCar = FEDatabase->GetCareerSettings()->GetCurrentCar();
        if ((FEDatabase->GetGameMode() & 0x8000) != 0) {
            filter = 0xf0001;
            UserProfile *profile = FEDatabase->GetUserProfile(0);
            if ((profile->GetCareer()->SpecialFlags & 2) == 0) {
                cFEng::Get()->QueuePackageMessage(FEHashUpper("DISABLE_INPUTS"), GetPackageName(), nullptr);
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
                if (stable) {
                    FECarRecord *car = stable->GetCarRecordByHandle(originalCar);
                    if (car) {
                        filter = (car->FilterBits & 0x3f) | 0xf0000;
                        goto init_list_handles;
                    }
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
    AccelerationSlider.Init(GetPackageName(), "AccelSlider", 0.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    TopSpeedSlider.Init(GetPackageName(), "TopSpeedSlider", 0.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    HandlingSlider.Init(GetPackageName(), "HandlingSlider", 0.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f);
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
    float acc_preview = bMin(bMax(perf2.Acceleration, AccelerationSlider.GetMin()), AccelerationSlider.GetMax());
    AccelerationSlider.SetPreviewValue(acc_preview);
    AccelerationSlider.Draw();

    TopSpeedSlider.SetValue(perf1.TopSpeed);
    float top_preview = bMin(bMax(perf2.TopSpeed, TopSpeedSlider.GetMin()), TopSpeedSlider.GetMax());
    TopSpeedSlider.SetPreviewValue(top_preview);
    TopSpeedSlider.Draw();

    HandlingSlider.SetValue(perf1.Handling);
    float hdl_preview = bMin(bMax(perf2.Handling, HandlingSlider.GetMin()), HandlingSlider.GetMax());
    HandlingSlider.SetPreviewValue(hdl_preview);
    HandlingSlider.Draw();
}

int UIQRCarSelect::GetFilterType() {
    switch (static_cast<unsigned short>(filter)) {
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
    SelectableCar *found = nullptr;
    SelectableCar *node = FilteredCarsList.GetHead();
    for (; node != FilteredCarsList.EndOfList(); node = static_cast<SelectableCar *>(node->GetNext())) {
        found = node;
        if (ForceCar == 0xFFFFFFFF) {
            int ft = GetFilterType();
            unsigned int nodeHandle = node->mHandle;
            if (nodeHandle == ListHandles[ft])
                break;
            unsigned int targetHandle;
            if ((FEDatabase->GetGameMode() & 1) == 0) {
                RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                nodeHandle = node->mHandle;
                targetHandle = settings->SelectedCar[iPlayerNum];
            } else {
                targetHandle = FEDatabase->CurrentUserProfiles[0]->GetCareer()->CurrentCar;
            }
            if (nodeHandle == targetHandle)
                break;
        } else {
            if (node->mHandle == ForceCar)
                break;
        }
    }
    if ((FEDatabase->GetGameMode() & 0x8000) != 0 && ForceCar == 0xFFFFFFFF) {
        CarViewer::CancelCarLoad(eCARVIEWER_PLAYER1_CAR);
        found = FilteredCarsList.GetHead();
        originalCar = found->mHandle;
    }
    ForceCar = 0xFFFFFFFF;
    if (found == nullptr && FilteredCarsList.GetHead() != FilteredCarsList.EndOfList()) {
        found = FilteredCarsList.GetHead();
    }
    SetSelectedCar(found, iPlayerNum);
    RefreshHeader();
}

int UIQRCarSelect::GetBonusUnlockText(FECarRecord *fe_car) {
    unsigned int handle = fe_car->Handle;
    if (handle < 0x136254u) {
        if (handle > 0x13624du || (handle < 0x9667u && handle > 0x965eu)) {
            return 0x4ef2a115;
        }
    } else {
        switch (handle) {
            case 0x2cf370f0u:
                return 0xbd8bac94;
            case 0x03a94520u:
                return 0xbd8bac93;
            case 0x2cf385b2u:
                return 0xbd8bac92;
            case 0xcb6aaf2fu:
                return 0xbd8bac91;
        }
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
    unsigned short list = static_cast<unsigned short>(filter);
    switch (list) {
        case 1:
            langhash = 0xd9d6b954;
            texhash = 0x3a541f7f;
            break;
        case 2:
            langhash = 0xee053562;
            texhash = 0xf0bddecd;
            break;
        case 4:
            langhash = 0x2414de28;
            texhash = 0x9996ca1e;
            break;
        case 8:
            langhash = 0xd8a058f7;
            texhash = 0xbe5ad8a2;
            break;
        case 0x10:
            langhash = 0x0d8500c3;
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
        FEngSetInvisible(GetPackageName(), 0x39dc21f9);
        FEngSetInvisible(GetPackageName(), 0xe998fe99);
    }

    FEngSetLanguageHash(GetPackageName(), 0xaa9834bc, langhash);
    FEImage *filterImg = FEngFindImage(GetPackageName(), 0xe3b271b8);
    FEngSetTextureHash(filterImg, texhash);
    FEngSetScript(GetPackageName(), 0xd0f7c7cc, 0x16a259, true);

    if (!pSelectedCar) {
        FEngSetInvisible(GetPackageName(), 0x7379349b);
    } else if (pSelectedCar->bLocked) {
        FEngSetInvisible(GetPackageName(), 0x7379349b);
    }

    if (!pSelectedCar) {
        FEngSetLanguageHash(GetPackageName(), 0x2d25b2c4, 0x58bbed2a);
        cFEng::Get()->QueuePackageMessage(0xd9420cd5, GetPackageName(), nullptr);
        if (filter & 4) {
            FEngSetLanguageHash(GetPackageName(), 0x36c1e04d, 0x0da87b01);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0x36c1e04d, 0x58bbed2a);
        }
        FEngSetInvisible(GetPackageName(), 0x0e9ed0a2);
        FEngSetInvisible(GetPackageName(), 0x18a4384f);
        CarViewer::CancelCarLoad(static_cast<eCarViewerWhichCar>(0));
        GarageMainScreen::GetInstance()->DisableCarRendering();
        cFEng::Get()->QueuePackageMessage(0x913fa282, GetPackageName(), nullptr);
        bLoadingBarActive = false;
        tLastEventTimer = 0;
        return;
    }

    if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
        FEngSetVisible(GetPackageName(), 0x0e9ed0a2);
        FEngSetVisible(GetPackageName(), 0x18a4384f);
    }

    FEngSetVisible(GetPackageName(), 0x7379349b);
    cFEng::Get()->QueuePackageMessage(0x7c4583dc, GetPackageName(), nullptr);
    // TODO
    // FEPrintf(GetPackageName(), 0x6f25a248, "%d", FilteredCarsList.TraversebList(pSelectedCar));
    FEPrintf(GetPackageName(), 0xb2037bdc, "%d", FilteredCarsList.CountElements());

    FEPlayerCarDB *stable;
    if (iPlayerNum < 2) {
        stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
    } else {
        stable = nullptr;
    }

    if (pSelectedCar->bLocked) {
        FEngSetScript(GetPackageName(), 0xd0f7c7cc, 0x1ca7c0, true);
        FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
        if (car->MatchesFilter(0xf0008)) {
            int unlockText = GetBonusUnlockText(car);
            if (unlockText == 0x4ef2a115) {
                int binNum = GetBonusUnlockBinNumber(car);
                char rival_name_locdb[128];
                bSNPrintf(rival_name_locdb, 0x80, "blacklist_rival_%02d_aka", binNum);
                const char *fmt = GetLocalizedString(0x4ef2a115);
                const char *name = GetLocalizedString(FEHashUpper(rival_name_locdb));
                FEPrintf(GetPackageName(), 0x2d25b2c4, fmt, name, binNum);
            } else {
                FEngSetLanguageHash(GetPackageName(), 0x2d25b2c4, static_cast<unsigned int>(unlockText));
            }
        } else {
            Attrib::Gen::frontend fe_attrib(car->FEKey, 0, nullptr);
            int rival_num = fe_attrib.UnlockedAt() + 1;
            char rival_name_locdb[128];
            bSNPrintf(rival_name_locdb, 0x80, "blacklist_rival_%02d_aka", rival_num);
            const char *fmt = GetLocalizedString(0x4ef2a115);
            const char *name = GetLocalizedString(FEHashUpper(rival_name_locdb));
            FEPrintf(GetPackageName(), 0x2d25b2c4, fmt, name, rival_num);
        }
    }

    FECarRecord *car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
    FEngSetInvisible(GetPackageName(), 0xd6d32016);
    FEngSetInvisible(GetPackageName(), 0x79d6e45c);
    FEngSetTextureHash(pManuLogo, car->GetManuLogoHash());
    FEngSetTextureHash(pCarBadge, car->GetLogoHash());

    if (FEDatabase->IsCarLotMode()) {
        FEPrintf(GetPackageName(), 0x1930b057, "%$d", FEDatabase->GetCareerSettings()->GetCash());
        FEPrintf(GetPackageName(), 0x20c83c31, "%$d", car->GetCost());
        FEngSetLanguageHash(GetPackageName(), 0xdc18c4d4, 0xa9950b93);
        FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x7010bbf2);
    }

    if (FEDatabase->IsCareerMode() && !FEDatabase->IsCarLotMode()) {
        TheHeatMeter.SetVisibility(true);
    } else {
        TheHeatMeter.SetVisibility(false);
    }

    if (car->IsCareer()) {
        FEngSetInvisible(GetPackageName(), 0x39dc21f9);
        FEngSetInvisible(GetPackageName(), 0xe998fe99);

        FECareerRecord *career = stable->GetCareerRecordByHandle(car->CareerHandle);

        int num_markers = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(0x14), 0);
        bool hasMarkers = num_markers > 0;
        if (hasMarkers || (CheatCanAddImpoundBox && career->TheImpoundData.ImpoundedState == 0)) {
            num_markers = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(0x14), 0);
            FEngSetVisible(GetPackageName(), 0x39dc21f9);
            FEPrintf(GetPackageName(), 0x5b875870, "%2d", num_markers);
            FEPrintf(GetPackageName(), 0xea8aecd9, "%2d", num_markers);
        } else {
            FEngSetInvisible(GetPackageName(), 0x39dc21f9);
        }

        if (career->TheImpoundData.ImpoundedState == FEImpoundData::IMPOUND_RELEASED) {
            FEngSetLanguageHash(GetPackageName(), 0x72e7ea88, 0x9db4df7d);
            FEngSetLanguageHash(GetPackageName(), 0x9d974df3, 0x073b79e0);
            unsigned int cost = car->GetReleaseFromImpoundCost();
            FEPrintf(GetPackageName(), 0x322b18f9, "%$0.0f", static_cast<float>(cost));
            FEPrintf(GetPackageName(), 0x7044a5a4, "%$d", FEDatabase->GetCareerSettings()->GetCash());
            FEngSetInvisible(GetPackageName(), 0x0e9ed0a2);
        } else if (career->TheImpoundData.ImpoundedState == FEImpoundData::IMPOUND_REASON_NONE) {
            FEngSetLanguageHash(GetPackageName(), 0x72e7ea88, 0x17574b0e);
            FEngSetLanguageHash(GetPackageName(), 0x9d974df3, 0x915f4d26);
            if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
                FEngSetVisible(GetPackageName(), 0x0e9ed0a2);
            }
            FECareerRecord *record = stable->GetCareerRecordByHandle(car->CareerHandle);
            if (record) {
                FEPrintf(GetPackageName(), 0x322b18f9, "%$d", record->GetBounty());
                FEPrintf(GetPackageName(), 0x7044a5a4, "%$d", record->GetInfractions(true).GetFineValue());
            }
        } else {
            FEngSetLanguageHash(GetPackageName(), 0x72e7ea88, 0x9db4df7d);
            FEngSetLanguageHash(GetPackageName(), 0x9d974df3, 0x073b79e0);
            FEngSetLanguageHash(GetPackageName(), 0x322b18f9, 0xaefedad9);
            FEPrintf(GetPackageName(), 0x7044a5a4, "%$d", FEDatabase->GetCareerSettings()->GetCash());
            FEngSetInvisible(GetPackageName(), 0x0e9ed0a2);

            num_markers = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(0x15), 0);
            bool hasReleaseMarkers = num_markers > 0;
            if (hasReleaseMarkers || CheatReleaseFromImpoundMarker) {
                num_markers = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(0x15), 0);
                FEngSetVisible(GetPackageName(), 0xe998fe99);
                FEPrintf(GetPackageName(), 0xcc59b910, "%2d", num_markers);
                FEPrintf(GetPackageName(), 0xb8f9938a, "%2d", num_markers);
                FEngSetInvisible(GetPackageName(), 0x39dc21f9);
            }
        }

        {
            FECareerRecord *record = stable->GetCareerRecordByHandle(car->CareerHandle);
            if (record) {
                if (!FEDatabase->IsOnlineMode() && !FEDatabase->IsLANMode()) {
                    FEngSetVisible(GetPackageName(), 0x18a4384f);
                }
                FEPrintf(GetPackageName(), 0xd6d32016, "%$d", record->GetBounty());
                FEPrintf(GetPackageName(), 0x79d6e45c, "%$d", record->GetInfractions(true).GetFineValue());
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
    DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(3), 0x317d3005, 0x8cd532a0, 0x5f5e3886, 0x1a2826e1, 0x34dc1bcf,
                                    (eDialogFirstButtons)(FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission == 0), 0x6f5401d1);
}

FECarRecord *UIQRCarSelect::GetSelectedCarRecord() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(iPlayerNum);
    FECarRecord *selected_car = stable->GetCarRecordByHandle(pSelectedCar->mHandle);
    return selected_car;
}

void UIQRCarSelect::SetSelectedCar(SelectableCar *newCar, int player_num) {
    pSelectedCar = newCar;
    if (TheBustedManager.IsImpoundInfoVisible() && pSelectedCar != nullptr) {
        FECarRecord *rec = GetSelectedCarRecord();
        TheBustedManager.SetSelectedCar(rec);
    }
    if (newCar != nullptr) {
        int filterIdx = GetFilterType();
        ListHandles[filterIdx] = newCar->mHandle;
        GarageMainScreen::GetInstance()->DisableCarRendering();
        cFEng::Get()->QueuePackageMessage(0xa05a328e, nullptr, nullptr);
        cFEng::Get()->QueuePackageMessage(0x9c0a27eb, GetPackageName(), nullptr);
        bLoadingBarActive = true;
        unsigned int mode = FEDatabase->GetGameMode();
        if ((mode & 1) != 0) {
            if ((mode & 0x8000) == 0) {
                FEDatabase->GetCareerSettings()->SetCurrentCar(newCar->mHandle);
            }
        } else if ((mode & 0x20) == 0) {
            RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            settings->SelectedCar[player_num] = newCar->mHandle;
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
    FilteredCarsList.AddTail(new SelectableCar(0x136253, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x136252, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x136252, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x136251, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x136251, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x136250, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x136250, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x13624f, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x13624f, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x13624e, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x13624e, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9666, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x9666, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9665, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x9665, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9664, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x9664, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9663, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x9663, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9662, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x9662, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9661, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x9661, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x9660, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x9660, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x965f, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x965f, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x3a94520, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x3a94520, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0xcb6aaf2f, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0xcb6aaf2f, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x2cf370f0, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x2cf370f0, !bCarUnlocked));
    bCarUnlocked = UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x2cf385b2, iPlayerNum);
    FilteredCarsList.AddTail(new SelectableCar(0x2cf385b2, !bCarUnlocked));
    if (UnlockSystem::IsCarUnlocked(UNLOCK_QUICK_RACE, 0x34498eb2, iPlayerNum)) {
        FilteredCarsList.AddTail(new SelectableCar(0x34498eb2, false));
    }
    if (GetIsCollectorsEdition()) {
        FilteredCarsList.AddTail(new SelectableCar(0x634d1bd2, false));
        FilteredCarsList.AddTail(new SelectableCar(0xe1075862, false));
        FilteredCarsList.AddTail(new SelectableCar(0x02d642b8, false));
        FilteredCarsList.AddTail(new SelectableCar(0x03d3401a, false));
        FilteredCarsList.AddTail(new SelectableCar(0x03d8a6d1, false));
        FilteredCarsList.AddTail(new SelectableCar(0x54653c71, false));
        FilteredCarsList.AddTail(new SelectableCar(0xe115ead0, false));
        FilteredCarsList.AddTail(new SelectableCar(0x54655133, false));
        FilteredCarsList.AddTail(new SelectableCar(0x582f21d9, false));
        FilteredCarsList.AddTail(new SelectableCar(0x363a1fea, false));
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
    unsigned int mode = FEDatabase->GetGameMode();
    unsigned int unlockFilter;
    if ((mode & 1) == 0) {
        if ((mode & 4) == 0 && ((mode & 8) != 0 || (mode & 0x40) != 0)) {
            unlockFilter = 7;
        } else {
            unlockFilter = 1;
        }
    } else {
        unlockFilter = 2;
    }
    int i = 0;
    do {
        FECarRecord *car = carDB->GetCarByIndex(i);
        if (car->Handle != 0xFFFFFFFF && car->MatchesFilter(filter)) {
            bool unlocked = UnlockSystem::IsCarUnlocked(static_cast<eUnlockFilters>(unlockFilter), car->Handle, iPlayerNum);
            if (!GetMikeMannBuild() || IsValidMikeMannCar(car, filter)) {
                SelectableCar *newCar = new SelectableCar(car->Handle, !unlocked);
                FilteredCarsList.AddTail(newCar);
            }
        }
        i++;
    } while (i < 200);
    unsigned short f = static_cast<unsigned short>(filter);
    if (f == 1 || f == 2 || f == 8 || f == 0x10) {
        FilteredCarsList.Sort(SortCarsByUnlock);
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
    SelectableCar *newCar = static_cast<SelectableCar *>(cur->GetPrev());
    if (newCar == FilteredCarsList.EndOfList()) {
        newCar = FilteredCarsList.GetTail();
    }
    if (dir == eSD_PREV) {
    } else if (dir == eSD_NEXT) {
        newCar = static_cast<SelectableCar *>(cur->GetNext());
        if (newCar == FilteredCarsList.EndOfList()) {
            newCar = FilteredCarsList.GetHead();
        }
    }
    if (newCar != cur) {
        SetSelectedCar(newCar, iPlayerNum);
        RefreshHeader();
    }
}

void UIQRCarSelect::ScrollLists(eScrollDir dir) {
    if (dir == eSD_NEXT) {
        filter++;
        if (filter >= NUM_LISTS)
            filter = 0;
    } else {
        filter--;
        if (filter < 0)
            filter = NUM_LISTS - 1;
    }
    RefreshCarList();
    RefreshHeader();
}

void UIQRCarSelect::OnlineActOnSelect() {
    unsigned int handle = pSelectedCar->mHandle;
    FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(1))->SetSelectedCar(handle, 0);
    FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0))->SetSelectedCar(handle, 0);
    FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(2))->SetSelectedCar(handle, 0);
}
