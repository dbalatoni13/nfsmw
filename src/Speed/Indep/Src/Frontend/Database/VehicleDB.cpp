#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Camera/CameraInfo.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Generated/CarTypes.hpp"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fecooling.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/infractions.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/presetride.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/camerainfo.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Misc/attribuserinclude.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"

#include "types.h"

#include <string.h>

extern int g_MaximumMaximumTimesBusted;
extern float g_fImpoundPercentageOfOriginalCost;
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
    static uint32 prevModelNameHash = 0;
    static uint32 modelNameKey = 0;
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player != nullptr) {
        ISimable *isimable = player->GetSimable();
        if (isimable == nullptr) {
            // TODO: BUG? this is dereferencing a null pointer
            const Attrib::StringKey &modelName = isimable->GetAttributes()->MODEL(0);

            if (static_cast<uint32>(modelName) != prevModelNameHash) {
                prevModelNameHash = modelName;
                const char *name = GetCarTypeInfo(bStringHash(modelName.GetString()))->GetName();

                if (name == nullptr) {
                    name = "";
                }

                modelNameKey = Attrib::StringToLowerCaseKey(name);
            }
        }
    }

    model_atrs.ChangeWithDefault(modelNameKey);

    Attrib::Gen::camerainfo camera_atrs(0xeec2271a, 0, nullptr);

    int index;

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

    return camera_atrs.SELECTABLE(static_cast<unsigned int>(eGetCurrentViewMode() == EVIEWMODE_TWOH));
}

ePlayerSettingsCameras GetPlayerCameraFromPOVType(POVTypes type) {
    switch (type) {
        case POV_BUMPER:
            return PSC_BUMPER;
        case POV_HOOD:
            return PSC_HOOD;
        case POV_OUTSIDE_CLOSE:
            return PSC_CLOSE;
        case POV_OUTSIDE_FAR:
            return PSC_FAR;
        case POV_SUPER_FAR:
            return PSC_SUPER_FAR;
        case POV_DRIFT:
            return PSC_DRIFT;
        case POV_PURSUIT:
            return PSC_PURSUIT;
        default:
            return PSC_CLOSE;
    }
}

void AdjustStableHeat_EvadePursuit(int playerNum) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(playerNum);
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
        if (fe_career != nullptr) {
            fe_career->AdjustHeatOnEvadePursuit();
        }
    }
}

void AdjustStableHeat_EventWin(int playerNum) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(playerNum);
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
        if (fe_career != nullptr) {
            fe_career->AdjustHeatOnEventWin();
        }
    }
}

void AdjustStableImpound_EventWin(int playerNum) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(playerNum);
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        FECareerRecord *fe_career = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
        if (fe_career != nullptr) {
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
        if (fe_career != nullptr) {
            fe_career->TheImpoundData.NotifyEvade();
        }
    }
}

FEPlayerCarDB::FEPlayerCarDB() : CarTable(), CareerRecords() {}

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
            CarTable[i].Default();
            CarTable[i].Handle = i;
            return &CarTable[i];
        }
    }
    return nullptr;
}

bool FEPlayerCarDB::CanCreateNewCarRecord() {
    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        if (CarTable[i].Handle == INVALID_CAR_HANDLE) {
            return true;
        }
    }
    return false;
}

bool FEPlayerCarDB::CanCreateNewCustomizationRecord() {
    for (int i = 0; i < 75; i++) {
        if (Customizations[i].Handle == INVALID_CUSTOMIZATION_HANDLE) {
            return true;
        }
    }
    return false;
}

FECustomizationRecord *FEPlayerCarDB::CreateNewCustomizationRecord() {
    for (int i = 0; i < 75; i++) {
        if (Customizations[i].Handle == INVALID_CUSTOMIZATION_HANDLE) {
            Customizations[i].Default();
            Customizations[i].Handle = i;
            return &Customizations[i];
        }
    }
    return nullptr;
}

FECareerRecord *FEPlayerCarDB::CreateNewCareerRecord() {
    for (int i = 0; i < MAX_CAREER_TOTAL_CARS; i++) {
        if (CareerRecords[i].Handle == INVALID_CAREER_HANDLE) {
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
        ~NumInfraction() override {}

        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetNumInfraction(type, get_unserved);
        }

        GInfractionManager::InfractionType type;
        bool get_unserved;
    };

    uint16 history = get_unserved ? SoldHistoryUnservedInfractions.GetValue(type) : SoldHistoryServedInfractions.GetValue(type);
    return ForAllCareerRecordsSum(NumInfraction(type, get_unserved)) + history;
}

