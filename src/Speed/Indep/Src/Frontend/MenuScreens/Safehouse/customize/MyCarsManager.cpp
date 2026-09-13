#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/MyCarsManager.hpp"

#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/SoundHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"

void CarDatum::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == __BUTTON_PRESSED__ || msg == __PAD_ACCEPT__) {
        if (Handle == INVALID_CAR_HANDLE) {
            FEDatabase->SetGameMode(eFE_GAME_MODE_CUSTOMIZE);
            cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
        } else {
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            FECarRecord *carRecord = stable->GetCarRecordByHandle(Handle);
            if (carRecord != nullptr) {
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
      HandlingSlider(),                  //
      bGoToShowcase(false),              //
      pSelectedCar(nullptr) {
    SetClickToSelectMode(true);
    Setup();
}

eMenuSoundTriggers MyCarsManager::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    ArrayScrollerMenu::NotifySoundMessage(msg, maybe);
    if (msg == FEHASH_SOUND_BACK) {
        return UISND_COMMON_BACK;
    }
    return maybe;
}

// UNSOLVED: CarLoadDelay
void MyCarsManager::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, obj, param1, param2);

    switch (msg) {
        case dialog_message_no: // TODO: get from define
            break;
        case FEHASH_INITCOMPLETE:
            FEDatabase->BackupCarStable();
            break;
        case FEMSG_SCREEN_TICK:
            if (tCarLoadTimer.IsSet()) {
                float CarLoadDelay = (RealTimer - tCarLoadTimer).GetSeconds();
                if (0.25f <= CarLoadDelay && (pSelectedCar != nullptr)) {
                    RideInfo ride;
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    stable->BuildRideForPlayer(pSelectedCar->Handle, 0, &ride);
                    CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
                    tCarLoadTimer.UnSet();
                }
            }
            break;

        case __PAD_BACK__: {
            extern int IsMemcardEnabled;
            if (pSelectedCar == nullptr) {
                RideInfo ride;
                FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                stable->BuildRideForPlayer(FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(0), 0, &ride);
                CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
            }
            if (FEDatabase->IsCarStableDirty() && IsMemcardEnabled) {
                MemcardEnter(GetPackageName(), "MainMenu.fng", 0x2000b3, nullptr, nullptr, 0, 0);
            } else {
                cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            }
            break;
        }
        case __PAD_BUTTON5__: {
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            uint32 handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
            FECarRecord *car = stable->GetCarRecordByHandle(handle);
            if (car->IsValid()) {
                DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes,
                                                dialog_message_no, dialog_message_no, first_dialog_button2, 0x4f68196e);
            }
            break;
        }
        case dialog_message_yes: {
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            uint32 car = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
            FEDatabase->NotifyDeleteCar(car);
            stable->DeleteCustomCar(car);
            RefreshCarList();
            if (GetNumDatum() >= 2) {
                CarDatum *d = static_cast<CarDatum *>(GetCurrentDatum());
                pSelectedCar = stable->GetCarRecordByHandle(d->Handle);
            } else {
                pSelectedCar = nullptr;
            }
            RefreshHeader();
            break;
        }
        case __PAD_BUTTON0__:
            if (pSelectedCar != nullptr) {
                cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
                bGoToShowcase = true;
            }
            break;
        case FEHASH_EXITCOMPLETE:
            if (bGoToShowcase) {
                Showcase::FromArgs = 0;
                Showcase::FromPackage = GetPackageName();
                cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(pSelectedCar), 0, false);
            }
            break;

        case 0x1bcf34dc:
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
    if (FEDatabase->GetPlayerCarStable(0)->GetNumQuickRaceCars() < 20) {
        if (FEDatabase->GetPlayerCarStable(0)->CanCreateNewCustomizationRecord()) {
            if (!FEDatabase->GetPlayerCarStable(0)->CanCreateNewCarRecord()) {
                return false;
            }
            return true;
        }
    }
    return false;
}

