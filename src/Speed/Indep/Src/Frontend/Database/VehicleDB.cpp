#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Camera/CameraInfo.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fecooling.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/infractions.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/presetride.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/camerainfo.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"

#include "types.h"

#include <string.h>

int g_MaximumMaximumTimesBusted = 5;
float g_fImpoundPercentageOfOriginalCost = 0.25f;
extern TextureInfo *GetTextureInfo(unsigned int hash, int, int);
extern bool DoesStringExist(unsigned int hash);

extern CarPartDatabase CarPartDB;
PresetCar *FindFEPresetCar(unsigned int key);
int GetNumPresetCars();
PresetCar *GetPresetCarAt(int index);
extern bool ShowAllCarsInFE;
extern bool ShowAllPresetsInFE;

POVTypes GetPOVTypeFromPlayerCamera(ePlayerSettingsCameras cam) {
    switch (cam) {
        case PSC_BUMPER:
            return POV_BUMPER;
        case PSC_HOOD:
            return POV_HOOD;
        case PSC_CLOSE:
            return POV_OUTSIDE_CLOSE;
        case PSC_FAR:
            return POV_OUTSIDE_FAR;
        case PSC_SUPER_FAR:
            return POV_SUPER_FAR;
        case PSC_DRIFT:
            return POV_DRIFT;
        case PSC_PURSUIT:
            return POV_PURSUIT;
        default:
            return POV_OUTSIDE_CLOSE;
    }
}

bool IsPlayerCameraSelectable(POVTypes pov_type) {
    Attrib::Gen::ecar model_atrs(0xeec2271a, 0, nullptr);
    static unsigned int prevModelNameHash = 0;
    static unsigned int modelNameKey = 0;

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player != nullptr) {
        ISimable *isimable = player->GetSimable();
        if (isimable == nullptr) {
            const Attrib::StringKey &modelName = isimable->GetAttributes()->MODEL(0);
            if (static_cast<unsigned int>(modelName) != prevModelNameHash) {
                prevModelNameHash = modelName;
                const char *name = modelName.GetString();
                CarTypeInfo *car_type = GetCarTypeInfoFromHash(bStringHash(name));
                modelNameKey = Attrib::StringToLowerCaseKey(car_type ? car_type->CarTypeName : "");
            }
        }
    }

    model_atrs.ChangeWithDefault(modelNameKey);

    Attrib::Gen::camerainfo camera_atrs(0xeec2271a, 0, nullptr);

    switch (pov_type) {
        case POV_BUMPER:
            camera_atrs.Change(model_atrs.CameraInfo_Bumper());
            break;
        case POV_HOOD:
            camera_atrs.Change(model_atrs.CameraInfo_Hood());
            break;
        case POV_OUTSIDE_CLOSE:
            camera_atrs.Change(model_atrs.CameraInfo_Close());
            break;
        case POV_OUTSIDE_FAR:
            camera_atrs.Change(model_atrs.CameraInfo_Far());
            break;
        case POV_SUPER_FAR:
            camera_atrs.Change(model_atrs.CameraInfo_SuperFar());
            break;
        case POV_DRIFT:
            camera_atrs.Change(model_atrs.CameraInfo_Drift());
            break;
        case POV_PURSUIT:
            camera_atrs.Change(model_atrs.CameraInfo_Pursuit());
            break;
        default:
            camera_atrs.Change(0xeec2271a);
            break;
    }

    return camera_atrs.SELECTABLE(eGetCurrentViewMode() == EVIEWMODE_TWOH);
}

ePlayerSettingsCameras GetPlayerCameraFromPOVType(POVTypes pov) {
    switch (pov) {
        case 0:
            return static_cast<ePlayerSettingsCameras>(0);
        case 1:
            return static_cast<ePlayerSettingsCameras>(1);
        case 2:
            return static_cast<ePlayerSettingsCameras>(2);
        case 3:
            return static_cast<ePlayerSettingsCameras>(3);
        case 4:
            return static_cast<ePlayerSettingsCameras>(4);
        case 5:
            return static_cast<ePlayerSettingsCameras>(5);
        case 6:
            return static_cast<ePlayerSettingsCameras>(6);
        default:
            return static_cast<ePlayerSettingsCameras>(2);
    }
}

void AdjustStableHeat_EvadePursuit(int playerNum) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(playerNum);
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
        if (fe_career) {
            fe_career->AdjustHeatOnEvadePursuit();
        }
    }
}

void AdjustStableHeat_EventWin(int playerNum) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(playerNum);
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
        if (fe_career) {
            fe_career->AdjustHeatOnEventWin();
        }
    }
}

void AdjustStableImpound_EventWin(int playerNum) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(playerNum);
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
        if (fe_career) {
            if (fe_career->TheImpoundData.NotifyWin()) {
                GManager::Get().AddSMS(0x78);
            }
        }
    }
}

void AdjustStableImpound_EvadePursuit(int playerNum) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(playerNum);
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
        if (fe_career) {
            fe_career->TheImpoundData.NotifyEvade();
        }
    }
}

FEPlayerCarDB::FEPlayerCarDB() {}

FEPlayerCarDB::~FEPlayerCarDB() {}

FECarRecord *FEPlayerCarDB::GetCarRecordByHandle(uint32 handle) {
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        if (CarTable[i].Handle == handle) {
            return &CarTable[i];
        }
    }
    return nullptr;
}

FECarRecord *FEPlayerCarDB::GetCarByIndex(int index) {
    if (index < MAX_CARS_IN_STABLE) {
        return &CarTable[index];
    }
    return nullptr;
}

