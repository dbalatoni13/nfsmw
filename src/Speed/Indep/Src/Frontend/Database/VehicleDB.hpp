#ifndef FRONTEND_DATABASE_VEHICLEDB_H
#define FRONTEND_DATABASE_VEHICLEDB_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"

// total size: 0x14
struct FECarRecord {
    unsigned int Handle;         // offset 0x0, size 0x4
    unsigned int FEKey;          // offset 0x4, size 0x4
    unsigned int VehicleKey;     // offset 0x8, size 0x4
    unsigned int FilterBits;     // offset 0xC, size 0x4
    unsigned char Customization; // offset 0x10, size 0x1
    unsigned char CareerHandle;  // offset 0x11, size 0x1
    unsigned short Padd;         // offset 0x12, size 0x2
};

// total size: 0x198
struct FECustomizationRecord {
    short InstalledPartIndices[139];             // offset 0x0, size 0x116
    Physics::Upgrades::Package InstalledPhysics; // offset 0x118, size 0x20
    Physics::Tunings Tunings[3];                 // offset 0x138, size 0x54
    Physics::eCustomTuningType ActiveTuning;     // offset 0x18C, size 0x4
    int Preset;                                  // offset 0x190, size 0x4
    unsigned char Handle;                        // offset 0x194, size 0x1
};

// total size: 0x8
struct FEImpoundData {
    enum eImpoundReasons {
        IMPOUND_REASON_NONE = 0,
        IMPOUND_REASON_BUSTED_BY_CROSS = 1,
        IMPOUND_REASON_STRIKE_LIMIT_REACHED = 2,
        IMPOUND_REASON_INSUFFICIENT_FUNDS = 3,
        IMPOUND_RELEASED = 4,
    };
    unsigned char MaxBusted;         // offset 0x0, size 0x1
    char TimesBusted;                // offset 0x1, size 0x1
    char ImpoundedState;             // offset 0x2, size 0x1
    unsigned char DaysBeforeRelease; // offset 0x3, size 0x1
    char EvadeCount;                 // offset 0x4, size 0x1
    char Pad1;                       // offset 0x5, size 0x1
    short Pad2;                      // offset 0x6, size 0x2
};

// total size: 0x10
struct FEInfractionsData {
    unsigned short Speeding;  // offset 0x0, size 0x2
    unsigned short Racing;    // offset 0x2, size 0x2
    unsigned short Reckless;  // offset 0x4, size 0x2
    unsigned short Assault;   // offset 0x6, size 0x2
    unsigned short HitAndRun; // offset 0x8, size 0x2
    unsigned short Damage;    // offset 0xA, size 0x2
    unsigned short Resist;    // offset 0xC, size 0x2
    unsigned short OffRoad;   // offset 0xE, size 0x2
};

// total size: 0x38
class FECareerRecord {
  public:
    unsigned int GetBounty() const {
        return Bounty;
    }

    // unsigned int GetNumEvadedPursuits() const {}

    // unsigned int GetNumBustedPursuits() const {}

    // int GetTimesBusted() {}

    // const FEInfractionsData &GetInfractions(bool get_unserved) const {}

    // void TweakBounty(unsigned int bounty) {}

    FECareerRecord() {}

    void Default();
    void SetVehicleHeat(float h);
    float GetVehicleHeat();
    void AdjustHeatOnEventWin();
    void AdjustHeatOnMilestoneComplete();
    void AdjustHeatOnEvadePursuit();
    void AdjustHeatOnDecalApplied(float extraAdjust);
    void AdjustHeatOnPaintApplied(float extraAdjust);
    void AdjustHeatOnVinylApplied(float extraAdjust);
    void AdjustHeatOnBodyKitApplied(float extraAdjust);
    void AdjustHeatOnHoodApplied(float extraAdjust);
    void AdjustHeatOnNumbersApplied(float extraAdjust);
    void AdjustHeatOnRimApplied(float extraAdjust);
    void AdjustHeatOnRimPaintApplied(float extraAdjust);
    void AdjustHeatOnRoofScoopApplied(float extraAdjust);
    void AdjustHeatOnSpoilerApplied(float extraAdjust);
    void AdjustHeatOnWindowTintApplied(float extraAdjust);
    void CommitPursuitCarData(unsigned int infractions, unsigned int accumulated_bounty, bool pursuit_evaded);
    void WaiveIncractions(unsigned int infractions);
    void ServeAllIncractions();
    // unsigned int GetNumInfraction(InfractionType type, bool get_unserved) const;

