#include "Scenery.hpp"

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "VisibleSection.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/Src/Ecstasy/eSolid.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/bWare/Inc/SpeedScript.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct ScenerySectionHeader : public bNode {
    void DrawAScenery(int scenery_instance_number, SceneryCullInfo *scenery_cull_info, int visibility_state);
    void TreeCull(SceneryCullInfo *scenery_cull_info);

    ScenerySectionHeader *GetNext() {
        return reinterpret_cast<ScenerySectionHeader *>(bNode::GetNext());
    }

    int GetSectionNumber() {
        return reinterpret_cast<int *>(this)[3];
    }
};

struct SceneryOverrideInfo {
    short SectionNumber;
    short OverrideSectionNumber;
    short OverrideIndex;

    void AssignOverrides();
    void AssignOverrides(ScenerySectionHeader *section_header);
};

struct ModelHeirarchy;

struct SceneryInfo {
    char DebugName[24];
    unsigned int NameHash[4];
    eModel *pModel[4];
    float Radius;
    unsigned int MeshChecksum;
    unsigned int mHeirarchyNameHash;
    ModelHeirarchy *mHeirarchy;
};

SceneryOverrideInfo *GetSceneryOverrideInfo(int override_info_number);
void *FindSceneryGroup(unsigned int name_hash);

struct SceneryGroup : public bTNode<SceneryGroup> {
    unsigned int NameHash;
    short GroupNumber;
    short NumObjects;
    char BarrierFlag;
    char DriveThroughBarrierFlag;
    short Pad2;
    unsigned short OverrideInfoNumbers[16384];

    SceneryOverrideInfo *GetOverrideInfo(int index) {
        return GetSceneryOverrideInfo(OverrideInfoNumbers[index]);
    }
};

struct _type_map;
typedef UTL::Std::map<unsigned int, ModelHeirarchy *, _type_map> ModelHeirarchyMap;

struct tPrecullerInfo {
    unsigned int Values[6];
    short Flags[6];

    bool IsVisible(int visibility_state) {
        return (Flags[0] & visibility_state) != 0;
    }

    bool IsNotVisible(int visibility_state) {
        return !IsVisible(visibility_state);
    }
};

struct SceneryTreeNode {
    SceneryBoundingBox BoundingBox;
    short NumChildren;
    short Children[5];
};

struct eLightContext;
struct PrecullerBooBooManager {
    unsigned char Data[0x800];

    void Reset() {
        bMemSet(this, 0, 0x800);
    }

    int GetSectionNumber(bVector3 &position);
    unsigned char *GetByte(int section_number);
    unsigned char GetBit(int section_number);

    void Set(bVector3 &pos) {
        int n = GetSectionNumber(pos);
        unsigned char *p = GetByte(n);
        *p |= GetBit(n);
    }

    void Clr(bVector3 &pos) {
        int n = GetSectionNumber(pos);
        unsigned char *p = GetByte(n);
        *p &= -GetBit(n) - 1U;
    }
};

class eViewSceneryRenderShim : public eView {
  public:
    void Render(eModel *model, bMatrix4 *matrix, eLightContext *light_context, unsigned int a4, unsigned int a5,
                unsigned int a6);
};

struct GrandSceneryCullInfo {
    // total size: 0x8E0
    SceneryCullInfo SceneryCullInfos[12]; // offset 0x0, size 0x8D0
    int NumCullInfos;                     // offset 0x8D0, size 0x4
    SceneryDrawInfo *pFirstDrawInfo;      // offset 0x8D4, size 0x4
    SceneryDrawInfo *pCurrentDrawInfo;    // offset 0x8D8, size 0x4
    SceneryDrawInfo *pTopDrawInfo;        // offset 0x8DC, size 0x4

    static SceneryDrawInfo SceneryDrawInfoTable[3500];

    int WhatSectionsShouldWeDraw(short *sections_to_draw, int max_sections_to_draw, SceneryCullInfo *scenery_cull_info);
    void CullView(SceneryCullInfo *scenery_cull_info);
    void DoCulling();
    void StuffScenery(eView *view, int stuff_flags);
};

extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern float EnvMapShadowExtraHeight;
extern eModel *pDebugModel;
extern PrecullerBooBooManager gPrecullerBooBooManager;
extern float EnablePrecullingSpeed;
extern int PrecullerMode;
extern int DisablePrecullerCounter;
extern int RealTimeFrames;
extern int CurrentZoneNumber;
extern int SeeulatorToolActive;
extern int ScenerySectionToBlink;
extern int SeeulatorRefreshTrackStreamer;
extern int ShowSectionBoarder;
void RefreshTrackStreamer();
void CreateWindRotMatrix(eView *view, bMatrix4 *matrix, int x, const bMatrix4 *world, int y);
void RenderVisibleSectionBoundary(VisibleSectionBoundary *boundary, eView *view);
ScenerySectionHeader *GetScenerySectionHeader(int section_number);
int IsInTable(short *section_numbers, int num_sections, int section_number);
int ToggleIsInTable(short *section_numbers, int num_sections, int max_sections, int section_number);
bList ScenerySectionHeaderList;
RegionQuery RegionInfo;
SceneryDetailLevel ForceAllSceneryDetailLevels = SCENERY_DETAIL_NONE;
SceneryOverrideInfo *SceneryOverrideInfoTable = 0;
int NumSceneryOverrideInfos = 0;
void *LightTable = 0;
int MaxSceneryLightContexts = 0;
void **SceneryLightContextTable = 0;
void (*ModelConnectionCallback)(ScenerySectionHeader *, int, eModel *) = 0;
void (*ModelDisconnectionCallback)(ScenerySectionHeader *, int, eModel *) = 0;
void (*SectionConnectionCallback)(ScenerySectionHeader *) = 0;
void (*SectionDisconnectionCallback)(ScenerySectionHeader *) = 0;
eModel *pVisibleZoneBoundaryModel = 0;
ModelHeirarchyMap HeirarchyMap;
short SceneryOverrideHashTable[257];
SceneryDrawInfo GrandSceneryCullInfo::SceneryDrawInfoTable[3500];
extern bTList<SceneryGroup> SceneryGroupList;
extern unsigned char SceneryGroupEnabledTable[0x1000];

inline int PrecullerBooBooManager::GetSectionNumber(bVector3 &position) {
    return ((static_cast<int>(position.y) & 0xFE0) << 2) |
           ((static_cast<unsigned int>(static_cast<int>(position.x)) >> 5) & 0x7F);
}

unsigned char *PrecullerBooBooManager::GetByte(int section_number) {
    return Data + (section_number >> 3);
}

unsigned char PrecullerBooBooManager::GetBit(int section_number) {
    return static_cast<unsigned char>(1 << (section_number & 7));
}

static inline char GetScenerySectionLetter_Scenery(int section_number) {
    return static_cast<char>(section_number / 100 + 'A' - 1);
}

static inline void EndianSwapSectionHeader_Scenery(int *section_header_words) {
    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0xC);
    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x10);
    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x14);
    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x38);
}

static inline void EndianSwapSceneryInfo_Scenery(unsigned char *data) {
    for (int i = 0; i < 4; i++) {
        bEndianSwap32(data + 0x18 + i * 4);
    }
    bEndianSwap32(data + 0x38);
    bEndianSwap32(data + 0x3C);
    bEndianSwap32(data + 0x40);
}

