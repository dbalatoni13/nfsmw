#ifndef WORLD_SCENERY_H
#define WORLD_SCENERY_H

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
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

// total size: 0x18
struct SceneryBoundingBox {
#ifdef EA_BUILD_A124
    short int BBoxMin[3]; // offset 0x0, size 0x6
    short int BBoxMax[3]; // offset 0x6, size 0x6
#else
    float BBoxMin[3]; // offset 0x0, size 0xC
    float BBoxMax[3]; // offset 0xC, size 0xC
#endif

    void GetBBox(bVector3 *bbox_min, bVector3 *bbox_max) {
        bFill(bbox_min, BBoxMin[0], BBoxMin[1], BBoxMin[2]);
        bFill(bbox_max, BBoxMax[0], BBoxMax[1], BBoxMax[2]);
    }
};

// total size: 0x40
class SceneryInstance : public SceneryBoundingBox {
  public:
    void GetRotation(bMatrix4 *matrix) {
        const float rotation_conversion = 1.0f / 8192.0f;
        float x = static_cast<float>(Rotation[0]) * rotation_conversion;
        float y = static_cast<float>(Rotation[1]) * rotation_conversion;
        float z = static_cast<float>(Rotation[2]) * rotation_conversion;
        matrix->v0.x = x;
        matrix->v0.y = y;
        matrix->v0.z = z;
        matrix->v0.w = 0.0f;

        x = static_cast<float>(Rotation[3]) * rotation_conversion;
        y = static_cast<float>(Rotation[4]) * rotation_conversion;
        z = static_cast<float>(Rotation[5]) * rotation_conversion;
        matrix->v1.x = x;
        matrix->v1.y = y;
        matrix->v1.z = z;
        matrix->v1.w = 0.0f;

        x = static_cast<float>(Rotation[6]) * rotation_conversion;
        y = static_cast<float>(Rotation[7]) * rotation_conversion;
        z = static_cast<float>(Rotation[8]) * rotation_conversion;
        matrix->v2.x = x;
        matrix->v2.y = y;
        matrix->v2.z = z;
        matrix->v2.w = 0.0f;
    }

    void GetPosition(bVector4 *position) {
        *position = *reinterpret_cast<bVector4 *>(Position);
        position->w = 1.0f;
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

    uint32 ExcludeFlags; // offset 0x18, size 0x4
#ifdef EA_BUILD_A124
    int32 PrecullerInfoIndex;
    int16 LightingContextNumber;
    int16 Pad1;
    int Pad[2];
#else
    int16 PrecullerInfoIndex;    // offset 0x1C, size 0x2
    int16 LightingContextNumber; // offset 0x1E, size 0x2
#endif
    float Position[3];       // offset 0x20, size 0xC
    short int Rotation[9];   // offset 0x2C, size 0x12
    int16 SceneryInfoNumber; // offset 0x3E, size 0x2
};

// total size: 0xC
struct SceneryDrawInfo {
    eModel *pModel;               // offset 0x0, size 0x4
    bMatrix4 *pMatrix;            // offset 0x4, size 0x4
    SceneryInstance *SceneryInst; // offset 0x8, size 0x4
};

// total size: 0xBC
struct SceneryCullInfo {
    bMatrix4 FacadeMatrix;             // offset 0x0, size 0x40
    bMatrix4 ClipMatrix;               // offset 0x40, size 0x40
    struct eView *pView;               // offset 0x80, size 0x4
    int ExcludeFlags;                  // offset 0x84, size 0x4
    SceneryDrawInfo *pFirstDrawInfo;   // offset 0x88, size 0x4
    SceneryDrawInfo *pCurrentDrawInfo; // offset 0x8C, size 0x4
    SceneryDrawInfo *pTopDrawInfo;     // offset 0x90, size 0x4
    bVector3 Position;                 // offset 0x94, size 0x10
    bVector3 Direction;                // offset 0xA4, size 0x10
    float H;                           // offset 0xB4, size 0x4
    int PrecullerSectionNumber;        // offset 0xB8, size 0x4
};

// total size: 0x8
struct ModelHeirarchy {
    enum Flags {
        F_INTERNAL = 1,
    };
    // total size: 0x10
    struct Node {
        UCrc32 mNodeName;   // offset 0x0, size 0x4
        uint32 mModelHash;  // offset 0x4, size 0x4
        eModel *mModel;     // offset 0x8, size 0x4
        uint8 mFlags;       // offset 0xC, size 0x1
        uint8 mParent;      // offset 0xD, size 0x1
        uint8 mNumChildren; // offset 0xE, size 0x1
        uint8 mChildIndex;  // offset 0xF, size 0x1
    };

    const Node *GetNodes() const {
        return reinterpret_cast<const Node *>(&this[1]);
    }

    Node *GetNodes() {
        return reinterpret_cast<Node *>(&this[1]);
    }

    // unsigned int GetSize() const {}

