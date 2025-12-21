#ifndef WORLD_WEATHERMAN_H
#define WORLD_WEATHERMAN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

// maybe place somewhere else
struct GenericRegion : public bTNode<GenericRegion> {
    // total size: 0xA4
    char Name[32]; // offset 0x8, size 0x20
    char GroupName[32]; // offset 0x28, size 0x20
    int Type; // offset 0x48, size 0x4
    unsigned int FogColour; // offset 0x4C, size 0x4
    float FogStart; // offset 0x50, size 0x4
    float Parameter1; // offset 0x54, size 0x4
    float PositionX; // offset 0x58, size 0x4
    float PositionY; // offset 0x5C, size 0x4
    float PositionZ; // offset 0x60, size 0x4
    float Radius; // offset 0x64, size 0x4
    float Intensity; // offset 0x68, size 0x4
    float FarFalloffStart; // offset 0x6C, size 0x4
    float FogFalloff; // offset 0x70, size 0x4
    float FogFalloffX; // offset 0x74, size 0x4
    float FogFalloffY; // offset 0x78, size 0x4
    float ScreenPosX; // offset 0x7C, size 0x4
    float ScreenPosY; // offset 0x80, size 0x4
    unsigned int NameHash; // offset 0x84, size 0x4
    unsigned int GroupNameHash; // offset 0x88, size 0x4
    int Shape; // offset 0x8C, size 0x4
    int Blend; // offset 0x90, size 0x4
    float modifier; // offset 0x94, size 0x4
    unsigned int inFlags; // offset 0x98, size 0x4
    float effect; // offset 0x9C, size 0x4
    int dynamic; // offset 0xA0, size 0x4
};

enum RegionType {
    REGION_RAIN,
    REGION_FOG,
    REGION_SNOW,
    REGION_WIND,
    REGION_CLEAR,
    REGION_TUNNEL,
    REGION_HORIZON_COLOUR,
    REGION_FOG_VOLUME,
    REGION_BLOOM,
    NUM_REGION_TYPES,
    ALL_REGIONS,
    NULL_REGION = -1,
};

struct RegionQuery {
    // total size: 0x1C
    float FogFalloff; // offset 0x0, size 0x4
    float FogFalloffX; // offset 0x4, size 0x4
    float FogFalloffY; // offset 0x8, size 0x4
    float DistFogStart; // offset 0xC, size 0x4
    float DistFogPower; // offset 0x10, size 0x4
    unsigned int DistFogColour; // offset 0x14, size 0x4
    GenericRegion *region; // offset 0x18, size 0x4

    int CalculateRegionInfo(eView *view, RegionType regionKind, int InFE);
};

#endif