FECarRecord *FEPlayerCarDB::CreateNewCarRecord() {
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        if (CarTable[i].Handle == INVALID_CAR_HANDLE) {
            FECarRecord *record = &CarTable[i];

            record->Default();
            record->Handle = i;
            return record;
        }
    }
    return nullptr;
}

bool FEPlayerCarDB::CanCreateNewCarRecord() {
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        if (this->CarTable[i].Handle == INVALID_CAR_HANDLE) {
            return true;
        }
    }
    return false;
}

bool FEPlayerCarDB::CanCreateNewCustomizationRecord() {
    for (int i = 0; i < 75; i++) {
        if (Customizations[i].Handle == 0xFF) {
            return true;
        }
    }
    return false;
}

FECustomizationRecord *FEPlayerCarDB::CreateNewCustomizationRecord() {
    for (int i = 0; i < 75; i++) {
        if (Customizations[i].Handle == 0xFF) {
            FECustomizationRecord *record = &Customizations[i];

            record->Default();
            Customizations[i].Handle = i;
            return record;
        }
    }
    return nullptr;
}

FECareerRecord *FEPlayerCarDB::CreateNewCareerRecord() {
    for (int i = 0; i < 25; i++) {
        if (CareerRecords[i].Handle == 0xFF) {
            CareerRecords[i].Default();
            CareerRecords[i].Handle = i;
            return &CareerRecords[i];
        }
    }
    return nullptr;
}


uint16 FEPlayerCarDB::GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved) {
    class NumInfraction : public FEPlayerCarDB::MyCallback {
      public:
        NumInfraction(GInfractionManager::InfractionType t, bool unserved) : type(t), get_unserved(unserved) {}
        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetNumInfraction(type, get_unserved);
        }

        GInfractionManager::InfractionType type;
        bool get_unserved;
    };

    uint16 history = static_cast<uint16>(get_unserved ? SoldHistoryUnservedInfractions.GetValue(type) : SoldHistoryServedInfractions.GetValue(type));
    return static_cast<uint16>(history + ForAllCareerRecordsSum(NumInfraction(type, get_unserved)));
}

uint32 FEPlayerCarDB::GetTotalNumInfractions(bool get_unserved) {
    class TotalNumInfractions : public FEPlayerCarDB::MyCallback {
      public:
        TotalNumInfractions(bool unserved) : get_unserved(unserved) {}
        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetInfractions(get_unserved).NumInfractions();
        }

        bool get_unserved;
    };

    unsigned int history = get_unserved ? SoldHistoryUnservedInfractions.NumInfractions() : SoldHistoryServedInfractions.NumInfractions();
    return ForAllCareerRecordsSum(TotalNumInfractions(get_unserved)) + history;
}

uint16 FEPlayerCarDB::GetNumInfractionsOnCar(uint32 car_handle, bool get_unserved) {
    FECarRecord *fe_car = GetCarRecordByHandle(car_handle);
    FECareerRecord *record = GetCareerRecordByHandle(fe_car->CareerHandle);
    if (record != nullptr) {
        return static_cast<uint16>(record->GetInfractions(get_unserved).NumInfractions());
    }

    return 0;
}

uint32 FEPlayerCarDB::GetTotalBounty() {
    class Bounty : public FEPlayerCarDB::MyCallback {
        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetBounty();
        }
    };

    Bounty callback;
    return ForAllCareerRecordsSum(callback) + SoldHistoryBounty;
}

uint32 FEPlayerCarDB::GetTotalEvadedPursuits() {
    class EvadedPursuits : public FEPlayerCarDB::MyCallback {
        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetNumEvadedPursuits();
        }
    };

    EvadedPursuits callback;
    return ForAllCareerRecordsSum(callback) + SoldHistoryNumEvadedPursuits;
}

uint32 FEPlayerCarDB::GetTotalBustedPursuits() {
    class BustedPursuits : public FEPlayerCarDB::MyCallback {
      public:
        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetNumBustedPursuits();
        }
    };

    BustedPursuits callback;
    return ForAllCareerRecordsSum(callback) + SoldHistoryNumBustedPursuits;
}

uint32 FEPlayerCarDB::GetNumImpoundedCars() {
    class IsImpounded : public FEPlayerCarDB::MyCallback {
      public:
        uint32 Callback(const FECareerRecord &record) const override {
            return record.TheImpoundData.IsImpounded();
        }
    };

    IsImpounded callback;
    return ForAllCareerRecordsSum(callback);
}

uint32 FEPlayerCarDB::GetTotalFines(bool get_unserved) {
    class Fines : public FEPlayerCarDB::MyCallback {
      public:
        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetInfractions(get_unserved).GetFineValue();
        }

        bool get_unserved;
    };

    Fines callback;
    callback.get_unserved = get_unserved;
    return ForAllCareerRecordsSum(callback);
}

uint32 FEPlayerCarDB::GetNumCareerCarsWithARecord() {
    class NumCars : public FEPlayerCarDB::MyCallback {
      public:
        uint32 Callback(const FECareerRecord &record) const override {
            return 1;
        }
    };

    NumCars callback;
    return ForAllCareerRecordsSum(callback);
}

// Los cuatro bloques asm() con los 16 cuerpos de las clases locales de
// FEPlayerCarDB VIVEN AHORA AL FINAL DE zFe2.cpp.  Un asm() de ambito de
// fichero GCC lo emite DONDE ESTA ESCRITO y no se puede diferir a la cola de
// finish_file; el objetivo los tiene en 0x8017FAA4, dentro de esa cola, o sea
// 110 kB por detras de aqui.  El motivo tecnico y el aviso del DECL_UID estan
// con ellos, en zFe2.cpp.
uint32 FEPlayerCarDB::ForAllCareerRecordsSum(const MyCallback &callback) {
    uint32 val = 0;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(0xF0002)) {
            const FECareerRecord *record = GetCareerRecordByHandle(fe_car->CareerHandle);
            if (record != nullptr) {
                val += callback.Callback(*record);
            }
        }
    }

    return val;
}

