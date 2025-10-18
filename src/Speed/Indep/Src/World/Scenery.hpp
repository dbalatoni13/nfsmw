#ifndef WORLD_SCENERY_H
#define WORLD_SCENERY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/World/WeatherMan.hpp"

struct SceneryBoundingBox {
    // total size: 0x18
    float BBoxMin[3]; // offset 0x0, size 0xC
    float BBoxMax[3]; // offset 0xC, size 0xC
};

struct SceneryInstance : public SceneryBoundingBox {
    // total size: 0x40
    unsigned int ExcludeFlags;   // offset 0x18, size 0x4
    short PrecullerInfoIndex;    // offset 0x1C, size 0x2
    short LightingContextNumber; // offset 0x1E, size 0x2
    float Position[3];           // offset 0x20, size 0xC
    short Rotation[9];           // offset 0x2C, size 0x12
    short SceneryInfoNumber;     // offset 0x3E, size 0x2
};

struct SceneryDrawInfo {
    // total size: 0xC
    eModel *pModel;    // offset 0x0, size 0x4
    bMatrix4 *pMatrix; // offset 0x4, size 0x4
    char unk08[4];
    SceneryInstance *SceneryInst; // offset 0x8, size 0x4
};

struct SceneryCullInfo {
    // total size: 0xBC
    struct bMatrix4 FacadeMatrix;             // offset 0x0, size 0x40
    struct bMatrix4 ClipMatrix;               // offset 0x40, size 0x40
    struct eView *pView;                      // offset 0x80, size 0x4
    int ExcludeFlags;                         // offset 0x84, size 0x4
    struct SceneryDrawInfo *pFirstDrawInfo;   // offset 0x88, size 0x4
    struct SceneryDrawInfo *pCurrentDrawInfo; // offset 0x8C, size 0x4
    struct SceneryDrawInfo *pTopDrawInfo;     // offset 0x90, size 0x4
    struct bVector3 Position;                 // offset 0x94, size 0x10
    struct bVector3 Direction;                // offset 0xA4, size 0x10
    float H;                                  // offset 0xB4, size 0x4
    int PrecullerSectionNumber;               // offset 0xB8, size 0x4
};

extern RegionQuery RegionInfo;

void InitVisibleZones();
void CloseVisibleZones();

#endif