uint32 FEPlayerCarDB::GetTotalNumInfractions(bool get_unserved) {
    class TotalNumInfractions : public FEPlayerCarDB::MyCallback {
      public:
        TotalNumInfractions(bool unserved) : get_unserved(unserved) {}
        ~TotalNumInfractions() override {}

        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetInfractions(get_unserved).NumInfractions();
        }

        bool get_unserved;
    };

    uint16 history = get_unserved ? SoldHistoryUnservedInfractions.NumInfractions() : SoldHistoryServedInfractions.NumInfractions();
    return ForAllCareerRecordsSum(TotalNumInfractions(get_unserved)) + history;
}

uint16 FEPlayerCarDB::GetNumInfractionsOnCar(uint32 car_handle, bool get_unserved) {
    FECarRecord *fe_car = GetCarRecordByHandle(car_handle);
    FECareerRecord *record = GetCareerRecordByHandle(fe_car->CareerHandle);
    if (record != nullptr) {
        return record->GetInfractions(get_unserved).NumInfractions();
    }

    return 0;
}

uint32 FEPlayerCarDB::GetTotalBounty() {
    class Bounty : public FEPlayerCarDB::MyCallback {
      public:
        Bounty() {}
        ~Bounty() override {}

        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetBounty();
        }
    };

    return ForAllCareerRecordsSum(Bounty()) + SoldHistoryBounty;
}

uint32 FEPlayerCarDB::GetTotalEvadedPursuits() {
    class EvadedPursuits : public FEPlayerCarDB::MyCallback {
      public:
        EvadedPursuits() {}
        ~EvadedPursuits() override {}

        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetNumEvadedPursuits();
        }
    };

    return ForAllCareerRecordsSum(EvadedPursuits()) + SoldHistoryNumEvadedPursuits;
}

uint32 FEPlayerCarDB::GetTotalBustedPursuits() {
    class BustedPursuits : public FEPlayerCarDB::MyCallback {
      public:
        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetNumBustedPursuits();
        }
    };

    return ForAllCareerRecordsSum(BustedPursuits()) + SoldHistoryNumBustedPursuits;
}

uint32 FEPlayerCarDB::GetNumImpoundedCars() {
    class IsImpounded : public FEPlayerCarDB::MyCallback {
      public:
        uint32 Callback(const FECareerRecord &record) const override {
            return static_cast<uint32>(record.TheImpoundData.IsImpounded());
        }
    };

    return ForAllCareerRecordsSum(IsImpounded());
}

uint32 FEPlayerCarDB::GetTotalFines(bool get_unserved) {
    class Fines : public FEPlayerCarDB::MyCallback {
      public:
        Fines(bool unserved) : get_unserved(unserved) {}
        ~Fines() override {}

        uint32 Callback(const FECareerRecord &record) const override {
            return record.GetInfractions(get_unserved).GetFineValue();
        }

        bool get_unserved;
    };

    return ForAllCareerRecordsSum(Fines(get_unserved));
}

uint32 FEPlayerCarDB::GetNumCareerCarsWithARecord() {
    class NumCars : public FEPlayerCarDB::MyCallback {
      public:
        uint32 Callback(const FECareerRecord &record) const override {
            return 1;
        }
    };

    return ForAllCareerRecordsSum(NumCars());
}

uint32 FEPlayerCarDB::ForAllCareerRecordsSum(const MyCallback &Callback) {
    uint32 val = 0;
    int filter = FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_CAREER;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(filter)) {
            const FECareerRecord *record = GetCareerRecordByHandle(fe_car->CareerHandle);
            if (record != nullptr) {
                val += Callback.Callback(*record);
            }
        }
    }

    return val;
}

void FEPlayerCarDB::BackupSoldCarHistory(uint8 sold_car) {
    FECareerRecord *record = GetCareerRecordByHandle(sold_car);
    if (record == nullptr) {
        return;
    }

    SoldHistoryBounty += record->GetBounty();
    SoldHistoryNumEvadedPursuits += record->GetNumEvadedPursuits();
    SoldHistoryNumBustedPursuits += record->GetNumBustedPursuits();
    SoldHistoryUnservedInfractions += record->GetInfractions(true);
    SoldHistoryServedInfractions += record->GetInfractions(false);
}

