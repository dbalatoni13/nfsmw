#ifndef WORLD_CARINFO_H
#define WORLD_CARINFO_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// TODO this stuff may or may not go here.
enum CarUsageType {
    CAR_USAGE_TYPE_RACING,
    CAR_USAGE_TYPE_COP,
    CAR_USAGE_TYPE_TRAFFIC,
    CAR_USAGE_TYPE_WHEELS,
    CAR_USAGE_TYPE_UNIVERSAL,
};
enum CARPART_LOD {
    CARPART_LOD_OFF,
    CARPART_LOD_A,
    CARPART_LOD_B,
    CARPART_LOD_C,
    CARPART_LOD_D,
    CARPART_LOD_E,
    CARPART_LOD_NUM,
};
enum CAR_SLOT_ID {
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
};
enum CAR_PART_ID {
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
    CARPARTID_NUM = 87,
};
enum CarRenderUsage {
    CarRenderUsage_Player,
    CarRenderUsage_RemotePlayer,
    CarRenderUsage_AIRacer,
    CarRenderUsage_AICop,
    CarRenderUsage_AITraffic,
    CarRenderUsage_AIHeli,
    carRenderUsage_NISCar,
    CarRenderUsage_Invalid,
};

struct CarPartAttribute {
    unsigned int NameHash;
    union {
        float fParam;
        int iParam;
        unsigned int uParam;
    } Params;

    unsigned int GetUParam() {
        return this->Params.uParam;
    }

    void EndianSwap();
};

struct CarPart {
    CarPartAttribute *GetAttribute(unsigned int namehash, CarPartAttribute *prev_attribute);
    CarPartAttribute *GetFirstAppliedAttribute(unsigned int namehash);
    CarPartAttribute *GetNextAppliedAttribute(unsigned int namehash, CarPartAttribute *prev_attribute);
    char *GetName();
    unsigned int GetCarTypeNameHash();
    unsigned int GetModelNameHash(int model, int lod);
    unsigned int GetAppliedAttributeUParam(unsigned int namehash, unsigned int default_value);
    int GetAppliedAttributeIParam(unsigned int namehash, int default_value);
    const char *GetAppliedAttributeString(unsigned int namehash, const char *default_string);
    int HasAppliedAttribute(unsigned int namehash);
    char GetGroupNumber();
    unsigned int GetPartNameHash() {
        return *reinterpret_cast<unsigned short *>(this) |
               (static_cast<unsigned int>(*(reinterpret_cast<unsigned short *>(this) + 1)) << 16);
    }

    unsigned int GetTextureNameHash() {
        return GetAppliedAttributeUParam(0x10C98090, 0);
    }

    unsigned int GetBrandNameHash() {
        return GetAppliedAttributeUParam(0xEBB03E66, 0);
    }
};

// total size: 0x310
struct FECarRecord;
class RideInfo {
  public:
    void Init(CarType type, CarRenderUsage usage, int has_dash, int can_be_vertex_damaged);
    struct CarPart *SetPart(int car_slot_id, struct CarPart *car_part, bool update_enabled);
    void SetStockParts();
    void SetRandomPart(CAR_SLOT_ID slot, int upgrade_level);
    void SetRandomPaint();
    void SetRandomParts();
    void SetUpgradePart(CAR_SLOT_ID car_slot_id, int upg_level);
    void UpdatePartsEnabled();
    struct CarPart *GetPart(int car_slot_id) const;
    int IsPartEnabled(int car_part_id);
    void DumpForPreset(FECarRecord *car);
    void FillWithPreset(unsigned int preset_name_hash);
    int GetSpecialLODRangeForCarSlot(int slot_id, CARPART_LOD *special_minimum, CARPART_LOD *special_maximum, bool in_front_end);
    unsigned int GetSkinNameHash();
    void SetCompositeNameHash(int skin_number);
    unsigned int GetCompositeSkinNameHash();
    void SetCompositeSkinNameHash(unsigned int namehash);
    unsigned int GetCompositeWheelNameHash();
    void SetCompositeWheelNameHash(unsigned int namehash);
    unsigned int GetCompositeSpinnerNameHash();
    void SetCompositeSpinnerNameHash(unsigned int namehash);
    int IsUsingCompositeSkin();
    struct CarPart *GetPreviewPart() {
        return this->PreviewPart;
    }

    RideInfo() {
        Init(CARTYPE_NONE, CarRenderUsage_Player, 0, 0);
    }

    void SetCarLoaderHandle(int car_loader_handle) {
        this->mMyCarLoaderHandle = car_loader_handle;
    }

    int GetCarLoaderHandle() {
        return this->mMyCarLoaderHandle;
    }

    CarRenderUsage GetCarRenderUsage() {
        return this->mMyCarRenderUsage;
    }

    CARPART_LOD GetMinLodLevel() const {
        return this->mMinLodLevel;
    }

