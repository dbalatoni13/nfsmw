#ifndef WORLD_CARINFO_H
#define WORLD_CARINFO_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// TODO: how to import this
typedef int32 CarLoaderHandle;

typedef enum {
    CarRenderUsage_Player = 0,
    CarRenderUsage_RemotePlayer = 1,
    CarRenderUsage_AIRacer = 2,
    CarRenderUsage_AICop = 3,
    CarRenderUsage_AITraffic = 4,
    CarRenderUsage_AIHeli = 5,
    carRenderUsage_NISCar = 6,
    CarRenderUsage_Invalid = 7
} CarRenderUsage;

typedef enum { CRD_LeftFront = 0, CRD_RightFront = 1, CRD_RightRear = 2, CRD_LeftRear = 3, CRD_Max = 4 } CarRenderDoorID;

typedef enum {
    PART_UPGRADE_LEVEL_UNSPECIFIED = -1,
    PART_UPGRADE_LEVEL_STOCK = 0,
    PART_UPGRADE_LEVEL_1 = 1,
    PART_UPGRADE_LEVEL_2 = 2,
    PART_UPGRADE_LEVEL_3 = 3,
    PART_UPGRADE_LEVEL_4 = 4,
    PART_UPGRADE_LEVEL_5 = 5,
    PART_UPGRADE_LEVEL_6 = 6,
    PART_UPGRADE_LEVEL_UNIQUE = 7,
    NUM_PART_UPGRADE_LEVELS = 8
} eFEPartUpgradeLevels;

typedef eFEPartUpgradeLevels reflection_typedef_eFEPartUpgradeLevels;

typedef enum {
    UNLOCKABLE_THING_UNKNOWN = 0,
    UNLOCKABLE_THING_CUSTOMIZE_PARTS = 1,
    UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE = 2,
    UNLOCKABLE_THING_CUSTOMIZE_VISUAL = 3,
    UNLOCKABLE_THING_PUT_TIRES = 4,
    UNLOCKABLE_THING_PUT_BRAKES = 5,
    UNLOCKABLE_THING_PUT_CHASSIS = 6,
    UNLOCKABLE_THING_PUT_TRANSMISSION = 7,
    UNLOCKABLE_THING_PUT_ENGINE = 8,
    UNLOCKABLE_THING_PUT_INDUCTION = 9,
    UNLOCKABLE_THING_PUT_NOS = 10,
    UNLOCKABLE_THING_BODY_KIT = 11,
    UNLOCKABLE_THING_SPOILERS = 12,
    UNLOCKABLE_THING_RIM_BRANDS = 13,
    UNLOCKABLE_THING_HOODS = 14,
    UNLOCKABLE_THING_ROOF_SCOOPS = 15,
    UNLOCKABLE_THING_LICENSE_PLATE = 16,
    UNLOCKABLE_THING_CUSTOM_HUD = 17,
    UNLOCKABLE_THING_WINDOW_TINT = 18,
    UNLOCKABLE_THING_PAINT_METALLIC = 19,
    UNLOCKABLE_THING_PAINT_PEARL = 20,
    UNLOCKABLE_THING_PAINT_GLOSS = 21,
    UNLOCKABLE_THING_PAINT_STOCK = 22,
    UNLOCKABLE_THING_PAINTABLE_BODY = 23,
    UNLOCKABLE_THING_PAINTABLE_RIMS = 24,
    UNLOCKABLE_THING_RIM_BRAND_5_ZIGEN = 25,
    UNLOCKABLE_THING_RIM_BRAND_ADR = 26,
    UNLOCKABLE_THING_RIM_BRAND_BBS = 27,
    UNLOCKABLE_THING_RIM_BRAND_ENKEI = 28,
    UNLOCKABLE_THING_RIM_BRAND_KONIG = 29,
    UNLOCKABLE_THING_RIM_BRAND_LOWENHART = 30,
    UNLOCKABLE_THING_RIM_BRAND_RACING_HART = 31,
    UNLOCKABLE_THING_RIM_BRAND_OZ = 32,
    UNLOCKABLE_THING_RIM_BRAND_VOLK = 33,
    UNLOCKABLE_THING_RIM_BRAND_ROJA = 34,
    UNLOCKABLE_VINYLS_GROUP_FLAME = 35,
    UNLOCKABLE_VINYLS_GROUP_TRIBAL = 36,
    UNLOCKABLE_VINYLS_GROUP_STRIPE = 37,
    UNLOCKABLE_VINYLS_GROUP_RACING_FLAG = 38,
    UNLOCKABLE_VINYLS_GROUP_NATIONAL_FLAG = 39,
    UNLOCKABLE_VINYLS_GROUP_BODY = 40,
    UNLOCKABLE_VINYLS_GROUP_UNIQUE = 41,
    UNLOCKABLE_VINYLS_GROUP_CONTEST = 42,
    UNLOCKABLE_DECAL_NUMBERS = 43,
    UNLOCKABLE_DECAL_WINDSHIELD = 44,
    UNLOCKABLE_DECAL_REAR_WINDOW = 45,
    UNLOCKABLE_DECAL_LEFT_DOOR = 46,
    UNLOCKABLE_DECAL_RIGHT_DOOR = 47,
    UNLOCKABLE_DECAL_LEFT_QP = 48,
    UNLOCKABLE_DECAL_RIGHT_QP = 49,
    UNLOCKABLE_DECAL_HOOD = 50,
    UNLOCKABLE_DECAL_SLOT_1 = 51,
    UNLOCKABLE_DECAL_SLOT_2 = 52,
    UNLOCKABLE_DECAL_SLOT_3 = 53,
    UNLOCKABLE_DECAL_SLOT_4 = 54,
    UNLOCKABLE_DECAL_SLOT_5 = 55,
    UNLOCKABLE_DECAL_SLOT_6 = 56,
    NUM_UNLOCKABLES = 57
} eUnlockableEntity;

typedef eUnlockableEntity reflection_typedef_eUnlockableEntity;

struct FECarPartInfo {
    eFEPartUpgradeLevels Level;
    float Rep;
    float Cost;
};

struct type_bStringHash {
    unsigned int hash;
};

struct TrackDataDesc {
    char DebugName[32];
    uint32 Handle;
    int32 TrackNumber;
};