    unsigned char Handle;         // offset 0x0, size 0x1
    FEImpoundData TheImpoundData; // offset 0x2, size 0x8
    float VehicleHeat;            // offset 0xC, size 0x4

  private:
    unsigned int Bounty;                   // offset 0x10, size 0x4
    unsigned short NumEvadedPursuits;      // offset 0x14, size 0x2
    unsigned short NumBustedPursuits;      // offset 0x16, size 0x2
    FEInfractionsData UnservedInfractions; // offset 0x18, size 0x10
    FEInfractionsData ServedInfractions;   // offset 0x28, size 0x10
};

// total size: 0x8CC8
class FEPlayerCarDB {
  public:
    // total size: 0x4
    class MyCallback {};

    void BuildRideForPlayer(unsigned int car, int player, RideInfo *ride);
    FECarRecord *GetCarRecordByHandle(unsigned int handle);
    FECustomizationRecord *GetCustomizationRecordByHandle(unsigned char handle);
    FECarRecord *GetCarByIndex(int index);
    FECarRecord *CreateNewCarRecord();
    bool CanCreateNewCarRecord();
    bool CanCreateNewCustomizationRecord();
    FECustomizationRecord *CreateNewCustomizationRecord();
    FECareerRecord *CreateNewCareerRecord();
    unsigned short GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved);
    unsigned int GetTotalNumInfractions(bool get_unserved);
    unsigned short GetNumInfractionsOnCar(unsigned int car_handle, bool get_unserved);
    unsigned int GetTotalBounty();
    unsigned int GetTotalEvadedPursuits();
    unsigned int GetTotalBustedPursuits();
    unsigned int GetNumImpoundedCars();
    unsigned int GetTotalFines(bool get_unserved);
    unsigned int GetNumCareerCarsWithARecord();
    unsigned int ForAllCareerRecordsSum(const MyCallback &Callback);
    void BackupSoldCarHistory(unsigned char sold_car);
    unsigned int GetPreferedCarName();
    int GetNumQuickRaceCars();
    int GetNumCareerCars();
    int GetNumPurchasedCars();
    int GetNumAvailableCareerCars();
    FECarRecord *GetCheapestEarliestReleaseImpoundCar(FECarRecord *selected_car);
    int GetNumCars(unsigned int filter);
    FECarRecord *CreateNewCustomCar(unsigned int fromCar);
    FECarRecord *AwardRivalCar(unsigned int preset);
    FECarRecord *CreateNewCareerCar(unsigned int fromCar);
    FECarRecord *CreateNewPresetCar(const char *preset_name);
    FECarRecord *CreateCar(unsigned int fromCar, int FilterBits);
    void DeleteCustomCar(unsigned int handle);
    void DeleteCareerCar(unsigned int handle, bool was_sold);
    bool DeleteCar(unsigned int handle, unsigned int filter, bool was_sold);
    void DeleteAllCars();
    void DeleteAllCustomizations();
    void DeleteAllCareerRecords();
    FECarRecord *GetRandomCar(int FilterBits);
    bool IsBonusCar(const char *preset_name);
    void Default();
    char *SaveToBuffer(char *buffer, int bufsize);
    char *LoadFromBuffer(char *buffer, int bufsize);
    int GetSaveBufferSize();
    void AwardBonusCars();
    bool IsHeroCar(unsigned int handle);
    void SetCarToPreset(unsigned int car, struct PresetCar *preset);
    FECareerRecord *GetCareerRecordByHandle(unsigned char handle);
    bool WriteRecordIntoPhysics(unsigned int car, Attrib::Gen::pvehicle &attributes);
    void WritePhysicsIntoRecord(unsigned int car, const Attrib::Gen::pvehicle &attributes);

  private:
    FECarRecord CarTable[200];                        // offset 0x0, size 0xFA0
    FECustomizationRecord Customizations[75];         // offset 0xFA0, size 0x7788
    FECareerRecord CareerRecords[25];                 // offset 0x8728, size 0x578
    unsigned int SoldHistoryBounty;                   // offset 0x8CA0, size 0x4
    unsigned short SoldHistoryNumEvadedPursuits;      // offset 0x8CA4, size 0x2
    unsigned short SoldHistoryNumBustedPursuits;      // offset 0x8CA6, size 0x2
    FEInfractionsData SoldHistoryUnservedInfractions; // offset 0x8CA8, size 0x10
    FEInfractionsData SoldHistoryServedInfractions;   // offset 0x8CB8, size 0x10
};

void AdjustStableImpound_EvadePursuit(int playerNum);

#endif