    CARPART_LOD GetMaxLodLevel() const {
        return this->mMaxLodLevel;
    }

    CARPART_LOD GetMinFELodLevel() const {
        return this->mMinFELodLevel;
    }

    CARPART_LOD GetMaxFELodLevel() const {
        return this->mMaxFELodLevel;
    }

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
    unsigned int mCompositeSkinHash;        // offset 0x3C, size 0x4
    unsigned int mCompositeWheelHash;       // offset 0x40, size 0x4
    unsigned int mCompositeSpinnerHash;     // offset 0x44, size 0x4
    struct CarPart *mPartsTable[139];       // offset 0x48, size 0x22C
    signed char mPartsEnabled[139];         // offset 0x274, size 0x8B
    struct CarPart *PreviewPart;            // offset 0x300, size 0x4
    int mMyCarLoaderHandle;                 // offset 0x304, size 0x4
    CarRenderUsage mMyCarRenderUsage;       // offset 0x308, size 0x4
    unsigned char mSpecialLODBehavior;      // offset 0x30C, size 0x1
};

enum DriverTypeEnum {
    DRIVER_TYPE_NONE,
    DRIVER_TYPE_PLAYER,
    DRIVER_TYPE_AI_RACER,
    DRIVER_TYPE_TRAFFIC_CAR,
    DRIVER_TYPE_PARKED_CAR,
    DRIVER_TYPE_COP,
    DRIVER_TYPE_SUPER_COP,
};

// total size: 0x34C
struct DriverInfo {
    char nEliminatedRank;      // offset 0x0, size 0x1
    char UniqueDriverNumber;   // offset 0x1, size 0x1
    char Pad;                  // offset 0x2, size 0x1
    char StartingPosition;     // offset 0x3, size 0x1
    DriverTypeEnum DriverType; // offset 0x4, size 0x4
    RideInfo CurrentRideInfo;  // offset 0x8, size 0x310
    char bOnlineDriver;        // offset 0x318, size 0x1
    char SkillLevel;           // offset 0x319, size 0x1
    char PersonalityType;      // offset 0x31A, size 0x1
    float ReputationStars;     // offset 0x31C, size 0x4
    CarType DesiredCarType;    // offset 0x320, size 0x4
    char DriverName[16];       // offset 0x324, size 0x10
    bool UseStartCoordinate;   // offset 0x334, size 0x1
    bVector3 StartCoordinate;  // offset 0x338, size 0x10
    unsigned short StartAngle; // offset 0x348, size 0x2
};

// total size: 0xD0
struct CarTypeInfo {
    char CarTypeName[16];                       // offset 0x0, size 0x10
    char BaseModelName[16];                     // offset 0x10, size 0x10
    char GeometryFilename[32];                  // offset 0x20, size 0x20
    char ManufacturerName[16];                  // offset 0x40, size 0x10
    unsigned int CarTypeNameHash;               // offset 0x50, size 0x4
    float HeadlightFOV;                         // offset 0x54, size 0x4
    char padHighPerformance;                    // offset 0x58, size 0x1
    char NumAvailableSkinNumbers;               // offset 0x59, size 0x1
    char WhatGame;                              // offset 0x5A, size 0x1
    char ConvertableFlag;                       // offset 0x5B, size 0x1
    char WheelOuterRadius;                      // offset 0x5C, size 0x1
    char WheelInnerRadiusMin;                   // offset 0x5D, size 0x1
    char WheelInnerRadiusMax;                   // offset 0x5E, size 0x1
    char pad0;                                  // offset 0x5F, size 0x1
    bVector3 HeadlightPosition;                 // offset 0x60, size 0x10
    bVector3 DriverRenderingOffset;             // offset 0x70, size 0x10
    bVector3 InCarSteeringWheelRenderingOffset; // offset 0x80, size 0x10
    CarType Type;                               // offset 0x90, size 0x4
    CarUsageType UsageType;                     // offset 0x94, size 0x4
    unsigned int CarMemTypeHash;                // offset 0x98, size 0x4
    signed char MaxInstances[5];                // offset 0x9C, size 0x5
    signed char WantToKeepLoaded[5];            // offset 0xA1, size 0x5
    signed char pad4[2];                        // offset 0xA6, size 0x2
    float MinTimeBetweenUses[5];                // offset 0xA8, size 0x14
    char AvailableSkinNumbers[10];              // offset 0xBC, size 0xA
    char DefaultSkinNumber;                     // offset 0xC6, size 0x1
    char Skinnable;                             // offset 0xC7, size 0x1
    int Padding;                                // offset 0xC8, size 0x4
    int DefaultBasePaint;                       // offset 0xCC, size 0x4

    char *GetBaseModelName();
    char *GetName() {
        return this->CarTypeName;
    }

    char *GetCarTypeName() {
        return this->CarTypeName;
    }

    CarUsageType GetCarUsageType() {
        return this->UsageType;
    }
};

#endif