    uint32 mNameHash; // offset 0x0, size 0x4
    uint8 mNumNodes;  // offset 0x4, size 0x1
    uint8 mFlags;     // offset 0x5, size 0x1
    uint16 pad;       // offset 0x6, size 0x2
};

// total size: 0x48
struct SceneryInfo {
    char DebugName[24];         // offset 0x0, size 0x18
    uint32 NameHash[4];         // offset 0x18, size 0x10
    eModel *pModel[4];          // offset 0x28, size 0x10
    float Radius;               // offset 0x38, size 0x4
    uint32 MeshChecksum;        // offset 0x3C, size 0x4
    uint32 mHeirarchyNameHash;  // offset 0x40, size 0x4
    ModelHeirarchy *mHeirarchy; // offset 0x44, size 0x4
};

class tPrecullerInfo {
  public:
    int IsNotVisible(int preculler_section_number) {
        return (VisibilityBits[preculler_section_number >> 3] & (1 << (preculler_section_number & 7)));
    }

    int IsVisible(int preculler_section_number) {
        return !IsNotVisible(preculler_section_number);
    }

    uint8 *GetBits() {
        return VisibilityBits;
    }

    void EndianSwap() {}

  private:
    uint8 VisibilityBits[0x80];
};

inline int GetPrecullerSectionNumber(float x, float y) {
    int nx = (static_cast<unsigned int>(static_cast<int>(x)) >> 5) & 0x1F;
    int ny = static_cast<int>(y) & 0x3E0;
    return ny + nx;
}

// total size: 0x24
class SceneryTreeNode : public SceneryBoundingBox {
  public:
    int16 NumChildren;       // offset 0x18, size 0x2
    short int ChildCodes[5]; // offset 0x1A, size 0xA
};

// TODO
class ScenerySectionHeader : public bTNode<ScenerySectionHeader> {
  public:
    void DrawAScenery(int scenery_instance_number, SceneryCullInfo *scenery_cull_info, int visibility_state);

    int IsVisible(SceneryCullInfo *scenery_cull_info);

    void CullBruteForce(SceneryCullInfo *scenery_cull_info);

    void TreeCull(SceneryCullInfo *scenery_cull_info);

    void CullNodeRecursive(SceneryTreeNode *node, SceneryCullInfo *scenery_cull_info, unsigned int visibility_state);

    SceneryInstance *GetSceneryInstance(int scenery_instance_number) {
        return &pSceneryInstance[scenery_instance_number];
    }

    SceneryInfo *GetSceneryInfo(int scenery_info_number) {
        return &pSceneryInfo[scenery_info_number];
    }

    int GetSectionNumber() {
        return this->SectionNumber;
    }

    tPrecullerInfo *GetPrecullerInfo(int preculler_info_index) {
        return &PrecullerInfoTable[preculler_info_index];
    }

    static float mLodLevelDistance[3]; // size: 0xC, address: 0xFFFFFFFF

    int ChunksLoaded;                      // offset 0x8, size 0x4
    int SectionNumber;                     // offset 0xC, size 0x4
    int NumPolygonsInMemory;               // offset 0x10, size 0x4
    int NumPolygonsInWorld;                // offset 0x14, size 0x4
    SceneryInfo *pSceneryInfo;             // offset 0x18, size 0x4
    int NumSceneryInfo;                    // offset 0x1C, size 0x4
    SceneryInstance *pSceneryInstance;     // offset 0x20, size 0x4
    int NumSceneryInstances;               // offset 0x24, size 0x4
    SceneryTreeNode *SceneryTreeNodeTable; // offset 0x28, size 0x4
    int NumSceneryTreeNodes;               // offset 0x2C, size 0x4
    tPrecullerInfo *PrecullerInfoTable;    // offset 0x30, size 0x4
    int NumPrecullerInfos;                 // offset 0x34, size 0x4
    int ViewsVisibleThisFrame;             // offset 0x38, size 0x4
};

// total size: 0x6
struct SceneryOverrideInfo {
    void EndianSwap() {
        bPlatEndianSwap(&SectionNumber);
        bPlatEndianSwap(&InstanceNumber);
        bPlatEndianSwap(&ExcludeFlags);
    }

    static int GetHashIndex(short section_number) {
        return section_number;
    }

    int GetHashIndex() {
        return GetHashIndex(SectionNumber);
    }

    void SetExcludeFlags(unsigned short exclude_flag_mask, unsigned short exclude_flag_override) {
        ExcludeFlags = (ExcludeFlags & exclude_flag_mask) | exclude_flag_override;
        AssignOverrides();
    }

    void AssignOverrides();
    void AssignOverrides(ScenerySectionHeader *section_header);

    int16 SectionNumber;  // offset 0x0, size 0x2
    int16 InstanceNumber; // offset 0x2, size 0x2
    uint16 ExcludeFlags;  // offset 0x4, size 0x2
};

extern signed char SceneryGroupEnabledTable[0x1000];

SceneryOverrideInfo *GetSceneryOverrideInfo(int override_info_number);

// total size: 0x4
struct SceneryOverrideInfoHookup {
    void EndianSwap() {
        bPlatEndianSwap(&OverrideInfoNumber);
        bPlatEndianSwap(&InstanceNumber);
    }

