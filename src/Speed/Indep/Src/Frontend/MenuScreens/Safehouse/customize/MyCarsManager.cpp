#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/MyCarsManager.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"

extern cFrontendDatabase *FEDatabase;
extern Timer RealTimer;
extern bool IsMemcardEnabled;
extern unsigned int FEngHashString(const char *, ...);

void MemcardEnter(const char *from, const char *to, unsigned int op, void (*termFunc)(void *), void *termParam, unsigned int successMsg,
                  unsigned int failedMsg);
extern void BeginCarCustomize(eCustomizeEntryPoint entry, FECarRecord *car);

MyCarsManager::MyCarsManager(ScreenConstructorData *sd)
    : ArrayScrollerMenu(sd, 5, 2, true) //
      ,
      AccelerationSlider() //
      ,
      TopSpeedSlider() //
      ,
      HandlingSlider() //
      ,
      bGoToShowcase(false) {
    bShouldPlaySound = true;
    pSelectedCar = nullptr;
    tCarLoadTimer.UnSet();
    Setup();
}

eMenuSoundTriggers MyCarsManager::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == 0x480c9a58) {
        return static_cast<eMenuSoundTriggers>(5);
    }
    return maybe;
}

void MyCarsManager::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
        case 0x34dc1bcf:
            break;
        case 0x35f8620b:
            FEDatabase->BackupCarStable();
            break;
        case 0xc98356ba: {
            if (tCarLoadTimer.IsSet()) {
                float elapsed = static_cast<float>(RealTimer.GetPackedTime() - tCarLoadTimer.GetPackedTime()) / 4000.0f;
                if (elapsed >= 0.5f && pSelectedCar) {
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
                DialogInterface::ShowTwoButtons(GetPackageName(), "", static_cast<eDialogTitle>(1), 0x70e01038, 0x417b25e4, 0xd05fc3a3, 0x34dc1bcf,
                                                0x34dc1bcf, static_cast<eDialogFirstButtons>(1), 0x4f68196e);
            }
            break;
        }
        case 0xd05fc3a3: {
            unsigned int handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
            FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
            FEDatabase->NotifyDeleteCar(handle);
            carDB->DeleteCustomCar(handle);
            RefreshCarList();
            if (data.CountElements() >= 2) {
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
        ImageArraySlot *slot = new ImageArraySlot(img);
        AddSlot(slot);
    }
    descLabel = 0xb271b295;
    RefreshCarList();
    AccelerationSlider.Init(GetPackageName(), "ACCELERATION", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    TopSpeedSlider.Init(GetPackageName(), "TOPSPEED", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    HandlingSlider.Init(GetPackageName(), "HANDLING", 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 160.0f);
    ArrayScroller::RefreshHeader();
}

bool MyCarsManager::CanAddMoreCars() {
    if (FEDatabase->GetPlayerCarStable(0)->GetNumQuickRaceCars() > 19) {
        return false;
    }
    if (!FEDatabase->GetPlayerCarStable(0)->CanCreateNewCustomizationRecord()) {
        return false;
    }
    return FEDatabase->GetPlayerCarStable(0)->CanCreateNewCarRecord() != false;
}

void MyCarsManager::RefreshCarList() {
    int selectedIdx = 0;
    int idx = 0;
    ClearData();
    FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
    for (int i = 0; i < 200; i++) {
        FECarRecord *car = carDB->GetCarByIndex(i);
        if (car->IsValid() && car->MatchesFilter(0xf0004)) {
            CarDatum *datum = new CarDatum(car->GetManuLogoHash(), car->GetNameHash(), car->Handle);
            AddDatum(datum);
            RaceSettings *rs = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            if (rs->SelectedCar[0] == static_cast<int>(car->Handle)) {
                selectedIdx = idx;
            }
            idx++;
        }
    }
    if (CanAddMoreCars()) {
        CarDatum *datum = new CarDatum(0x91d3a6ba, 0x29ae0714, 0xFFFFFFFF);
        AddDatum(datum);
    }
    SetInitialPosition(selectedIdx);
    GarageMainScreen::GetInstance()->DisableCarRendering();
    UpdateCar();
}

void MyCarsManager::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();
    if (GetCurrentDatum()) {
        unsigned int handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
        FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
        FECarRecord *car = carDB->GetCarRecordByHandle(handle);
        if (car->IsValid()) {
            unsigned int logo = carDB->GetCarRecordByHandle(handle)->GetLogoHash();
            FEImage *img = FEngFindImage(GetPackageName(), 0xb05dd708);
            FEngSetTextureHash(img, logo);
        }
        if (static_cast<CarDatum *>(GetCurrentDatum())->Handle == 0xFFFFFFFF) {
            FEngSetLanguageHash(GetPackageName(), 0xbfa25765, 0xc2598bd8);
            cFEng::Get()->QueuePackageMessage(0x42ea22dd, GetPackageName(), nullptr);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0xbfa25765, 0xc9847935);
            cFEng::Get()->QueuePackageMessage(0x06d41ccc, GetPackageName(), nullptr);
            for (int i = 0; i < 0xb; i++) {
                RaceSettings *rs = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i));
                rs->SelectedCar[0] = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
            }
        }
        FEPrintf(GetPackageName(), 0x6f25a248, "%d", data.IsInList(static_cast<bNode *>(GetCurrentDatum())));
        FEPrintf(GetPackageName(), 0xb2037bdc, "%d", data.CountElements());
        UpdateSliders();
        UpdateCar();
    }
}

void MyCarsManager::UpdateSliders() {
    if (static_cast<CarDatum *>(GetCurrentDatum())->Handle != 0xFFFFFFFF) {
        FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(static_cast<CarDatum *>(GetCurrentDatum())->Handle);
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
    if (GetCurrentDatum()) {
        int handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
        FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
        bool samecar;
        if (!pSelectedCar) {
            samecar = handle == -1;
        } else {
            samecar = static_cast<int>(pSelectedCar->Handle) == handle;
        }
        if (!samecar) {
            GarageMainScreen::GetInstance()->DisableCarRendering();
            if (handle == -1) {
                cFEng::Get()->QueuePackageMessage(0x913fa282, nullptr, nullptr);
                pSelectedCar = nullptr;
                CarViewer::CancelCarLoad(static_cast<eCarViewerWhichCar>(0));
            } else {
                cFEng::Get()->QueuePackageMessage(0xa05a328e, nullptr, nullptr);
                pSelectedCar = carDB->GetCarRecordByHandle(handle);
            }
            tCarLoadTimer = RealTimer;
        }
    }
}

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
                BeginCarCustomize(static_cast<eCustomizeEntryPoint>(1), carRecord);
            }
        }
    }
}
