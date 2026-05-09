#ifndef FRONTEND_DATABASE_VEHICLEDB_H
#define FRONTEND_DATABASE_VEHICLEDB_H

#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Camera/CameraInfo.hpp"

typedef enum { CTT_SETTING_1 = 0, CTT_SETTING_2 = 1, CTT_SETTING_3 = 2, NUM_CUSTOM_TUNINGS = 3 } eCustomTuningType;

// total size: 0x198
class FECustomizationRecord {
  public:
    static void *operator new(size_t size, void *ptr) {}

    static void operator delete(void *mem, void *ptr) {}

    static void *operator new(size_t size) {}

    static void operator delete(void *mem, size_t size) {}

    static void *operator new(size_t size, const char *name) {}

    static void operator delete(void *mem, const char *name) {}

    static void operator delete(void *mem, size_t size, const char *name) {}

    FECustomizationRecord();
    void Default();
    bool IsPreset() const {}
    int GetNumInstalledPartNames() {}
    bool WriteRecordIntoPhysics(Attrib::Gen::pvehicle &attributes) const;
    void WritePhysicsIntoRecord(const Attrib::Gen::pvehicle &attributes);
    void WriteRecordIntoRide(RideInfo *ride) const;
    void WriteRideIntoRecord(const RideInfo *ride);
    void BecomePreset(PresetCar *preset);
    CarPart *GetInstalledPart(CarType cartype, int carslotid) const;
    void SetInstalledPart(int carslotid, CarPart *part);
    void SetActiveTuning(eCustomTuningType type) {}
    bool GetInstalledJunkman(Attrib::Type id) {}
    void SetInstalledJunkman(Physics::Upgrades::Type id, bool b) {}
    void SetInstalledPhysics(const Physics::Upgrades::Package &package) {}
    void SetInstalledPhysics(Physics::Upgrades::Type id, int level) {}
    const Physics::Upgrades::Package *GetInstalledPhysics() const {}
    void SetTuning(Physics::Tunings::Path id, float value) {}
    void SetTuning(Physics::Tunings::Path id, eCustomTuningType type, float value) {}
    float GetTuning(Physics::Tunings::Path id) const {}
    const Physics::Tunings *GetTunings() const {}
    const Physics::Tunings *GetTunings(unsigned int type) const {}

  private:
    int16 InstalledPartIndices[139];             // offset 0x0, size 0x116
    Physics::Upgrades::Package InstalledPhysics; // offset 0x118, size 0x20
    Physics::Tunings Tunings[3];                 // offset 0x138, size 0x54
    Physics::eCustomTuningType ActiveTuning;     // offset 0x18C, size 0x4
    int32 Preset;                                // offset 0x190, size 0x4
  public:
    uint8 Handle; // offset 0x194, size 0x1
};

typedef enum {
    PSC_BUMPER = 0,
    PSC_HOOD = 1,
    PSC_CLOSE = 2,
    PSC_FAR = 3,
    PSC_SUPER_FAR = 4,
    PSC_DRIFT = 5,
    PSC_PURSUIT = 6,
    NUM_CAMERAS_IN_OPTIONS = 7,
    PSC_DEFAULT = 2
} ePlayerSettingsCameras;

// total size: 0x10
class FEInfractionsData {
  public:
    FEInfractionsData() {
        bMemSet(this, 0, sizeof(FEInfractionsData));
    }
    FEInfractionsData(uint32 infractions);
    void operator+=(const FEInfractionsData &rhs);
    uint16 NumInfractions() const;
    uint16 GetValue(GInfractionManager::InfractionType type) const;
    uint32 GetFineValue() const;

    uint16 Speeding;  // offset 0x0, size 0x2
    uint16 Racing;    // offset 0x2, size 0x2
    uint16 Reckless;  // offset 0x4, size 0x2
    uint16 Assault;   // offset 0x6, size 0x2
    uint16 HitAndRun; // offset 0x8, size 0x2
    uint16 Damage;    // offset 0xA, size 0x2
    uint16 Resist;    // offset 0xC, size 0x2
    uint16 OffRoad;   // offset 0xE, size 0x2
};

// total size: 0x8
class FEImpoundData {
  public:
    typedef enum {
        IMPOUND_REASON_NONE = 0,
        IMPOUND_REASON_BUSTED_BY_CROSS = 1,
        IMPOUND_REASON_STRIKE_LIMIT_REACHED = 2,
        IMPOUND_REASON_INSUFFICIENT_FUNDS = 3,
        IMPOUND_RELEASED = 4
    } eImpoundReasons;

    void Default();
    void BecomeImpounded(eImpoundReasons reason);
    bool IsImpounded() const {
        return ImpoundedState != 0;
    }
    bool IsReleasable() const {}
    void NotifyPlayerPaidToRelease();
    void NotifyPlayerUsedMarkerToRelease();
    bool NotifyWin();
    bool NotifyEvade();
    bool NotifyBusted();
    void AddMaxBusted();
    bool CanAddMaxBusted();
    int GetDaysBeforeRelease() {}