uint32 FEPlayerCarDB::GetPreferedCarName() {
    uint32 highestTimes = 0;
    uint32 highestCar = 0;
    int filter = FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_CAREER;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *fe_car = GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(filter)) {
            FECareerRecord *record = GetCareerRecordByHandle(fe_car->CareerHandle);
            if (record != nullptr) {
                uint32 times = record->GetNumBustedPursuits() + record->GetNumEvadedPursuits();
                if (times > highestTimes) {
                    highestTimes = times;
                    highestCar = fe_car->GetNameHash();
                }
            }
        }
    }

    return highestCar;
}

int FEPlayerCarDB::GetNumQuickRaceCars() {
    return GetNumCars(FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_QUICK_RACE);
}

int FEPlayerCarDB::GetNumCareerCars() {
    return GetNumCars(FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_CAREER);
}

int FEPlayerCarDB::GetNumPurchasedCars() {
    int numCars = 0;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *car = &CarTable[i];
        if (car->IsValid() && car->MatchesFilter(FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_CAREER) &&
            (car->FilterBits & FE_CAR_FILTER_PINKSLIP) == 0) {
            numCars++;
        }
    }

    return numCars;
}

int FEPlayerCarDB::GetNumAvailableCareerCars() {
    int numCars = 0;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *car = &CarTable[i];
        if (car->IsValid() && car->IsCareer()) {
            FECareerRecord *career = GetCareerRecordByHandle(car->CareerHandle);
            if (!career->TheImpoundData.IsImpounded()) {
                numCars++;
            }
        }
    }

    return numCars;
}

int FEPlayerCarDB::GetNumCars(uint32 filter) {
    int numCars = 0;

    for (int i = 0; i < MAX_CARS_IN_STABLE; i++) {
        FECarRecord *car = &CarTable[i];
        if (car->IsValid() && car->MatchesFilter(filter)) {
            numCars++;
        }
    }

    return numCars;
}

FECarRecord *FEPlayerCarDB::CreateNewCustomCar(FECarHandle fromCar) {
    if (GetNumQuickRaceCars() < MAX_QUICKRACE_CARS_IN_STABLE) {
        return CreateCar(fromCar, FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_QUICK_RACE);
    }
    return nullptr;
}

FECarRecord *FEPlayerCarDB::AwardRivalCar(uint32 preset) {
    if (preset == 0x03A94520) {
        FEDatabase->GetCareerSettings()->SetHasBeenAwardedEpicCar();
    }

    PresetCar *preset_car = FindFEPresetCar(preset);
    FECarRecord *new_preset = GetCarRecordByHandle(preset);
    if (new_preset == nullptr) {
        // TODO: BUG? this doesn't assign new_preset
        FECarRecord *new_preset = CreateNewPresetCar(preset_car->PresetName);
    }

    FECarRecord *new_rec = CreateNewCareerCar(new_preset->Handle);
    new_rec->FilterBits |= FE_CAR_FILTER_PINKSLIP;

    FECustomizationRecord *customizations = GetCustomizationRecordByHandle(new_rec->Customization);
    RideInfo ride;

    ride.FillWithPreset(FEHashUpper(preset_car->PresetName));
    customizations->WriteRideIntoRecord(&ride);

    Attrib::Gen::presetride physics_preset(Attrib::StringToLowerCaseKey(preset_car->PresetName), 0, nullptr);
    if (physics_preset.IsValid() && customizations != nullptr) {
        Attrib::Gen::pvehicle pvehicle(new_rec->VehicleKey, 0, nullptr);
        if (Physics::Upgrades::ApplyPreset(pvehicle, physics_preset)) {
            customizations->WritePhysicsIntoRecord(pvehicle);
        }
    }

    return new_rec;
}

FECarRecord *FEPlayerCarDB::CreateNewCareerCar(FECarHandle fromCar) {
    if (GetNumCareerCars() >= MAX_CAREER_TOTAL_CARS) {
        return nullptr;
    }
    FECarRecord *the_car = CreateCar(fromCar, FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_CAREER);
    if (the_car != nullptr) {
        FECareerRecord *empty_career_record = CreateNewCareerRecord();
        if (empty_career_record == nullptr) {
            GetCustomizationRecordByHandle(the_car->Customization)->Handle = INVALID_CUSTOMIZATION_HANDLE;
            the_car->Handle = INVALID_CAR_HANDLE;
            return nullptr;
        } else {
            the_car->CareerHandle = empty_career_record->Handle;
        }
    }

    return the_car;
}

