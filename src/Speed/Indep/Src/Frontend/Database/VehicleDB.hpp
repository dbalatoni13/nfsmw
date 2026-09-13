#ifndef VEHICLEDB_HPP
#define VEHICLEDB_HPP

#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Camera/CameraInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

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

// total size: 0x198
// Decl: 102
class FECustomizationRecord {
  private:
    int16 InstalledPartIndices[CARSLOTID_NUM];    // offset 0x0, size 0x116
    Physics::Upgrades::Package InstalledPhysics;  // offset 0x118, size 0x20, Decl: 110
    Physics::Tunings Tunings[NUM_CUSTOM_TUNINGS]; // offset 0x138, size 0x54, Decl: 115
    eCustomTuningType ActiveTuning;               // offset 0x18C, size 0x4, Decl: 116
    int32 Preset;                                 // offset 0x190, size 0x4, Decl: 118
  public:
    FECustomizationHandle Handle; // offset 0x194, size 0x1, Decl: 119

    static void operator delete(void *mem, const char *name) {} // Decl: 137

    static void *operator new(size_t size, void *ptr) {}

    static void operator delete(void *mem, void *ptr) {}

    static void *operator new(size_t size) {}

    static void operator delete(void *mem, size_t size) {}

    static void *operator new(size_t size, const char *name) {}

    static void operator delete(void *mem, size_t size, const char *name) {}

    FECustomizationRecord(); // Decl: 139

    void Default(); // Decl: 149

    eCustomTuningType GetActiveTuning() const { // Decl: 152
        return ActiveTuning;
    }
    void SetActiveTuning(eCustomTuningType type) { // Decl: 153
        ActiveTuning = type;
    }

    // Decl: 155
    bool IsPreset() const {
        return this->Preset != 0;
    }

    int GetNumInstalledPartNames() {} // Decl: 158

    bool WriteRecordIntoPhysics(Attrib::Gen::pvehicle &attributes) const; // Decl: 160
    void WritePhysicsIntoRecord(const Attrib::Gen::pvehicle &attributes); // Decl: 161

    void WriteRecordIntoRide(RideInfo *ride) const; // Decl: 165
    void WriteRideIntoRecord(const RideInfo *ride); // Decl: 166

    void BecomePreset(PresetCar *preset); // Decl: 169

    CarPart *GetInstalledPart(CarType cartype, int carslotid) const; // Decl: 173

    void SetInstalledPart(int carslotid, CarPart *part); // Decl: 184

    bool GetInstalledJunkman(Physics::Upgrades::Type id) { // Decl: 251
        int mask = (1 << id);
        return (InstalledPhysics.Junkman & mask) != 0;
    }

    void SetInstalledJunkman(Physics::Upgrades::Type id, bool b) { // Decl: 255
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

    void SetTuning(Physics::Tunings::Path id, float value) {
        Tunings[ActiveTuning].Value[id] = value;
    }

    void SetTuning(Physics::Tunings::Path id, eCustomTuningType type, float value) {
        Tunings[type].Value[id] = value;
    }

    float GetTuning(Physics::Tunings::Path id) const { // Decl: 277
        return Tunings[ActiveTuning].Value[id];
    }

    const Physics::Tunings *GetTunings() const {
        return &Tunings[ActiveTuning];
    }

    const Physics::Tunings *GetTunings(uint32 type) const {
        return &Tunings[type];
    }
};

// Decl: 302
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
// Decl: 322
class FEInfractionsData {
  public:
    FEInfractionsData() { // Decl: 324
        bMemSet(this, 0, sizeof(*this));
    }
    FEInfractionsData(uint32 infractions);

    void operator+=(const FEInfractionsData &rhs); // Decl: 327

    uint16 Speeding;  // offset 0x0, size 0x2, Decl: 331
    uint16 Racing;    // offset 0x2, size 0x2, Decl: 332
    uint16 Reckless;  // offset 0x4, size 0x2, Decl: 333
    uint16 Assault;   // offset 0x6, size 0x2, Decl: 334
    uint16 HitAndRun; // offset 0x8, size 0x2, Decl: 335
    uint16 Damage;    // offset 0xA, size 0x2, Decl: 336
    uint16 Resist;    // offset 0xC, size 0x2, Decl: 337
    uint16 OffRoad;   // offset 0xE, size 0x2, Decl: 338