static inline void EndianSwapSceneryInstance_Scenery(SceneryInstance *instance) {
    bPlatEndianSwap(&instance->ExcludeFlags);
    bPlatEndianSwap(&instance->PrecullerInfoIndex);
    bPlatEndianSwap(&instance->LightingContextNumber);
    for (int i = 0; i < 3; i++) {
        bPlatEndianSwap(&instance->Position[i]);
    }
    for (int i = 0; i < 9; i++) {
        bPlatEndianSwap(&instance->Rotation[i]);
    }
    bPlatEndianSwap(&instance->SceneryInfoNumber);
    for (int i = 0; i < 3; i++) {
        bPlatEndianSwap(&instance->BBoxMin[i]);
        bPlatEndianSwap(&instance->BBoxMax[i]);
    }
}

static inline void EndianSwapPrecullerInfo_Scenery(unsigned char *data) {
    bEndianSwap32(data + 0x00);
    bEndianSwap32(data + 0x04);
    bEndianSwap32(data + 0x08);
    bEndianSwap32(data + 0x0C);
    bEndianSwap32(data + 0x10);
    bEndianSwap32(data + 0x14);
    bEndianSwap16(data + 0x18);
    for (int i = 0; i < 5; i++) {
        bEndianSwap16(data + 0x1A + i * 2);
    }
}

static inline SceneryOverrideInfo *FindMatchingOverrideInfo_Scenery(int section_number, int override_index) {
    for (int i = 0; i < NumSceneryOverrideInfos; i++) {
        SceneryOverrideInfo *override_info = &SceneryOverrideInfoTable[i];
        if (override_info->SectionNumber == section_number && override_info->OverrideIndex == override_index) {
            return override_info;
        }
    }
    return 0;
}

static inline eModel *FindExistingModel_Scenery(unsigned char *scenery_info, int model_slot) {
    unsigned int name_hash = *reinterpret_cast<unsigned int *>(scenery_info + 0x18 + model_slot * 4);
    for (int i = 0; i < model_slot; i++) {
        eModel *model = *reinterpret_cast<eModel **>(scenery_info + 0x28 + i * 4);
        if (model && model->NameHash == name_hash) {
            return model;
        }
    }
    return 0;
}

static inline unsigned char *GetSceneryInfo_Scenery(int *section_header_words, short scenery_info_number) {
    return reinterpret_cast<unsigned char *>(section_header_words[6]) + scenery_info_number * 0x48;
}

static inline eModel *GetSceneryModel_Scenery(unsigned char *scenery_info, int model_slot) {
    return *reinterpret_cast<eModel **>(scenery_info + 0x28 + model_slot * 4);
}

static inline float GetSceneryRadius_Scenery(unsigned char *scenery_info) {
    return *reinterpret_cast<float *>(scenery_info + 0x38);
}

void BuildSceneryOverrideHashTable() {
    int index = 0;

    for (int i = 0; i < 0x100; i++) {
        SceneryOverrideHashTable[i] = static_cast<short>(index);
        while (index < NumSceneryOverrideInfos &&
               reinterpret_cast<unsigned char *>(&SceneryOverrideInfoTable[index])[0] == i) {
            index += 1;
        }
    }

    SceneryOverrideHashTable[0x100] = static_cast<short>(index);
}

ModelHeirarchy *FindSceneryHeirarchyByName(unsigned int name_hash) {
    ModelHeirarchyMap::iterator it = HeirarchyMap.find(name_hash);
    if (it == HeirarchyMap.end()) {
        return 0;
    }
    return it->second;
}

SceneryOverrideInfo *GetSceneryOverrideInfo(int override_info_number) {
    return reinterpret_cast<SceneryOverrideInfo *>(reinterpret_cast<char *>(SceneryOverrideInfoTable) + override_info_number * 6);
}

void SceneryOverrideInfo::AssignOverrides() {
    ScenerySectionHeader *section_header = GetScenerySectionHeader(SectionNumber);
    if (section_header) {
        AssignOverrides(section_header);
    }
}

void SceneryOverrideInfo::AssignOverrides(ScenerySectionHeader *section_header) {
    int *section_header_words = reinterpret_cast<int *>(section_header);
    SceneryInstance *instance = reinterpret_cast<SceneryInstance *>(section_header_words[8]) +
                                OverrideSectionNumber;

    if ((instance->ExcludeFlags & 0x800000) != 0 &&
        ((instance->ExcludeFlags ^ *reinterpret_cast<unsigned short *>(&OverrideIndex)) & 0x400) != 0) {
        bMatrix4 rotation;
        bMatrix4 flip_matrix;

        rotation.v0.x = static_cast<float>(instance->Rotation[0]) * 0.00012207031f;
        rotation.v0.y = static_cast<float>(instance->Rotation[1]) * 0.00012207031f;
        rotation.v0.z = static_cast<float>(instance->Rotation[2]) * 0.00012207031f;
        rotation.v0.w = 0.0f;
        rotation.v1.x = static_cast<float>(instance->Rotation[3]) * 0.00012207031f;
        rotation.v1.y = static_cast<float>(instance->Rotation[4]) * 0.00012207031f;
        rotation.v1.z = static_cast<float>(instance->Rotation[5]) * 0.00012207031f;
        rotation.v1.w = 0.0f;
        rotation.v2.x = static_cast<float>(instance->Rotation[6]) * 0.00012207031f;
        rotation.v2.y = static_cast<float>(instance->Rotation[7]) * 0.00012207031f;
        rotation.v2.z = static_cast<float>(instance->Rotation[8]) * 0.00012207031f;
        rotation.v2.w = 0.0f;
        rotation.v3.x = 0.0f;
        rotation.v3.y = 0.0f;
        rotation.v3.z = 0.0f;
        rotation.v3.w = 1.0f;

        bIdentity(&flip_matrix);
        flip_matrix.v0.x = -1.0f;
        bMulMatrix(&rotation, &rotation, &flip_matrix);
        rotation.v3.x = instance->Position[0];
        rotation.v3.y = instance->Position[1];
        rotation.v3.z = instance->Position[2];
        rotation.v3.w = 1.0f;

        instance->Rotation[0] = static_cast<short>(rotation.v0.x * 8192.0f);
        instance->Rotation[1] = static_cast<short>(rotation.v0.y * 8192.0f);
        instance->Rotation[2] = static_cast<short>(rotation.v0.z * 8192.0f);
        instance->Rotation[3] = static_cast<short>(rotation.v1.x * 8192.0f);
        instance->Rotation[4] = static_cast<short>(rotation.v1.y * 8192.0f);
        instance->Rotation[5] = static_cast<short>(rotation.v1.z * 8192.0f);
        instance->Rotation[6] = static_cast<short>(rotation.v2.x * 8192.0f);
        instance->Rotation[7] = static_cast<short>(rotation.v2.y * 8192.0f);
        instance->Rotation[8] = static_cast<short>(rotation.v2.z * 8192.0f);
        instance->Position[0] = rotation.v3.x;
        instance->Position[1] = rotation.v3.y;
        instance->Position[2] = rotation.v3.z;
    }

    instance->ExcludeFlags = (instance->ExcludeFlags & 0xFFFF0000) + *reinterpret_cast<unsigned short *>(&OverrideIndex);
}

