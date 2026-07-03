#ifndef VEHICLEDB_HPP
#define VEHICLEDB_HPP

#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Camera/CameraInfo.hpp"

#define INVALID_CAR_HANDLE 0xFFFFFFFF        // :35
#define QUICK_PLAY_HANDLE 0x12345678         // :36
#define INVALID_CUSTOMIZATION_HANDLE 0xFF    // :37
#define INVALID_CAREER_HANDLE 0xFF           // :38
#define INVALID_CAR_PART_INDEX 0x7FFF        // :39
#define INVALID_CAR_PART_RECORD_INDEX 0xFFFF // :40
#define INVALID_VINYL_LAYER (~0)             // :41
#define MAX_CAR_NAME_LENGTH 64               // :42
#define MAX_PHYSICS_PARTS 32                 // :43
#define FECarHandle uint32                   // :45
#define FECustomizationHandle uint8          // :46
#define FECareerHandle uint8                 // :47
#define FECarPartIndex uint16                // :48
#define FECarPartRecordIndex uint16          // :49

// File: speed/indep/src/database/VehicleDB.hpp
// total size: 0x198
// Decl: speed/indep/src/database/VehicleDB.hpp:102
class FECustomizationRecord {
  private:
    FECarPartIndex InstalledPartIndices[139];     // offset 0x0, size 0x116
    Physics::Upgrades::Package InstalledPhysics;  // offset 0x118, size 0x20, Decl: speed/indep/src/database/VehicleDB.hpp:110
    Physics::Tunings Tunings[NUM_CUSTOM_TUNINGS]; // offset 0x138, size 0x54, Decl: speed/indep/src/database/VehicleDB.hpp:115
    eCustomTuningType ActiveTuning;               // offset 0x18C, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:116
    int32 Preset;                                 // offset 0x190, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:118
  public:
    FECustomizationHandle Handle; // offset 0x194, size 0x1, Decl: speed/indep/src/database/VehicleDB.hpp:119

    static void operator delete(void *mem, const char *name) {} // Decl: speed/indep/src/database/VehicleDB.hpp:137

    static void *operator new(size_t size, void *ptr) {}

    static void operator delete(void *mem, void *ptr) {}

    static void *operator new(size_t size) {}

    static void operator delete(void *mem, size_t size) {}

    static void *operator new(size_t size, const char *name) {}

    static void operator delete(void *mem, size_t size, const char *name) {}

    FECustomizationRecord(); // Decl: speed/indep/src/database/VehicleDB.hpp:139

    void Default(); // Decl: speed/indep/src/database/VehicleDB.hpp:149

    eCustomTuningType GetActiveTuning() const {}    // Decl: speed/indep/src/database/VehicleDB.hpp:152
    void SetActiveTuning(eCustomTuningType type) {} // Decl: speed/indep/src/database/VehicleDB.hpp:153

    // Decl: speed/indep/src/database/VehicleDB.hpp:155
    bool IsPreset() const {
        return this->Preset != 0;
    }

    int GetNumInstalledPartNames() {} // Decl: speed/indep/src/database/VehicleDB.hpp:158

    bool WriteRecordIntoPhysics(Attrib::Gen::pvehicle &attributes) const; // Decl: speed/indep/src/database/VehicleDB.hpp:160
    void WritePhysicsIntoRecord(const Attrib::Gen::pvehicle &attributes); // Decl: speed/indep/src/database/VehicleDB.hpp:161

    void WriteRecordIntoRide(RideInfo *ride) const; // Decl: speed/indep/src/database/VehicleDB.hpp:165
    void WriteRideIntoRecord(const RideInfo *ride); // Decl: speed/indep/src/database/VehicleDB.hpp:166

    void BecomePreset(PresetCar *preset); // Decl: speed/indep/src/database/VehicleDB.hpp:169

    CarPart *GetInstalledPart(CarType cartype, int carslotid) const; // Decl: speed/indep/src/database/VehicleDB.hpp:173