typedef enum {
    CAREER_UPGRADE_LEVEL_UNSPECIFIED = -1,
    CAREER_UPGRADE_LEVEL_STOCK = 0,
    CAREER_UPGRADE_LEVEL_1 = 1,
    CAREER_UPGRADE_LEVEL_2 = 2,
    CAREER_UPGRADE_LEVEL_3 = 3,
    CAREER_UPGRADE_LEVEL_UNIQUE = 4,
    NUM_CAREER_UPGRADE_LEVELS = 5
} eCareerUpgradeLevels;

typedef enum {
    UG2_VISUAL_INVALID = -1,
    UG2_VISUAL_FRONT_BUMPER = 0,
    UG2_VISUAL_REAR_BUMPER = 1,
    UG2_VISUAL_SIDE_SKIRTS = 2,
    UG2_VISUAL_SIDE_MIRRORS = 3,
    UG2_VISUAL_ROOF_SCOOP = 4,
    UG2_VISUAL_HOOD = 5,
    UG2_VISUAL_SPOILER = 6,
    UG2_VISUAL_HEADLIGHTS = 7,
    UG2_VISUAL_TAILLIGHTS = 8,
    UG2_VISUAL_MUFFLER_TIPS = 9,
    UG2_VISUAL_SPINNERS = 10,
    UG2_VISUAL_NEON = 11,
    UG2_VISUAL_RIMS = 12,
    UG2_VISUAL_WIDE_BODY = 13
} eCareerVisualPartsCategories;

struct VisualPartDesc {
    eCareerVisualPartsCategories Category;
    int Costs[5];
    int Reps[5];
};

struct BrandDesc {
    char Name[32];
    uint32 Hash;
};

typedef enum {
    PERF_PART_UNSPECIFIED = -1,
    PERF_PART_WT_REMOVE_REAR_SEATS = 0,
    PERF_PART_WT_REMOVE_INTERIOR_PANELS = 1,
    PERF_PART_WT_LIGHTWEIGHT_WINDOWS = 2,
    PERF_PART_WT_LIGHTWEIGHT_SEATS = 3,
    PERF_PART_WT_LIGHTWEIGHT_DOORS = 4,
    PERF_PART_WT_FOAM_FILLED_INTERIOR = 5,
    PERF_PART_TR_SHORT_THROW_SHIFT_KIT = 6,
    PERF_PART_TR_LIGHT_FLYWHEEL = 7,
    PERF_PART_TR_DIFFERENTIAL = 8,
    PERF_PART_TR_LIMITED_SLIP_DIFFERENTIAL = 9,
    PERF_PART_TR_HIGH_PERFORMANCE_CLUTCH = 10,
    PERF_PART_TR_6_SPEED_TRANSMISSION = 11,
    PERF_PART_NO_DRY_SHOT_OF_NITROUS = 12,
    PERF_PART_NO_WET_SHOT_OF_NITROUS = 13,
    PERF_PART_NO_DIRECT_PORT_NITROUS_OXIDE = 14,
    PERF_PART_EN_COLD_AIR_INTAKE_SYSTEM = 15,
    PERF_PART_EN_REPLACE_HEADERS = 16,
    PERF_PART_EN_MILD_CAMSHAFT_AND_CAM_GEARS = 17,
    PERF_PART_EN_CAT_BACK_EXHAUST_SYSTEM = 18,
    PERF_PART_EN_HIGH_FLOW_INTAKE_MANIFOLD = 19,
    PERF_PART_EN_LARGER_DIAMETER_DOWNPIPE = 20,
    PERF_PART_EN_RACING_CAMSHAFT_AND_GEARS = 21,
    PERF_PART_EN_PORT_AND_POLISH_HEADS = 22,
    PERF_PART_EN_BLUEPRINT_THE_BLOCK = 23,
    PERF_PART_EN_HIGH_FLOW_HEADERS = 24,
    PERF_PART_TU_STAGE_1_TURBO_KIT = 25,
    PERF_PART_TU_STAGE_2_TURBO_KIT = 26,
    PERF_PART_TU_STAGE_3_TURBO_KIT = 27,
    PERF_PART_SU_SPORT_SPRINGS_AND_SHOCKS = 28,
    PERF_PART_SU_STRUT_TOWER_BAR = 29,
    PERF_PART_SU_PERFORMANCE_SPRINGS_AND_SHOCKS = 30,
    PERF_PART_SU_FRONT_AND_REAR_SWAY_BARS = 31,
    PERF_PART_SU_FRONT_AND_REAR_TIE_BARS = 32,
    PERF_PART_SU_UPGRADE_BUSHINGS = 33,
    PERF_PART_SU_COIL_OVER_SUSPENSION_SYSTEM = 34,
    PERF_PART_SU_CAMBER_KIT = 35,
    PERF_PART_SU_LARGE_DIAMETER_SWAY_BARS = 36,
    PERF_PART_BR_STREET_COMPOUND_BRAKE_PADS = 37,
    PERF_PART_BR_STEEL_BRAIDED_BRAKE_LINES = 38,
    PERF_PART_BR_CROSS_DRILLED_ROTORS = 39,
    PERF_PART_BR_LARGE_DIAMETER_ROTORS = 40,
    PERF_PART_BR_RACE_COMPOUND_BRAKE_PADS = 41,
    PERF_PART_BR_CROSS_DRILLED_AND_SLOTTED_ROTORS = 42,
    PERF_PART_BR_6_PISTON_RACING_CALIPERS = 43,
    PERF_PART_EC_PERFORMANCE_CHIP = 44,
    PERF_PART_EC_HIGH_FLOW_FUEL_PUMP = 45,
    PERF_PART_EC_FUEL_PRESSURE_REGULATOR = 46,
    PERF_PART_EC_FUEL_RAIL = 47,
    PERF_PART_EC_FUEL_FILTER = 48,
    PERF_PART_EC_ENGINE_MANAGEMENT_UNIT = 49,
    PERF_PART_EC_FUEL_INJECTORS = 50,
    PERF_PART_TI_STREET_PERFORMANCE_TIRES = 51,
    PERF_PART_TI_PRO_PERFORMANCE_TIRES = 52,
    PERF_PART_TI_EXTREME_PERFORMANCE_TIRES = 53,
    PERF_PART_NUM_PERF_PARTS = 54
} ePhysicsPerformanceParts;