int LoaderSceneryGroup(bChunk *chunk) {
    if (chunk->GetID() == 0x34109) {
        int chunk_size = chunk->Size;
        int group_offset = 0;
        if (group_offset < chunk_size) {
            do {
                SceneryGroup *group = reinterpret_cast<SceneryGroup *>(reinterpret_cast<char *>(chunk) + group_offset + 8);
                SceneryGroup *head = SceneryGroupList.GetHead();
                head->Prev = group;
                group->Next = head;
                SceneryGroupList.HeadNode.Next = group;
                group->Prev = reinterpret_cast<bNode *>(&SceneryGroupList);

                bEndianSwap32(&group->NameHash);
                bEndianSwap16(&group->GroupNumber);
                bEndianSwap16(&group->NumObjects);
                for (int i = 0; i < group->NumObjects; i++) {
                    bEndianSwap16(&group->OverrideInfoNumbers[i]);
                }

                group_offset += (group->NumObjects * sizeof(unsigned short) + 0x17U) & 0xFFFFFFFC;
            } while (group_offset < chunk_size);
        }
        return 1;
    }

    return 0;
}

int UnloaderSceneryGroup(bChunk *chunk) {
    if (chunk->GetID() == 0x34109) {
        bMemSet(SceneryGroupEnabledTable, 0, 0x1000);
        SceneryGroupEnabledTable[0] = 1;
        SceneryGroupList.InitList();
        return 1;
    }

    return 0;
}

void *FindSceneryGroup(unsigned int name_hash) {
    for (SceneryGroup *group = SceneryGroupList.GetHead(); group != SceneryGroupList.EndOfList(); group = group->GetNext()) {
        if (group->NameHash == name_hash) {
            return group;
        }
    }
    return 0;
}

void EnableSceneryGroup(unsigned int name_hash, bool flip_artwork) {
    SceneryGroup *group = static_cast<SceneryGroup *>(FindSceneryGroup(name_hash));
    if (group) {
        unsigned short override_flags = 0;
        if (flip_artwork) {
            override_flags = 0x400;
        }

        for (int i = 0; i < group->NumObjects; i++) {
            SceneryOverrideInfo *override_info = group->GetOverrideInfo(i);
            *reinterpret_cast<unsigned short *>(&override_info->OverrideIndex) =
                override_flags | (*reinterpret_cast<unsigned short *>(&override_info->OverrideIndex) & 0xFBEF);
            override_info->AssignOverrides();
        }

        SceneryGroupEnabledTable[group->GroupNumber] = 1;
        if (flip_artwork) {
            SceneryGroupEnabledTable[group->GroupNumber] |= 2;
        }
        if (group->DriveThroughBarrierFlag) {
            SceneryGroupEnabledTable[group->GroupNumber] |= 4;
        }
    }
}

void DisableSceneryGroup(unsigned int name_hash) {
    SceneryGroup *group = static_cast<SceneryGroup *>(FindSceneryGroup(name_hash));
    if (group) {
        for (int i = 0; i < group->NumObjects; i++) {
            SceneryOverrideInfo *override_info = group->GetOverrideInfo(i);
            *reinterpret_cast<unsigned short *>(&override_info->OverrideIndex) =
                *reinterpret_cast<unsigned short *>(&override_info->OverrideIndex) | 0x10;
            override_info->AssignOverrides();
        }
        SceneryGroupEnabledTable[group->GroupNumber] = 0;
    }
}

void DisableAllSceneryGroups() {
    for (SceneryGroup *group = SceneryGroupList.GetHead(); group != SceneryGroupList.EndOfList(); group = group->GetNext()) {
        if (SceneryGroupEnabledTable[group->GroupNumber]) {
            for (int i = 0; i < group->NumObjects; i++) {
                SceneryOverrideInfo *override_info = group->GetOverrideInfo(i);
                *reinterpret_cast<unsigned short *>(&override_info->OverrideIndex) =
                    *reinterpret_cast<unsigned short *>(&override_info->OverrideIndex) | 0x10;
                override_info->AssignOverrides();
            }
            SceneryGroupEnabledTable[group->GroupNumber] = 0;
        }
    }
}

void InitVisibleZones() {
    if (pVisibleZoneBoundaryModel == 0) {
        eModel *model = reinterpret_cast<eModel *>(bOMalloc(eModelSlotPool));
        unsigned int name_hash = bStringHash("MARKER_BOUNDARY");
        model->NameHash = 0;
        model->Solid = 0;
        model->Init(name_hash);
        pVisibleZoneBoundaryModel = model;
    }
}

void CloseVisibleZones() {
    eModel *model = pVisibleZoneBoundaryModel;
    if (pVisibleZoneBoundaryModel) {
        pVisibleZoneBoundaryModel->UnInit();
        bFree(eModelSlotPool, model);
    }
    pVisibleZoneBoundaryModel = 0;
    if (SeeulatorToolActive) {
        int data = 0;
        bFunkCallASync("Seeulator", 4, &data, 4);
        bFunkCallASync("Seeulator", 5, &data, 4);
        bFunkCallASync("Seeulator", 6, &data, 4);
    }
}

void ServicePreculler() {}

void LoadPrecullerBooBooScript(const char *filename, bool reset) {
    if (reset) {
        gPrecullerBooBooManager.Reset();
    }

    SpeedScript script(filename, 1);
    while (true) {
        char *region = script.GetNextCommand("BOOBOO:");
        if (!region) {
            return;
        }
        region = script.GetNextArgumentString();
        if (bStrICmp(region, LoadedTrackInfo->GetLoadedTrackInfo()) == 0) {
            char *section = script.GetNextArgumentString();
            char *option = script.GetNextArgumentString();
            bool set_booboo = bStrICmp(option, "SET") == 0;
            bool clr_booboo = bStrICmp(option, "CLR") == 0;

            script.GetNextArgumentString();
            bVector3 pos = script.GetNextArgumentVector3();
            (void)section;
            if (set_booboo) {
                gPrecullerBooBooManager.Set(pos);
            } else if (clr_booboo) {
                gPrecullerBooBooManager.Clr(pos);
            }
        }
    }
}

void LoadPrecullerBooBooScripts() {
    LoadPrecullerBooBooScript("TRACKS\\PrecullerBooBooScript.hoo", 1);
}

SceneryInfo *FindSceneryInfo(unsigned int name_hash) {
    for (ScenerySectionHeader *section_header = reinterpret_cast<ScenerySectionHeader *>(ScenerySectionHeaderList.GetHead());
         section_header != reinterpret_cast<ScenerySectionHeader *>(ScenerySectionHeaderList.EndOfList());
         section_header = reinterpret_cast<ScenerySectionHeader *>(section_header->GetNext())) {
        int *section_header_words = reinterpret_cast<int *>(section_header);
        for (int i = 0; i < section_header_words[7]; i++) {
            SceneryInfo *scenery_info = reinterpret_cast<SceneryInfo *>(section_header_words[6]) + i;
            eModel *model = scenery_info->pModel[0];
            if (model && model->NameHash == name_hash) {
                return scenery_info;
            }
        }
    }
    return 0;
}

SceneryInstance *FindSceneryInstance(unsigned int name_hash) {
    for (ScenerySectionHeader *section_header = reinterpret_cast<ScenerySectionHeader *>(ScenerySectionHeaderList.GetHead());
         section_header != reinterpret_cast<ScenerySectionHeader *>(ScenerySectionHeaderList.EndOfList());
         section_header = reinterpret_cast<ScenerySectionHeader *>(section_header->GetNext())) {
        int *section_header_words = reinterpret_cast<int *>(section_header);
        for (int i = 0; i < section_header_words[9]; i++) {
            SceneryInstance *instance = reinterpret_cast<SceneryInstance *>(section_header_words[8]) + i;
            SceneryInfo *scenery_info = reinterpret_cast<SceneryInfo *>(section_header_words[6]) + instance->SceneryInfoNumber;
            eModel *model = scenery_info->pModel[0];
            if (model && model->NameHash == name_hash) {
                return instance;
            }
        }
    }
    return 0;
}