    uint8 MaxBusted;         // offset 0x0, size 0x1
    int8 TimesBusted;        // offset 0x1, size 0x1
    int8 ImpoundedState;     // offset 0x2, size 0x1
    uint8 DaysBeforeRelease; // offset 0x3, size 0x1
    int8 EvadeCount;         // offset 0x4, size 0x1
    int8 Pad1;               // offset 0x5, size 0x1
    int16 Pad2;              // offset 0x6, size 0x2
};

// total size: 0x38
class FECareerRecord {
  public:
    FECareerRecord() {}

    void Default();
    void SetVehicleHeat(float h);
    float GetVehicleHeat();
    void AdjustHeatOnEventWin();
    void AdjustHeatOnMilestoneComplete();
    void AdjustHeatOnEvadePursuit();
    void AdjustHeatOnVinylApplied(float extraAdjust);
    void AdjustHeatOnDecalApplied(float extraAdjust);
    void AdjustHeatOnPaintApplied(float extraAdjust);
    void AdjustHeatOnBodyKitApplied(float extraAdjust);
    void AdjustHeatOnHoodApplied(float extraAdjust);
    void AdjustHeatOnNumbersApplied(float extraAdjust);
    void AdjustHeatOnRimApplied(float extraAdjust);
    void AdjustHeatOnRimPaintApplied(float extraAdjust);
    void AdjustHeatOnRoofScoopApplied(float extraAdjust);
    void AdjustHeatOnSpoilerApplied(float extraAdjust);
    void AdjustHeatOnWindowTintApplied(float extraAdjust);
    void CommitPursuitCarData(unsigned int infractions, uint32 accumulated_bounty, bool pursuit_evaded);
    void ServeAllIncractions();
    void WaiveIncractions(unsigned int infractions);
    uint32 GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved) const;

    uint32 GetBounty() const {
        return Bounty;
    }

    uint32 GetNumEvadedPursuits() const {
        return NumEvadedPursuits;
    }

    uint32 GetNumBustedPursuits() const {
        return NumBustedPursuits;
    }

    int GetTimesBusted() {
        return TheImpoundData.TimesBusted;
    }

    const FEInfractionsData &GetInfractions(bool get_unserved) const {
        if (get_unserved) {
            return UnservedInfractions;
        }
        return ServedInfractions;
    }

    void TweakBounty(unsigned int bounty) {}

    uint8 Handle;                 // offset 0x0, size 0x1
    FEImpoundData TheImpoundData; // offset 0x2, size 0x8
    float VehicleHeat;            // offset 0xC, size 0x4

  private:
    uint32 Bounty;                         // offset 0x10, size 0x4
    uint16 NumEvadedPursuits;              // offset 0x14, size 0x2
    uint16 NumBustedPursuits;              // offset 0x16, size 0x2
    FEInfractionsData UnservedInfractions; // offset 0x18, size 0x10
    FEInfractionsData ServedInfractions;   // offset 0x28, size 0x10
};

// total size: 0x14
class FECarRecord {
  public:
    FECarRecord();
    FECarRecord &operator=(const FECarRecord &other_record);
    void Default();
    bool MatchesFilter(int theFilter);
    bool IsValid() {
        return Handle != 0xFFFFFFFF;
    }
    bool IsCustomized() {
        return Customization != 0xFF;
    }
    bool IsCareer() {
        return CareerHandle != 0xFF;
    }
    const char *GetDebugName();
    const char *GetManufacturerName();
    CarType GetType();
    uint32 GetNameHash();
    uint32 GetLogoHash();
    uint32 GetManuLogoHash();
    uint32 GetCost();
    uint32 GetReleaseFromImpoundCost();

    uint32 Handle;          // offset 0x0, size 0x4
    Attrib::Key FEKey;      // offset 0x4, size 0x4
    Attrib::Key VehicleKey; // offset 0x8, size 0x4
    uint32 FilterBits;      // offset 0xC, size 0x4
    uint8 Customization;    // offset 0x10, size 0x1
    uint8 CareerHandle;     // offset 0x11, size 0x1
    uint16 Padd;            // offset 0x12, size 0x2
};

typedef enum {
    FE_CAR_FILTER_LIST_STOCK = 1,
    FE_CAR_FILTER_LIST_CAREER = 2,
    FE_CAR_FILTER_LIST_QUICK_RACE = 4,
    FE_CAR_FILTER_LIST_BONUS = 8,
    FE_CAR_FILTER_LIST_PRESET = 16,
    FE_CAR_FILTER_LIST_DEBUG = 32,
    FE_CAR_FILTER_LIST_MIN = 1,
    FE_CAR_FILTER_LIST_MAX = 32,
    FE_CAR_FILTER_TYPE_MASK = 15,
    FE_CAR_FILTER_LIST_MASK = 65535,
    FE_CAR_FILTER_REGION_AMERICA = 65536,
    FE_CAR_FILTER_REGION_EUROPE = 131072,
    FE_CAR_FILTER_REGION_JAPAN = 262144,
    FE_CAR_FILTER_REGION_DEBUG = 524288,
    FE_CAR_FILTER_REGION_MIN = 65536,
    FE_CAR_FILTER_REGION_MAX = 524288,
    FE_CAR_FILTER_REGION_ALL = 983040,
    FE_CAR_FILTER_REGION_MASK = -65536
} FEPlayerCarDBFilterBits;