typedef enum {
    PERF_PACKAGE_UNSPECIFIED = -1,
    PERF_PACKAGE_TYPE_WEIGHT_REDUCTION = 0,
    PERF_PACKAGE_TYPE_DRIVETRAIN = 1,
    PERF_PACKAGE_TYPE_NOS = 2,
    PERF_PACKAGE_TYPE_ENGINE = 3,
    PERF_PACKAGE_TYPE_TURBO = 4,
    PERF_PACKAGE_TYPE_SUSPENSION = 5,
    PERF_PACKAGE_TYPE_BRAKES = 6,
    PERF_PACKAGE_TYPE_ECU = 7,
    PERF_PACKAGE_TYPE_TIRES = 8,
    NUM_PERF_PACKAGE_TYPES = 9
} ePerfPackageTypes;

class PerfPackagePart {
  public:
    int GetNumWeights() {};
    int GetNumReplaces() {};
    int GetNumDependencies() {};
    float GetAverageWeight() {};

    uint32 UID;
    uint32 LanguageHash;
    int32 Price;
    uint32 NumBrands;
    int Brands[8];
    float PhysicsWeight[4];
    int ReplacesUIDs[5];
    int DependencyUIDs[2];
};

class PerformancePackage {
  public:
    ePerfPackageTypes Type;
    eCareerUpgradeLevels Level;
    uint32 NumParts;
    PerfPackagePart PartsList[4];
};

typedef enum {
    CAR_USAGE_TYPE_RACING = 0,
    CAR_USAGE_TYPE_COP = 1,
    CAR_USAGE_TYPE_TRAFFIC = 2,
    CAR_USAGE_TYPE_WHEELS = 3,
    CAR_USAGE_TYPE_UNIVERSAL = 4
} CarUsageType;

typedef enum {
    CARPARTID_INVALID = -1,
    CARPARTID_BASE = 0,
    CARPARTID_DAMAGE_FRONT_WINDOW = 1,
    CARPARTID_DAMAGE_BODY = 2,
    CARPARTID_DAMAGE_COP_LIGHTS = 3,
    CARPARTID_DAMAGE_COP_SPOILER = 4,
    CARPARTID_DAMAGE_FRONT_WHEEL = 5,
    CARPARTID_DAMAGE_LEFT_BRAKELIGHT = 6,
    CARPARTID_DAMAGE_RIGHT_BRAKELIGHT = 7,
    CARPARTID_DAMAGE_LEFT_HEADLIGHT = 8,
    CARPARTID_DAMAGE_RIGHT_HEADLIGHT = 9,
    CARPARTID_DAMAGE_HOOD = 10,
    CARPARTID_DAMAGE_BUSHGUARD = 11,
    CARPARTID_DAMAGE_FRONT_BUMPER = 12,
    CARPARTID_DAMAGE_RIGHT_DOOR = 13,
    CARPARTID_DAMAGE_RIGHT_REAR_DOOR = 14,
    CARPARTID_DAMAGE_TRUNK = 15,
    CARPARTID_DAMAGE_REAR_BUMPER = 16,
    CARPARTID_DAMAGE_REAR_LEFT_WINDOW = 17,
    CARPARTID_DAMAGE_FRONT_LEFT_WINDOW = 18,
    CARPARTID_DAMAGE_FRONT_RIGHT_WINDOW = 19,
    CARPARTID_DAMAGE_REAR_RIGHT_WINDOW = 20,
    CARPARTID_DAMAGE_LEFT_DOOR = 21,
    CARPARTID_DAMAGE_LEFT_REAR_DOOR = 22,
    CARPARTID_BODY = 23,
    CARPARTID_FRONT_BRAKE = 24,
    CARPARTID_FRONT_LEFT_WINDOW = 25,
    CARPARTID_FRONT_RIGHT_WINDOW = 26,
    CARPARTID_FRONT_WINDOW = 27,
    CARPARTID_INTERIOR = 28,
    CARPARTID_LEFT_BRAKELIGHT = 29,
    CARPARTID_LEFT_BRAKELIGHT_GLASS = 30,
    CARPARTID_LEFT_HEADLIGHT = 31,
    CARPARTID_LEFT_HEADLIGHT_GLASS = 32,
    CARPARTID_LEFT_SIDE_MIRROR = 33,
    CARPARTID_REAR_BRAKE = 34,
    CARPARTID_REAR_LEFT_WINDOW = 35,
    CARPARTID_REAR_RIGHT_WINDOW = 36,
    CARPARTID_REAR_WINDOW = 37,
    CARPARTID_RIGHT_BRAKELIGHT = 38,
    CARPARTID_RIGHT_BRAKELIGHT_GLASS = 39,
    CARPARTID_RIGHT_HEADLIGHT = 40,
    CARPARTID_RIGHT_HEADLIGHT_GLASS = 41,
    CARPARTID_RIGHT_SIDE_MIRROR = 42,
    CARPARTID_DRIVER = 43,
    CARPARTID_SPOILER = 44,
    CARPARTID_UNIVERSAL_SPOILER_BASE = 45,
    CARPARTID_DAMAGE0_FRONT = 46,
    CARPARTID_DAMAGE0_FRONTLEFT = 47,
    CARPARTID_DAMAGE0_FRONTRIGHT = 48,
    CARPARTID_DAMAGE0_REAR = 49,
    CARPARTID_DAMAGE0_REARLEFT = 50,
    CARPARTID_DAMAGE0_REARRIGHT = 51,
    CARPARTID_ATTACHMENT0 = 52,
    CARPARTID_ATTACHMENT1 = 53,
    CARPARTID_ATTACHMENT2 = 54,
    CARPARTID_ATTACHMENT3 = 55,
    CARPARTID_ATTACHMENT4 = 56,
    CARPARTID_ATTACHMENT5 = 57,
    CARPARTID_ATTACHMENT6 = 58,
    CARPARTID_ATTACHMENT7 = 59,
    CARPARTID_ATTACHMENT8 = 60,
    CARPARTID_ATTACHMENT9 = 61,
    CARPARTID_ROOF = 62,
    CARPARTID_HOOD = 63,
    CARPARTID_HEADLIGHT = 64,
    CARPARTID_BRAKELIGHT = 65,
    CARPARTID_BRAKE = 66,
    CARPARTID_WHEEL = 67,
    CARPARTID_SPINNER = 68,
    CARPARTID_LICENSE_PLATE = 69,
    CARPARTID_DECAL_FRONT_WINDOW = 70,
    CARPARTID_DECAL_REAR_WINDOW = 71,
    CARPARTID_DECAL_LEFT_DOOR = 72,
    CARPARTID_DECAL_RIGHT_DOOR = 73,
    CARPARTID_DECAL_LEFT_QUARTER = 74,
    CARPARTID_DECAL_RIGHT_QUARTER = 75,
    CARPARTID_PAINT = 76,
    CARPARTID_VINYL_PAINT = 77,
    CARPARTID_RIM_PAINT = 78,
    CARPARTID_VINYL = 79,
    CARPARTID_DECAL_TEXTURE = 80,
    CARPARTID_WINDOW_TINT = 81,
    CARPARTID_CUSTOM_HUD = 82,
    CARPARTID_CUSTOM_HUD_PAINT = 83,
    CARPARTID_CV = 84,
    CARPARTID_WHEEL_MANUFACTURER = 85,
    CARPARTID_MISC = 86,
    CARPARTID_NUM = 87
} CAR_PART_ID;