void ScenerySectionHeader::DrawAScenery(int scenery_instance_number, SceneryCullInfo *scenery_cull_info, int visibility_state) {
    int *section_header_words = reinterpret_cast<int *>(this);
    SceneryInstance *instances = reinterpret_cast<SceneryInstance *>(section_header_words[8]);
    int num_instances = section_header_words[9];

    if (!instances || scenery_instance_number < 0 || scenery_instance_number >= num_instances) {
        return;
    }

    SceneryInstance *instance = &instances[scenery_instance_number];
    if (visibility_state >= 0 && section_header_words[12] && instance->PrecullerInfoIndex >= 0) {
        unsigned char *visibility_table = reinterpret_cast<unsigned char *>(section_header_words[12]) + instance->PrecullerInfoIndex * 0x80;
        if ((visibility_table[visibility_state >> 3] & (1 << (visibility_state & 7))) != 0) {
            return;
        }
    }

    unsigned char *scenery_info = reinterpret_cast<unsigned char *>(section_header_words[6]) + instance->SceneryInfoNumber * 0x48;
    if (((instance->ExcludeFlags ^ 0x60) & scenery_cull_info->ExcludeFlags) != 0) {
        return;
    }

    if (visibility_state == EVISIBLESTATE_PARTIAL) {
        bVector3 bbox_min(instance->BBoxMin[0], instance->BBoxMin[1], instance->BBoxMin[2]);
        bVector3 bbox_max(instance->BBoxMax[0], instance->BBoxMax[1], instance->BBoxMax[2]);
        visibility_state = scenery_cull_info->pView->GetVisibleState(&bbox_min, &bbox_max, 0);
        if (visibility_state == EVISIBLESTATE_NOT) {
            return;
        }
    }

    float to_instance_x = instance->Position[0] - scenery_cull_info->Position.x;
    float to_instance_y = instance->Position[1] - scenery_cull_info->Position.y;
    float to_instance_z = instance->Position[2] - scenery_cull_info->Position.z;
    float radius = GetSceneryRadius_Scenery(scenery_info) + 6.0f;
    float forward_distance = to_instance_x * scenery_cull_info->Direction.x + to_instance_y * scenery_cull_info->Direction.y +
                             to_instance_z * scenery_cull_info->Direction.z;
    int pixel_size_int = 0;
    if (-radius <= forward_distance) {
        float distance = bSqrt(
            to_instance_x * to_instance_x + to_instance_y * to_instance_y + to_instance_z * to_instance_z
        );
        float pixel_size = scenery_cull_info->H;
        if (radius < distance - radius) {
            pixel_size = (radius * pixel_size) / (distance - radius);
        }
        pixel_size_int = static_cast<int>(pixel_size);
    }

    if (pixel_size_int < 2) {
        return;
    }
    unsigned int instance_flags = instance->ExcludeFlags;
    if ((instance_flags & 0x2000000) != 0) {
        pixel_size_int += 10;
    }

    eModel *model = 0;
    if ((scenery_cull_info->ExcludeFlags & 0x1800) == 0) {
        if ((scenery_cull_info->ExcludeFlags & 0x20) != 0) {
            if (pixel_size_int > 0x1F) {
                model = GetSceneryModel_Scenery(scenery_info, 2);
            }
        } else if (eGetCurrentViewMode() > EVIEWMODE_ONE_RVM) {
            if (pixel_size_int > 0x16) {
                model = GetSceneryModel_Scenery(scenery_info, 2);
            }
        } else if (pixel_size_int > 0x11) {
            model = GetSceneryModel_Scenery(scenery_info, 0);
            if (model && model->Solid && model->Solid->NumPolys > 0x27) {
                float lod_scale = model->Solid->Volume;
                if (lod_scale < 6.0f) {
                    lod_scale = 6.0f;
                }
                if ((static_cast<float>(pixel_size_int) / lod_scale) < 8.7f) {
                    model = GetSceneryModel_Scenery(scenery_info, 2);
                }
            }
        }
    } else if ((instance_flags & 0x80) == 0) {
        if (pixel_size_int > 0x1F) {
            if ((instance_flags & 0x1000100) == 0) {
                model = GetSceneryModel_Scenery(scenery_info, 3);
            } else {
                model = GetSceneryModel_Scenery(scenery_info, 0);
            }
        }
    } else if (pixel_size_int > 0x1F) {
        model = GetSceneryModel_Scenery(scenery_info, 2);
    }

    if (!model) {
        return;
    }

    if (scenery_cull_info->pCurrentDrawInfo == scenery_cull_info->pTopDrawInfo) {
        return;
    }

    SceneryDrawInfo *draw_info = scenery_cull_info->pCurrentDrawInfo;
    int *draw_info_words = reinterpret_cast<int *>(draw_info);
    bMatrix4 *matrix = 0;

    if ((instance->ExcludeFlags & 0x200) == 0) {
        unsigned char *matrix_memory = CurrentBufferPos;
        unsigned char *next_buffer_pos = CurrentBufferPos + sizeof(bMatrix4);
        if (CurrentBufferEnd <= next_buffer_pos) {
            FrameMallocFailed = 1;
            FrameMallocFailAmount += sizeof(bMatrix4);
            next_buffer_pos = CurrentBufferPos;
        } else {
            matrix = reinterpret_cast<bMatrix4 *>(matrix_memory);
        }
        CurrentBufferPos = next_buffer_pos;

        if (matrix) {
            matrix->v0.x = static_cast<float>(instance->Rotation[0]) * 0.00012207031f;
            matrix->v0.y = static_cast<float>(instance->Rotation[1]) * 0.00012207031f;
            matrix->v0.z = static_cast<float>(instance->Rotation[2]) * 0.00012207031f;
            matrix->v0.w = 0.0f;
            matrix->v1.x = static_cast<float>(instance->Rotation[3]) * 0.00012207031f;
            matrix->v1.y = static_cast<float>(instance->Rotation[4]) * 0.00012207031f;
            matrix->v1.z = static_cast<float>(instance->Rotation[5]) * 0.00012207031f;
            matrix->v1.w = 0.0f;
            matrix->v2.x = static_cast<float>(instance->Rotation[6]) * 0.00012207031f;
            matrix->v2.y = static_cast<float>(instance->Rotation[7]) * 0.00012207031f;
            matrix->v2.z = static_cast<float>(instance->Rotation[8]) * 0.00012207031f;
            matrix->v2.w = 0.0f;
            matrix->v3.x = instance->Position[0];
            matrix->v3.y = instance->Position[1];
            matrix->v3.z = instance->Position[2];
            matrix->v3.w = 1.0f;

            if ((instance->ExcludeFlags & scenery_cull_info->ExcludeFlags & 0x100) != 0) {
                matrix->v3.z += EnvMapShadowExtraHeight;
            }
            if ((scenery_cull_info->ExcludeFlags & 0x800) != 0) {
                matrix->v2.z = -matrix->v2.z;
            }
        }
    }

    draw_info_words[0] = reinterpret_cast<int>(model) + visibility_state;
    draw_info_words[1] = reinterpret_cast<int>(matrix);
    draw_info_words[2] = reinterpret_cast<int>(instance);
    scenery_cull_info->pCurrentDrawInfo = draw_info + 1;
}

