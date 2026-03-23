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

struct ModelHeirarchy;
struct SceneryTreeNode;
struct tPrecullerInfo;

struct SceneryInfo {
    char DebugName[24];
    unsigned int NameHash[4];
    eModel *pModel[4];
    float Radius;
    unsigned int MeshChecksum;
    unsigned int mHeirarchyNameHash;
    ModelHeirarchy *mHeirarchy;
};

struct ScenerySectionHeader : public bTNode< ScenerySectionHeader > {
    int ChunksLoaded;
    int SectionNumber;
    int NumPolygonsInMemory;
    int NumPolygonsInWorld;
    SceneryInfo *pSceneryInfo;
    int NumSceneryInfo;
    SceneryInstance *pSceneryInstance;
    int NumSceneryInstances;
    SceneryTreeNode *SceneryTreeNodeTable;
    int NumSceneryTreeNodes;
    tPrecullerInfo *PrecullerInfoTable;
    int NumPrecullerInfos;
    int ViewsVisibleThisFrame;

    SceneryInstance *GetSceneryInstance(int scenery_instance_number) {
        return &pSceneryInstance[scenery_instance_number];
    }

    SceneryInfo *GetSceneryInfo(SceneryInstance *scenery_instance) {
        return &pSceneryInfo[scenery_instance->SceneryInfoNumber];
    }
};

struct SceneryOverrideInfo {
    short SectionNumber;
    short InstanceNumber;
    unsigned short ExcludeFlags;

    void SetExcludeFlags(unsigned short exclude_flag_mask, unsigned short exclude_flag_override) {
        ExcludeFlags = (ExcludeFlags & ~exclude_flag_mask) | exclude_flag_override;
    }

    void EnableRendering() {
        ExcludeFlags &= ~0x10;
        SetExcludeFlags(0, 0);
    }

    void DisableRendering() {
        SetExcludeFlags(0x10, 0x10);
    }

    void AssignOverrides();
    void AssignOverrides(ScenerySectionHeader *section_header);
};

SceneryOverrideInfo *GetSceneryOverrideInfo(int index);
ScenerySectionHeader *GetScenerySectionHeader(int section_number);

// total size: 0x8014
struct SceneryGroup : public bTNode<SceneryGroup> {
    // SceneryGroup(unsigned int name_hash) {}

    // int GetMemoryImageSize() {}

    // int GetNumObjects() {}

    // int GetOverrideInfoNumber(int index) {}

    // struct SceneryOverrideInfo *GetOverrideInfo(int index) {}

    // void EndianSwap() {}

    // void EnableRendering(bool flip_artwork) {}

    // void DisableRendering() {}

    uint32 NameHash;                               // offset 0x8, size 0x4
    int16 GroupNumber;                             // offset 0xC, size 0x2
    int16 NumObjects;                              // offset 0xE, size 0x2
    int8 BarrierFlag;                              // offset 0x10, size 0x1
    int8 DriveThroughBarrierFlag;                  // offset 0x11, size 0x1
    int16 Pad2;                                    // offset 0x12, size 0x2
    short unsigned int OverrideInfoNumbers[16384]; // offset 0x14, size 0x8000
};

extern RegionQuery RegionInfo;
extern SceneryDetailLevel ForceAllSceneryDetailLevels;
extern bTList<SceneryGroup> SceneryGroupList;

void InitVisibleZones();
void CloseVisibleZones();
void ServicePreculler();
void LoadPrecullerBooBooScripts();
void EnableSceneryGroup(unsigned int group_name_hash, bool flip_artwork);
SceneryGroup *FindSceneryGroup(unsigned int name_hash); // TODO remove "class"

#endif