void FEPlayerCarDB::BackupSoldCarHistory(uint8 sold_car) {
    FECareerRecord *careerRecord = GetCareerRecordByHandle(sold_car);
    if (careerRecord == nullptr) {
        return;
    }

    SoldHistoryBounty += careerRecord->GetBounty();
    SoldHistoryNumEvadedPursuits += careerRecord->GetNumEvadedPursuits();
    SoldHistoryNumBustedPursuits += careerRecord->GetNumBustedPursuits();
    SoldHistoryUnservedInfractions += careerRecord->GetInfractions(true);
    SoldHistoryServedInfractions += careerRecord->GetInfractions(false);
}

uint32 FEPlayerCarDB::GetPreferedCarName() {
    unsigned int max_pursuits = 0;
    unsigned int name = 0;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(0xF0002)) {
            FECareerRecord *career_record = GetCareerRecordByHandle(fe_car->CareerHandle);
            if (career_record != nullptr) {
                unsigned int pursuits = career_record->GetNumBustedPursuits() + career_record->GetNumEvadedPursuits();
                if (pursuits > max_pursuits) {
                    max_pursuits = pursuits;
                    name = fe_car->GetNameHash();
                }
            }
        }
    }

    return name;
}

int FEPlayerCarDB::GetNumQuickRaceCars() {
    return GetNumCars(0xF0004);
}

int FEPlayerCarDB::GetNumCareerCars() {
    return GetNumCars(0xF0002);
}

int FEPlayerCarDB::GetNumPurchasedCars() {
    int total = 0;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *record = &CarTable[i];
        if (record->IsValid() && record->MatchesFilter(0xF0002) && (record->FilterBits & 0x40) == 0) {
            total++;
        }
    }

    return total;
}

int FEPlayerCarDB::GetNumAvailableCareerCars() {
    int total = 0;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *record = &CarTable[i];
        if (record->IsValid()) {
            bool has_career = record->CareerHandle != 0xFF;
            if (has_career) {
                FECareerRecord *career_record = GetCareerRecordByHandle(record->CareerHandle);
                if (!career_record->TheImpoundData.IsImpounded()) {
                    total++;
                }
            }
        }
    }

    return total;
}

int FEPlayerCarDB::GetNumCars(uint32 filter) {
    int total = 0;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *car = &CarTable[i];
        if (car->IsValid() && car->MatchesFilter(filter)) {
            total++;
        }
    }

    return total;
}

FECarRecord *FEPlayerCarDB::CreateNewCustomCar(uint32 fromCar) {
    if (GetNumQuickRaceCars() < 20) {
        return CreateCar(fromCar, 0xF0004);
    }
    return nullptr;
}

FECarRecord *FEPlayerCarDB::AwardRivalCar(uint32 preset) {
    if (preset == 0x03A94520) {
        FEDatabase->GetCareerSettings()->SpecialFlags |= 0x100000;
    }

    PresetCar *presetCar = FindFEPresetCar(preset);
    FECarRecord *presetRecord = GetCarRecordByHandle(preset);
    if (presetRecord == nullptr) {
        CreateNewPresetCar(presetCar->PresetName);
    }

    FECarRecord *car = CreateNewCareerCar(presetRecord->Handle);

    car->FilterBits |= 0x40;

    FECustomizationRecord *customization = GetCustomizationRecordByHandle(car->Customization);
    RideInfo ride;

    ride.FillWithPreset(FEHashUpper(presetCar->PresetName));
    customization->WriteRideIntoRecord(&ride);

    Attrib::Gen::presetride ridePreset(Attrib::StringToLowerCaseKey(presetCar->PresetName), 0, nullptr);
    if (ridePreset.IsValid() && customization != nullptr) {
        Attrib::Gen::pvehicle vehicle(car->VehicleKey, 0, nullptr);
        if (Physics::Upgrades::ApplyPreset(vehicle, ridePreset)) {
            customization->WritePhysicsIntoRecord(vehicle);
        }
    }

    return car;
}

FECarRecord *FEPlayerCarDB::CreateNewCareerCar(uint32 fromCar) {
    if (GetNumCareerCars() > 0x18) {

        return nullptr;
    }

    FECarRecord *car = CreateCar(fromCar, 0xF0002);

    if (car != nullptr) {
        FECareerRecord *careerRecord = CreateNewCareerRecord();
        if (careerRecord == nullptr) {

            GetCustomizationRecordByHandle(car->Customization)->Handle = 0xFF;
            car->Handle = 0xFFFFFFFF;
            return nullptr;
        }
        car->CareerHandle = careerRecord->Handle;
    }

    return car;
}

