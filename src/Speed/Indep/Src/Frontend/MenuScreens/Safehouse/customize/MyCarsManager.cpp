#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/MyCarsManager.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeBusted.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeWorldMapQuickList.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

void CarDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == 0xc407210 || msg == 0x406415e3) {
        if (Handle == 0xFFFFFFFF) {
            FEDatabase->SetGameMode(static_cast<eFEGameModes>(FEDatabase->GetGameMode() | 0x20));
            cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
        } else {
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            FECarRecord *carRecord = stable->GetCarRecordByHandle(Handle);
            if (carRecord) {
                if (!carRecord->IsCustomized()) {
                    carRecord = stable->CreateNewCustomCar(carRecord->Handle);
                }
                BeginCarCustomize(CEP_MAIN_MENU, carRecord);
            }
        }
    }
}

MyCarsManager::MyCarsManager(ScreenConstructorData *sd)
    : ArrayScrollerMenu(sd, 5, 2, true), //
      AccelerationSlider(),              //
      TopSpeedSlider(),                  //
      HandlingSlider() {
    pSelectedCar = nullptr;
    SetClickToSelectMode(true);
    tCarLoadTimer.UnSet();
    bGoToShowcase = false;
    Setup();
}

eMenuSoundTriggers MyCarsManager::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x480c9a58) {
        return static_cast<eMenuSoundTriggers>(5);
    }
    return maybe;
}

void MyCarsManager::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
        case 0x1a2826e1:
            break;
        case 0x34dc1bcf:
            break;
        case 0x35f8620b:
            FEDatabase->BackupCarStable();
            break;
        case 0xc98356ba: {
            if (tCarLoadTimer.IsSet()) {
                float elapsed = static_cast<float>(RealTimer.GetPackedTime() - tCarLoadTimer.GetPackedTime()) / 4000.0f;
                if (elapsed >= 0.25f && pSelectedCar) {
                    RideInfo ride;
                    FEDatabase->GetPlayerCarStable(0)->BuildRideForPlayer(pSelectedCar->Handle, 0, &ride);
                    CarViewer::SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
                    tCarLoadTimer.UnSet();
                }
            }
            break;
        }
        case 0x911ab364: {
            if (!pSelectedCar) {
                RideInfo ride;
                FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
                RaceSettings *rs = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
                carDB->BuildRideForPlayer(rs->SelectedCar[0], 0, &ride);
                CarViewer::SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(1), static_cast<eCarViewerWhichCar>(0));
            }
            if (FEDatabase->IsCarStableDirty() && IsMemcardEnabled) {
                MemcardEnter(GetPackageName(), "MainMenu.fng", 0x2000b3, nullptr, nullptr, 0, 0);
            } else {
                cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            }
            break;
        }
        case 0xc519bfc4: {
            FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(static_cast<CarDatum *>(GetCurrentDatum())->Handle);
            if (car->IsValid()) {
                DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf,
                                                first_dialog_button2, 0x4f68196e);
            }
            break;
        }
        case 0xd05fc3a3: {
            unsigned int handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
            FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
            FEDatabase->NotifyDeleteCar(handle);
            carDB->DeleteCustomCar(handle);
            RefreshCarList();
            if (GetNumDatum() >= 2) {
                pSelectedCar = carDB->GetCarRecordByHandle(static_cast<CarDatum *>(GetCurrentDatum())->Handle);
            } else {
                pSelectedCar = nullptr;
            }
            RefreshHeader();
            break;
        }
        case 0xc519bfbf:
            if (pSelectedCar) {
                cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
                bGoToShowcase = true;
            }
            break;
        case 0xe1fde1d1:
            if (bGoToShowcase) {
                Showcase::FromArgs = 0;
                Showcase::FromPackage = GetPackageName();
                cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(pSelectedCar), 0, false);
            }
            break;
    }
}