    uint16 NumInfractions() const;                                  // Decl: 340
    uint16 GetValue(GInfractionManager::InfractionType type) const; // Decl: 341
    uint32 GetFineValue() const;                                    // Decl: 342
};

// total size: 0x8
// Decl: 346
class FEImpoundData {
  public:
    enum eImpoundReasons {
        IMPOUND_REASON_NONE = 0,
        IMPOUND_REASON_BUSTED_BY_CROSS = 1,
        IMPOUND_REASON_STRIKE_LIMIT_REACHED = 2,
        IMPOUND_REASON_INSUFFICIENT_FUNDS = 3,
        IMPOUND_RELEASED = 4,
    };

    // offset 0x4, size 0x1

    void Default(); // Decl: 352
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
    void AddMaxBusted();    // Decl: 359
    bool CanAddMaxBusted(); // Decl: 358
    int GetDaysBeforeRelease() {
        return DaysBeforeRelease;
    }

    uint8 MaxBusted;         // offset 0x0, size 0x1
    int8 TimesBusted;        // offset 0x1, size 0x1
    int8 ImpoundedState;     // offset 0x2, size 0x1
    uint8 DaysBeforeRelease; // offset 0x3, size 0x1
    int8 EvadeCount;         // offset 0x4, size 0x1
    int8 Pad1;               // offset 0x5, size 0x1, Decl: 367
    int16 Pad2;              // offset 0x6, size 0x2
};

// total size: 0x38
// Decl: 377
class FECareerRecord {
  public:
    FECareerRecord() {}

    void Default(); // Decl: 380

    void SetVehicleHeat(float h); // Decl: 383
    float GetVehicleHeat();       // Decl: 384

    void AdjustHeatOnEventWin();          // Decl: 387
    void AdjustHeatOnMilestoneComplete(); // Decl: 388
    void AdjustHeatOnEvadePursuit();      // Decl: 389

    void AdjustHeatOnVinylApplied(float extraAdjust);      // Decl: 391
    void AdjustHeatOnDecalApplied(float extraAdjust);      // Decl: 392
    void AdjustHeatOnPaintApplied(float extraAdjust);      // Decl: 393
    void AdjustHeatOnBodyKitApplied(float extraAdjust);    // Decl: 394
    void AdjustHeatOnHoodApplied(float extraAdjust);       // Decl: 395
    void AdjustHeatOnNumbersApplied(float extraAdjust);    // Decl: 396
    void AdjustHeatOnRimApplied(float extraAdjust);        // Decl: 397
    void AdjustHeatOnRimPaintApplied(float extraAdjust);   // Decl: 398
    void AdjustHeatOnRoofScoopApplied(float extraAdjust);  // Decl: 399
    void AdjustHeatOnSpoilerApplied(float extraAdjust);    // Decl: 400
    void AdjustHeatOnWindowTintApplied(float extraAdjust); // Decl: 401

    FECareerHandle Handle;        // offset 0x0, size 0x1, Decl: 404
    FEImpoundData TheImpoundData; // offset 0x2, size 0x8, Decl: 406
    float VehicleHeat;            // offset 0xC, size 0x4, Decl: 407

    void CommitPursuitCarData(unsigned int infractions, uint32 accumulated_bounty,
                              bool pursuit_evaded);  // Decl: 410
    void ServeAllIncractions();                      // Decl: 411
    void WaiveIncractions(unsigned int infractions); // Decl: 412

    uint32 GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved) const; // Decl: 415

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

    void TweakBounty(uint32 bounty) {} // Decl: 422

  private:
    uint32 Bounty;                         // offset 0x10, size 0x4, Decl: 425
    uint16 NumEvadedPursuits;              // offset 0x14, size 0x2, Decl: 426
    uint16 NumBustedPursuits;              // offset 0x16, size 0x2, Decl: 427
    FEInfractionsData UnservedInfractions; // offset 0x18, size 0x10, Decl: 429
    FEInfractionsData ServedInfractions;   // offset 0x28, size 0x10, Decl: 430
};

// total size: 0x14
// Decl: 437
class FECarRecord {
  public:
    FECarHandle Handle;     // offset 0x0, size 0x4, Decl: 439
    Attrib::Key FEKey;      // offset 0x4, size 0x4, Decl: 440
    Attrib::Key VehicleKey; // offset 0x8, size 0x4, Decl: 441
    uint32 FilterBits;      // offset 0xC, size 0x4, Decl: 442
    uint8 Customization;    // offset 0x10, size 0x1, Decl: 443
    uint8 CareerHandle;     // offset 0x11, size 0x1, Decl: 444
    uint16 Padd;            // offset 0x12, size 0x2, Decl: 445

    FECarRecord(); // Decl: 447

    FECarRecord &operator=(const FECarRecord &other_record); // Decl: 449