FECarRecord *FEPlayerCarDB::CreateNewPresetCar(const char *preset_name) {
    unsigned int presetHash = FEHashUpper(preset_name);
    PresetCar *preset = FindFEPresetCar(presetHash);
    Attrib::Gen::pvehicle vehicle(static_cast<unsigned int>(preset->VehicleKey), 0, nullptr);

    if (!vehicle.IsValid()) {
        return nullptr;
    }

    FECarRecord *car = CreateNewCarRecord();
    if (car == nullptr || preset == nullptr) {
        return nullptr;
    }

    car->Handle = presetHash;
    FECustomizationRecord *customization = CreateNewCustomizationRecord();
    if (customization == nullptr) {
        car->Handle = 0xFFFFFFFF;
        return nullptr;
    }

    car->Customization = customization->Handle;
    SetCarToPreset(car->Handle, preset);

    Attrib::Gen::presetride ridePreset(Attrib::StringToLowerCaseKey(preset->PresetName), 0, nullptr);
    if (ridePreset.IsValid()) {
        Attrib::Gen::pvehicle vehicleWithPreset(vehicle);
        if (Physics::Upgrades::ApplyPreset(vehicleWithPreset, ridePreset)) {
            customization->WritePhysicsIntoRecord(vehicleWithPreset);
        }
        Physics::Upgrades::Clear(vehicleWithPreset);
    }

    car->FilterBits = 0xF0010;
    return car;
}

FECarRecord *FEPlayerCarDB::CreateCar(uint32 fromCar, int FilterBits) {
    FECarRecord *source = GetCarRecordByHandle(fromCar);
    if (source == nullptr) {
        return nullptr;
    }

    FECarRecord *car = CreateNewCarRecord();
    if (car == nullptr) {
        return nullptr;
    }

    *car = *source;
    FECustomizationRecord *customization = CreateNewCustomizationRecord();
    if (customization == nullptr) {
        car->Handle = 0xFFFFFFFF;
        return nullptr;
    }

    car->Customization = customization->Handle;
    car->FilterBits = (car->FilterBits & 0xFFFF0000) | static_cast<unsigned int>(FilterBits);

    RideInfo ride;
    ride.Init(car->GetType(), CarRenderUsage_Player, 0, 0);
    ride.SetRandomPaint();
    ride.SetStockParts();
    customization->WriteRideIntoRecord(&ride);
    return car;
}

void FEPlayerCarDB::DeleteCustomCar(unsigned int handle) {
    DeleteCar(handle, 4, false);
}

void FEPlayerCarDB::DeleteCareerCar(unsigned int handle, bool was_sold) {
    DeleteCar(handle, 2, was_sold);
}

bool FEPlayerCarDB::DeleteCar(uint32 handle, uint32 filter, bool was_sold) {
    FECarRecord *car = GetCarRecordByHandle(handle);

    if (car == nullptr || !car->IsValid() ||

        (car->FilterBits & filter) == 0) {
        return false;
    }
    if (was_sold) {
        BackupSoldCarHistory(car->CareerHandle);
    }

    car->Handle = 0xFFFFFFFF;
    if (car->IsCustomized()) {
        GetCustomizationRecordByHandle(car->Customization)->Handle = 0xFF;
    }
    if (car->IsCareer()) {
        GetCareerRecordByHandle(car->CareerHandle)->Handle = 0xFF;
    }

    return true;
}

void FEPlayerCarDB::DeleteAllCars() {
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        CarTable[i].Handle = INVALID_CAR_HANDLE;
    }
}

void FEPlayerCarDB::DeleteAllCustomizations() {
    for (int i = 0; i < 75; i++) {
        Customizations[i].Handle = 0xFF;
    }
}

void FEPlayerCarDB::DeleteAllCareerRecords() {
    for (int i = 0; i < 25; i++) {
        CareerRecords[i].Handle = 0xFF;
    }
}

bool FEPlayerCarDB::IsBonusCar(const char *preset_name) {
    switch (FEHashUpper(preset_name)) {
        case STRINGHASH_BONUS_SL65:
        case STRINGHASH_BONUS_C6R:
        case STRINGHASH_BONUS_GT2:
        case STRINGHASH_CASTROLGT:
        case STRINGHASH_E3_DEMO_BMW:
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
            return true;
    }
    return false;
}

void FEPlayerCarDB::Default() {
    DeleteAllCars();
    DeleteAllCustomizations();
    DeleteAllCareerRecords();

    FECarRecord *careerStart = CreateNewPresetCar("M3GTRCAREERSTART");
    careerStart->Handle = 0x12345678;
    careerStart->FilterBits = 0xF0020;

    for (int i = 0; i < GetNumPresetCars(); i++) {
        PresetCar *preset = GetPresetCarAt(i);

        if (UnlockSystem::IsBonusCarCEOnly(FEHashUpper(preset->PresetName))) {
            FECarRecord *feCar = CreateNewPresetCar(preset->PresetName);
            if (feCar) {
                feCar->FilterBits = 0xF0008;
            }
        } else if (IsBonusCar(preset->PresetName)) {
            FECarRecord *feCar = CreateNewPresetCar(preset->PresetName);
            if (feCar) {
                feCar->FilterBits = 0xF0008;
            }
        } else if (!bStrICmp(preset->PresetName, "M3GTRCAREERSTART") || ShowAllPresetsInFE) {
            FECarRecord *feCar = CreateNewPresetCar(preset->PresetName);
        }
        Physics::Upgrades::Flush();
    }

    const Attrib::Class *carClass = Attrib::Database::Get().GetClass(Attrib::ClassName::pvehicle);
    unsigned int key = carClass->GetFirstCollection();
    while (key != 0) {
        Attrib::Gen::pvehicle pv_car(key, 0, nullptr);
        if (!pv_car.IsDynamic()) {
            Attrib::Gen::frontend fe_car(pv_car.frontend(), 0, nullptr);
            Attrib::StringKey modelName;
            pv_car.MODEL(modelName);
            if (pv_car.PlayerUsable() || ShowAllCarsInFE) {
                if (modelName.IsNotEmpty()) {
                    if (!pv_car.IsDynamic()) {
                        FECarRecord *car = CreateNewCarRecord();
                        if (car) {
                            car->FEKey = fe_car.GetCollection();
                            car->VehicleKey = pv_car.GetCollection();
                            car->Default();
                        }
                    }
                }
            }
            Physics::Upgrades::Flush();
        }
        key = carClass->GetNextCollection(key);
    }

    SoldHistoryBounty = 0;
    SoldHistoryNumEvadedPursuits = 0;
    SoldHistoryNumBustedPursuits = 0;
    SoldHistoryUnservedInfractions = FEInfractionsData();
    SoldHistoryServedInfractions = FEInfractionsData();
}