void MyCarsManager::Setup() {
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("CAR_ICON_%d", i + 1));
        AddSlot(new ("ImageArraySlot", 0) ImageArraySlot(img));
    }
    const u32 FEObj_CARNAME = 0xb271b295;
    SetDescLabel(FEObj_CARNAME);
    RefreshCarList();
    AccelerationSlider.Init(GetPackageName(), "ACCELERATION", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    TopSpeedSlider.Init(GetPackageName(), "TOPSPEED", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    HandlingSlider.Init(GetPackageName(), "HANDLING", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    RefreshHeader();
}

bool MyCarsManager::CanAddMoreCars() {
    if (FEDatabase->GetPlayerCarStable(0)->GetNumQuickRaceCars() > 19) {
        return false;
    }
    if (!FEDatabase->GetPlayerCarStable(0)->CanCreateNewCustomizationRecord()) {
        return false;
    }
    if (!FEDatabase->GetPlayerCarStable(0)->CanCreateNewCarRecord()) {
        return false;
    }
    return true;
}

void MyCarsManager::RefreshCarList() {
    int selectedIdx = 0;
    int idx = 0;
    ClearData();
    FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
    for (int i = 0; i < 200; i++) {
        FECarRecord *car = carDB->GetCarByIndex(i);
        if (car->IsValid() && car->MatchesFilter(0xf0004)) {
            CarDatum *datum = new ("CarDatum", 0) CarDatum(car->GetManuLogoHash(), car->GetNameHash(), car->Handle);
            AddDatum(datum);
            RaceSettings *rs = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            if (rs->SelectedCar[0] == static_cast<int>(car->Handle)) {
                selectedIdx = idx;
            }
            idx++;
        }
    }
    if (CanAddMoreCars()) {
        CarDatum *datum = new ("CarDatum", 0) CarDatum(0x91d3a6ba, 0x29ae0714, 0xFFFFFFFF);
        AddDatum(datum);
    }
    SetInitialPosition(selectedIdx);
    GarageMainScreen::GetInstance()->DisableCarRendering();
    UpdateCar();
}

void MyCarsManager::RefreshHeader() {

    const u32 FEObj_addcar = 0x42ea22dd;
    const u32 FEObj_editdeletecar = 0x06d41ccc;
    ArrayScrollerMenu::RefreshHeader();
    if (GetCurrentDatum()) {
        unsigned int handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
        FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
        FECarRecord *car = carDB->GetCarRecordByHandle(handle);
        if (car->IsValid()) {
            const char *pkg = GetPackageName();
            unsigned int logo = carDB->GetCarRecordByHandle(handle)->GetLogoHash();
            FEImage *img = FEngFindImage(pkg, 0xb05dd708);
            FEngSetTextureHash(img, logo);
        }
        if (static_cast<CarDatum *>(GetCurrentDatum())->Handle == 0xFFFFFFFF) {
            FEngSetLanguageHash(GetPackageName(), 0xbfa25765, 0xc2598bd8);
            cFEng::Get()->QueuePackageMessage(FEObj_addcar, GetPackageName(), nullptr);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0xbfa25765, 0xc9847935);
            cFEng::Get()->QueuePackageMessage(FEObj_editdeletecar, GetPackageName(), nullptr);
            for (int i = 0; i < 0xb; i++) {
                RaceSettings *rs = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i));
                rs->SelectedCar[0] = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
            }
        }
        FEPrintf(GetPackageName(), __NUMBER__, "%d", GetCurrentDatumNum());
        FEPrintf(GetPackageName(), __NUMBER_OF__, "%d", GetNumDatum());
        UpdateSliders();
        UpdateCar();
    }
}

void MyCarsManager::UpdateSliders() {
    if (static_cast<CarDatum *>(GetCurrentDatum())->Handle != 0xFFFFFFFF) {
        unsigned int handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
        FECarRecord *car = FEDatabase->GetPlayerCarRecordByHandle(0, handle);
        Physics::Info::Performance performance;
        if (car) {
            Attrib::Gen::pvehicle pv(car->VehicleKey, 0, nullptr);
            if (car->IsCustomized()) {
                FECustomizationRecord *cr = FEDatabase->GetPlayerCarStable(0)->GetCustomizationRecordByHandle(car->Customization);
                cr->WriteRecordIntoPhysics(pv);
            }
            Physics::Info::EstimatePerformance(pv, performance);
        }
        AccelerationSlider.SetValue(performance.Acceleration);
        AccelerationSlider.SetPreviewValue(performance.Acceleration);
        AccelerationSlider.Draw();
        TopSpeedSlider.SetValue(performance.TopSpeed);
        TopSpeedSlider.SetPreviewValue(performance.TopSpeed);
        TopSpeedSlider.Draw();
        HandlingSlider.SetValue(performance.Handling);
        HandlingSlider.SetPreviewValue(performance.Handling);
        HandlingSlider.Draw();
    }
}

void MyCarsManager::UpdateCar() {
    FEPlayerCarDB *stable;
    unsigned int handle;

    if (GetCurrentDatum()) {
        handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
        stable = FEDatabase->GetPlayerCarStable(0);
        if (!pSelectedCar) {
            if (handle == -1) {
                return;
            }
        } else if (pSelectedCar->Handle == handle) {
            return;
        }
        GarageMainScreen::GetInstance()->DisableCarRendering();
        if (handle == -1) {
            const u32 FEObj_LOADERLEAVE = 0x913FA282;
            cFEng::Get()->QueuePackageMessage(FEObj_LOADERLEAVE, nullptr, nullptr);
            pSelectedCar = nullptr;
            CarViewer::CancelCarLoad(static_cast<eCarViewerWhichCar>(0));
        } else {
            const u32 FEObj_LOADERAPPEAR = 0xA05A328E;
            cFEng::Get()->QueuePackageMessage(FEObj_LOADERAPPEAR, nullptr, nullptr);
            pSelectedCar = stable->GetCarRecordByHandle(handle);
        }
        tCarLoadTimer = RealTimer;
    }
}