    void Default();                    // Decl: 451
    bool MatchesFilter(int theFilter); // Decl: 452
    bool IsValid() {                   // Decl: 453
        return Handle != INVALID_CAR_HANDLE;
    }
    bool IsCustomized() { // Decl: 454
        return Customization != INVALID_CUSTOMIZATION_HANDLE;
    }
    bool IsCareer() { // Decl: 455
        return CareerHandle != INVALID_CAREER_HANDLE;
    }

    const char *GetDebugName();         // Decl: 458
    const char *GetManufacturerName();  // Decl: 459
    CarType GetType();                  // Decl: 460
    uint32 GetNameHash();               // Decl: 461
    uint32 GetLogoHash();               // Decl: 462
    uint32 GetManuLogoHash();           // Decl: 463
    uint32 GetCost();                   // Decl: 464
    uint32 GetReleaseFromImpoundCost(); // Decl: 465
};

// TODO: values
static const int MAX_CAREER_PURCHASED_CARS_IN_STABLE = 0; // size: 0x4, Decl: 478
static const int MAX_CAREER_AWARDED_CARS_IN_STABLE = 0;   // size: 0x4, Decl: 479
#ifdef EA_BUILD_A124
static const int MAX_CAREER_TOTAL_CARS = 10; // size: 0x4, Decl: 480
#else
static const int MAX_CAREER_TOTAL_CARS = 25; // size: 0x4, Decl: 480
#endif
static const int MAX_QUICKRACE_CARS_IN_STABLE = 20; // size: 0x4
static const int MAX_PRESET_CARS_IN_STABLE = 0;     // size: 0x4
static const int MAX_CARS_IN_STABLE = 200;          // size: 0x4
static const int MAX_CUSTOMIZE_BLOCKS = 0;          // size: 0x4, Decl: 485

enum FEPlayerCarDBFilterBits {
    FE_CAR_FILTER_LIST_STOCK = 1 << 0,
    FE_CAR_FILTER_LIST_CAREER = 1 << 1,
    FE_CAR_FILTER_LIST_QUICK_RACE = 1 << 2,
    FE_CAR_FILTER_LIST_BONUS = 1 << 3,
    FE_CAR_FILTER_LIST_PRESET = 1 << 4,
    FE_CAR_FILTER_LIST_DEBUG = 1 << 5,
    FE_CAR_FILTER_PINKSLIP = 1 << 6,
    FE_CAR_FILTER_LIST_MIN = FE_CAR_FILTER_LIST_STOCK,
    FE_CAR_FILTER_LIST_MAX = FE_CAR_FILTER_LIST_BONUS,
    FE_CAR_FILTER_LIST_MASK = 0xFFFF,
    FE_CAR_FILTER_REGION_AMERICA = 1 << 16,
    FE_CAR_FILTER_REGION_EUROPE = 1 << 17,
    FE_CAR_FILTER_REGION_JAPAN = 1 << 18,
    FE_CAR_FILTER_REGION_DEBUG = 1 << 19,
    FE_CAR_FILTER_REGION_MIN = FE_CAR_FILTER_REGION_AMERICA,
    FE_CAR_FILTER_REGION_MAX = FE_CAR_FILTER_REGION_DEBUG,
    FE_CAR_FILTER_REGION_ALL = FE_CAR_FILTER_REGION_AMERICA | FE_CAR_FILTER_REGION_EUROPE | FE_CAR_FILTER_REGION_JAPAN | FE_CAR_FILTER_REGION_DEBUG,
    FE_CAR_FILTER_REGION_MASK = 0xFFFF0000,
};

// total size: 0x8CC8
// Decl: 532
class FEPlayerCarDB {
  public:
    FEPlayerCarDB(); // Decl: 534

    ~FEPlayerCarDB();

    void Default(); // Decl: 536

    FECarRecord *GetCarByIndex(int index); // Decl: 543
    char *SaveToBuffer(char *buffer, int32 bufsize);
    char *LoadFromBuffer(char *buffer, int32 bufsize);
    int32 GetSaveBufferSize();
    void AwardBonusCars(); // Decl: 547

    FECarRecord *GetCarRecordByHandle(FECarHandle handle); // Decl: 549

    FECustomizationRecord *GetCustomizationRecordByHandle(FECustomizationHandle handle); // Decl: 551

    FECareerRecord *GetCareerRecordByHandle(FECareerHandle handle); // Decl: 552

    FECarRecord *CreateNewCustomCar(FECarHandle fromCar); // Decl: 554
    void DeleteCustomCar(FECarHandle handle);             // Decl: 555

    void DeleteCareerCar(FECarHandle handle, bool was_sold); // Decl: 557

    FECarRecord *CreateNewPresetCar(const char *preset_name); // Decl: 559