typedef enum {
    CARSLOTID_BASE = 0,
    CARSLOTID_DAMAGE_FRONT_WINDOW = 1,
    CARSLOTID_DAMAGE_BODY = 2,
    CARSLOTID_DAMAGE_COP_LIGHTS = 3,
    CARSLOTID_DAMAGE_COP_SPOILER = 4,
    CARSLOTID_DAMAGE_FRONT_WHEEL = 5,
    CARSLOTID_DAMAGE_LEFT_BRAKELIGHT = 6,
    CARSLOTID_DAMAGE_RIGHT_BRAKELIGHT = 7,
    CARSLOTID_DAMAGE_LEFT_HEADLIGHT = 8,
    CARSLOTID_DAMAGE_RIGHT_HEADLIGHT = 9,
    CARSLOTID_DAMAGE_HOOD = 10,
    CARSLOTID_DAMAGE_BUSHGUARD = 11,
    CARSLOTID_DAMAGE_FRONT_BUMPER = 12,
    CARSLOTID_DAMAGE_RIGHT_DOOR = 13,
    CARSLOTID_DAMAGE_RIGHT_REAR_DOOR = 14,
    CARSLOTID_DAMAGE_TRUNK = 15,
    CARSLOTID_DAMAGE_REAR_BUMPER = 16,
    CARSLOTID_DAMAGE_REAR_LEFT_WINDOW = 17,
    CARSLOTID_DAMAGE_FRONT_LEFT_WINDOW = 18,
    CARSLOTID_DAMAGE_FRONT_RIGHT_WINDOW = 19,
    CARSLOTID_DAMAGE_REAR_RIGHT_WINDOW = 20,
    CARSLOTID_DAMAGE_LEFT_DOOR = 21,
    CARSLOTID_DAMAGE_LEFT_REAR_DOOR = 22,
    CARSLOTID_BODY = 23,
    CARSLOTID_FRONT_BRAKE = 24,
    CARSLOTID_FRONT_LEFT_WINDOW = 25,
    CARSLOTID_FRONT_RIGHT_WINDOW = 26,
    CARSLOTID_FRONT_WINDOW = 27,
    CARSLOTID_INTERIOR = 28,
    CARSLOTID_LEFT_BRAKELIGHT = 29,
    CARSLOTID_LEFT_BRAKELIGHT_GLASS = 30,
    CARSLOTID_LEFT_HEADLIGHT = 31,
    CARSLOTID_LEFT_HEADLIGHT_GLASS = 32,
    CARSLOTID_LEFT_SIDE_MIRROR = 33,
    CARSLOTID_REAR_BRAKE = 34,
    CARSLOTID_REAR_LEFT_WINDOW = 35,
    CARSLOTID_REAR_RIGHT_WINDOW = 36,
    CARSLOTID_REAR_WINDOW = 37,
    CARSLOTID_RIGHT_BRAKELIGHT = 38,
    CARSLOTID_RIGHT_BRAKELIGHT_GLASS = 39,
    CARSLOTID_RIGHT_HEADLIGHT = 40,
    CARSLOTID_RIGHT_HEADLIGHT_GLASS = 41,
    CARSLOTID_RIGHT_SIDE_MIRROR = 42,
    CARSLOTID_DRIVER = 43,
    CARSLOTID_SPOILER = 44,
    CARSLOTID_UNIVERSAL_SPOILER_BASE = 45,
    CARSLOTID_DAMAGE0_FRONT = 46,
    CARSLOTID_DAMAGE0_FRONTLEFT = 47,
    CARSLOTID_DAMAGE0_FRONTRIGHT = 48,
    CARSLOTID_DAMAGE0_REAR = 49,
    CARSLOTID_DAMAGE0_REARLEFT = 50,
    CARSLOTID_DAMAGE0_REARRIGHT = 51,
    CARSLOTID_ATTACHMENT0 = 52,
    CARSLOTID_ATTACHMENT1 = 53,
    CARSLOTID_ATTACHMENT2 = 54,
    CARSLOTID_ATTACHMENT3 = 55,
    CARSLOTID_ATTACHMENT4 = 56,
    CARSLOTID_ATTACHMENT5 = 57,
    CARSLOTID_ATTACHMENT6 = 58,
    CARSLOTID_ATTACHMENT7 = 59,
    CARSLOTID_ATTACHMENT8 = 60,
    CARSLOTID_ATTACHMENT9 = 61,
    CARSLOTID_ROOF = 62,
    CARSLOTID_HOOD = 63,
    CARSLOTID_HEADLIGHT = 64,
    CARSLOTID_BRAKELIGHT = 65,
    CARSLOTID_FRONT_WHEEL = 66,
    CARSLOTID_REAR_WHEEL = 67,
    CARSLOTID_SPINNER = 68,
    CARSLOTID_LICENSE_PLATE = 69,
    CARSLOTID_DECAL_FRONT_WINDOW = 70,
    CARSLOTID_DECAL_REAR_WINDOW = 71,
    CARSLOTID_DECAL_LEFT_DOOR = 72,
    CARSLOTID_DECAL_RIGHT_DOOR = 73,
    CARSLOTID_DECAL_LEFT_QUARTER = 74,
    CARSLOTID_DECAL_RIGHT_QUARTER = 75,
    CARSLOTID_BASE_PAINT = 76,
    CARSLOTID_VINYL_LAYER0 = 77,
    CARSLOTID_PAINT_RIM = 78,
    CARSLOTID_VINYL_COLOUR0_0 = 79,
    CARSLOTID_VINYL_COLOUR0_1 = 80,
    CARSLOTID_VINYL_COLOUR0_2 = 81,
    CARSLOTID_VINYL_COLOUR0_3 = 82,
    CARSLOTID_DECAL_FRONT_WINDOW_TEX0 = 83,
    CARSLOTID_DECAL_FRONT_WINDOW_TEX1 = 84,
    CARSLOTID_DECAL_FRONT_WINDOW_TEX2 = 85,
    CARSLOTID_DECAL_FRONT_WINDOW_TEX3 = 86,
    CARSLOTID_DECAL_FRONT_WINDOW_TEX4 = 87,
    CARSLOTID_DECAL_FRONT_WINDOW_TEX5 = 88,
    CARSLOTID_DECAL_FRONT_WINDOW_TEX6 = 89,
    CARSLOTID_DECAL_FRONT_WINDOW_TEX7 = 90,
    CARSLOTID_DECAL_REAR_WINDOW_TEX0 = 91,
    CARSLOTID_DECAL_REAR_WINDOW_TEX1 = 92,
    CARSLOTID_DECAL_REAR_WINDOW_TEX2 = 93,
    CARSLOTID_DECAL_REAR_WINDOW_TEX3 = 94,
    CARSLOTID_DECAL_REAR_WINDOW_TEX4 = 95,
    CARSLOTID_DECAL_REAR_WINDOW_TEX5 = 96,
    CARSLOTID_DECAL_REAR_WINDOW_TEX6 = 97,
    CARSLOTID_DECAL_REAR_WINDOW_TEX7 = 98,
    CARSLOTID_DECAL_LEFT_DOOR_TEX0 = 99,
    CARSLOTID_DECAL_LEFT_DOOR_TEX1 = 100,
    CARSLOTID_DECAL_LEFT_DOOR_TEX2 = 101,
    CARSLOTID_DECAL_LEFT_DOOR_TEX3 = 102,
    CARSLOTID_DECAL_LEFT_DOOR_TEX4 = 103,
    CARSLOTID_DECAL_LEFT_DOOR_TEX5 = 104,
    CARSLOTID_DECAL_LEFT_DOOR_TEX6 = 105,
    CARSLOTID_DECAL_LEFT_DOOR_TEX7 = 106,
    CARSLOTID_DECAL_RIGHT_DOOR_TEX0 = 107,
    CARSLOTID_DECAL_RIGHT_DOOR_TEX1 = 108,
    CARSLOTID_DECAL_RIGHT_DOOR_TEX2 = 109,
    CARSLOTID_DECAL_RIGHT_DOOR_TEX3 = 110,
    CARSLOTID_DECAL_RIGHT_DOOR_TEX4 = 111,
    CARSLOTID_DECAL_RIGHT_DOOR_TEX5 = 112,
    CARSLOTID_DECAL_RIGHT_DOOR_TEX6 = 113,
    CARSLOTID_DECAL_RIGHT_DOOR_TEX7 = 114,
    CARSLOTID_DECAL_LEFT_QUARTER_TEX0 = 115,
    CARSLOTID_DECAL_LEFT_QUARTER_TEX1 = 116,
    CARSLOTID_DECAL_LEFT_QUARTER_TEX2 = 117,
    CARSLOTID_DECAL_LEFT_QUARTER_TEX3 = 118,
    CARSLOTID_DECAL_LEFT_QUARTER_TEX4 = 119,
    CARSLOTID_DECAL_LEFT_QUARTER_TEX5 = 120,
    CARSLOTID_DECAL_LEFT_QUARTER_TEX6 = 121,
    CARSLOTID_DECAL_LEFT_QUARTER_TEX7 = 122,
    CARSLOTID_DECAL_RIGHT_QUARTER_TEX0 = 123,
    CARSLOTID_DECAL_RIGHT_QUARTER_TEX1 = 124,
    CARSLOTID_DECAL_RIGHT_QUARTER_TEX2 = 125,
    CARSLOTID_DECAL_RIGHT_QUARTER_TEX3 = 126,
    CARSLOTID_DECAL_RIGHT_QUARTER_TEX4 = 127,
    CARSLOTID_DECAL_RIGHT_QUARTER_TEX5 = 128,
    CARSLOTID_DECAL_RIGHT_QUARTER_TEX6 = 129,
    CARSLOTID_DECAL_RIGHT_QUARTER_TEX7 = 130,
    CARSLOTID_WINDOW_TINT = 131,
    CARSLOTID_CUSTOM_HUD = 132,
    CARSLOTID_HUD_BACKING_COLOUR = 133,
    CARSLOTID_HUD_NEEDLE_COLOUR = 134,
    CARSLOTID_HUD_CHARACTER_COLOUR = 135,
    CARSLOTID_CV = 136,
    CARSLOTID_WHEEL_MANUFACTURER = 137,
    CARSLOTID_MISC = 138,
    CARSLOTID_NUM = 139,
    CARSLOTID_MODEL_FIRST = 0,
    CARSLOTID_MODEL_LAST = 75,
    CARSLOTID_MODEL_NUM = 76,
    CARSLOTID_DAMAGE_PLAYER_MODEL_FIRST = 46,
    CARSLOTID_DAMAGE_PLAYER_MODEL_LAST = 51,
    CARSLOTID_DAMAGE_PLAYER_MODEL_NUM = 6,
    CARSLOTID_DAMAGE_MODEL_FIRST = 1,
    CARSLOTID_DAMAGE_MODEL_LAST = 23,
    CARSLOTID_DAMAGE_MODEL_NUM = 23,
    CARSLOTID_VINYL_LAYER_FIRST = 77,
    CARSLOTID_VINYL_LAYER_LAST = 77,
    CARSLOTID_VINYL_LAYER_NUM = 1,
    CARSLOTID_VINYL_COLOUR_FIRST = 79,
    CARSLOTID_VINYL_COLOUR_LAST = 82,
    CARSLOTID_VINYL_COLOUR_NUM = 4,
    CARSLOTID_DECAL_MODEL_FIRST = 70,
    CARSLOTID_DECAL_MODEL_LAST = 75,
    CARSLOTID_DECAL_MODEL_NUM = 6,
    CARSLOTID_DECAL_TEXTURE_FIRST = 83,
    CARSLOTID_DECAL_TEXTURE_LAST = 130,
    CARSLOTID_DECAL_TEXTURE_NUM = 48,
    CARSLOTID_DECAL_LEFT_DOOR_LNUMBER = 105,
    CARSLOTID_DECAL_LEFT_DOOR_RNUMBER = 106,
    CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER = 113,
    CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER = 114
} CAR_SLOT_ID;