    void SetInstalledPart(int carslotid, CarPart *part); // Decl: speed/indep/src/database/VehicleDB.hpp:184

    bool GetInstalledJunkman(Physics::Upgrades::Type id) { // Decl: speed/indep/src/database/VehicleDB.hpp:251
        int mask = (1 << id);
        return (InstalledPhysics.Junkman & mask) != 0;
    }

    void SetInstalledJunkman(Physics::Upgrades::Type id, bool b) { // Decl: speed/indep/src/database/VehicleDB.hpp:255
        int mask = (1 << id);
        if (b) {
            InstalledPhysics.Junkman |= mask;
        } else {
            InstalledPhysics.Junkman &= ~mask;
        }
    }

    void SetInstalledPhysics(const Physics::Upgrades::Package &package) {
        InstalledPhysics = package;
    }

    void SetInstalledPhysics(Physics::Upgrades::Type id, int level) {
        InstalledPhysics.Part[id] = level;
    }

    const Physics::Upgrades::Package *GetInstalledPhysics() const {
        return &InstalledPhysics;
    }

    void SetTuning(Physics::Tunings::Path id, float value) {}

    void SetTuning(Physics::Tunings::Path id, eCustomTuningType type, float value) {}

    float GetTuning(Physics::Tunings::Path id) const {} // Decl: speed/indep/src/database/VehicleDB.hpp:277

    const Physics::Tunings *GetTunings() const {}

    const Physics::Tunings *GetTunings(uint32 type) const {}
};

// Decl: speed/indep/src/database/VehicleDB.hpp:302
enum ePlayerSettingsCameras {
    PSC_BUMPER = 0,
    PSC_HOOD = 1,
    PSC_CLOSE = 2,
    PSC_FAR = 3,
    PSC_SUPER_FAR = 4,
    PSC_DRIFT = 5,
    PSC_PURSUIT = 6,
    NUM_CAMERAS_IN_OPTIONS = 7,
    PSC_DEFAULT = 2
};

// total size: 0x10
// Decl: speed/indep/src/database/VehicleDB.hpp:322
class FEInfractionsData {
  public:
    FEInfractionsData() {} // Decl: speed/indep/src/database/VehicleDB.hpp:324
    FEInfractionsData(uint32 infractions);

    void operator+=(const FEInfractionsData &rhs); // Decl: speed/indep/src/database/VehicleDB.hpp:327

    uint16 Speeding;  // offset 0x0, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:331
    uint16 Racing;    // offset 0x2, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:332
    uint16 Reckless;  // offset 0x4, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:333
    uint16 Assault;   // offset 0x6, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:334
    uint16 HitAndRun; // offset 0x8, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:335
    uint16 Damage;    // offset 0xA, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:336
    uint16 Resist;    // offset 0xC, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:337
    uint16 OffRoad;   // offset 0xE, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:338

    uint16 NumInfractions() const;                                  // Decl: speed/indep/src/database/VehicleDB.hpp:340
    uint16 GetValue(GInfractionManager::InfractionType type) const; // Decl: speed/indep/src/database/VehicleDB.hpp:341
    uint32 GetFineValue() const;                                    // Decl: speed/indep/src/database/VehicleDB.hpp:342
};

// total size: 0x8
// Decl: speed/indep/src/database/VehicleDB.hpp:346
class FEImpoundData {
  public:
    typedef enum {
        IMPOUND_REASON_NONE = 0,
        IMPOUND_REASON_BUSTED_BY_CROSS = 1,
        IMPOUND_REASON_STRIKE_LIMIT_REACHED = 2,
        IMPOUND_REASON_INSUFFICIENT_FUNDS = 3,
        IMPOUND_RELEASED = 4
    } eImpoundReasons;

    // offset 0x4, size 0x1