char *FEPlayerCarDB::SaveToBuffer(char *buffer, int bufsize) {
    bMemCpy(buffer, this, sizeof(FEPlayerCarDB));
    return buffer + sizeof(FEPlayerCarDB);
}

char *FEPlayerCarDB::LoadFromBuffer(char *buffer, int bufsize) {
    bMemCpy(this, buffer, sizeof(FEPlayerCarDB));
    return buffer + sizeof(FEPlayerCarDB);
}

int32 FEPlayerCarDB::GetSaveBufferSize() {
    return sizeof(FEPlayerCarDB);
}

void FEPlayerCarDB::AwardBonusCars() {
    if (gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_CASTROL)) {
        if (!FEDatabase->GetCareerSettings()->HasBeenAwardedCastrolGT()) {
            FEDatabase->GetCareerSettings()->SetAwardedCastrolGT();
        }
    }
}

void FEPlayerCarDB::SetCarToPreset(uint32 car, PresetCar *preset) {
    FECarRecord *record = GetCarRecordByHandle(car);

    record->FEKey = static_cast<unsigned int>(preset->FEKey);
    record->VehicleKey = static_cast<unsigned int>(preset->VehicleKey);
    record->FilterBits = preset->FilterBits;

    FECustomizationRecord *customization = GetCustomizationRecordByHandle(record->Customization);
    if (customization != nullptr) {
        customization->BecomePreset(preset);
    }
}

void FEPlayerCarDB::BuildRideForPlayer(uint32 car, int player, RideInfo *ride) {
    FECarRecord *record = GetCarRecordByHandle(car);

    ride->Init(record->GetType(), CarRenderUsage_Player, 0, 0);
    FECustomizationRecord *customization = GetCustomizationRecordByHandle(record->Customization);
    if (customization != nullptr) {
        customization->WriteRecordIntoRide(ride);
    } else {
        ride->SetRandomPaint();
        ride->SetStockParts();
    }
}

FECustomizationRecord *FEPlayerCarDB::GetCustomizationRecordByHandle(uint8 handle) {
    if (handle > 75) {
        return nullptr;
    }
    return &Customizations[handle];
}

FECareerRecord *FEPlayerCarDB::GetCareerRecordByHandle(unsigned char handle) {
    if (handle > 25 || this->CareerRecords[handle].Handle == 0xFF) {
        return nullptr;
    }
    return &this->CareerRecords[handle];
}

bool FEPlayerCarDB::WriteRecordIntoPhysics(uint32 car, Attrib::Gen::pvehicle &attributes) {
    FECarRecord *record = GetCarRecordByHandle(car);
    if (record == nullptr) {
        return false;
    }
    FECustomizationRecord *customization = GetCustomizationRecordByHandle(record->Customization);
    if (customization == nullptr) {
        return false;
    }
    return customization->WriteRecordIntoPhysics(attributes);
}

FECarRecord::FECarRecord() {
    Handle = 0xFFFFFFFF;
    FEKey = 0;
    VehicleKey = 0;
    Customization = 0xFF;
    CareerHandle = 0xFF;
    FilterBits = 0;
}

FECarRecord &FECarRecord::operator=(const FECarRecord &other_record) {
    FEKey = other_record.FEKey;
    VehicleKey = other_record.VehicleKey;
    FilterBits = other_record.FilterBits;
    return *this;
}

void FECarRecord::Default() {
    this->Customization = 0xFF;
    this->CareerHandle = 0xFF;
    this->FilterBits = 0;

    bool has_car = (this->Handle != -1);

    if (!has_car) {
        return;
    }

    Attrib::Gen::pvehicle pvehicle(this->VehicleKey, 0, nullptr);
    Attrib::Gen::frontend frontend(this->FEKey, 0, nullptr);

    switch (frontend.region()) {
    case 0:
        this->FilterBits |= 0x10000;
        break;
    case 1:
        this->FilterBits |= 0x20000;
        break;
    case 2:
        this->FilterBits |= 0x40000;
        break;
    case 3:
        this->FilterBits |= 0x80000;
        break;
    }

    if (frontend.region() == 3 || !frontend.IsCustomizable()) {
        this->FilterBits |= 0x20;
    } else {
        this->FilterBits |= 0x1;
    }
}

CarType FECarRecord::GetType() {
    Attrib::Gen::pvehicle pvehicle(this->VehicleKey, 0, nullptr);

    return CarPartDB.GetCarType(bStringHash(pvehicle.MODEL().GetString()));
}

unsigned int FECarRecord::GetCost() {
    Attrib::Gen::frontend frontend(FEKey, 0, 0);

    return frontend.Cost();
}

unsigned int FECarRecord::GetReleaseFromImpoundCost() {
    return static_cast<int>(static_cast<float>(GetCost()) * g_fImpoundPercentageOfOriginalCost);
}