class CarPartIDNameMap {
  public:
    int CarPartID;
    char *Name;
    UCrc32 Crc;
};

class CarSlotIDNameMap {
  public:
    int CarSlotID;
    char *Name;
};

struct CarSlotTypeOverride {
    uint32 CarType;
    uint32 SlotId;
    unsigned int LookupType[2];
};

class PresetCar : bTNode<PresetCar> {
  public:
    char CarTypeName[32];
    char PresetName[32];
    uint64_t FEKey;
    uint64_t VehicleKey;
    uint32 FilterBits;
    int32 PhysicsLevel;
    int PartNameHashes[139];
};

typedef enum {
    CARPART_LOD_OFF = -1,
    CARPART_LOD_A = 0,
    CARPART_LOD_B = 1,
    CARPART_LOD_C = 2,
    CARPART_LOD_D = 3,
    CARPART_LOD_E = 4,
    CARPART_LOD_NUM = 5
} CARPART_LOD;

typedef enum { CARANIM_OPEN = 0, CARANIM_CLOSED = 1, CARANIM_OPENING = 2, CARANIM_CLOSING = 3 } CarAnimState;

typedef enum { CARANIM_HOOD = 0, CARANIM_TRUNK = 1, CARANIM_LEFT_DOOR = 2, CARANIM_RIGHT_DOOR = 3, CARANIM_NUM = 4 } CarAnimLocation;

