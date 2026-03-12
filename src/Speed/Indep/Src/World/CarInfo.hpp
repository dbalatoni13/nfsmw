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

// total size: 0x310
class RideInfo {
  public:
    void Init(CarType type, CarRenderUsage usage, int has_dash, int can_be_vertex_damaged);

    RideInfo() {
        Init(CARTYPE_NONE, CarRenderUsage_Player, 0, 0);
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
};

#endif