FECarRecord *FEPlayerCarDB::CreateNewPresetCar(const char *preset_name) {
    uint32 nameHash = FEHashUpper(preset_name);
    PresetCar *preset = FindFEPresetCar(nameHash);
    Attrib::Gen::pvehicle car(preset->VehicleKey, 0, nullptr);

    if (!car.IsValid()) {
        return nullptr;
    }

    FECarRecord *emptyCarRecord = CreateNewCarRecord();
    if (emptyCarRecord == nullptr || preset == nullptr) {
        return nullptr;
    }

    emptyCarRecord->Handle = nameHash;
    FECustomizationRecord *emptyCustomizationRecord = CreateNewCustomizationRecord();
    if (emptyCustomizationRecord == nullptr) {
        emptyCarRecord->Handle = INVALID_CAR_HANDLE;
        return nullptr;
    }

    emptyCarRecord->Customization = emptyCustomizationRecord->Handle;
    SetCarToPreset(emptyCarRecord->Handle, preset);

    Attrib::Gen::presetride physics_preset(Attrib::StringToLowerCaseKey(preset->PresetName), 0, nullptr);
    if (physics_preset.IsValid()) {
        Attrib::Gen::pvehicle pvehicle(car);
        if (Physics::Upgrades::ApplyPreset(pvehicle, physics_preset)) {
            emptyCustomizationRecord->WritePhysicsIntoRecord(pvehicle);
        }
        Physics::Upgrades::Clear(pvehicle);
    }

    emptyCarRecord->FilterBits = FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_PRESET;
    return emptyCarRecord;
}

FECarRecord *FEPlayerCarDB::CreateCar(FECarHandle fromCar, int FilterBits) {
    FECarRecord *fromCarRecord = GetCarRecordByHandle(fromCar);
    if (fromCarRecord == nullptr) {
        return nullptr;
    }

    FECarRecord *emptyCarRecord = CreateNewCarRecord();
    if (emptyCarRecord == nullptr) {
        return nullptr;
    }

    *emptyCarRecord = *fromCarRecord;
    FECustomizationRecord *emptyCustomizationRecord = CreateNewCustomizationRecord();
    if (emptyCustomizationRecord == nullptr) {
        emptyCarRecord->Handle = INVALID_CAR_HANDLE;
        return nullptr;
    }

    emptyCarRecord->Customization = emptyCustomizationRecord->Handle;
    emptyCarRecord->FilterBits = (emptyCarRecord->FilterBits & FE_CAR_FILTER_REGION_MASK) | FilterBits;

    RideInfo ride;
    ride.Init(emptyCarRecord->GetType(), CarRenderUsage_Player, 0, 0);
    ride.SetRandomPaint();
    ride.SetStockParts();
    emptyCustomizationRecord->WriteRideIntoRecord(&ride);
    return emptyCarRecord;
}

void FEPlayerCarDB::DeleteCustomCar(FECarHandle handle) {
    DeleteCar(handle, FE_CAR_FILTER_LIST_QUICK_RACE, false);
}

void FEPlayerCarDB::DeleteCareerCar(FECarHandle handle, bool was_sold) {
    DeleteCar(handle, FE_CAR_FILTER_LIST_CAREER, was_sold);
}