    uint16 OverrideInfoNumber; // offset 0x0, size 0x2
    uint16 InstanceNumber;     // offset 0x2, size 0x2
};

// total size: 0x8014
struct SceneryGroup : public bTNode<SceneryGroup> {
    // SceneryGroup(unsigned int name_hash) {}

    int GetMemoryImageSize() {
        // TODO magic
        int size = (NumObjects * sizeof(*OverrideInfoNumbers) + 0x17U) & ~3;
        return size;
    }

    int GetNumObjects() {
        return NumObjects;
    }

    // int GetOverrideInfoNumber(int index) {}

    SceneryOverrideInfo *GetOverrideInfo(int index) {
        return GetSceneryOverrideInfo(OverrideInfoNumbers[index]);
    }

    void EndianSwap() {
        bPlatEndianSwap(&NameHash);
        bPlatEndianSwap(&GroupNumber);
        bPlatEndianSwap(&NumObjects);
        for (int n = 0; n < NumObjects; n++) {
            bPlatEndianSwap(&OverrideInfoNumbers[n]);
        }
    }

    void EnableRendering(bool flip_artwork) {
        int exclude_flag_mask = 0xFBEF;
        int exclude_flag_override = 0;
        if (flip_artwork) {
            exclude_flag_override = 0x400;
        }

        for (int n = 0; n < GetNumObjects(); n++) {
            GetOverrideInfo(n)->SetExcludeFlags(exclude_flag_mask, exclude_flag_override);
        }

        SceneryGroupEnabledTable[this->GroupNumber] = 1;
        if (flip_artwork) {
            SceneryGroupEnabledTable[this->GroupNumber] |= 2;
        }
        if (this->DriveThroughBarrierFlag) {
            SceneryGroupEnabledTable[this->GroupNumber] |= 4;
        }
    }

    void DisableRendering() {
        for (int n = 0; n < GetNumObjects(); n++) {
            SceneryOverrideInfo *override_info = GetOverrideInfo(n);
            override_info->SetExcludeFlags(0xFFFF, 0x10);
        }
    }

    uint32 NameHash;                               // offset 0x8, size 0x4
    int16 GroupNumber;                             // offset 0xC, size 0x2
    int16 NumObjects;                              // offset 0xE, size 0x2
    int8 BarrierFlag;                              // offset 0x10, size 0x1
    int8 DriveThroughBarrierFlag;                  // offset 0x11, size 0x1
    int16 Pad2;                                    // offset 0x12, size 0x2
    short unsigned int OverrideInfoNumbers[16384]; // offset 0x14, size 0x8000
};

// total size: 0x8E0
class GrandSceneryCullInfo {
  public:
    int WhatSectionsShouldWeDraw(short *sections_to_draw, int max_sections_to_draw, SceneryCullInfo *scenery_cull_info);
    void CullView(SceneryCullInfo *scenery_cull_info);
    void DoCulling();
    void StuffScenery(eView *view, int stuff_flags);

    SceneryCullInfo SceneryCullInfos[12]; // offset 0x0, size 0x8D0
    int NumCullInfos;                     // offset 0x8D0, size 0x4
    SceneryDrawInfo *pFirstDrawInfo;      // offset 0x8D4, size 0x4
    SceneryDrawInfo *pCurrentDrawInfo;    // offset 0x8D8, size 0x4
    SceneryDrawInfo *pTopDrawInfo;        // offset 0x8DC, size 0x4

    static SceneryDrawInfo SceneryDrawInfoTable[3500];
};

// TODO making this extern messes with the order
// extern RegionQuery RegionInfo;
extern SceneryDetailLevel ForceAllSceneryDetailLevels;
extern bTList<SceneryGroup> SceneryGroupList;
extern int DisablePrecullerCounter;

void InitVisibleZones();
void CloseVisibleZones();
void ServicePreculler();
void LoadPrecullerBooBooScripts();
void EnableSceneryGroup(unsigned int group_name_hash, bool flip_artwork);
SceneryGroup *FindSceneryGroup(unsigned int name_hash);

inline int IsSceneryGroupEnabled(int group_number) {
    return SceneryGroupEnabledTable[group_number];
}

inline int InlinedViewGetPixelSize(SceneryCullInfo *scenery_cull_info, const bVector3 *position, float radius) {
    bVector3 dir = *position - scenery_cull_info->Position;
    float distance_ahead = bDot(&dir, &scenery_cull_info->Direction);
    if (distance_ahead < -radius) {
        return 0;
    }

    float distance_away = bLength(&dir) - radius;
    float pixel_size = scenery_cull_info->H;
    if (distance_away > radius) {
        pixel_size = (radius * pixel_size) / distance_away;
    }
    return static_cast<int>(pixel_size);
}

inline void EnablePreculler() {
    DisablePrecullerCounter--;
}

inline void DisablePreculler() {
    DisablePrecullerCounter++;
}

#endif