    void Default(); // Decl: speed/indep/src/database/VehicleDB.hpp:352
    void BecomeImpounded(eImpoundReasons reason);
    bool IsImpounded() const {
        return ImpoundedState != IMPOUND_REASON_NONE;
    }
    bool IsReleasable() const {
        return ImpoundedState == IMPOUND_RELEASED;
    }
    void NotifyPlayerPaidToRelease();
    void NotifyPlayerUsedMarkerToRelease();
    bool NotifyWin();
    bool NotifyEvade();
    bool NotifyBusted();
    void AddMaxBusted();    // Decl: speed/indep/src/database/VehicleDB.hpp:359
    bool CanAddMaxBusted(); // Decl: speed/indep/src/database/VehicleDB.hpp:358
    int GetDaysBeforeRelease() {
        return DaysBeforeRelease;
    }

    uint8 MaxBusted;         // offset 0x0, size 0x1
    int8 TimesBusted;        // offset 0x1, size 0x1
    int8 ImpoundedState;     // offset 0x2, size 0x1
    uint8 DaysBeforeRelease; // offset 0x3, size 0x1
    int8 EvadeCount;         // offset 0x4, size 0x1
    int8 Pad1;               // offset 0x5, size 0x1, Decl: speed/indep/src/database/VehicleDB.hpp:367
    int16 Pad2;              // offset 0x6, size 0x2
};

// total size: 0x38
// Decl: speed/indep/src/database/VehicleDB.hpp:377
class FECareerRecord {
  public:
    void Default(); // Decl: speed/indep/src/database/VehicleDB.hpp:380

    void SetVehicleHeat(float h); // Decl: speed/indep/src/database/VehicleDB.hpp:383
    float GetVehicleHeat();       // Decl: speed/indep/src/database/VehicleDB.hpp:384

    void AdjustHeatOnEventWin();          // Decl: speed/indep/src/database/VehicleDB.hpp:387
    void AdjustHeatOnMilestoneComplete(); // Decl: speed/indep/src/database/VehicleDB.hpp:388
    void AdjustHeatOnEvadePursuit();      // Decl: speed/indep/src/database/VehicleDB.hpp:389

    void AdjustHeatOnVinylApplied(float extraAdjust);      // Decl: speed/indep/src/database/VehicleDB.hpp:391
    void AdjustHeatOnDecalApplied(float extraAdjust);      // Decl: speed/indep/src/database/VehicleDB.hpp:392
    void AdjustHeatOnPaintApplied(float extraAdjust);      // Decl: speed/indep/src/database/VehicleDB.hpp:393
    void AdjustHeatOnBodyKitApplied(float extraAdjust);    // Decl: speed/indep/src/database/VehicleDB.hpp:394
    void AdjustHeatOnHoodApplied(float extraAdjust);       // Decl: speed/indep/src/database/VehicleDB.hpp:395
    void AdjustHeatOnNumbersApplied(float extraAdjust);    // Decl: speed/indep/src/database/VehicleDB.hpp:396
    void AdjustHeatOnRimApplied(float extraAdjust);        // Decl: speed/indep/src/database/VehicleDB.hpp:397
    void AdjustHeatOnRimPaintApplied(float extraAdjust);   // Decl: speed/indep/src/database/VehicleDB.hpp:398
    void AdjustHeatOnRoofScoopApplied(float extraAdjust);  // Decl: speed/indep/src/database/VehicleDB.hpp:399
    void AdjustHeatOnSpoilerApplied(float extraAdjust);    // Decl: speed/indep/src/database/VehicleDB.hpp:400
    void AdjustHeatOnWindowTintApplied(float extraAdjust); // Decl: speed/indep/src/database/VehicleDB.hpp:401

    FECareerHandle Handle;        // offset 0x0, size 0x1, Decl: speed/indep/src/database/VehicleDB.hpp:404
    FEImpoundData TheImpoundData; // offset 0x2, size 0x8, Decl: speed/indep/src/database/VehicleDB.hpp:406
    float VehicleHeat;            // offset 0xC, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:407

