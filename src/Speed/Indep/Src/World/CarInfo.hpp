#ifndef WORLD_CARINFO_H
#define WORLD_CARINFO_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// this stuff may or may not go here.
enum CarUsageType {
    CAR_USAGE_TYPE_UNIVERSAL = 4,
    CAR_USAGE_TYPE_WHEELS = 3,
    CAR_USAGE_TYPE_TRAFFIC = 2,
    CAR_USAGE_TYPE_COP = 1,
    CAR_USAGE_TYPE_RACING = 0,
};
enum CARPART_LOD {
    CARPART_LOD_NUM = 5,
    CARPART_LOD_E = 4,
    CARPART_LOD_D = 3,
    CARPART_LOD_C = 2,
    CARPART_LOD_B = 1,
    CARPART_LOD_A = 0,
    CARPART_LOD_OFF = -1,
};
enum CarRenderUsage {
    CarRenderUsage_Invalid = 7,
    carRenderUsage_NISCar = 6,
    CarRenderUsage_AIHeli = 5,
    CarRenderUsage_AITraffic = 4,
    CarRenderUsage_AICop = 3,
    CarRenderUsage_AIRacer = 2,
    CarRenderUsage_RemotePlayer = 1,
    CarRenderUsage_Player = 0,
};

struct RideInfo {
    // total size: 0x310
    CarType Type; // offset 0x0, size 0x4
    char InstanceIndex; // offset 0x4, size 0x1
    char HasDash; // offset 0x5, size 0x1
    char CanBeVertexDamaged; // offset 0x6, size 0x1
    char SkinType; // offset 0x7, size 0x1
    CARPART_LOD mMinLodLevel; // offset 0x8, size 0x4
    CARPART_LOD mMaxLodLevel; // offset 0xC, size 0x4
    CARPART_LOD mMinFELodLevel; // offset 0x10, size 0x4
    CARPART_LOD mMaxFELodLevel; // offset 0x14, size 0x4
    CARPART_LOD mMaxLicenseLodLevel; // offset 0x18, size 0x4
    CARPART_LOD mMinTrafficDiffuseLodLevel; // offset 0x1C, size 0x4
    CARPART_LOD mMinShadowLodLevel; // offset 0x20, size 0x4
    CARPART_LOD mMaxShadowLodLevel; // offset 0x24, size 0x4
    CARPART_LOD mMaxTireLodLevel; // offset 0x28, size 0x4
    CARPART_LOD mMaxBrakeLodLevel; // offset 0x2C, size 0x4
    CARPART_LOD mMaxSpoilerLodLevel; // offset 0x30, size 0x4
    CARPART_LOD mMaxRoofScoopLodLevel; // offset 0x34, size 0x4
    CARPART_LOD mMinReflectionLodLevel; // offset 0x38, size 0x4
    unsigned int mCompositeSkinHash; // offset 0x3C, size 0x4
    unsigned int mCompositeWheelHash; // offset 0x40, size 0x4
    unsigned int mCompositeSpinnerHash; // offset 0x44, size 0x4
    struct CarPart * mPartsTable[139]; // offset 0x48, size 0x22C
    signed char mPartsEnabled[139]; // offset 0x274, size 0x8B
    struct CarPart * PreviewPart; // offset 0x300, size 0x4
    int mMyCarLoaderHandle; // offset 0x304, size 0x4
    CarRenderUsage mMyCarRenderUsage; // offset 0x308, size 0x4
    unsigned char mSpecialLODBehavior; // offset 0x30C, size 0x1
};

enum DriverTypeEnum {
    DRIVER_TYPE_SUPER_COP = 6,
    DRIVER_TYPE_COP = 5,
    DRIVER_TYPE_PARKED_CAR = 4,
    DRIVER_TYPE_TRAFFIC_CAR = 3,
    DRIVER_TYPE_AI_RACER = 2,
    DRIVER_TYPE_PLAYER = 1,
    DRIVER_TYPE_NONE = 0,
};

struct DriverInfo {
    // total size: 0x34C
    char nEliminatedRank; // offset 0x0, size 0x1
    char UniqueDriverNumber; // offset 0x1, size 0x1
    char Pad; // offset 0x2, size 0x1
    char StartingPosition; // offset 0x3, size 0x1
    DriverTypeEnum DriverType; // offset 0x4, size 0x4
    RideInfo CurrentRideInfo; // offset 0x8, size 0x310
    char bOnlineDriver; // offset 0x318, size 0x1
    char SkillLevel; // offset 0x319, size 0x1
    char PersonalityType; // offset 0x31A, size 0x1
    float ReputationStars; // offset 0x31C, size 0x4
    CarType DesiredCarType; // offset 0x320, size 0x4
    char DriverName[16]; // offset 0x324, size 0x10
    bool UseStartCoordinate; // offset 0x334, size 0x1
    bVector3 StartCoordinate; // offset 0x338, size 0x10
    unsigned short StartAngle; // offset 0x348, size 0x2
};

#endif
