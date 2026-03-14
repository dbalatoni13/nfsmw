// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRCarSelect.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"

extern int GetCurrentLanguage();
extern FEMarkerManager TheFEMarkerManager;
extern int CheatImpounded;
extern int CheatBustedCount;
extern int CheatMaxBusted;
extern int CheatReleasable;
extern int CheatCanAddImpoundBox;
extern int g_MaximumMaximumTimesBusted;
extern int gPlayerNum;

extern void LoadOneTexture(const char *pkg_name, unsigned int hash, void (*callback)(unsigned int), unsigned int param);

unsigned int UIQRCarSelect::ForceCar;
bool QRCarSelectBustedManager::bPlayerJustGotBusted;

QRCarSelectBustedManager::QRCarSelectBustedManager(const char *pkg_name, int flags) {
    WorkingCareerRecord = nullptr;
    WorkingCarRecord = nullptr;
    Flags = static_cast<eBustedAnimationTypes>(flags);
    ImpoundStampHash = 0;
    ParentPkg = pkg_name;
    bWantsImpound = false;
}

QRCarSelectBustedManager::~QRCarSelectBustedManager() {}

bool QRCarSelectBustedManager::IsImpoundInfoVisible() {
    if (!FEDatabase->IsCareerMode()) return false;
    return !FEDatabase->IsCarLotMode();
}

bool QRCarSelectBustedManager::ShowImpoundedTexture() {
    return WorkingCareerRecord->TheImpoundData.EvadeCount != 0;
}

void QRCarSelectBustedManager::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0xe47966ea) {
        if (param1 == 0xa0fc39f9) {
            unsigned int cost = WorkingCarRecord->GetReleaseFromImpoundCost();
            FEDatabase->GetCareerSettings()->SpendCash(cost);
            WorkingCareerRecord->TheImpoundData.NotifyPlayerPaidToRelease();
            RefreshHeader();
        } else if (param1 == 0xcad5722e) {
            TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);
            WorkingCareerRecord->TheImpoundData.NotifyPlayerUsedMarkerToRelease();
            RefreshHeader();
        } else if (param1 == 0x8defa48b) {
            TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0);
            WorkingCareerRecord->TheImpoundData.AddMaxBusted();
            RefreshHeader();
        } else if (param1 == 0xb4edeb6d || param1 == 0x5ee58948) {
            if (CalcGameOver()) {
                DialogInterface::ShowOneButton(ParentPkg, "", static_cast<eDialogTitle>(1),
                    0x417b2601, 0x5ee58948, 0xe96fa0c5);
            }
        } else if (param1 == 0xe96fa0c5) {
            FEDatabase->GetCareerSettings()->SetGameOver();
        }
    }
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
    if (lang == 7) {
        hash = 0xce183c96;
    } else if (lang == 3) {
        hash = 0xce185441;
    } else if (lang == 5) {
        hash = 0xce183f30;
    } else if (lang == 4) {
        hash = 0xce187e47;
    } else if (lang == 6) {
        hash = 0xce187f32;
    } else if (lang == 1) {
        hash = 0xce184740;
    } else if (lang == 2) {
        hash = 0xce1849e1;
    } else if (lang == 10) {
        hash = 0xce18561e;
    } else if (lang == 8) {
        hash = 0xce185c2f;
    } else {
        hash = 0xce184740;
    }
    ImpoundStampHash = hash;
    LoadOneTexture(ParentPkg, hash, TextureLoadedCallbackAccessor, reinterpret_cast<unsigned int>(this));
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
    if (!WorkingCareerRecord) return;

    bool isImpounded = WorkingCareerRecord->TheImpoundData.IsImpounded();
    if (isImpounded) {
        FEngSetVisible(FEngFindObject(ParentPkg, 0x19398802));
        FEngSetVisible(FEngFindObject(ParentPkg, 0x1930b057));
        FEPrintf(ParentPkg, 0x9ab6a1a5, "%d", static_cast<int>(WorkingCareerRecord->TheImpoundData.TimesBusted));
        FEPrintf(ParentPkg, 0x9ad9c3c6, "%d", static_cast<int>(WorkingCareerRecord->TheImpoundData.MaxBusted));
    } else {
        FEngSetInvisible(FEngFindObject(ParentPkg, 0x19398802));
        FEngSetInvisible(FEngFindObject(ParentPkg, 0x1930b057));

        if (WorkingCareerRecord->TheImpoundData.TimesBusted > 0) {
            FEngSetVisible(FEngFindObject(ParentPkg, 0x20d113dc));
            FEngSetVisible(FEngFindObject(ParentPkg, 0x20c83c31));
            FEPrintf(ParentPkg, 0x9ab6a1a5, "%d", static_cast<int>(WorkingCareerRecord->TheImpoundData.TimesBusted));
            FEPrintf(ParentPkg, 0x9ad9c3c6, "%d", static_cast<int>(WorkingCareerRecord->TheImpoundData.MaxBusted));
        } else {
            FEngSetInvisible(FEngFindObject(ParentPkg, 0x20d113dc));
            FEngSetInvisible(FEngFindObject(ParentPkg, 0x20c83c31));
        }
    }

    if (bWantsImpound) {
        bWantsImpound = false;
        MaybeAddImpoundBox();
    }
}

