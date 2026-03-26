#ifndef WORLD_SCENERY_H
#define WORLD_SCENERY_H

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
    eModel *pModel;               // offset 0x0, size 0x4
    bMatrix4 *pMatrix;            // offset 0x4, size 0x4
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

// total size: 0x8
struct ModelHeirarchy {
    enum Flags {
        F_INTERNAL = 1,
    };
    // total size: 0x10
    struct Node {
        UCrc32 mNodeName;           // offset 0x0, size 0x4
        unsigned int mModelHash;    // offset 0x4, size 0x4
        eModel *mModel;             // offset 0x8, size 0x4
        unsigned char mFlags;       // offset 0xC, size 0x1
        unsigned char mParent;      // offset 0xD, size 0x1
        unsigned char mNumChildren; // offset 0xE, size 0x1
        unsigned char mChildIndex;  // offset 0xF, size 0x1
    };

    const Node *GetNodes() const {}

    Node *GetNodes() {}

    unsigned int GetSize() const {}

    unsigned int mNameHash;  // offset 0x0, size 0x4
    unsigned char mNumNodes; // offset 0x4, size 0x1
    unsigned char mFlags;    // offset 0x5, size 0x1
    unsigned short pad;      // offset 0x6, size 0x2
};

// total size: 0x48
struct SceneryInfo {
    // Members
    char DebugName[24];              // offset 0x0, size 0x18
    unsigned int NameHash[4];        // offset 0x18, size 0x10
    eModel *pModel[4];               // offset 0x28, size 0x10
    float Radius;                    // offset 0x38, size 0x4
    unsigned int MeshChecksum;       // offset 0x3C, size 0x4
    unsigned int mHeirarchyNameHash; // offset 0x40, size 0x4
    ModelHeirarchy *mHeirarchy;      // offset 0x44, size 0x4
};

class tPrecullerInfo {
  public:
    bool IsVisible(int preculler_section_number) {
        return (VisibilityBits[preculler_section_number >> 3] & (1 << (preculler_section_number & 7))) != 0;
    }

    bool IsNotVisible(int preculler_section_number) {
        return !IsVisible(preculler_section_number);
    }

    unsigned char *GetBits() {
        return VisibilityBits;
    }

  private:
    unsigned char VisibilityBits[0x80];
};

// total size: 0x24
struct SceneryTreeNode : public SceneryBoundingBox {
    short NumChildren;   // offset 0x18, size 0x2
    short ChildCodes[5]; // offset 0x1A, size 0xA
};

struct ScenerySectionHeader : public bTNode<ScenerySectionHeader> {
    void DrawAScenery(int scenery_instance_number, SceneryCullInfo *scenery_cull_info, int visibility_state);

    int IsVisible(SceneryCullInfo *scenery_cull_info);

    void CullBruteForce(SceneryCullInfo *scenery_cull_info);

    void TreeCull(SceneryCullInfo *scenery_cull_info);

    void CullNodeRecursive(SceneryTreeNode *node, SceneryCullInfo *scenery_cull_info, unsigned int visibility_state);

    SceneryInstance *GetSceneryInstance(int scenery_instance_number) {
        return &pSceneryInstance[scenery_instance_number];
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

struct SceneryOverrideInfo {
    void EndianSwap() {
        bPlatEndianSwap(&SectionNumber);
        bPlatEndianSwap(&InstanceNumber);
        bPlatEndianSwap(&ExcludeFlags);
    }

    void AssignOverrides();
    void AssignOverrides(ScenerySectionHeader *section_header);

    short SectionNumber;         // offset 0x0, size 0x2
    short InstanceNumber;        // offset 0x2, size 0x2
    unsigned short ExcludeFlags; // offset 0x4, size 0x2
};

extern SceneryOverrideInfo *SceneryOverrideInfoTable;

// total size: 0x8014
struct SceneryGroup : public bTNode<SceneryGroup> {
    // SceneryGroup(unsigned int name_hash) {}

    // int GetMemoryImageSize() {}

    // int GetNumObjects() {}

    // int GetOverrideInfoNumber(int index) {}

    SceneryOverrideInfo *GetOverrideInfo(int index) {
        return &SceneryOverrideInfoTable[OverrideInfoNumbers[index]];
    }

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
SceneryGroup *FindSceneryGroup(unsigned int name_hash);

#endif