unsigned int FECarRecord::GetNameHash() {
    const char *manu = GetManufacturerName();
    if (bStrCmp(manu, "")) {
        char buf[128];
        Attrib::Gen::frontend frontend(FEKey, 0, nullptr);
        FEngSNPrintf(buf, 0x80, "CARNAME_%s_%s", manu, frontend.CollectionName());
        unsigned int hash = FEHashUpper(buf);
        if (DoesStringExist(hash)) {
            return hash;
        }
    }
    return 0x9BB9CCC3;
}

unsigned int FECarRecord::GetLogoHash() {
    const char *manu = GetManufacturerName();
    if (bStrCmp(manu, "")) {
        char buf[128];
        Attrib::Gen::frontend frontend(FEKey, 0, 0);
        FEngSNPrintf(buf, 0x80, "SECONDARY_LOGO_%s", frontend.CollectionName());
        unsigned int texHash = FEHashUpper(buf);
        if (GetTextureInfo(texHash, 0, 0)) {
            return texHash;
        }
    }
    return FEHashUpper("GENERIC_LOGO_256");
}

unsigned int FECarRecord::GetManuLogoHash() {
    const char *manu = GetManufacturerName();
    if (bStrCmp(manu, "")) {
        char buf[128];
        FEngSNPrintf(buf, 0x80, "CARSELECT_MANUFACTURER_%s", manu);
        unsigned int texHash = FEHashUpper(buf);
        if (GetTextureInfo(texHash, 0, 0)) {
            return texHash;
        }
    }
    return FEHashUpper("GENERIC_LOGO_128");
}

const char *FECarRecord::GetManufacturerName() {
    Attrib::Gen::frontend fe(FEKey, 0, 0);
    unsigned char Manufacturer = static_cast<unsigned char>(fe.manufacturer());
    switch (Manufacturer) {
        case 0:
            return "";
        case 1:
            return "BMW";
        case 2:
            return "FORD";
        case 3:
            return "SUBARU";
        case 4:
            return "PORSCHE";
        case 5:
            return "AUDI";
        case 6:
            return "MAZDA";
        case 7:
            return "GENERAL_MOTORS";
        case 8:
            return "DODGE";
        case 9:
            return "TOYOTA";
        case 10:
            return "MITSUBISHI";
        case 11:
            return "MCLAREN";
        case 12:
            return "MERCEDES";
        case 13:
            return "NISSAN";
        case 14:
            return "LOTUS";
        case 15:
            return "LAMBORGHINI";
        case 16:
            return "RENAULT";
        case 17:
            return "LEXUS";
        case 18:
            return "PONTIAC";
        case 19:
            return "CHEVROLET";
        case 20:
            return "VAUXHALL";
        case 21:
            return "ASTONMARTIN";
        case 22:
            return "VOLKSWAGEN";
        case 23:
            return "FIAT";
        case 24:
            return "CADILLAC";
        case 25:
            return "CORVETTE";
        default:
            return "";
    }
}

bool FECarRecord::MatchesFilter(int theFilter) {
    int theRegion = theFilter & FE_CAR_FILTER_REGION_MASK;
    int theList = theFilter & FE_CAR_FILTER_LIST_MASK;
    int myRegion = FilterBits & FE_CAR_FILTER_REGION_MASK;
    int myList = FilterBits & FE_CAR_FILTER_LIST_MASK;
    bool regionCompare = (theRegion & myRegion) != 0;
    bool listCompare = (theList & myList) != 0;

    return regionCompare && listCompare;
}

const char *FECarRecord::GetDebugName() {
    Attrib::Gen::pvehicle vehicle(VehicleKey, 0, 0);
    const unsigned char *vehicleLayout = reinterpret_cast<const unsigned char *>(vehicle.GetLayoutPointer());

    return *reinterpret_cast<const char *const *>(vehicleLayout + 0x24);
}

uint32 GetFECarNameHashFromFEKey(Attrib::Key feKey) {
    if (!feKey) {
        return 0;
    }
    FECarRecord rec;
    rec.FEKey = feKey;
    return rec.GetNameHash();
}

void FECustomizationRecord::Default() {
    for (int i = 0; i < 139; i++) {
        InstalledPartIndices[i] = INVALID_CAR_PART_RECORD_INDEX;
    }

    InstalledPhysics.Default();

    for (int i = 0; i < NUM_CUSTOM_TUNINGS; i++) {
        Tunings[i].Default();
    }

    ActiveTuning = CTT_SETTING_1;
    Preset = 0;
}

bool FECustomizationRecord::WriteRecordIntoPhysics(Attrib::Gen::pvehicle &attributes) const {
    if (!Physics::Upgrades::SetPackage(attributes, InstalledPhysics)) {
        return false;
    }
    return true;
}

void FECustomizationRecord::WritePhysicsIntoRecord(const Attrib::Gen::pvehicle &attributes) {
    Physics::Upgrades::GetPackage(attributes, InstalledPhysics);
}

CarPart *FECustomizationRecord::GetInstalledPart(CarType cartype, int carslotid) const {
    return CarPartDB.GetCarPartByIndex(static_cast<short>(InstalledPartIndices[carslotid]));
}

void FECustomizationRecord::SetInstalledPart(int carslotid, CarPart *part) {
    if (part != nullptr) {
        CarPartDB.GetPartIndex(part);

        InstalledPartIndices[carslotid] = static_cast<short>(CarPartDB.GetPartIndex(part));
    } else {
        InstalledPartIndices[carslotid] = INVALID_CAR_PART_RECORD_INDEX;
    }
}

void FECustomizationRecord::WriteRecordIntoRide(RideInfo *ride) const {
    for (int i = 0; i <= 0x8A; i++) {
        ride->SetPart(i, GetInstalledPart(ride->Type, i), true);
    }
}