class CarPartAttribute {
  public:
    char *GetName() {};
    uint32 GetNameHash() {
        return NameHash;
    };
    float GetFParam() {
        return Params.fParam;
    };
    int32 GetIParam() {
        return Params.iParam;
    };
    uint32 GetUParam() {
        return Params.uParam;
    };
    void EndianSwap() {};

    uint32 NameHash; // offset 0x0, size 0x4
    union {
        float fParam;  // offset 0x0, size 0x4
        int32 iParam;  // offset 0x0, size 0x4
        uint32 uParam; // offset 0x0, size 0x4
    } Params;
};

class CarPartAttributeTable {
    void EndianSwap() {};
    uint32 GetByteSize() {};
    CarPartAttribute *GetAttribute() {};

    int16 NumAttributes;
    int16 AttributeOffsetTable[1];
};

class CarPartModelTable {
  public:
    void EndianSwap() {};
    uint32 GetModelNameHash(uint32 base_namehash, int model_num, int lod);

    int8 TemplatedNameHashes;
    int8 pad;
    uint16 MiddleStringOffset;
    char *ModelNames[1][5];
};

class CarPart {
  public:
    CarPart() {}
    ~CarPart() {}
    inline void InPlaceInit() {}
    const char *GetName();
    CarPartAttribute *GetFirstAppliedAttribute(uint32 namehash);
    CarPartAttribute *GetLastAppliedAttribute(uint32 namehash);
    CarPartAttribute *GetNextAppliedAttribute(uint32 namehash, CarPartAttribute *prev_attribute);
    CarPartAttribute *GetPrevAppliedAttribute(uint32 namehash, CarPartAttribute *next_attribute);
    int HasAppliedAttribute(uint32 namehash);
    const char *GetAppliedAttributeString(uint32 namehash, const char *default_string);
    float GetAppliedAttributeFParam(uint32 namehash, float default_value);
    int32 GetAppliedAttributeIParam(uint32 namehash, int default_value);
    uint32 GetAppliedAttributeUParam(uint32 namehash, uint32 default_value);
    uint32 GetBrandNameHash() {};
    uint32 GetTextureNameHash() {};
    uint32 GetLightMaterialNameHash() {};
    int8 GetInnerRadius() {};
    int8 GetOuterRadius() {};
    int8 GetSpokeCount() {};
    bool GetMirrored() {};
    uint32 GetCarTypeNameHash();
    uint32 GetPartNameHash() {};
    int8 GetPartID() {};
    int8 GetUpgradeLevel() {};
    int8 GetGroupNumber() {};
    uint32 GetModelNameHash(int model_num, int lod);
    void Print();
    void EndianSwap() {};

  private:
    CarPartAttribute *GetAttribute(uint32 namehash, CarPartAttribute *prev_attribute);

    uint16 PartNameHashBot;
    uint16 PartNameHashTop;
    int8 PartID;
    uint8 GroupNumber_UpgradeLevel;
    int8 BaseModelNameHashSelector;
    uint8 CarTypeNameHashIndex;
    uint16 NameOffset;
    uint16 AttributeTableOffset;
    uint16 ModelNameHashTableOffset;
};

class CarPartPack : bTNode<CarPartPack> {
  public:
    CarPartPack() {};
    ~CarPartPack() {};
    void InPlaceInit() {};
    void EndianSwap() {};

    uint32 Version;
    char *StringTable;
    uint32 StringTableSize;
    CarPartAttributeTable *AttributeTableTable;
    uint32 NumAttributeTables;
    CarPartAttribute *AttributesTable;
    uint32 NumAttributes;
    uint32 *TypeNameTable;
    uint32 NumTypeNames;
    CarPartModelTable *ModelTable;
    uint32 NumModelTables;
    CarPart *PartsTable;
    uint32 NumParts;
};

class CarPartIndex {
  public:
    CarPart *Part;
    int NumParts;
};

class CarPartDatabase {
  public:
    CarPartDatabase();
    const char *GetCarPartIDName(int car_part_id);
    uint32 GetCarTypeNameHash(CarType car_type);
    CarType GetCarType(uint32 car_type_name_hash);
    int GetPartIndex(uint32 cartype_namehash, int32 car_part_id, uint32 car_part_namehash);
    int GetPartIndex(CarPart *car_part);
    CarPart *GetCarPartByIndex(int index);
    int32 NewGetNumCarParts(CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upgrade_level);
    CarPart *NewGetCarPart(CarType car_type, int32 car_slot_id, uint32 car_part_namehash, CarPart *prev_part, int32 upg_level);
    CarPart *NewGetFirstCarPart(CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upg_level);
    CarPart *NewGetLastCarPart(CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upg_level);
    CarPart *NewGetPrevCarPart(CarPart *car_part, CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upg_level);
    CarPart *NewGetNextCarPart(CarPart *car_part, CarType car_type, int32 car_slot_id, uint32 car_part_namehash, int32 upg_level);