    void CommitPursuitCarData(unsigned int infractions, uint32 accumulated_bounty,
                              bool pursuit_evaded);  // Decl: speed/indep/src/database/VehicleDB.hpp:410
    void ServeAllIncractions();                      // Decl: speed/indep/src/database/VehicleDB.hpp:411
    void WaiveIncractions(unsigned int infractions); // Decl: speed/indep/src/database/VehicleDB.hpp:412

    uint32 GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved) const; // Decl: speed/indep/src/database/VehicleDB.hpp:415

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

    void TweakBounty(uint32 bounty) {} // Decl: speed/indep/src/database/VehicleDB.hpp:422

  private:
    uint32 Bounty;                         // offset 0x10, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:425
    uint16 NumEvadedPursuits;              // offset 0x14, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:426
    uint16 NumBustedPursuits;              // offset 0x16, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:427
    FEInfractionsData UnservedInfractions; // offset 0x18, size 0x10, Decl: speed/indep/src/database/VehicleDB.hpp:429
    FEInfractionsData ServedInfractions;   // offset 0x28, size 0x10, Decl: speed/indep/src/database/VehicleDB.hpp:430
};

// total size: 0x14
// Decl: speed/indep/src/database/VehicleDB.hpp:437
class FECarRecord {
  public:
    FECarHandle Handle;     // offset 0x0, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:439
    Attrib::Key FEKey;      // offset 0x4, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:440
    Attrib::Key VehicleKey; // offset 0x8, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:441
    uint32 FilterBits;      // offset 0xC, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:442
    uint8 Customization;    // offset 0x10, size 0x1, Decl: speed/indep/src/database/VehicleDB.hpp:443
    uint8 CareerHandle;     // offset 0x11, size 0x1, Decl: speed/indep/src/database/VehicleDB.hpp:444
    uint16 Padd;            // offset 0x12, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:445

    FECarRecord(); // Decl: speed/indep/src/database/VehicleDB.hpp:447

    FECarRecord &operator=(const FECarRecord &other_record); // Decl: speed/indep/src/database/VehicleDB.hpp:449

    void Default();                    // Decl: speed/indep/src/database/VehicleDB.hpp:451
    bool MatchesFilter(int theFilter); // Decl: speed/indep/src/database/VehicleDB.hpp:452
    bool IsValid() {                   // Decl: speed/indep/src/database/VehicleDB.hpp:453
        return Handle != INVALID_CAR_HANDLE;
    }
    bool IsCustomized() { // Decl: speed/indep/src/database/VehicleDB.hpp:454
        return Customization != INVALID_CUSTOMIZATION_HANDLE;
    }
    bool IsCareer() { // Decl: speed/indep/src/database/VehicleDB.hpp:455
        return CareerHandle != INVALID_CAREER_HANDLE;
    }

    const char *GetDebugName();         // Decl: speed/indep/src/database/VehicleDB.hpp:458
    const char *GetManufacturerName();  // Decl: speed/indep/src/database/VehicleDB.hpp:459
    CarType GetType();                  // Decl: speed/indep/src/database/VehicleDB.hpp:460
    uint32 GetNameHash();               // Decl: speed/indep/src/database/VehicleDB.hpp:461
    uint32 GetLogoHash();               // Decl: speed/indep/src/database/VehicleDB.hpp:462
    uint32 GetManuLogoHash();           // Decl: speed/indep/src/database/VehicleDB.hpp:463
    uint32 GetCost();                   // Decl: speed/indep/src/database/VehicleDB.hpp:464
    uint32 GetReleaseFromImpoundCost(); // Decl: speed/indep/src/database/VehicleDB.hpp:465
};

// TODO: values
static const int MAX_CAREER_PURCHASED_CARS_IN_STABLE = 0; // size: 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:478
static const int MAX_CAREER_AWARDED_CARS_IN_STABLE = 0;   // size: 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:479
static const int MAX_CAREER_TOTAL_CARS = 0;               // size: 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:480

static const int MAX_CUSTOMIZE_BLOCKS = 0; // size: 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:485

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
// Decl: speed/indep/src/database/VehicleDB.hpp:532
class FEPlayerCarDB {
  public:
    FEPlayerCarDB(); // Decl: speed/indep/src/database/VehicleDB.hpp:534