void FECustomizationRecord::WriteRideIntoRecord(const RideInfo *ride) {
    for (int i = 0; i <= 0x8A; i++) {
        SetInstalledPart(i, ride->GetPart(i));
    }
}

FECustomizationRecord::FECustomizationRecord() : InstalledPhysics(), Tunings(), Handle(INVALID_CUSTOMIZATION_HANDLE) {
    Default();
}

void FECustomizationRecord::BecomePreset(PresetCar *preset) {
    Default();

    CarType cartype = CarPartDB.GetCarType(bStringHash(preset->CarTypeName));
    for (int i = 0; i <= 0x8A; i++) {
        unsigned int part_name_hash = preset->PartNameHashes[i];
        if (part_name_hash > 1) {
            struct CarPart *part = CarPartDB.NewGetCarPart(cartype, i, part_name_hash, nullptr, -1);
            InstalledPartIndices[i] = static_cast<short>(CarPartDB.GetPartIndex(part));
        }
    }

    Preset = bStringHashUpper(preset->PresetName);
}

FEInfractionsData::FEInfractionsData(unsigned int infractions) {
    *this = FEInfractionsData();

    if ((infractions & 8) != 0) {
        Assault++;
    }
    if ((infractions & 0x20) != 0) {
        Damage++;
    }
    if ((infractions & 0x10) != 0) {
        HitAndRun++;
    }
    if ((infractions & 0x80) != 0) {
        OffRoad++;
    }
    if ((infractions & 2) != 0) {
        Racing++;
    }
    if ((infractions & 4) != 0) {
        Reckless++;
    }
    if ((infractions & 0x40) != 0) {
        Resist++;
    }
    if ((infractions & 1) != 0) {
        Speeding++;
    }
}

unsigned short FEInfractionsData::NumInfractions() const {
    return Speeding + Racing + Reckless + Assault + HitAndRun + Damage + Resist + OffRoad;
}

void FEInfractionsData::operator+=(const FEInfractionsData &rhs) {
    Speeding += rhs.Speeding;
    Racing += rhs.Racing;
    Reckless += rhs.Reckless;
    Assault += rhs.Assault;
    HitAndRun += rhs.HitAndRun;
    Damage += rhs.Damage;
    Resist += rhs.Resist;
    OffRoad += rhs.OffRoad;
}

unsigned short FEInfractionsData::GetValue(GInfractionManager::InfractionType type) const {
    switch (type) {
    case GInfractionManager::kInfraction_Assault:
        return Assault;
    case GInfractionManager::kInfraction_Damage:
        return Damage;
    case GInfractionManager::kInfraction_HitAndRun:
        return HitAndRun;
    case GInfractionManager::kInfraction_OffRoad:
        return OffRoad;
    case GInfractionManager::kInfraction_Racing:
        return Racing;
    case GInfractionManager::kInfraction_Reckless:
        return Reckless;
    case GInfractionManager::kInfraction_Resist:
        return Resist;
    case GInfractionManager::kInfraction_Speeding:
        return Speeding;
    default:
        return 0;
    }
}

unsigned int FEInfractionsData::GetFineValue() const {
    if (NumInfractions() == 0) {
        return 0;
    }
    unsigned int fines = 0;
    Attrib::Gen::infractions AssaultFine(Attrib::StringToKey("assault"), 0, nullptr);
    if (AssaultFine.IsValid()) {
        fines = static_cast<unsigned int>(Assault) * AssaultFine.amount();
    }
    Attrib::Gen::infractions DamageFine(Attrib::StringToKey("damage"), 0, nullptr);
    if (DamageFine.IsValid()) {
        fines += static_cast<unsigned int>(Damage) * DamageFine.amount();
    }
    Attrib::Gen::infractions HitAndRunFine(Attrib::StringToKey("hit_and_run"), 0, nullptr);
    if (HitAndRunFine.IsValid()) {
        fines += static_cast<unsigned int>(HitAndRun) * HitAndRunFine.amount();
    }
    Attrib::Gen::infractions OffRoadFine(Attrib::StringToKey("off_road"), 0, nullptr);
    if (OffRoadFine.IsValid()) {
        fines += static_cast<unsigned int>(OffRoad) * OffRoadFine.amount();
    }
    Attrib::Gen::infractions RacingFine(Attrib::StringToKey("racing"), 0, nullptr);
    if (RacingFine.IsValid()) {
        fines += static_cast<unsigned int>(Racing) * RacingFine.amount();
    }
    Attrib::Gen::infractions RecklessFine(Attrib::StringToKey("reckless_driving"), 0, nullptr);
    if (RecklessFine.IsValid()) {
        fines += static_cast<unsigned int>(Reckless) * RecklessFine.amount();
    }
    Attrib::Gen::infractions ResistFine(Attrib::StringToKey("resisting_arrest"), 0, nullptr);
    if (ResistFine.IsValid()) {
        fines += static_cast<unsigned int>(Resist) * ResistFine.amount();
    }
    Attrib::Gen::infractions SpeedingFine(Attrib::StringToKey("speeding"), 0, nullptr);
    if (SpeedingFine.IsValid()) {
        fines += static_cast<unsigned int>(Speeding) * SpeedingFine.amount();
    }
    return fines;
}

void FEImpoundData::Default() {
    MaxBusted = 3;
    TimesBusted = 0;
    ImpoundedState = 0;
    DaysBeforeRelease = 0;
    EvadeCount = 0;
    Pad1 = Pad2 = 0;
}