void ScenerySectionHeader::TreeCull(SceneryCullInfo *scenery_cull_info) {
    const int max_depth = 64;
    SceneryTreeNode *node_stack[max_depth];
    unsigned char visibility_state_stack[max_depth];
    int *section_header_words = reinterpret_cast<int *>(this);
    SceneryTreeNode **pnode = node_stack + 1;
    unsigned char *pvisibility_state = visibility_state_stack + 1;

    node_stack[0] = reinterpret_cast<SceneryTreeNode *>(section_header_words[10]);
    visibility_state_stack[0] = 1;
    while (pnode != node_stack) {
        pvisibility_state -= 1;
        unsigned char visibility_state = *pvisibility_state;
        pnode -= 1;
        SceneryTreeNode *node = *pnode;
        if (visibility_state == 1) {
            bVector3 bbox_min(node->BoundingBox.BBoxMin[0], node->BoundingBox.BBoxMin[1], node->BoundingBox.BBoxMin[2]);
            bVector3 bbox_max(node->BoundingBox.BBoxMax[0], node->BoundingBox.BBoxMax[1], node->BoundingBox.BBoxMax[2]);
            visibility_state = scenery_cull_info->pView->GetVisibleState(&bbox_min, &bbox_max, 0);
        }

        if (visibility_state != 0) {
            for (int child_number = 0; child_number < node->NumChildren; child_number++) {
                short child_code = node->Children[child_number];
                if (child_code < 0) {
                    *pnode = reinterpret_cast<SceneryTreeNode *>(reinterpret_cast<char *>(section_header_words[10]) + -child_code * 0x24);
                    *pvisibility_state = visibility_state;
                    pnode += 1;
                    pvisibility_state += 1;
                } else {
                    DrawAScenery(child_code, scenery_cull_info, visibility_state);
                }
            }
        }
    }
}