bool QRCarSelectBustedManager::CalcGameOver() {
    int numCars = FEDatabase->GetPlayerCarStable(0)->GetNumAvailableCareerCars();
    if (numCars < 1) {
        int numMarkers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);
        if (numMarkers < 1) return true;
    }
    return false;
}

void QRCarSelectBustedManager::MaybeReleaseCar() {
    FECareerRecord *career = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(WorkingCarRecord->CareerHandle);
    unsigned int cost = WorkingCarRecord->GetReleaseFromImpoundCost();
    int cash = FEDatabase->GetCareerSettings()->GetCash();
    int numMarkers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_IMPOUND_RELEASE, 0);

    if (career->TheImpoundData.ImpoundedState == 4 && static_cast<int>(cost) <= cash) {
        if (numMarkers > 0) {
            DialogInterface::ShowThreeButtons(ParentPkg, "", static_cast<eDialogTitle>(3),
                0xf9c73cc2, 0x4eb9591f, 0x1a294dad, 0xe845bc1c, 0xa0fc39f9, 0x5ee58948, 0x5ee58948,
                static_cast<eDialogFirstButtons>(1), 0xb715ae8f, cost);
        } else {
            DialogInterface::ShowTwoButtons(ParentPkg, "", static_cast<eDialogTitle>(3),
                0xf9c73cc2, 0x4eb9591f, 0x1a294dad, 0xa0fc39f9, 0xcad5722e,
                static_cast<eDialogFirstButtons>(1), 0xb715ae8f, cost);
        }
    } else {
        if (numMarkers > 0) {
            DialogInterface::ShowTwoButtons(ParentPkg, "", static_cast<eDialogTitle>(3),
                0x417b2601, 0x1a294dad, 0xe845bc1c, 0xcad5722e, 0x5ee58948,
                static_cast<eDialogFirstButtons>(1), 0xb715ae8f, cost);
        } else {
            DialogInterface::ShowOneButton(ParentPkg, "", static_cast<eDialogTitle>(1),
                0x417b2601, 0x5ee58948, 0xe96fa0c5);
        }
    }
}

void QRCarSelectBustedManager::MaybeAddImpoundBox() {
    bool canAdd = false;
    if (WorkingCareerRecord->TheImpoundData.CanAddMaxBusted()) {
        int numMarkers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0);
        if (numMarkers > 0) canAdd = true;
    }
    bool showDialog = canAdd || CheatCanAddImpoundBox != 0;
    if (showDialog) {
        DialogInterface::ShowTwoButtons(ParentPkg, "", static_cast<eDialogTitle>(3),
            0x417b2601, 0x1a294dad, 0x8defa48b, 0xb4edeb6d, 0xb4edeb6d,
            static_cast<eDialogFirstButtons>(1), 0xcebda20);
    } else if (g_MaximumMaximumTimesBusted <= static_cast<int>(WorkingCareerRecord->TheImpoundData.MaxBusted)) {
        DialogInterface::ShowOneButton(ParentPkg, "", static_cast<eDialogTitle>(2),
            0x417b2601, 0xb4edeb6d, 0xbcae8539);
    }
}

UIQRCarSelect::UIQRCarSelect(ScreenConstructorData *sd) : MenuScreen(sd) //
    , TheBustedManager(GetPackageName(), sd->Arg >> 8)
{
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

UIQRCarSelect::~UIQRCarSelect() {
    ClearCarList();
}

bool UIQRCarSelect::IsCarImpounded(unsigned int handle) {
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(handle);
    if (!car) return false;
    FECareerRecord *career = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(car->CareerHandle);
    if (!career) return false;
    return career->TheImpoundData.IsImpounded();
}

void UIQRCarSelect::CommitChangeStartRace(bool allowError) {
    FECarRecord *car = GetSelectedCarRecord();
    if (car) {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(FEDatabase->RaceMode);
        settings->SetSelectedCar(car->Handle, iPlayerNum);
    }
}

void UIQRCarSelect::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    TheBustedManager.NotificationMessage(msg, pobj, param1, param2);

    if (msg == 0xf18e2bee) {
        ScrollCars(eSD_NEXT);
        RefreshHeader();
    } else if (msg == 0x5fba7cbb) {
        ScrollCars(eSD_PREV);
        RefreshHeader();
    } else if (msg == 0x1fe68f0d) {
        ScrollLists(eSD_NEXT);
    } else if (msg == 0x1aae03ee) {
        ScrollLists(eSD_PREV);
    }
}