    FECarRecord *CreateNewCareerCar(FECarHandle fromCar); // Decl: 560

    FECarRecord *AwardRivalCar(FECarHandle preset); // Decl: 562
    bool CanCreateNewCarRecord();                   // Decl: 563
    bool CanCreateNewCustomizationRecord();         // Decl: 564
    bool IsBonusCar(const char *preset_name);

    FECarRecord *GetRandomCar(int FilterBits); // Decl: 567

    void BuildRideForPlayer(FECarHandle car, int player, RideInfo *ride); // Decl: 569

    int GetNumQuickRaceCars();       // Decl: 571
    int GetNumCareerCars();          // Decl: 572
    int GetNumPurchasedCars();       // Decl: 573
    int GetNumAvailableCareerCars(); // Decl: 574

    FECarRecord *GetCheapestEarliestReleaseImpoundCar(FECarRecord *selected_car);

    bool WriteRecordIntoPhysics(FECarHandle car, Attrib::Gen::pvehicle &attributes);       // Decl: 578
    void WritePhysicsIntoRecord(FECarHandle car, const Attrib::Gen::pvehicle &attributes); // Decl: 579

    void SetCarToPreset(FECarHandle car, PresetCar *preset); // Decl: 581

    uint16 GetNumInfraction(GInfractionManager::InfractionType type, bool get_unserved); // Decl: 583
    uint16 GetNumInfractionsOnCar(FECarHandle car_handle, bool get_unserved);            // Decl: 584

    uint32 GetTotalNumInfractions(bool get_unserved); // Decl: 587
    uint32 GetTotalBounty();                          // Decl: 588
    uint32 GetTotalEvadedPursuits();                  // Decl: 589
    uint32 GetTotalBustedPursuits();                  // Decl: 590
    uint32 GetNumImpoundedCars();
    uint32 GetPreferedCarName();             // Decl: 591
    uint32 GetTotalFines(bool get_unserved); // Decl: 592
    uint32 GetNumCareerCarsWithARecord();    // Decl: 593

    class MyCallback {
      public:
        MyCallback() {}
        virtual ~MyCallback() {}
        virtual uint32 Callback(const FECareerRecord &record) const = 0;
    };

    bool IsHeroCar(FECarHandle handle); // Decl: 626

  private:
    FECarRecord *CreateCar(FECarHandle fromCar, int FilterBits);      // Decl: 640
    bool DeleteCar(FECarHandle handle, uint32 filter, bool was_sold); // Decl: 641
    void DeleteAllCars();                                             // Decl: 642

    int GetNumCars(uint32 filter); // Decl: 644

    void DeleteAllCustomizations(); // Decl: 646
    void DeleteAllCareerRecords();  // Decl: 647

    FECarRecord *CreateNewCarRecord(); // Decl: 651

    FECustomizationRecord *CreateNewCustomizationRecord(); // Decl: 654
    FECareerRecord *CreateNewCareerRecord();               // Decl: 655

    uint32 ForAllCareerRecordsSum(const MyCallback &Callback); // Decl: 663
    void BackupSoldCarHistory(uint8 sold_car);                 // Decl: 664

    FECarRecord CarTable[200];                           // offset 0x0, size 0xFA0, Decl: 666
    FECustomizationRecord Customizations[75];            // offset 0xFA0, size 0x7788, Decl: 667
    FECareerRecord CareerRecords[MAX_CAREER_TOTAL_CARS]; // offset 0x8728, size 0x578, Decl: 668

    uint32 SoldHistoryBounty;                         // offset 0x8CA0, size 0x4, Decl: 689
    uint16 SoldHistoryNumEvadedPursuits;              // offset 0x8CA4, size 0x2, Decl: 690
    uint16 SoldHistoryNumBustedPursuits;              // offset 0x8CA6, size 0x2, Decl: 691
    FEInfractionsData SoldHistoryUnservedInfractions; // offset 0x8CA8, size 0x10, Decl: 692
    FEInfractionsData SoldHistoryServedInfractions;   // offset 0x8CB8, size 0x10, Decl: 693

    friend class DebugCarCustomizeScreen;
};

void AdjustStableImpound_EvadePursuit(int playerNum);
void AdjustStableHeat_EvadePursuit(int playerNum);

POVTypes GetPOVTypeFromPlayerCamera(ePlayerSettingsCameras cam);
bool IsPlayerCameraSelectable(POVTypes pov);

uint32 GetFECarNameHashFromFEKey(Attrib::Key feKey);

ePlayerSettingsCameras GetPlayerCameraFromPOVType(POVTypes type);

#endif