int LoaderScenery(bChunk *chunk) {
    unsigned int chunk_id = chunk->GetID();

    if (chunk_id == 0x34108) {
        SceneryOverrideInfoTable = reinterpret_cast<SceneryOverrideInfo *>(chunk->GetData());
        NumSceneryOverrideInfos = static_cast<unsigned int>(chunk->Size) / 6;
        for (int i = 0; i < NumSceneryOverrideInfos; i++) {
            unsigned char *override_data = reinterpret_cast<unsigned char *>(SceneryOverrideInfoTable) + i * 6;
            bEndianSwap16(override_data + 0);
            bEndianSwap16(override_data + 2);
            bEndianSwap16(override_data + 4);
        }
        BuildSceneryOverrideHashTable();
        return 1;
    }

    if (chunk_id == 0x80034100) {
        ScenerySectionHeader *section_header = 0;
        int *section_header_words = 0;
        bChunk *last_chunk = chunk->GetLastChunk();

        for (bChunk *subchunk = chunk->GetFirstChunk(); subchunk != last_chunk; subchunk = subchunk->GetNext()) {
            unsigned int subchunk_id = subchunk->GetID();
            if (subchunk_id == 0x34101) {
                section_header = reinterpret_cast<ScenerySectionHeader *>(subchunk->GetAlignedData(0x10));
                section_header_words = reinterpret_cast<int *>(section_header);
                if (section_header_words[2] == 0) {
                    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0xC);
                    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x10);
                    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x14);
                    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x38);
                }

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.AllocateUserInfo(section_header_words[3]);
                user_info->pScenerySectionHeader = section_header;

                if (static_cast<char>(section_header_words[3] / 100 + 'A' - 1) == 'Z') {
                    ScenerySectionHeaderList.AddHead(section_header);
                } else {
                    ScenerySectionHeaderList.AddTail(section_header);
                }
            } else if (subchunk_id == 0x34102) {
                if (!section_header_words) {
                    continue;
                }

                section_header_words[6] = reinterpret_cast<int>(subchunk->GetData());
                section_header_words[7] = static_cast<unsigned int>(subchunk->Size) / 0x48;
                if (section_header_words[2] == 0) {
                    for (int i = 0; i < section_header_words[7]; i++) {
                        int scenery_info_offset = i * 0x48;
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[6] + scenery_info_offset + 0x40));
                        for (int n = 0; n < 4; n++) {
                            bEndianSwap32(
                                reinterpret_cast<unsigned char *>(section_header_words[6] + scenery_info_offset + 0x18 + n * 4)
                            );
                        }
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[6] + scenery_info_offset + 0x38));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[6] + scenery_info_offset + 0x3C));
                    }
                }

                for (int i = 0; i < section_header_words[7]; i++) {
                    unsigned char *scenery_info = reinterpret_cast<unsigned char *>(section_header_words[6] + i * 0x48);
                    unsigned int hierarchy_name = *reinterpret_cast<unsigned int *>(scenery_info + 0x40);
                    if (hierarchy_name != 0) {
                        *reinterpret_cast<unsigned int *>(scenery_info + 0x44) =
                            reinterpret_cast<unsigned int>(FindSceneryHeirarchyByName(hierarchy_name));
                    }
                }
            } else if (subchunk_id == 0x34103) {
                if (!section_header_words) {
                    continue;
                }

                section_header_words[8] = (reinterpret_cast<int>(subchunk) + 0x17) & 0xFFFFFFF0;
                section_header_words[9] = static_cast<unsigned int>(
                    subchunk->Size - (section_header_words[8] - reinterpret_cast<int>(subchunk->GetData()))
                ) >> 6;
                if (section_header_words[2] == 0) {
                    for (int i = 0; i < section_header_words[9]; i++) {
                        unsigned char *instance = reinterpret_cast<unsigned char *>(section_header_words[8] + i * 0x40);
                        bEndianSwap16(instance + 0x3E);
                        bEndianSwap32(instance + 0x18);
                        for (int n = 0; n < 3; n++) {
                            bEndianSwap32(instance + 0x20 + n * 4);
                        }
                        for (int n = 0; n < 9; n++) {
                            bEndianSwap16(instance + 0x2C + n * 2);
                        }
                        bEndianSwap32(instance + 0x00);
                        bEndianSwap32(instance + 0x04);
                        bEndianSwap32(instance + 0x08);
                        bEndianSwap32(instance + 0x0C);
                        bEndianSwap32(instance + 0x10);
                        bEndianSwap32(instance + 0x14);
                        bEndianSwap16(instance + 0x1C);
                        bEndianSwap16(instance + 0x1E);
                    }
                }
            } else if (subchunk_id == 0x34105) {
                if (!section_header_words) {
                    continue;
                }

                section_header_words[10] = reinterpret_cast<int>(subchunk->GetData());
                section_header_words[11] = static_cast<unsigned int>(subchunk->Size) / 0x24;
                if (section_header_words[2] == 0) {
                    for (int i = 0; i < section_header_words[11]; i++) {
                        int preculler_offset = i * 0x24;
                        bEndianSwap16(reinterpret_cast<unsigned char *>(section_header_words[10] + preculler_offset + 0x18));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + preculler_offset + 0x00));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + preculler_offset + 0x04));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + preculler_offset + 0x08));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + preculler_offset + 0x0C));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + preculler_offset + 0x10));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + preculler_offset + 0x14));
                        for (int n = 0; n < 5; n++) {
                            bEndianSwap16(
                                reinterpret_cast<unsigned char *>(section_header_words[10] + preculler_offset + 0x1A + n * 2)
                            );
                        }
                    }
                }
            } else if (subchunk_id == 0x34106) {
                if (!section_header_words) {
                    continue;
                }

                int num_overrides = static_cast<unsigned int>(subchunk->Size) >> 2;
                unsigned short *override_data_base = reinterpret_cast<unsigned short *>(subchunk->GetData());
                for (int i = 0; i < num_overrides; i++) {
                    unsigned short *override_data =
                        reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(override_data_base) + i * 4);
                    bEndianSwap16(&override_data[0]);
                    bEndianSwap16(&override_data[1]);
                    SceneryOverrideInfo *override_info = reinterpret_cast<SceneryOverrideInfo *>(
                        reinterpret_cast<unsigned char *>(SceneryOverrideInfoTable) + override_data[1] * 6
                    );
                    if (override_info->OverrideSectionNumber == override_data[0] &&
                        override_info->SectionNumber == section_header_words[3]) {
                        override_info->AssignOverrides(section_header);
                    }
                }
            } else if (subchunk_id == 0x34107) {
                if (!section_header_words) {
                    continue;
                }

                section_header_words[12] = reinterpret_cast<int>(subchunk->GetData());
                section_header_words[13] = static_cast<unsigned int>(subchunk->Size) >> 7;
            }
        }

        if (section_header_words && ChunkMovementOffset == 0) {
            unsigned char *scenery_infos = reinterpret_cast<unsigned char *>(section_header_words[6]);
            for (int i = 0; i < section_header_words[7]; i++) {
                unsigned char *scenery_info = scenery_infos + i * 0x48;
                unsigned int *name_hashes = reinterpret_cast<unsigned int *>(scenery_info + 0x18);
                eModel **models = reinterpret_cast<eModel **>(scenery_info + 0x28);
                for (int j = 0; j < 4; j++) {
                    unsigned int name_hash = name_hashes[j];
                    if (name_hash != 0 && name_hash != 0xBE43EDBB && name_hash != 0x90F70174) {
                        eModel *model = 0;
                        for (int n = 0; n < j; n++) {
                            model = models[n];
                            if (model && model->NameHash == name_hash) {
                                break;
                            }
                            model = 0;
                        }
                        if (!model) {
                            model = reinterpret_cast<eModel *>(bOMalloc(eModelSlotPool));
                            model->Solid = 0;
                            model->pReplacementTextureTable = 0;
                            model->NumReplacementTextures = 0;
                            model->Init(name_hash);
                            if (ModelConnectionCallback) {
                                ModelConnectionCallback(section_header, i, model);
                            }
                        }
                        models[j] = model;
                    }
                }

                if (models[2] == 0 && models[1] != 0) {
                    models[2] = models[1];
                }
                if (models[0] == 0 && models[1] != 0) {
                    models[0] = models[1];
                }
            }

            if (SectionConnectionCallback) {
                SectionConnectionCallback(section_header);
            }
        }

        if (section_header_words) {
            section_header_words[2] = 1;
        }
        return 1;
    }

    if (chunk_id == 0x80034115) {
        bChunk *last_chunk = chunk->GetLastChunk();
        for (bChunk *subchunk = chunk->GetFirstChunk(); subchunk != last_chunk; subchunk = subchunk->GetNext()) {
            switch (subchunk->GetID()) {
                case 0x34116:
                    LightTable = subchunk->GetData();
                    break;
                case 0x34117:
                    SceneryLightContextTable = reinterpret_cast<void **>(subchunk->GetData());
                    MaxSceneryLightContexts = (subchunk->Size + 3) >> 2;
                    break;
                case 0x34118: {
                    unsigned char *light_context = reinterpret_cast<unsigned char *>(subchunk->GetAlignedData(0x10));
                    bEndianSwap32(light_context + 0x00);
                    bEndianSwap32(light_context + 0x2C);
                    bEndianSwap16(light_context + 0x26);
                    *reinterpret_cast<unsigned int *>(light_context + 0x28) = reinterpret_cast<unsigned int>(light_context + 0x30);
                    unsigned int num_vectors = *reinterpret_cast<unsigned int *>(light_context + 0x2C);
                    for (unsigned int i = 0; i < num_vectors; i++) {
                        bPlatEndianSwap(reinterpret_cast<bVector4 *>(light_context + 0x30 + i * 0x40 + 0x00));
                        bPlatEndianSwap(reinterpret_cast<bVector4 *>(light_context + 0x30 + i * 0x40 + 0x10));
                        bPlatEndianSwap(reinterpret_cast<bVector4 *>(light_context + 0x30 + i * 0x40 + 0x20));
                        bPlatEndianSwap(reinterpret_cast<bVector4 *>(light_context + 0x30 + i * 0x40 + 0x30));
                    }
                    if (SceneryLightContextTable) {
                        SceneryLightContextTable[*reinterpret_cast<short *>(light_context + 0x26)] = light_context;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        return 1;
    }

    if (chunk_id == 0x8003410B) {
        bChunk *last_chunk = chunk->GetLastChunk();
        for (bChunk *subchunk = chunk->GetFirstChunk(); subchunk != last_chunk; subchunk = subchunk->GetNext()) {
            unsigned int *entry_words = reinterpret_cast<unsigned int *>(subchunk->GetData());
            bEndianSwap32(&entry_words[0]);

            unsigned int num_models = *reinterpret_cast<unsigned char *>(&entry_words[1]);
            for (unsigned int i = 0; i < num_models; i++) {
                bEndianSwap32(&entry_words[i * 4 + 2]);
                bEndianSwap32(&entry_words[i * 4 + 3]);
            }

            HeirarchyMap[entry_words[0]] = reinterpret_cast<ModelHeirarchy *>(entry_words);
            for (unsigned int i = 0; i < num_models; i++) {
                eModel *model = 0;
                if (entry_words[i * 4 + 3] != 0) {
                    model = reinterpret_cast<eModel *>(bOMalloc(eModelSlotPool));
                    if (model) {
                        model->Solid = 0;
                        model->pReplacementTextureTable = 0;
                        model->NumReplacementTextures = 0;
                        model->Init(entry_words[i * 4 + 3]);
                    }
                }
                entry_words[i * 4 + 4] = reinterpret_cast<unsigned int>(model);
            }
        }
        return 1;
    }

    return 0;
}

ScenerySectionHeader *GetScenerySectionHeader(int section_number) {
    VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(section_number);
    if (!user_info) {
        return 0;
    }
    return user_info->pScenerySectionHeader;
}

int IsInTable(short *section_numbers, int num_sections, int section_number) {
    for (int i = 0; i < num_sections; i++) {
        if (section_numbers[i] == section_number) {
            return 1;
        }
    }
    return 0;
}

int ToggleIsInTable(short *section_numbers, int num_sections, int max_sections, int section_number) {
    for (int i = 0; i < num_sections; i++) {
        if (section_numbers[i] == section_number) {
            for (int n = i + 1; n < num_sections; n++) {
                section_numbers[n - 1] = section_numbers[n];
            }
            return num_sections - 1;
        }
    }

    if (num_sections < max_sections) {
        section_numbers[num_sections] = static_cast<short>(section_number);
        num_sections += 1;
    }
    return num_sections;
}

int UnloaderScenery(bChunk *chunk) {
    unsigned int chunk_id = chunk->GetID();

    if (chunk_id == 0x34108) {
        SceneryOverrideInfoTable = 0;
        NumSceneryOverrideInfos = 0;
        BuildSceneryOverrideHashTable();
        return 1;
    }

    if (chunk_id == 0x80034100) {
        ScenerySectionHeader *section_header = reinterpret_cast<ScenerySectionHeader *>(chunk->GetAlignedData(0x10));
        int *section_header_words = reinterpret_cast<int *>(section_header);
        VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(section_header_words[3]);
        if (user_info) {
            user_info->pScenerySectionHeader = 0;
        }
        TheVisibleSectionManager.UnallocateUserInfo(section_header_words[3]);
        section_header->Remove();

        if (!AreChunksBeingMoved()) {
            unsigned char *scenery_infos = reinterpret_cast<unsigned char *>(section_header_words[6]);
            for (int i = 0; i < section_header_words[7]; i++) {
                unsigned char *scenery_info = scenery_infos + i * 0x48;
                eModel **model_slots = reinterpret_cast<eModel **>(scenery_info + 0x28);
                for (int j = 0; j < 4; j++) {
                    if (AreChunksBeingMoved()) {
                        break;
                    }

                    eModel **model_slot = &model_slots[j];
                    if (*model_slot) {
                        eModel *slot_model = *model_slot;
                        for (int k = j + 1; k < 4; k++) {
                            eModel **duplicate_slot = &model_slots[k];
                            if (*duplicate_slot == slot_model) {
                                *duplicate_slot = 0;
                            }
                        }
                        if (ModelDisconnectionCallback) {
                            ModelDisconnectionCallback(section_header, i, slot_model);
                        }

                        eModel *model = *model_slot;
                        if (model) {
                            model->UnInit();
                            bFree(eModelSlotPool, model);
                        }
                        *model_slot = 0;
                    }
                }
            }

            if (SectionDisconnectionCallback) {
                SectionDisconnectionCallback(section_header);
            }
        }

        return 1;
    }

    if (chunk_id == 0x8003410B) {
        bChunk *last_chunk = chunk->GetLastChunk();
        for (bChunk *subchunk = chunk->GetFirstChunk(); subchunk != last_chunk; subchunk = subchunk->GetNext()) {
            unsigned int *entry_words = reinterpret_cast<unsigned int *>(subchunk->GetData());
            unsigned int num_models = *reinterpret_cast<unsigned char *>(&entry_words[1]);

            for (unsigned int i = 0; i < num_models; i++) {
                eModel *model = reinterpret_cast<eModel *>(entry_words[i * 4 + 4]);
                if (model) {
                    model->UnInit();
                    bFree(eModelSlotPool, model);
                    entry_words[i * 4 + 4] = 0;
                }
            }

            ModelHeirarchyMap::iterator it = HeirarchyMap.find(entry_words[0]);
            if (it != HeirarchyMap.end()) {
                HeirarchyMap.erase(it);
            }
        }
        return 1;
    }

    if (chunk_id == 0x80034115) {
        LightTable = 0;
        MaxSceneryLightContexts = 0;
        SceneryLightContextTable = 0;
        return 1;
    }

    return 0;
}

int GrandSceneryCullInfo::WhatSectionsShouldWeDraw(short *sections_to_draw, int max_sections_to_draw,
                                                   SceneryCullInfo *scenery_cull_info) {
    DrivableScenerySection *drivable_scenery_section;
    int iViewID = scenery_cull_info->pView->GetID();
    if (iViewID == EVIEW_SHADOWMAP1 || iViewID == EVIEW_SHADOWMAP2) {
        int iViewPlayer = EVIEW_PLAYER1;
        if (iViewID == EVIEW_SHADOWMAP2) {
            iViewPlayer = EVIEW_PLAYER2;
        }
        drivable_scenery_section = TheVisibleSectionManager.FindDrivableSection(
            reinterpret_cast<const bVector2 *>(eGetView(iViewPlayer, false)->GetCamera()->GetPosition())
        );
    } else {
        drivable_scenery_section = TheVisibleSectionManager.FindDrivableSection(
            reinterpret_cast<const bVector2 *>(scenery_cull_info->pView->GetCamera()->GetPosition())
        );
    }

    int num_sections_to_draw = 0;
    if (!drivable_scenery_section) {
        for (ScenerySectionHeader *section_header = reinterpret_cast<ScenerySectionHeader *>(ScenerySectionHeaderList.GetHead());
             section_header != reinterpret_cast<ScenerySectionHeader *>(ScenerySectionHeaderList.EndOfList());
             section_header = section_header->GetNext()) {
            int section_number = section_header->GetSectionNumber();
            int subsection_number = section_number % 100;
            if (subsection_number < 10 && num_sections_to_draw < max_sections_to_draw) {
                sections_to_draw[num_sections_to_draw] = static_cast<short>(section_number);
                num_sections_to_draw += 1;
            }
        }
    } else {
        for (int section_number = 0xA28; section_number < 0xA8C; section_number++) {
            if (GetScenerySectionHeader(section_number) && num_sections_to_draw < max_sections_to_draw) {
                sections_to_draw[num_sections_to_draw] = static_cast<short>(section_number);
                num_sections_to_draw += 1;
            }
        }

        for (int i = 0; i < drivable_scenery_section->NumVisibleSections; i++) {
            int section_number = drivable_scenery_section->GetVisibleSection(i);
            if (section_number > -1 && GetScenerySectionHeader(section_number) && num_sections_to_draw < max_sections_to_draw) {
                sections_to_draw[num_sections_to_draw] = static_cast<short>(section_number);
                num_sections_to_draw += 1;
            }
        }
    }

    if (iViewID == EVIEW_PLAYER1) {
        int current_zone_number = -1;
        if (drivable_scenery_section) {
            current_zone_number = drivable_scenery_section->GetSectionNumber();
        }

        if (current_zone_number != CurrentZoneNumber) {
            if (!SeeulatorToolActive) {
                CurrentZoneNumber = current_zone_number;
                return num_sections_to_draw;
            }
            CurrentZoneNumber = current_zone_number;
            if (drivable_scenery_section) {
                bFunkCallASync("Seeulator", 1, &CurrentZoneNumber, 4);
            }
        }

        if (SeeulatorToolActive) {
            if (ScenerySectionToBlink != 0 && ((RealTimeFrames / 5) & 1U) != 0) {
                num_sections_to_draw =
                    ToggleIsInTable(sections_to_draw, num_sections_to_draw, max_sections_to_draw, ScenerySectionToBlink);
            }
            if (SeeulatorToolActive && SeeulatorRefreshTrackStreamer != 0) {
                RefreshTrackStreamer();
                SeeulatorRefreshTrackStreamer = 0;
            }
        }
    }

    return num_sections_to_draw;
}

void GrandSceneryCullInfo::CullView(SceneryCullInfo *scenery_cull_info) {
    short sections_to_draw[128];
    int num_sections = WhatSectionsShouldWeDraw(sections_to_draw, 0x80, scenery_cull_info);

    for (int i = 0; i < num_sections; i++) {
        if (sections_to_draw[i] > -1) {
            ScenerySectionHeader *section_header = GetScenerySectionHeader(sections_to_draw[i]);
            if (section_header && reinterpret_cast<int *>(section_header)[10] != 0) {
                section_header->TreeCull(scenery_cull_info);
            }
        }
    }
}

void GrandSceneryCullInfo::DoCulling() {
    pFirstDrawInfo = SceneryDrawInfoTable;
    pCurrentDrawInfo = SceneryDrawInfoTable;
    pTopDrawInfo = SceneryDrawInfoTable + 3500;

    for (int i = 0; i < NumCullInfos; i++) {
        SceneryCullInfo *scenery_cull_info = &SceneryCullInfos[i];
        eView *view = scenery_cull_info->pView;
        unsigned char *camera = view ? reinterpret_cast<unsigned char *>(view->GetCamera()) : 0;
        bool do_precull = true;

        if (camera) {
            scenery_cull_info->Position.x = *reinterpret_cast<float *>(camera + 0x40);
            scenery_cull_info->Position.y = *reinterpret_cast<float *>(camera + 0x44);
            scenery_cull_info->Position.z = *reinterpret_cast<float *>(camera + 0x48);
            scenery_cull_info->Direction.x = *reinterpret_cast<float *>(camera + 0x50);
            scenery_cull_info->Direction.y = *reinterpret_cast<float *>(camera + 0x54);
            scenery_cull_info->Direction.z = *reinterpret_cast<float *>(camera + 0x58);
        }
        scenery_cull_info->H = view ? view->H : 0.0f;

        if (PrecullerMode == 0) {
            do_precull = false;
        } else if (PrecullerMode == 2) {
            if (RealTimeFrames % 0x3D < 0xF) {
                do_precull = false;
            }
        } else if (PrecullerMode == 3 && camera) {
            float speed_x = *reinterpret_cast<float *>(camera + 0x1E8);
            float speed_y = *reinterpret_cast<float *>(camera + 0x1EC);
            float speed_z = *reinterpret_cast<float *>(camera + 0x1F0);
            float speed = bSqrt(speed_x * speed_x + speed_y * speed_y + speed_z * speed_z);
            if (speed < EnablePrecullingSpeed) {
                do_precull = false;
            }
        }

        if (DisablePrecullerCounter > 0 && PrecullerMode != 2) {
            do_precull = false;
        }

        scenery_cull_info->PrecullerSectionNumber = -1;
        if (do_precull) {
            int section_number = gPrecullerBooBooManager.GetSectionNumber(scenery_cull_info->Position);
            unsigned char *section_byte = gPrecullerBooBooManager.GetByte(section_number);
            unsigned char section_bit = gPrecullerBooBooManager.GetBit(section_number);
            if (section_byte && (*section_byte & section_bit) == 0) {
                scenery_cull_info->PrecullerSectionNumber =
                    (static_cast<int>(scenery_cull_info->Position.y) & 0x3E0) +
                    ((static_cast<unsigned int>(static_cast<int>(scenery_cull_info->Position.x)) >> 5) & 0x1F);
            }
        }
    }

    for (int i = 0; i < NumCullInfos; i++) {
        SceneryCullInfo *scenery_cull_info = &SceneryCullInfos[i];
        scenery_cull_info->pFirstDrawInfo = pCurrentDrawInfo;
        scenery_cull_info->pCurrentDrawInfo = pCurrentDrawInfo;
        scenery_cull_info->pTopDrawInfo = pTopDrawInfo;
        CullView(scenery_cull_info);
        pCurrentDrawInfo = scenery_cull_info->pCurrentDrawInfo;
    }
}

void RenderVisibleZones(eView *view) {
    if (ShowSectionBoarder != 0 && pVisibleZoneBoundaryModel != 0) {
        DrivableScenerySection *drivable_section =
            TheVisibleSectionManager.FindDrivableSection(reinterpret_cast<const bVector2 *>(view->GetCamera()->GetPosition()));
        if (drivable_section) {
            RenderVisibleSectionBoundary(drivable_section->pBoundary, view);
        }
    }
}

void GrandSceneryCullInfo::StuffScenery(eView *view, int stuff_flags) {
    unsigned int base_flags = 0;
    unsigned int required_flags = 0;
    unsigned int forbidden_flags = 0;

    if ((stuff_flags & 1) != 0) {
        base_flags = 0x1000;
    }
    if ((stuff_flags & 0x400) != 0) {
        required_flags = 0x100000;
    }
    if ((stuff_flags & 0x80) != 0) {
        base_flags |= 0x100;
    }
    if ((stuff_flags & 0x10) == 0) {
        if ((stuff_flags & 8) != 0) {
            forbidden_flags = 0x2000;
        }
    } else {
        required_flags = 0x2000;
    }
    if ((stuff_flags & 0x300) != 0) {
        required_flags = 0x10000;
    }
    if ((stuff_flags & 0x800) != 0) {
        forbidden_flags |= 0x400000;
    }
    if ((stuff_flags & 0x1000) != 0) {
        required_flags = 0x1000000;
    }

    for (int i = 0; i < NumCullInfos; i++) {
        SceneryCullInfo *scenery_cull_info = &SceneryCullInfos[i];
        if (scenery_cull_info->pView != view) {
            continue;
        }

        for (SceneryDrawInfo *draw_info = scenery_cull_info->pFirstDrawInfo; draw_info < scenery_cull_info->pCurrentDrawInfo;
             draw_info++) {
            unsigned int model_word = reinterpret_cast<unsigned int>(draw_info->pModel);
            unsigned int exclude_flags = draw_info->SceneryInst->ExcludeFlags;
            unsigned int render_flags = base_flags;

            pDebugModel = reinterpret_cast<eModel *>(model_word & ~3);
            if ((exclude_flags & 0x80) != 0) {
                render_flags |= 0x2000;
            }
            if ((exclude_flags & 0x1000000) != 0) {
                render_flags |= 0x100000;
            }
            if ((exclude_flags & 0x100) != 0) {
                render_flags |= 0x20000;
            }
            if ((exclude_flags & 0x400000) != 0) {
                render_flags |= 0x40000;
            }
            if ((exclude_flags & 0x20000) != 0) {
                render_flags |= 0x800000;
            }
            if ((exclude_flags & 0x80000) != 0) {
                render_flags |= 0x400000;
            }
            if ((stuff_flags & 0x200) != 0) {
                if ((exclude_flags & 0x200000) != 0 && ((exclude_flags >> 0x1A) & 1U) == 0) {
                    render_flags |= 0x10000;
                }
                if ((exclude_flags & 0x40000000) != 0) {
                    render_flags |= 0x10000;
                }
            }
            if ((stuff_flags & 0x100) != 0) {
                if ((exclude_flags & 0x200000) != 0) {
                    render_flags |= 0x10000;
                }
                if ((exclude_flags & 0x40000000) != 0) {
                    render_flags |= 0x10000;
                }
            }
            if ((model_word & 3) == 2) {
                render_flags |= 4;
            }

            bool required_ok = required_flags == 0 || (render_flags & required_flags) != 0;
            bool forbidden_ok = forbidden_flags == 0 || (render_flags & forbidden_flags) == 0;
            if (required_ok && forbidden_ok) {
                bMatrix4 *matrix = draw_info->pMatrix;
                bMatrix4 identity;
                if (!matrix) {
                    bIdentity(&identity);
                    matrix = &identity;
                }
                reinterpret_cast<eViewSceneryRenderShim *>(view)->Render(pDebugModel, matrix, 0, render_flags, 0, 0);
                pDebugModel = 0;
            }
        }
        return;
    }
}