// total size: 0x8CC8
class FEPlayerCarDB {
  public:
    // total size: 0x4
    class MyCallback {
      public:
        virtual ~MyCallback() {}
        virtual uint32 Callback(const FECareerRecord &record) const = 0;
    };

    FEPlayerCarDB();
    ~FEPlayerCarDB();
    void Default();
    FECarRecord *GetCarByIndex(int index);
    char *SaveToBuffer(char *buffer, int bufsize);
    char *LoadFromBuffer(char *buffer, int bufsize);
    int32 GetSaveBufferSize();
    void AwardBonusCars();
    FECarRecord *GetCarRecordByHandle(uint32 handle);
    FECustomizationRecord *GetCustomizationRecordByHandle(uint8 handle);
    FECareerRecord *GetCareerRecordByHandle(uint8 handle);
    FECarRecord *CreateNewCustomCar(uint32 fromCar);
    void DeleteCustomCar(uint32 handle);
    void DeleteCareerCar(uint32 handle, bool was_sold);
    FECarRecord *CreateNewPresetCar(const char *preset_name);
    FECarRecord *CreateNewCareerCar(uint32 fromCar);
    FECarRecord *AwardRivalCar(uint32 preset);
    bool CanCreateNewCarRecord();
    bool CanCreateNewCustomizationRecord();
    bool IsBonusCar(const char *preset_name);
    bool IsHeroCar(uint32 handle);
    FECarRecord *GetRandomCar(int FilterBits);
    void BuildRideForPlayer(uint32 car, int player, RideInfo *ride);
    int GetNumQuickRaceCars();
    int GetNumCareerCars();
    int GetNumPurchasedCars();
    int GetNumAvailableCareerCars();
    FECarRecord *GetCheapestEarliestReleaseImpoundCar(FECarRecord *selected_car);
    bool WriteRecordIntoPhysics(uint32 car, Attrib::Gen::pvehicle &attributes);
    void WritePhysicsIntoRecord(uint32 car, const Attrib::Gen::pvehicle &attributes);
    void SetCarToPreset(uint32 car, PresetCar *preset);
    uint16 GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved);
    uint16 GetNumInfractionsOnCar(uint32 car_handle, bool get_unserved);
    uint32 GetTotalNumInfractions(bool get_unserved);
    uint32 GetTotalBounty();
    uint32 GetTotalEvadedPursuits();
    uint32 GetTotalBustedPursuits();
    uint32 GetNumImpoundedCars();
    uint32 GetPreferedCarName();
    uint32 GetTotalFines(bool get_unserved);
    uint32 GetNumCareerCarsWithARecord();

  private:
    FECarRecord *CreateCar(uint32 fromCar, int FilterBits);
    bool DeleteCar(uint32 handle, uint32 filter, bool was_sold);
    void DeleteAllCars();
    int GetNumCars(uint32 filter);
    void DeleteAllCustomizations();
    void DeleteAllCareerRecords();
    FECarRecord *CreateNewCarRecord();
    FECustomizationRecord *CreateNewCustomizationRecord();
    FECareerRecord *CreateNewCareerRecord();
    uint32 ForAllCareerRecordsSum(const MyCallback &Callback);
    void BackupSoldCarHistory(uint8 sold_car);

    FECarRecord CarTable[200];                        // offset 0x0, size 0xFA0
    FECustomizationRecord Customizations[75];         // offset 0xFA0, size 0x7788
    FECareerRecord CareerRecords[25];                 // offset 0x8728, size 0x578
    uint32 SoldHistoryBounty;                         // offset 0x8CA0, size 0x4
    uint16 SoldHistoryNumEvadedPursuits;              // offset 0x8CA4, size 0x2
    uint16 SoldHistoryNumBustedPursuits;              // offset 0x8CA6, size 0x2
    FEInfractionsData SoldHistoryUnservedInfractions; // offset 0x8CA8, size 0x10
    FEInfractionsData SoldHistoryServedInfractions;   // offset 0x8CB8, size 0x10
};

void AdjustStableImpound_EvadePursuit(int playerNum);

POVTypes GetPOVTypeFromPlayerCamera(ePlayerSettingsCameras cam);
bool IsPlayerCameraSelectable(POVTypes pov);

uint32 GetFECarNameHashFromFEKey(Attrib::Key feKey);

#endif