    bTList<CarPartPack> CarPartPackList;
    int NumPacks;
    int NumParts;
    int NumBytes;
    CarPartIndex PaintPart_Gloss[3];
    CarPartIndex PaintPart_Metallic[3];
    CarPartIndex PaintPart_Pearl[3];
    CarPartIndex PaintPart_Vinyl[3];
    CarPartIndex PaintPart_Rims[3];
    CarPartIndex PaintPart_Caliper[3];
    CarPartIndex VinylPart_All[3];
    CarPartIndex VinylPart_Body[3];
    CarPartIndex VinylPart_Hood[3];
    CarPartIndex VinylPart_Side[3];
    CarPartIndex VinylPart_Manufacturer[3];
};

class CarTypeInfo {
  public:
    char *GetName() {};
    char *GetCarTypeName() {};
    char *GetBaseModelName() {};
    CarType GetCarType() {};
    CarUsageType GetCarUsageType() {};
    bool HasArtwork() {};
    int IsConvertable() {};
    bVector3 *GetHeadlightPosition() {};
    float GetHeadlightFOV() {};
    bool IsUG2Car() {};
    bool IsMWCar() {};
    uint32 GetDefaultBasePaint() {};
    int GetNumAvailableSkins();
    int GetAvailableSkin(int index);
    int GetRandomSkin(unsigned int *pseed);
    int GetDefaultSkinNumber() {};
    int GetSkinnable() {};
    int GetWheelOuterRadius() {};
    int GetWheelInnerRadiusMin() {};
    int GetWheelInnerRadiusMax() {};

    char CarTypeName[16];
    char BaseModelName[16];
    char GeometryFilename[32];
    char ManufacturerName[16];
    uint32 CarTypeNameHash;
    float HeadlightFOV;
    int8 padHighPerformance;
    int8 NumAvailableSkinNumbers;
    int8 WhatGame;
    int8 ConvertableFlag;
    int8 WheelOuterRadius;
    int8 WheelInnerRadiusMin;
    int8 WheelInnerRadiusMax;
    int8 pad0;
    bVector3 HeadlightPosition;
    bVector3 DriverRenderingOffset;
    bVector3 InCarSteeringWheelRenderingOffset;
    CarType Type;
    CarUsageType UsageType;
    uint32 CarMemTypeHash;
    int8 MaxInstances[5];
    int8 WantToKeepLoaded[5];
    int8 pad4[2];
    float MinTimeBetweenUses[5];
    char AvailableSkinNumbers[10];
    int8 DefaultSkinNumber;
    int8 Skinnable;
    int32 Padding;
    int32 DefaultBasePaint;
};

class RideInfo {
  public:
    RideInfo() {
        Init(CARTYPE_NONE, CarRenderUsage_Player, 0, 0);
    }

    RideInfo(CarType type, int skin_number, int has_dash, int can_be_vertex_damaged) {}

    CARPART_LOD GetMinLodLevel() const {}

    CARPART_LOD GetMaxLodLevel() const {}

    CARPART_LOD GetMinFELodLevel() const {}

    CARPART_LOD GetMaxFELodLevel() const {}

    CARPART_LOD GetMaxLicenseLodLevel() const {}

    CARPART_LOD GetMinTrafficDiffuseLodLevel() const {}

    CARPART_LOD GetMinReflectionLodLevel() const {}

    CARPART_LOD GetMinShadowLodLevel() const {}

    CARPART_LOD GetMaxShadowLodLevel() const {}

    CARPART_LOD GetMaxTireLodLevel() const {}

    CARPART_LOD GetMaxBrakeLodLevel() const {}

    CARPART_LOD GetMaxSpoilerLodLevel() const {}

    CARPART_LOD GetMaxRoofScoopLodLevel() const {}

    void Init(CarType type, CarRenderUsage usage, int has_dash, int can_be_vertex_damaged);

    int GetSpecialLODRangeForCarSlot(int32 slot_id, CARPART_LOD *special_minimum, CARPART_LOD *special_maximum, bool in_front_end);

    void SetCarLoaderHandle(int car_loader_handle) {}

    CarLoaderHandle GetCarLoaderHandle() {}

    CarRenderUsage GetCarRenderUsage() {}

    void SetUpgradePart(CAR_SLOT_ID car_slot_id, int upg_level);

    void SetStockParts();

    void SetRandomPart(CAR_SLOT_ID slot, int upgrade_level);

    void SetRandomParts();

    void SetRandomPaint();

    CarPart *GetPart(int32 car_slot_id) const;

    CarPart *SetPart(int32 car_slot_id, CarPart *car_part, bool update_enabled);

    void UpdatePartsEnabled();

    int IsPartEnabled(int32 car_part_id);

    void DisablePart(int32 car_part_id) {}

    void EnablePart(int32 car_part_id) {}

    void SetPreviewPart(CarPart *part) {}

    CarPart *GetPreviewPart() {}

    uint8 NumbersInstalled();

    uint8 VinylsInstalled();

    float DecalsInstalledPercent();

    uint32 GetSkinNameHash();

    void SetCompositeNameHash(int skin_number);

    uint32 GetCompositeSkinNameHash();

    void SetCompositeSkinNameHash(uint32 namehash);

    uint32 GetCompositeWheelNameHash();

    void SetCompositeWheelNameHash(uint32 namehash);

    uint32 GetCompositeSpinnerNameHash();

    void SetCompositeSpinnerNameHash(uint32 namehash);

    int IsUsingCompositeSkin();

    uint32 GetCollisionVolumeNameHash();

    uint32 GetDefaultCollisionVolumeNameHash();

    void Print();

    void DumpForPreset(class FECarRecord *car);

    void FillWithPreset(uint32 preset_name_hash);

    CarType Type;            // offset 0x0, size 0x4
    char InstanceIndex;      // offset 0x4, size 0x1
    char HasDash;            // offset 0x5, size 0x1
    char CanBeVertexDamaged; // offset 0x6, size 0x1
    char SkinType;           // offset 0x7, size 0x1