eMenuSoundTriggers UIQRCarSelect::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (msg == 0xf18e2bee || msg == 0x5fba7cbb) {
        return static_cast<eMenuSoundTriggers>(0x1e);
    }
    if (msg == 0x1fe68f0d || msg == 0x1aae03ee) {
        return static_cast<eMenuSoundTriggers>(0x1e);
    }
    return MenuScreen::NotifySoundMessage(msg, maybe);
}

void UIQRCarSelect::Setup() {
    if (FEDatabase->IsCarLotMode()) {
        filter = LIST_STOCK;
    }
    RefreshCarList();
    RefreshBonusCarList();
    if (ForceCar != 0) {
        SelectableCar *node = FilteredCarsList.GetHead();
        while (node != FilteredCarsList.EndOfList()) {
            if (node->mHandle == ForceCar) {
                pSelectedCar = node;
                break;
            }
            node = static_cast<SelectableCar *>(node->GetNext());
        }
        ForceCar = 0;
    }
    SetupForPlayer(iPlayerNum);
    RefreshHeader();
    UpdateSliders();
}

void UIQRCarSelect::InitStatsSliders() {
    AccelerationSlider.Init(GetPackageName(), "AccelSlider", 0.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    TopSpeedSlider.Init(GetPackageName(), "TopSpeedSlider", 0.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    HandlingSlider.Init(GetPackageName(), "HandlingSlider", 0.0f, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f);
}

void UIQRCarSelect::UpdateSliders() {
    FECarRecord *car = GetSelectedCarRecord();
    if (!car) return;

    Physics::Info::Performance performance;
    Physics::Info::EstimatePerformance(performance);

    AccelerationSlider.SetValue(performance.Acceleration);
    TopSpeedSlider.SetValue(performance.TopSpeed);
    HandlingSlider.SetValue(performance.Handling);
    AccelerationSlider.Draw();
    TopSpeedSlider.Draw();
    HandlingSlider.Draw();
}

int UIQRCarSelect::GetFilterType() {
    if (FEDatabase->IsCareerMode()) return LIST_CAREER;
    if (FEDatabase->IsCarLotMode()) return LIST_STOCK;
    return filter;
}

void UIQRCarSelect::SetupForPlayer(int player) {
    if (FEDatabase->IsSplitScreenMode()) {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(FEDatabase->RaceMode);
        unsigned int selectedCar = settings->SelectedCar[player];
        SelectableCar *node = FilteredCarsList.GetHead();
        while (node != FilteredCarsList.EndOfList()) {
            if (node->mHandle == selectedCar) {
                pSelectedCar = node;
                return;
            }
            node = static_cast<SelectableCar *>(node->GetNext());
        }
    }
    pSelectedCar = FilteredCarsList.GetHead();
}

int UIQRCarSelect::GetBonusUnlockText(FECarRecord *fe_car) {
    if (!fe_car) return 0;
    Attrib::Gen::frontend fe_attrib(fe_car->FEKey, 0, nullptr);
    return fe_attrib.UnlockedAt();
}

int UIQRCarSelect::GetBonusUnlockBinNumber(FECarRecord *fe_car) {
    if (!fe_car) return 0;
    int unlockText = GetBonusUnlockText(fe_car);
    if (unlockText == 0x49e69969) return 1;
    if (unlockText == 0xc58f5dbe) return 2;
    if (unlockText == 0x3b8d38cb) return 3;
    if (unlockText == 0xb7666ce4) return 4;
    if (unlockText == 0x2968ad4f) return 5;
    if (unlockText == 0xa523c938) return 6;
    if (unlockText == 0x1b210e25) return 7;
    if (unlockText == 0x97ea4a02) return 8;
    return 0;
}

void UIQRCarSelect::RefreshHeader() {
    FECarRecord *car = GetSelectedCarRecord();
    if (!car) return;

    FEngSetTextureHash(pManuLogo, car->GetManuLogoHash());
    FEngSetTextureHash(pCarBadge, car->GetLogoHash());
    FEngSetLanguageHash(pCarName, car->GetNameHash());
    FEngSetLanguageHash(pCarNameShadow, car->GetNameHash());

    int filterType = GetFilterType();
    if (filterType == LIST_BONUS) {
        int unlockText = GetBonusUnlockText(car);
        if (unlockText) {
            FEngSetLanguageHash(pFilter, static_cast<unsigned int>(unlockText));
        }
    }

    if (FEDatabase->IsCareerMode()) {
        TheBustedManager.SetSelectedCar(car);
    }

    UpdateSliders();
}

void UIQRCarSelect::ChooseTransmission() {
    DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(3),
        0x317d3005, 0x8cd532a0, 0x5f5e3886, 0x1a2826e1, 0x34dc1bcf,
        (eDialogFirstButtons)(FEDatabase->GetPlayerSettings(iPlayerNum)->Transmission == 0), 0x6f5401d1);
}

FECarRecord *UIQRCarSelect::GetSelectedCarRecord() {
    if (!pSelectedCar) return nullptr;
    return FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pSelectedCar->mHandle);
}

