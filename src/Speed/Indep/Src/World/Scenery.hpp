#ifndef WORLD_SCENERY_H
#define WORLD_SCENERY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/World/WeatherMan.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum SceneryDetailLevel {
    SCENERY_DETAIL_NONE = -1,
    SCENERY_DETAIL_HIGH = 0,
    SCENERY_DETAIL_MEDIUM = 1,
    SCENERY_DETAIL_LOW = 2,
    SCENERY_DETAIL_REFLECTION = 3,
    NUM_SCENERY_DETAIL_LEVELS = 4,
};

struct SceneryBoundingBox {
    // total size: 0x18
    float BBoxMin[3]; // offset 0x0, size 0xC
    float BBoxMax[3]; // offset 0xC, size 0xC

    void GetBBox(bVector3 *bbox_min, bVector3 *bbox_max) {
        bFill(bbox_min, BBoxMin[0], BBoxMin[1], BBoxMin[2]);
        bFill(bbox_max, BBoxMax[0], BBoxMax[1], BBoxMax[2]);
    }
};

struct SceneryInstance : public SceneryBoundingBox {
    // total size: 0x40
    unsigned int ExcludeFlags;   // offset 0x18, size 0x4
    short PrecullerInfoIndex;    // offset 0x1C, size 0x2
    short LightingContextNumber; // offset 0x1E, size 0x2
    float Position[3];           // offset 0x20, size 0xC
    short Rotation[9];           // offset 0x2C, size 0x12
    short SceneryInfoNumber;     // offset 0x3E, size 0x2

    void GetRotation(bMatrix4 *matrix) {
        const float rotation_conversion = 0.00012207031f;
        float x = static_cast<float>(Rotation[0]) * rotation_conversion;
        float y = static_cast<float>(Rotation[1]) * rotation_conversion;
        float z = static_cast<float>(Rotation[2]) * rotation_conversion;
        bFill(&matrix->v0, x, y, z, 0.0f);
        x = static_cast<float>(Rotation[3]) * rotation_conversion;
        y = static_cast<float>(Rotation[4]) * rotation_conversion;
        z = static_cast<float>(Rotation[5]) * rotation_conversion;
        bFill(&matrix->v1, x, y, z, 0.0f);
        x = static_cast<float>(Rotation[6]) * rotation_conversion;
        y = static_cast<float>(Rotation[7]) * rotation_conversion;
        z = static_cast<float>(Rotation[8]) * rotation_conversion;
        bFill(&matrix->v2, x, y, z, 0.0f);
        bFill(&matrix->v3, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    void GetPosition(bVector4 *position) {
        bFill(position, Position[0], Position[1], Position[2], 1.0f);
    }

    bVector3 *GetPosition() {
        return reinterpret_cast<bVector3 *>(Position);
    }

    void SetMatrix(const bMatrix4 *matrix) {
        const float rotation_conversion = 8192.0f;
        Rotation[0] = static_cast<short>(matrix->v0.x * rotation_conversion);
        Rotation[1] = static_cast<short>(matrix->v0.y * rotation_conversion);
        Rotation[2] = static_cast<short>(matrix->v0.z * rotation_conversion);
        Rotation[3] = static_cast<short>(matrix->v1.x * rotation_conversion);
        Rotation[4] = static_cast<short>(matrix->v1.y * rotation_conversion);
        Rotation[5] = static_cast<short>(matrix->v1.z * rotation_conversion);
        Rotation[6] = static_cast<short>(matrix->v2.x * rotation_conversion);
        Rotation[7] = static_cast<short>(matrix->v2.y * rotation_conversion);
        Rotation[8] = static_cast<short>(matrix->v2.z * rotation_conversion);
        Position[0] = matrix->v3.x;
        Position[1] = matrix->v3.y;
        Position[2] = matrix->v3.z;
    }
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
extern SceneryDetailLevel ForceAllSceneryDetailLevels;

void InitVisibleZones();
void CloseVisibleZones();
void ServicePreculler();

#endif