  private:
    CARPART_LOD mMinLodLevel;               // offset 0x8, size 0x4
    CARPART_LOD mMaxLodLevel;               // offset 0xC, size 0x4
    CARPART_LOD mMinFELodLevel;             // offset 0x10, size 0x4
    CARPART_LOD mMaxFELodLevel;             // offset 0x14, size 0x4
    CARPART_LOD mMaxLicenseLodLevel;        // offset 0x18, size 0x4
    CARPART_LOD mMinTrafficDiffuseLodLevel; // offset 0x1C, size 0x4
    CARPART_LOD mMinShadowLodLevel;         // offset 0x20, size 0x4
    CARPART_LOD mMaxShadowLodLevel;         // offset 0x24, size 0x4
    CARPART_LOD mMaxTireLodLevel;           // offset 0x28, size 0x4
    CARPART_LOD mMaxBrakeLodLevel;          // offset 0x2C, size 0x4
    CARPART_LOD mMaxSpoilerLodLevel;        // offset 0x30, size 0x4
    CARPART_LOD mMaxRoofScoopLodLevel;      // offset 0x34, size 0x4
    CARPART_LOD mMinReflectionLodLevel;     // offset 0x38, size 0x4
    uint32 mCompositeSkinHash;              // offset 0x3C, size 0x4
    uint32 mCompositeWheelHash;             // offset 0x40, size 0x4
    uint32 mCompositeSpinnerHash;           // offset 0x44, size 0x4
    CarPart *mPartsTable[139];              // offset 0x48, size 0x22C
    int8 mPartsEnabled[139];                // offset 0x274, size 0x8B
    CarPart *PreviewPart;                   // offset 0x300, size 0x4
    CarLoaderHandle mMyCarLoaderHandle;     // offset 0x304, size 0x4
    CarRenderUsage mMyCarRenderUsage;       // offset 0x308, size 0x4
    uint8 mSpecialLODBehavior;              // offset 0x30C, size 0x1
};

typedef enum { CAR_LIGHT_STATE_OFF = 0, CAR_LIGHT_STATE_ON = 1, CAR_LIGHT_STATE_DAMAGED = 2, NUM_CAR_LIGHT_STATES = 3 } CarLightStateEnum;

typedef enum {
    CAR_LIGHT_HEADLIGHT_LEFT = 0,
    CAR_LIGHT_HEADLIGHT_RIGHT = 1,
    CAR_LIGHT_BRAKELIGHT_LEFT = 2,
    CAR_LIGHT_BRAKELIGHT_RIGHT = 3,
    CAR_LIGHT_BRAKELIGHT_CENTRE = 4,
    CAR_LIGHT_HEADLIGHT_GLASS_LEFT = 5,
    CAR_LIGHT_HEADLIGHT_GLASS_RIGHT = 6,
    CAR_LIGHT_BRAKELIGHT_GLASS_LEFT = 7,
    CAR_LIGHT_BRAKELIGHT_GLASS_RIGHT = 8,
    CAR_LIGHT_BRAKELIGHT_GLASS_CENTRE = 9,
    CAR_LIGHT_REVERSELIGHT = 10,
    NUM_CAR_LIGHT_TYPES = 11
} CarLightTypeEnum;

struct UsedCarTextureInfo {
    uint32 TexturesToLoadPerm[87];
    uint32 TexturesToLoadTemp[87];
    int NumTexturesToLoadPerm;
    int NumTexturesToLoadTemp;
    uint32 MappedSkinHash;
    uint32 MappedSkinBHash;
    uint32 MappedGlobalHash;
    uint32 MappedWheelHash;
    uint32 MappedSpinnerHash;
    uint32 MappedBadging;
    uint32 MappedSpoilerHash;
    uint32 MappedRoofScoopHash;
    uint32 MappedDashSkinHash;
    uint32 MappedLightHash[11];
    uint32 MappedTireHash;
    uint32 MappedRimHash;
    uint32 MappedRimBlurHash;
    uint32 MappedLicensePlateHash;
    uint32 ReplaceSkinHash;
    uint32 ReplaceSkinBHash;
    uint32 ReplaceGlobalHash;
    uint32 ReplaceWheelHash;
    uint32 ReplaceSpinnerHash;
    uint32 ReplaceSpoilerHash;
    uint32 ReplaceRoofScoopHash;
    uint32 ReplaceDashSkinHash;
    uint32 ReplaceHeadlightHash[3];
    uint32 ReplaceHeadlightGlassHash[3];
    uint32 ReplaceBrakelightHash[3];
    uint32 ReplaceBrakelightGlassHash[3];
    uint32 ReplaceReverselightHash[3];
    uint32 ShadowHash;
};

typedef enum {
    DRIVER_TYPE_NONE = 0,
    DRIVER_TYPE_PLAYER = 1,
    DRIVER_TYPE_AI_RACER = 2,
    DRIVER_TYPE_TRAFFIC_CAR = 3,
    DRIVER_TYPE_PARKED_CAR = 4,
    DRIVER_TYPE_COP = 5,
    DRIVER_TYPE_SUPER_COP = 6
} DriverTypeEnum;

typedef enum {
    AI_RACER_1 = 100,
    AI_RACER_2 = 101,
    AI_RACER_3 = 102,
    AI_RACER_4 = 103,
    AI_RACER_5 = 104,
    AI_RACER_6 = 105,
    AI_RACER_7 = 106,
    AI_RACER_8 = 107
} AIRacerNumbers;

class DriverInfo {
  public:
    DriverInfo() {};
    DriverInfo(int unique_driver_number, DriverTypeEnum driver_type, struct RideInfo *ride_info, const char *character_name, int skill_level,
               int personality_type, int starting_position);
    void DoSnapshot();
    const char *GetName() {}
    void SetRideInfo(struct RideInfo *new_ride_info) {}
    RideInfo *GetRideInfo() {}
    CarType GetCarType() {}
    bool IsPlayer() {}
    bool IsAI() {}
    bool IsTraffic() {}
    bool IsParkedCar() {}
    bool IsCop() {}
    bool IsSuperCop() {}
    bool IsOnlineDriver() {}
    int GetEliminatedRank() {}
    bool IsEliminated() {}
    bool StillRacing() {}
    void SetEliminatedRank(int nRank) {}
    void ClearEliminated() {}
    int8 GetSkillLevel();
    int8 GetPersonalityType() {}
    float GetReputationStars() {}
    void SetReputationStars(float s) {}

    int8 nEliminatedRank;
    int8 UniqueDriverNumber;
    int8 Pad;
    int8 StartingPosition;
    DriverTypeEnum DriverType;
    RideInfo CurrentRideInfo;
    int8 bOnlineDriver;
    int8 SkillLevel;
    int8 PersonalityType;
    float ReputationStars;
    CarType DesiredCarType;
    char DriverName[16];
    bool UseStartCoordinate;
    bVector3 StartCoordinate;
    bAngle StartAngle;
};

extern CarTypeInfo *CarTypeInfoArray;

inline CarTypeInfo *GetCarTypeInfo(CarType type) {
    return &CarTypeInfoArray[type];
}

CarTypeInfo *GetCarTypeInfoFromHash(uint32 car_type_hash);

#endif