void FEImpoundData::BecomeImpounded(eImpoundReasons reason) {
    ImpoundedState = reason;
    TimesBusted = MaxBusted;
    DaysBeforeRelease = 5;
}

void FEImpoundData::NotifyPlayerPaidToRelease() {
    TimesBusted = 0;
    ImpoundedState = 0;
    DaysBeforeRelease = 0;
}

void FEImpoundData::NotifyPlayerUsedMarkerToRelease() {
    NotifyPlayerPaidToRelease();
}

bool FEImpoundData::NotifyWin() {
    bool impounded = ImpoundedState != 0;
    if (impounded && ((DaysBeforeRelease == 0 || --DaysBeforeRelease == 0) && ImpoundedState != IMPOUND_RELEASED)) {
        ImpoundedState = IMPOUND_RELEASED;
        return true;
    }
    return false;
}

bool FEImpoundData::NotifyBusted() {
    EvadeCount = 0;
    TimesBusted = TimesBusted + 1;
    return TimesBusted >= MaxBusted;
}

bool FEImpoundData::NotifyEvade() {
    bool impounded = ImpoundedState != 0;

    if (!impounded) {
        EvadeCount = EvadeCount + 1;
        if (EvadeCount > 2) {
            EvadeCount = 0;
            TimesBusted--;
        }
        if (TimesBusted < 0) {
            TimesBusted = 0;
        }
    }

    return NotifyWin();
}

bool FEImpoundData::CanAddMaxBusted() {
    if (static_cast<int>(MaxBusted) < g_MaximumMaximumTimesBusted && !IsImpounded()) {
        return true;
    }
    return false;
}

void FEImpoundData::AddMaxBusted() {
    MaxBusted++;
    if (static_cast<int>(MaxBusted) > g_MaximumMaximumTimesBusted) {
        MaxBusted = static_cast<unsigned char>(g_MaximumMaximumTimesBusted);
    }
}

void FECareerRecord::Default() {
    Handle = 0xFF;
    TheImpoundData.Default();
    VehicleHeat = 0.0f;
    Bounty = 0;
    NumEvadedPursuits = 0;
    NumBustedPursuits = 0;
    FEInfractionsData *pUnserved = &UnservedInfractions;
    FEInfractionsData *pServed = &ServedInfractions;
    {
        FEInfractionsData unserved;
        *pUnserved = unserved;
    }
    {
        FEInfractionsData served;
        *pServed = served;
    }
}

void FECareerRecord::SetVehicleHeat(float h) {
    VehicleHeat = h;
}

float FECareerRecord::GetVehicleHeat() {
    return VehicleHeat;
}

void FECareerRecord::AdjustHeatOnEventWin() {
    Attrib::Gen::pursuitlevels pursuitLevels(0xEEC2271A, 0, 0);

    VehicleHeat = VehicleHeat * pursuitLevels.EventWinHeatAdjust();
}

void FECareerRecord::AdjustHeatOnEvadePursuit() {
    Attrib::Gen::pursuitlevels pursuitLevels(0xEEC2271A, 0, 0);

    VehicleHeat = VehicleHeat * pursuitLevels.EvadeSuccessHeatAdjust();
}

void FECareerRecord::AdjustHeatOnDecalApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewDecal() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnPaintApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewPaint() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnVinylApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewVinyl() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnBodyKitApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewBodyKit() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnHoodApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewHood() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnRimApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewRim() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnRimPaintApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewRimPaint() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnRoofScoopApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewRoofScoop() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnSpoilerApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewSpoiler() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnWindowTintApplied(float extraAdjust) {
    Attrib::Gen::fecooling cooling(0xEEC2271A, 0, 0);

    VehicleHeat *= cooling.NewWindowTint() * extraAdjust;
}

void FECareerRecord::CommitPursuitCarData(unsigned int infractions, uint32 accumulated_bounty, bool pursuit_evaded) {
    UnservedInfractions += FEInfractionsData(infractions);

    if (pursuit_evaded) {
        Bounty += accumulated_bounty;
        NumEvadedPursuits++;
    } else {
        NumBustedPursuits++;
    }
}

void FECareerRecord::WaiveIncractions(unsigned int infractions) {
    if ((infractions & GInfractionManager::kInfraction_Assault) != 0) {
        UnservedInfractions.Assault--;
    }
    if ((infractions & GInfractionManager::kInfraction_Damage) != 0) {
        UnservedInfractions.Damage--;
    }
    if ((infractions & GInfractionManager::kInfraction_HitAndRun) != 0) {
        UnservedInfractions.HitAndRun--;
    }
    if ((infractions & GInfractionManager::kInfraction_OffRoad) != 0) {
        UnservedInfractions.OffRoad--;
    }
    if ((infractions & GInfractionManager::kInfraction_Racing) != 0) {
        UnservedInfractions.Racing--;
    }
    if ((infractions & GInfractionManager::kInfraction_Reckless) != 0) {
        UnservedInfractions.Reckless--;
    }
    if ((infractions & GInfractionManager::kInfraction_Resist) != 0) {
        UnservedInfractions.Resist--;
    }
    if ((infractions & GInfractionManager::kInfraction_Speeding) != 0) {
        UnservedInfractions.Speeding--;
    }
    NumBustedPursuits--;
}

void FECareerRecord::ServeAllIncractions() {
    FEInfractionsData *pUnserved = &UnservedInfractions;
    ServedInfractions += UnservedInfractions;
    {
        FEInfractionsData cleared;
        *pUnserved = cleared;
    }
}

uint32 FECareerRecord::GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved) const {
    return GetInfractions(get_unserved).GetValue(type);
}