bool FEPlayerCarDB::DeleteCar(FECarHandle handle, uint32 filter, bool was_sold) {
    FECarRecord *car = GetCarRecordByHandle(handle);
    if (car == nullptr || !car->IsValid() || (car->FilterBits & filter) == 0) {
        return false;
    }

    if (was_sold) {
        BackupSoldCarHistory(car->CareerHandle);
    }

    car->Handle = INVALID_CAR_HANDLE;

    if (car->IsCustomized()) {
        GetCustomizationRecordByHandle(car->Customization)->Handle = INVALID_CUSTOMIZATION_HANDLE;
    }

    if (car->IsCareer()) {
        GetCareerRecordByHandle(car->CareerHandle)->Handle = INVALID_CAREER_HANDLE;
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
    for (int i = 0; i < MAX_CAREER_TOTAL_CARS; i++) {
        CareerRecords[i].Handle = 0xFF;
    }
}

bool FEPlayerCarDB::IsBonusCar(const char *preset_name) {
    uint32 nameHash = FEHashUpper(preset_name);

    switch (nameHash) {
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
        default:
            return false;
    }
}

void FEPlayerCarDB::Default() {
    DeleteAllCars();
    DeleteAllCustomizations();
    DeleteAllCareerRecords();

    FECarRecord *car = CreateNewPresetCar("M3GTRCAREERSTART");
    car->Handle = 0x12345678;
    car->FilterBits = FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_DEBUG;

    for (int i = 0; i < GetNumPresetCars(); i++) {
        PresetCar *pCar = GetPresetCarAt(i);

        if (UnlockSystem::IsBonusCarCEOnly(FEHashUpper(pCar->PresetName))) {
            FECarRecord *feCar = CreateNewPresetCar(pCar->PresetName);
            if (feCar != nullptr) {
                feCar->FilterBits = FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_BONUS;
            }
        } else if (IsBonusCar(pCar->PresetName)) {
            FECarRecord *feCar = CreateNewPresetCar(pCar->PresetName);
            if (feCar != nullptr) {
                feCar->FilterBits = FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_BONUS;
            }
        } else if (!bStrICmp(pCar->PresetName, "M3GTRCAREERSTART") || ShowAllPresetsInFE) {
            FECarRecord *feCar = CreateNewPresetCar(pCar->PresetName);
        }
        Physics::Upgrades::Flush();
    }

    const Attrib::Class *carClass = Attrib::Database::Get().GetClass(Attrib::ClassName::pvehicle);
    Attrib::Key key = carClass->GetFirstCollection();
    while (key != 0) {
        Attrib::Gen::pvehicle pv_car(key, 0, nullptr);
        if (!pv_car.IsDynamic()) {
            Attrib::Gen::frontend fe_car(pv_car.frontend(), 0, nullptr);
            Attrib::StringKey modelName;
            pv_car.MODEL(modelName);

            if (pv_car.PlayerUsable() || ShowAllCarsInFE) {

                if (modelName.IsNotEmpty() && !pv_car.IsDynamic()) {
                    FECarRecord *newCar = CreateNewCarRecord();
                    if (newCar != nullptr) {
                        newCar->FEKey = fe_car.GetCollection();
                        newCar->VehicleKey = pv_car.GetCollection();
                        newCar->Default();
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
    if (!gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_CASTROL)) {
        return;
    }
    if (!FEDatabase->GetCareerSettings()->HasBeenAwardedCastrolGT()) {
        FEDatabase->GetCareerSettings()->SetAwardedCastrolGT();
    }
}

void FEPlayerCarDB::SetCarToPreset(uint32 car, PresetCar *preset) {
    FECarRecord *car_record = GetCarRecordByHandle(car);

    car_record->FEKey = preset->FEKey;
    car_record->VehicleKey = preset->VehicleKey;
    car_record->FilterBits = preset->FilterBits;

    FECustomizationRecord *pCustomization = GetCustomizationRecordByHandle(car_record->Customization);
    if (pCustomization != nullptr) {
        pCustomization->BecomePreset(preset);
    }
}

void FEPlayerCarDB::BuildRideForPlayer(uint32 car, int player, RideInfo *ride) {
    FECarRecord *car_record = GetCarRecordByHandle(car);

    ride->Init(car_record->GetType(), CarRenderUsage_Player, 0, 0);
    FECustomizationRecord *pCustomization = GetCustomizationRecordByHandle(car_record->Customization);
    if (pCustomization != nullptr) {
        pCustomization->WriteRecordIntoRide(ride);
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

FECareerRecord *FEPlayerCarDB::GetCareerRecordByHandle(uint8 handle) {
    if (handle > MAX_CAREER_TOTAL_CARS || CareerRecords[handle].Handle == INVALID_CAREER_HANDLE) {
        return nullptr;
    }
    return &CareerRecords[handle];
}

bool FEPlayerCarDB::WriteRecordIntoPhysics(uint32 car, Attrib::Gen::pvehicle &attributes) {
    FECarRecord *pCar = GetCarRecordByHandle(car);
    if (pCar == nullptr) {
        return false;
    }
    FECustomizationRecord *pCustomization = GetCustomizationRecordByHandle(pCar->Customization);
    if (pCustomization == nullptr) {
        return false;
    }
    return pCustomization->WriteRecordIntoPhysics(attributes);
}

FECarRecord::FECarRecord() {
    FEKey = 0;
    VehicleKey = 0;
    Handle = INVALID_CAR_HANDLE;
    Customization = INVALID_CUSTOMIZATION_HANDLE;
    CareerHandle = INVALID_CAREER_HANDLE;
    FilterBits = 0;
}

FECarRecord &FECarRecord::operator=(const FECarRecord &other_record) {
    FEKey = other_record.FEKey;
    VehicleKey = other_record.VehicleKey;
    FilterBits = other_record.FilterBits;
    return *this;
}

void FECarRecord::Default() {
    Customization = INVALID_CUSTOMIZATION_HANDLE;
    CareerHandle = INVALID_CAREER_HANDLE;
    FilterBits = 0;

    if (!IsValid()) {
        return;
    }

    Attrib::Gen::pvehicle car(VehicleKey, 0, nullptr);
    Attrib::Gen::frontend fe(FEKey, 0, nullptr);

    switch (fe.region()) {
        case 0:
            FilterBits |= FE_CAR_FILTER_REGION_AMERICA;
            break;
        case 1:
            FilterBits |= FE_CAR_FILTER_REGION_EUROPE;
            break;
        case 2:
            FilterBits |= FE_CAR_FILTER_REGION_JAPAN;
            break;
        case 3:
            FilterBits |= FE_CAR_FILTER_REGION_DEBUG;
            break;
    }

    if (fe.region() == 3 || !fe.IsCustomizable()) {
        FilterBits |= FE_CAR_FILTER_LIST_DEBUG;
    } else {
        FilterBits |= FE_CAR_FILTER_LIST_STOCK;
    }
}

CarType FECarRecord::GetType() {
    Attrib::Gen::pvehicle car(VehicleKey, 0, nullptr);

    return CarPartDB.GetCarType(bStringHash(car.MODEL().GetString()));
}

uint32 FECarRecord::GetCost() {
    Attrib::Gen::frontend car(FEKey, 0, nullptr);

    return car.Cost();
}

// UNSOLVED  asm
uint32 FECarRecord::GetReleaseFromImpoundCost() {
    float cost = GetCost();
    return cost * g_fImpoundPercentageOfOriginalCost;
}

// UNSOLVED (dwarf)
uint32 FECarRecord::GetNameHash() {
    const char *manu = GetManufacturerName();
    if (bStrCmp(manu, "")) {
        char buf[128];
        Attrib::Gen::frontend frontend(FEKey, 0, nullptr);
        FEngSNPrintf(buf, sizeof(buf), "CARNAME_%s_%s", manu, frontend.CollectionName());
        uint32 CarNameHash = FEHashUpper(buf);
        if (DoesStringExist(CarNameHash)) {
            return CarNameHash;
        }
    }
    return 0x9BB9CCC3;
}

uint32 FECarRecord::GetLogoHash() {
    const char *manu = GetManufacturerName();
    if (bStrCmp(manu, "")) {
        char buf[128];
        Attrib::Gen::frontend frontend(FEKey, 0, nullptr);
        FEngSNPrintf(buf, sizeof(buf), "SECONDARY_LOGO_%s", frontend.CollectionName());
        uint32 texHash = FEHashUpper(buf);
        if (GetTextureInfo(texHash, 0, 0) != nullptr) {
            return texHash;
        }
    }
    return FEHashUpper("GENERIC_LOGO_256");
}

uint32 FECarRecord::GetManuLogoHash() {
    const char *manu = GetManufacturerName();
    if (bStrCmp(manu, "")) {
        char buf[128];
        FEngSNPrintf(buf, sizeof(buf), "CARSELECT_MANUFACTURER_%s", manu);
        uint32 texHash = FEHashUpper(buf);
        if (GetTextureInfo(texHash, 0, 0) != nullptr) {
            return texHash;
        }
    }
    return FEHashUpper("GENERIC_LOGO_128");
}

const char *FECarRecord::GetManufacturerName() {
    Attrib::Gen::frontend fe(FEKey, 0, nullptr);
    uint8 Manufacturer = static_cast<uint8>(fe.manufacturer());
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
    Attrib::Gen::pvehicle pvehicle(VehicleKey, 0, nullptr);

    return pvehicle.CollectionName();
}

uint32 GetFECarNameHashFromFEKey(Attrib::Key feKey) {
    if (!feKey) {
        return 0;
    }
    FECarRecord fe_car;
    fe_car.FEKey = feKey;
    return fe_car.GetNameHash();
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
    return CarPartDB.GetCarPartByIndex(InstalledPartIndices[carslotid]);
}

void FECustomizationRecord::SetInstalledPart(int carslotid, CarPart *part) {
    if (part != nullptr) {
#ifndef FIX_BUGS // Useless code
        uint16 idx = CarPartDB.GetPartIndex(part);
#endif
        InstalledPartIndices[carslotid] = CarPartDB.GetPartIndex(part);
    } else {
        InstalledPartIndices[carslotid] = INVALID_CAR_PART_RECORD_INDEX;
    }
}

void FECustomizationRecord::WriteRecordIntoRide(RideInfo *ride) const {
    CarType cartype;
    for (int i = 0; i < CARSLOTID_NUM; i++) {
        CarPart *part = GetInstalledPart(ride->Type, i);
        ride->SetPart(i, part, true);
    }
}

void FECustomizationRecord::WriteRideIntoRecord(const RideInfo *ride) {
    for (int i = 0; i < CARSLOTID_NUM; i++) {
        CarPart *part = ride->GetPart(i);
        SetInstalledPart(i, part);
    }
}

FECustomizationRecord::FECustomizationRecord() : InstalledPhysics(), Tunings(), Handle(INVALID_CUSTOMIZATION_HANDLE) {
    Default();
}

void FECustomizationRecord::BecomePreset(PresetCar *preset) {
    Default();

    CarType type = CarPartDB.GetCarType(bStringHash(preset->CarTypeName));
    for (int i = 0; i < CARSLOTID_NUM; i++) {
        if (preset->PartNameHashes[i] > 1u) { // TODO: where does this 1u come from
            struct CarPart *part = CarPartDB.NewGetCarPart(type, i, preset->PartNameHashes[i], nullptr, -1);
            InstalledPartIndices[i] = CarPartDB.GetPartIndex(part);
        }
    }

    Preset = bStringHashUpper(preset->PresetName);
}

FEInfractionsData::FEInfractionsData(uint32 infractions) {
    *this = FEInfractionsData();

    if ((infractions & GInfractionManager::kInfraction_Assault) != 0) {
        Assault++;
    }
    if ((infractions & GInfractionManager::kInfraction_Damage) != 0) {
        Damage++;
    }
    if ((infractions & GInfractionManager::kInfraction_HitAndRun) != 0) {
        HitAndRun++;
    }
    if ((infractions & GInfractionManager::kInfraction_OffRoad) != 0) {
        OffRoad++;
    }
    if ((infractions & GInfractionManager::kInfraction_Racing) != 0) {
        Racing++;
    }
    if ((infractions & GInfractionManager::kInfraction_Reckless) != 0) {
        Reckless++;
    }
    if ((infractions & GInfractionManager::kInfraction_Resist) != 0) {
        Resist++;
    }
    if ((infractions & GInfractionManager::kInfraction_Speeding) != 0) {
        Speeding++;
    }
}

uint16 FEInfractionsData::NumInfractions() const {
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

uint16 FEInfractionsData::GetValue(GInfractionManager::InfractionType type) const {
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
            break;
        default:
            return 0;
    }
}

uint32 FEInfractionsData::GetFineValue() const {
    if (NumInfractions() == 0) {
        return 0;
    }
    uint32 fines = 0;
    Attrib::Gen::infractions AssaultFine(Attrib::StringToKey("assault"), 0, nullptr);
    if (AssaultFine.IsValid()) {
        fines = Assault * AssaultFine.amount();
    }
    Attrib::Gen::infractions DamageFine(Attrib::StringToKey("damage"), 0, nullptr);
    if (DamageFine.IsValid()) {
        fines += Damage * DamageFine.amount();
    }
    Attrib::Gen::infractions HitAndRunFine(Attrib::StringToKey("hit_and_run"), 0, nullptr);
    if (HitAndRunFine.IsValid()) {
        fines += HitAndRun * HitAndRunFine.amount();
    }
    Attrib::Gen::infractions OffRoadFine(Attrib::StringToKey("off_road"), 0, nullptr);
    if (OffRoadFine.IsValid()) {
        fines += OffRoad * OffRoadFine.amount();
    }
    Attrib::Gen::infractions RacingFine(Attrib::StringToKey("racing"), 0, nullptr);
    if (RacingFine.IsValid()) {
        fines += Racing * RacingFine.amount();
    }
    Attrib::Gen::infractions RecklessFine(Attrib::StringToKey("reckless_driving"), 0, nullptr);
    if (RecklessFine.IsValid()) {
        fines += Reckless * RecklessFine.amount();
    }
    Attrib::Gen::infractions ResistFine(Attrib::StringToKey("resisting_arrest"), 0, nullptr);
    if (ResistFine.IsValid()) {
        fines += Resist * ResistFine.amount();
    }
    Attrib::Gen::infractions SpeedingFine(Attrib::StringToKey("speeding"), 0, nullptr);
    if (SpeedingFine.IsValid()) {
        fines += Speeding * SpeedingFine.amount();
    }
    return fines;
}

void FEImpoundData::Default() {
    TimesBusted = 0;
    ImpoundedState = 0;
    DaysBeforeRelease = 0;
    MaxBusted = 3;
    EvadeCount = 0;
    Pad2 = 0;
    Pad1 = 0;
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
    if (IsImpounded() && ((DaysBeforeRelease == 0 || --DaysBeforeRelease == 0) && ImpoundedState != IMPOUND_RELEASED)) {
        ImpoundedState = IMPOUND_RELEASED;
        return true;
    }
    return false;
}

bool FEImpoundData::NotifyBusted() {
    EvadeCount = 0;
    TimesBusted++;
    return TimesBusted >= MaxBusted;
}

bool FEImpoundData::NotifyEvade() {
    if (!IsImpounded()) {
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
    if (MaxBusted < g_MaximumMaximumTimesBusted && !IsImpounded()) {
        return true;
    }
    return false;
}

void FEImpoundData::AddMaxBusted() {
    MaxBusted++;
    if (MaxBusted > g_MaximumMaximumTimesBusted) {
        MaxBusted = g_MaximumMaximumTimesBusted;
    }
}

void FECareerRecord::Default() {
    Handle = INVALID_CAREER_HANDLE;
    TheImpoundData.Default();
    VehicleHeat = 0.0f;
    Bounty = 0;
    NumEvadedPursuits = 0;
    NumBustedPursuits = 0;
    UnservedInfractions = FEInfractionsData();
    ServedInfractions = FEInfractionsData();
}

void FECareerRecord::SetVehicleHeat(float h) {
    VehicleHeat = h;
}

float FECareerRecord::GetVehicleHeat() {
    return VehicleHeat;
}

void FECareerRecord::AdjustHeatOnEventWin() {
    Attrib::Gen::pursuitlevels DefaultPursuitLevelAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat = VehicleHeat * DefaultPursuitLevelAttrib.EventWinHeatAdjust();
}

void FECareerRecord::AdjustHeatOnEvadePursuit() {
    Attrib::Gen::pursuitlevels DefaultPursuitLevelAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat = VehicleHeat * DefaultPursuitLevelAttrib.EvadeSuccessHeatAdjust();
}

void FECareerRecord::AdjustHeatOnDecalApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewDecal() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnPaintApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewPaint() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnVinylApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewVinyl() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnBodyKitApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewBodyKit() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnHoodApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewHood() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnRimApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewRim() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnRimPaintApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewRimPaint() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnRoofScoopApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewRoofScoop() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnSpoilerApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewSpoiler() * extraAdjust;
}

void FECareerRecord::AdjustHeatOnWindowTintApplied(float extraAdjust) {
    Attrib::Gen::fecooling FeCoolingAttrib(0xEEC2271A, 0, nullptr);

    VehicleHeat *= FeCoolingAttrib.NewWindowTint() * extraAdjust;
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
    ServedInfractions += UnservedInfractions;
    UnservedInfractions = FEInfractionsData();
}

uint32 FECareerRecord::GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved) const {
    const FEInfractionsData &infract = get_unserved ? UnservedInfractions : ServedInfractions;
    return infract.GetValue(type);
}