void MyCarsManager::RefreshCarList() {
    int filter = 0xf0004;
    ClearData();
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    int start_index = 0;
    int added = 0;
    for (int i = 0; i < 200; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(filter)) {
            AddDatum(new ("CarDatum", 0) CarDatum(fe_car->GetManuLogoHash(), fe_car->GetNameHash(), fe_car->Handle));
            if (FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(0) == fe_car->Handle) {
                start_index = added;
            }
            added++;
        }
    }
    if (CanAddMoreCars()) {
        AddDatum(new ("CarDatum", 0) CarDatum(0x91d3a6ba, 0x29ae0714, INVALID_CAR_HANDLE));
    }
    SetInitialPosition(start_index);
    GarageMainScreen::GetInstance()->DisableCarRendering();
    UpdateCar();
}

void MyCarsManager::RefreshHeader() {
    ArrayScrollerMenu::RefreshHeader();

    if (GetCurrentDatum() == nullptr) {
        return;
    }

    uint32 handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car = stable->GetCarRecordByHandle(handle);
    const u32 FEObj_addcar = 0x42ea22dd;
    const u32 FEObj_editdeletecar = 0x06d41ccc;
    if (car->IsValid()) {
        FEngSetTextureHash(GetPackageName(), 0xb05dd708, stable->GetCarRecordByHandle(handle)->GetLogoHash());
    }
    if (static_cast<CarDatum *>(GetCurrentDatum())->Handle == INVALID_CAR_HANDLE) {
        FEngSetLanguageHash(GetPackageName(), 0xbfa25765, 0xc2598bd8);
        cFEng::Get()->QueuePackageMessage(FEObj_addcar, GetPackageName(), nullptr);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xbfa25765, 0xc9847935);
        cFEng::Get()->QueuePackageMessage(FEObj_editdeletecar, GetPackageName(), nullptr);
        for (int i = 0; i < GRace::kRaceType_NumTypes; i++) {
            FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i))->SetSelectedCar(static_cast<CarDatum *>(GetCurrentDatum())->Handle, 0);
        }
    }
    FEPrintf(GetPackageName(), 0x6f25a248, "%d", GetCurrentDatumNum());
    FEPrintf(GetPackageName(), 0xb2037bdc, "%d", GetNumDatum());
    UpdateSliders();
    UpdateCar();
}

void MyCarsManager::UpdateSliders() {
    uint32 handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;
    if (handle != INVALID_CAR_HANDLE) {
        FECarRecord *car = FEDatabase->GetPlayerCarRecordByHandle(0, handle);
        Physics::Info::Performance performance;
        if (car != nullptr) {
            Attrib::Gen::pvehicle pvehicle(car->VehicleKey, 0, nullptr);
            if (car->IsCustomized()) {
                FEDatabase->GetPlayerCarStable(0)->GetCustomizationRecordByHandle(car->Customization)->WriteRecordIntoPhysics(pvehicle);
            }
            Physics::Info::EstimatePerformance(pvehicle, performance);
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
    if (GetCurrentDatum() == nullptr) {
        return;
    }

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    uint32 handle = static_cast<CarDatum *>(GetCurrentDatum())->Handle;

    if ((pSelectedCar == nullptr) ? handle == INVALID_CAR_HANDLE : pSelectedCar->Handle == handle) {
        return;
    }

    GarageMainScreen::GetInstance()->DisableCarRendering();
    if (handle == INVALID_CAR_HANDLE) {
        const u32 FEObj_LOADERLEAVE = 0x913fa282;
        cFEng::Get()->QueuePackageMessage(FEObj_LOADERLEAVE, nullptr, nullptr);
        pSelectedCar = nullptr;
        CarViewer::CancelCarLoad(eCARVIEWER_PLAYER1_CAR);
    } else {
        const u32 FEObj_LOADERAPPEAR = 0xa05a328e;
        cFEng::Get()->QueuePackageMessage(FEObj_LOADERAPPEAR, nullptr, nullptr);
        pSelectedCar = stable->GetCarRecordByHandle(handle);
    }

    tCarLoadTimer = RealTimer;
}