void UIQRCarSelect::SetSelectedCar(SelectableCar *newCar, int player_num) {
    pSelectedCar = newCar;
    if (newCar) {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(FEDatabase->RaceMode);
        settings->SetSelectedCar(newCar->mHandle, player_num);
    }
    RefreshHeader();
    UpdateSliders();
}

int SortCarsByUnlock(SelectableCar *a, SelectableCar *b) {
    FECarRecord *carA = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(a->mHandle);
    FECarRecord *carB = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(b->mHandle);
    Attrib::Gen::frontend fe_a(carA->FEKey, 0, nullptr);
    Attrib::Gen::frontend fe_b(carB->FEKey, 0, nullptr);
    int binA = fe_a.UnlockedAt();
    int binB = fe_b.UnlockedAt();
    if (binA != binB) return binA - binB;
    return 0;
}

bool IsValidMikeMannCar(FECarRecord *car, unsigned int filterBits) {
    if (!car) return false;
    if (!car->IsValid()) return false;
    if ((car->FilterBits & filterBits) == 0) return false;
    Attrib::Gen::frontend fe_attrib(car->FEKey, 0, nullptr);
    return fe_attrib.UnlockedAt() != 0;
}

void UIQRCarSelect::RefreshBonusCarList() {
    FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
    int numCars = carDB->GetNumCars(0xFFFFFFFF);
    for (int i = 0; i < numCars; i++) {
        FECarRecord *car = carDB->GetCarByIndex(i);
        if (!car || !car->IsValid()) continue;
        if (!IsValidMikeMannCar(car, 0xFFFFFFFF)) continue;
        SelectableCar *newCar = new SelectableCar(car->Handle, false);
        FilteredCarsList.AddTail(newCar);
    }
}

void UIQRCarSelect::RefreshCarList() {
    ClearCarList();
    FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
    int filterType = GetFilterType();
    int numCars = carDB->GetNumCars(0xFFFFFFFF);
    for (int i = 0; i < numCars; i++) {
        FECarRecord *car = carDB->GetCarByIndex(i);
        if (!car || !car->IsValid()) continue;
        if (!car->MatchesFilter(filterType)) continue;
        SelectableCar *newCar = new SelectableCar(car->Handle, false);
        FilteredCarsList.AddTail(newCar);
    }
    pSelectedCar = FilteredCarsList.GetHead();
}

void UIQRCarSelect::ClearCarList() {
    FilteredCarsList.DeleteAllElements();
    pSelectedCar = nullptr;
}

void UIQRCarSelect::ScrollCars(eScrollDir dir) {
    if (dir == eSD_NEXT) {
        pSelectedCar = static_cast<SelectableCar *>(pSelectedCar->GetNext());
        if (pSelectedCar == FilteredCarsList.EndOfList()) {
            pSelectedCar = FilteredCarsList.GetHead();
        }
    } else {
        pSelectedCar = static_cast<SelectableCar *>(pSelectedCar->GetPrev());
        if (pSelectedCar == FilteredCarsList.EndOfList()) {
            pSelectedCar = FilteredCarsList.GetTail();
        }
    }
}

void UIQRCarSelect::ScrollLists(eScrollDir dir) {
    if (dir == eSD_NEXT) {
        filter++;
        if (filter >= NUM_LISTS) filter = 0;
    } else {
        filter--;
        if (filter < 0) filter = NUM_LISTS - 1;
    }
    RefreshCarList();
    RefreshHeader();
}

void UIQRCarSelect::OnlineActOnSelect() {
    FECarRecord *car = GetSelectedCarRecord();
    if (!car) return;
    if (FEDatabase->IsCareerMode() && IsCarImpounded(car->Handle)) {
        TheBustedManager.MaybeReleaseCar();
        return;
    }
    ChooseTransmission();
}