    ~FEPlayerCarDB();

    void Default(); // Decl: speed/indep/src/database/VehicleDB.hpp:536

    FECarRecord *GetCarByIndex(int index); // Decl: speed/indep/src/database/VehicleDB.hpp:543
    char *SaveToBuffer(char *buffer, int32 bufsize);
    char *LoadFromBuffer(char *buffer, int32 bufsize);
    int32 GetSaveBufferSize();
    void AwardBonusCars(); // Decl: speed/indep/src/database/VehicleDB.hpp:547

    FECarRecord *GetCarRecordByHandle(FECarHandle handle); // Decl: speed/indep/src/database/VehicleDB.hpp:549

    FECustomizationRecord *GetCustomizationRecordByHandle(FECustomizationHandle handle); // Decl: speed/indep/src/database/VehicleDB.hpp:551

    FECareerRecord *GetCareerRecordByHandle(FECareerHandle handle); // Decl: speed/indep/src/database/VehicleDB.hpp:552

    FECarRecord *CreateNewCustomCar(FECarHandle fromCar); // Decl: speed/indep/src/database/VehicleDB.hpp:554
    void DeleteCustomCar(FECarHandle handle);             // Decl: speed/indep/src/database/VehicleDB.hpp:555

    void DeleteCareerCar(FECarHandle handle, bool was_sold); // Decl: speed/indep/src/database/VehicleDB.hpp:557

    FECarRecord *CreateNewPresetCar(const char *preset_name); // Decl: speed/indep/src/database/VehicleDB.hpp:559

    FECarRecord *CreateNewCareerCar(FECarHandle fromCar); // Decl: speed/indep/src/database/VehicleDB.hpp:560

    FECarRecord *AwardRivalCar(FECarHandle preset); // Decl: speed/indep/src/database/VehicleDB.hpp:562
    bool CanCreateNewCarRecord();                   // Decl: speed/indep/src/database/VehicleDB.hpp:563
    bool CanCreateNewCustomizationRecord();         // Decl: speed/indep/src/database/VehicleDB.hpp:564
    bool IsBonusCar(const char *preset_name);

    FECarRecord *GetRandomCar(int FilterBits); // Decl: speed/indep/src/database/VehicleDB.hpp:567

    void BuildRideForPlayer(FECarHandle car, int player, RideInfo *ride); // Decl: speed/indep/src/database/VehicleDB.hpp:569

    int GetNumQuickRaceCars();       // Decl: speed/indep/src/database/VehicleDB.hpp:571
    int GetNumCareerCars();          // Decl: speed/indep/src/database/VehicleDB.hpp:572
    int GetNumPurchasedCars();       // Decl: speed/indep/src/database/VehicleDB.hpp:573
    int GetNumAvailableCareerCars(); // Decl: speed/indep/src/database/VehicleDB.hpp:574

    FECarRecord *GetCheapestEarliestReleaseImpoundCar(FECarRecord *selected_car);

    bool WriteRecordIntoPhysics(FECarHandle car, Attrib::Gen::pvehicle &attributes);       // Decl: speed/indep/src/database/VehicleDB.hpp:578
    void WritePhysicsIntoRecord(FECarHandle car, const Attrib::Gen::pvehicle &attributes); // Decl: speed/indep/src/database/VehicleDB.hpp:579

    void SetCarToPreset(FECarHandle car, PresetCar *preset); // Decl: speed/indep/src/database/VehicleDB.hpp:581

    uint16 GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved); // Decl: speed/indep/src/database/VehicleDB.hpp:583
    uint16 GetNumInfractionsOnCar(FECarHandle car_handle, bool get_unserved);            // Decl: speed/indep/src/database/VehicleDB.hpp:584

    uint32 GetTotalNumInfractions(bool get_unserved); // Decl: speed/indep/src/database/VehicleDB.hpp:587
    uint32 GetTotalBounty();                          // Decl: speed/indep/src/database/VehicleDB.hpp:588
    uint32 GetTotalEvadedPursuits();                  // Decl: speed/indep/src/database/VehicleDB.hpp:589
    uint32 GetTotalBustedPursuits();                  // Decl: speed/indep/src/database/VehicleDB.hpp:590
    uint32 GetNumImpoundedCars();
    uint32 GetPreferedCarName();             // Decl: speed/indep/src/database/VehicleDB.hpp:591
    uint32 GetTotalFines(bool get_unserved); // Decl: speed/indep/src/database/VehicleDB.hpp:592
    uint32 GetNumCareerCarsWithARecord();    // Decl: speed/indep/src/database/VehicleDB.hpp:593

    class MyCallback {
      public:
        MyCallback() {}
        virtual ~MyCallback() {}
        virtual uint32 Callback(const FECareerRecord &record) const = 0;
    };

    bool IsHeroCar(FECarHandle handle); // Decl: speed/indep/src/database/VehicleDB.hpp:626

  private:
    FECarRecord *CreateCar(FECarHandle fromCar, int FilterBits);      // Decl: speed/indep/src/database/VehicleDB.hpp:640
    bool DeleteCar(FECarHandle handle, uint32 filter, bool was_sold); // Decl: speed/indep/src/database/VehicleDB.hpp:641
    void DeleteAllCars();                                             // Decl: speed/indep/src/database/VehicleDB.hpp:642

    int GetNumCars(uint32 filter); // Decl: speed/indep/src/database/VehicleDB.hpp:644

    void DeleteAllCustomizations(); // Decl: speed/indep/src/database/VehicleDB.hpp:646
    void DeleteAllCareerRecords();  // Decl: speed/indep/src/database/VehicleDB.hpp:647

    FECarRecord *CreateNewCarRecord(); // Decl: speed/indep/src/database/VehicleDB.hpp:651

    FECustomizationRecord *CreateNewCustomizationRecord(); // Decl: speed/indep/src/database/VehicleDB.hpp:654
    FECareerRecord *CreateNewCareerRecord();               // Decl: speed/indep/src/database/VehicleDB.hpp:655

    uint32 ForAllCareerRecordsSum(const MyCallback &Callback); // Decl: speed/indep/src/database/VehicleDB.hpp:663
    void BackupSoldCarHistory(uint8 sold_car);                 // Decl: speed/indep/src/database/VehicleDB.hpp:664

    FECarRecord CarTable[200];                        // offset 0x0, size 0xFA0, Decl: speed/indep/src/database/VehicleDB.hpp:666
    FECustomizationRecord Customizations[75];         // offset 0xFA0, size 0x7788, Decl: speed/indep/src/database/VehicleDB.hpp:667
    FECareerRecord CareerRecords[10];                 // offset 0x8728, size 0x578, Decl: speed/indep/src/database/VehicleDB.hpp:668
    uint32 SoldHistoryBounty;                         // offset 0x8CA0, size 0x4, Decl: speed/indep/src/database/VehicleDB.hpp:689
    uint16 SoldHistoryNumEvadedPursuits;              // offset 0x8CA4, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:690
    uint16 SoldHistoryNumBustedPursuits;              // offset 0x8CA6, size 0x2, Decl: speed/indep/src/database/VehicleDB.hpp:691
    FEInfractionsData SoldHistoryUnservedInfractions; // offset 0x8CA8, size 0x10, Decl: speed/indep/src/database/VehicleDB.hpp:692
    FEInfractionsData SoldHistoryServedInfractions;   // offset 0x8CB8, size 0x10, Decl: speed/indep/src/database/VehicleDB.hpp:693

    friend class DebugCarCustomizeScreen;
};

void AdjustStableImpound_EvadePursuit(int playerNum);

POVTypes GetPOVTypeFromPlayerCamera(ePlayerSettingsCameras cam);
bool IsPlayerCameraSelectable(POVTypes pov);

uint32 GetFECarNameHashFromFEKey(Attrib::Key feKey);

#endif
