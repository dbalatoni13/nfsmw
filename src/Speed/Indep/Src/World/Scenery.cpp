#include "Scenery.hpp"

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "VisibleSection.hpp"
#include "WorldModel.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/bWare/Inc/SpeedScript.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"

SceneryOverrideInfo *GetSceneryOverrideInfo(int override_info_number);
int LoaderSceneryGroup(bChunk *chunk);
int UnloaderSceneryGroup(bChunk *chunk);
int LoaderScenery(bChunk *chunk);
int UnloaderScenery(bChunk *chunk);

struct _type_map;
typedef UTL::Std::map<unsigned int, ModelHeirarchy *, _type_map> ModelHeirarchyMap;

struct eSceneryLightContext : public eLightContext {
    char Name[34];
    short LightingContextNumber;
    bMatrix4 *LocalLights;
    unsigned int NumLights;

    void EndianSwap() {
        bPlatEndianSwap(&Type);
        bPlatEndianSwap(&NumLights);
        bEndianSwap16(&LightingContextNumber);
    }
};

class PrecullerBooBooManager {
  private:
    unsigned char BitField[0x800];

  public:
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

    bool IsSet(bVector3 &pos) {
        int n = GetSectionNumber(pos);
        unsigned char *p = GetByte(n);
        return (*p & GetBit(n)) != 0;
    }
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
static const float EnablePrecullingSpeed = 40.0f * 0.4470272660255432f;
extern int PrecullerMode;
extern int DisablePrecullerCounter;
extern int RealTimeFrames;
extern int CurrentZoneNumber;
extern int SeeulatorToolActive;
extern int ScenerySectionToBlink;
extern int SeeulatorRefreshTrackStreamer;
extern int ShowSectionBoarder;
void RefreshTrackStreamer();
void CreateWindRotMatrix(eView *view, bMatrix4 *matrix, int x, const bMatrix4 *world);
void RenderVisibleSectionBoundary(VisibleSectionBoundary *boundary, eView *view);
ScenerySectionHeader *GetScenerySectionHeader(int section_number);
int IsInTable(short *section_numbers, int num_sections, int section_number);
int ToggleIsInTable(short *section_numbers, int num_sections, int max_sections, int section_number);
bTList<ScenerySectionHeader> ScenerySectionHeaderList;
RegionQuery RegionInfo;
ModelHeirarchyMap HeirarchyMap;
bTList<SceneryGroup> SceneryGroupList;
bChunkLoader bChunkLoaderSceneryGroup(0x34109, LoaderSceneryGroup, UnloaderSceneryGroup);
bChunkLoader bChunkLoaderScenerySection(0x80034100, LoaderScenery, UnloaderScenery);
bChunkLoader bChunkLoaderOverrideInfos(0x34108, LoaderScenery, UnloaderScenery);
bChunkLoader bChunkLoaderSceneryHeirarchy(0x8003410B, LoaderScenery, UnloaderScenery);
bChunkLoader bChunkLoaderSceneryLighting(0x80034115, LoaderScenery, UnloaderScenery);
SceneryDetailLevel ForceAllSceneryDetailLevels = SCENERY_DETAIL_NONE;
SceneryOverrideInfo *SceneryOverrideInfoTable = 0;
int NumSceneryOverrideInfos = 0;
eLight *LightTable = 0;
int MaxSceneryLightContexts = 0;
eSceneryLightContext **SceneryLightContextTable = 0;
void (*ModelConnectionCallback)(ScenerySectionHeader *, int, eModel *) = 0;
void (*ModelDisconnectionCallback)(ScenerySectionHeader *, int, eModel *) = 0;
void (*SectionConnectionCallback)(ScenerySectionHeader *) = 0;
void (*SectionDisconnectionCallback)(ScenerySectionHeader *) = 0;
eModel *pVisibleZoneBoundaryModel = 0;
short SceneryOverrideHashTable[257];
SceneryDrawInfo GrandSceneryCullInfo::SceneryDrawInfoTable[3500];
extern unsigned char SceneryGroupEnabledTable[0x1000];

inline int PrecullerBooBooManager::GetSectionNumber(bVector3 &position) {
    int x_section = (static_cast<unsigned int>(static_cast<int>(position.x)) >> 5) & 0x7F;
    return ((static_cast<int>(position.y) & 0xFE0) << 2) | x_section;
}

static inline int GetPrecullerSectionNumber(float x, float y) {
    return ((static_cast<unsigned int>(static_cast<int>(x)) >> 5) & 0x1F) + (static_cast<int>(y) & 0x3E0);
}

unsigned char *PrecullerBooBooManager::GetByte(int section_number) {
    return BitField + (section_number >> 3);
}

unsigned char PrecullerBooBooManager::GetBit(int section_number) {
    return static_cast<unsigned char>(1 << (section_number & 7));
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
        if (override_info->SectionNumber == section_number && override_info->InstanceNumber == override_index) {
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

static inline int InlinedViewGetPixelSize(SceneryCullInfo *scenery_cull_info, const bVector3 *position, float radius) {
    bVector3 dir = *position - scenery_cull_info->Position;
    float distance_ahead = bDot(&dir, &scenery_cull_info->Direction);
    if (distance_ahead < -radius) {
        return 0;
    }

    float distance_away = bLength(&dir);
    float pixel_size_float = scenery_cull_info->H;
    float distance_minus_radius = distance_away - radius;
    if (distance_minus_radius > radius) {
        pixel_size_float = (radius * pixel_size_float) / distance_minus_radius;
    }
    return static_cast<int>(pixel_size_float);
}

static inline bMatrix4 *eFrameMallocMatrix(int num_matrices) {
    unsigned char *address = CurrentBufferPos;
    unsigned int size = num_matrices * sizeof(bMatrix4);
    unsigned char *next_buffer_pos = address + size;
    if (CurrentBufferEnd <= next_buffer_pos) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += size;
        address = 0;
    } else {
        CurrentBufferPos = next_buffer_pos;
    }
    return reinterpret_cast<bMatrix4 *>(address);
}

void BuildSceneryOverrideHashTable() {
    int num_scenery_override_infos = NumSceneryOverrideInfos;
    SceneryOverrideInfo *scenery_override_info_table = SceneryOverrideInfoTable;
    int index = 0;
    unsigned int i = 0;
    do {
        unsigned int next_i = i + 1;
        SceneryOverrideHashTable[i] = static_cast<short>(index);
        while (index < num_scenery_override_infos &&
               (static_cast<int>(reinterpret_cast<unsigned short *>(&scenery_override_info_table[index])[0]) & 0xFF) == i) {
            index += 1;
        }
        i = next_i;
    } while (static_cast<int>(i) < 0x100);
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

void SceneryOverrideInfo::AssignOverrides(ScenerySectionHeader *section_header) {
    SceneryInstance *scenery_instance = section_header->GetSceneryInstance(InstanceNumber);

    if ((scenery_instance->ExcludeFlags & 0x800000) != 0 && ((scenery_instance->ExcludeFlags ^ ExcludeFlags) & 0x400) != 0) {
        bMatrix4 matrix;
        bMatrix4 flip_matrix;

        scenery_instance->GetRotation(&matrix);

        bIdentity(&flip_matrix);
        flip_matrix.v0.x = -1.0f;
        bMulMatrix(&matrix, &matrix, &flip_matrix);
        scenery_instance->GetPosition(&matrix.v3);
        scenery_instance->SetMatrix(&matrix);
    }

    scenery_instance->ExcludeFlags = ExcludeFlags + (scenery_instance->ExcludeFlags & 0xFFFF0000);
}

void SceneryOverrideInfo::AssignOverrides() {
    ScenerySectionHeader *section_header = GetScenerySectionHeader(SectionNumber);
    if (section_header) {
        AssignOverrides(section_header);
    }
}

void LoadPrecullerBooBooScript(const char *filename, bool reset) {
    if (reset) {
        gPrecullerBooBooManager.Reset();
    }

    SpeedScript script(filename, 1);
    while (script.GetNextCommand("BOOBOO:")) {
        if (bStrICmp(script.GetNextArgumentString(), TrackInfo::GetLoadedTrackInfo()->RegionName) == 0) {
            script.GetNextArgumentString();
            char *option = script.GetNextArgumentString();
            bool set_booboo = bStrICmp(option, "SET") == 0;
            bool clr_booboo = bStrICmp(option, "CLR") == 0;
            script.GetNextArgumentString();
            bVector3 pos = script.GetNextArgumentVector3();
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

SceneryGroup *FindSceneryGroup(unsigned int name_hash) {
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
            override_info->ExcludeFlags = override_flags | (override_info->ExcludeFlags & 0xFBEF);
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
            override_info->ExcludeFlags = override_info->ExcludeFlags | 0x10;
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
                override_info->ExcludeFlags = override_info->ExcludeFlags | 0x10;
                override_info->AssignOverrides();
            }
            SceneryGroupEnabledTable[group->GroupNumber] = 0;
        }
    }
}

ScenerySectionHeader *GetScenerySectionHeader(int section_number) {
    VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(section_number);
    if (!user_info) {
        return 0;
    }
    return user_info->pScenerySectionHeader;
}

int LoaderScenery(bChunk *chunk) {
    if (chunk->GetID() == 0x34108) {
        SceneryOverrideInfoTable = reinterpret_cast<SceneryOverrideInfo *>(chunk->GetData());
        NumSceneryOverrideInfos = static_cast<unsigned int>(chunk->Size) / 6;
        for (int i = 0; i < NumSceneryOverrideInfos; i++) {
            SceneryOverrideInfo *override_info = &SceneryOverrideInfoTable[i];
            override_info->EndianSwap();
        }
        BuildSceneryOverrideHashTable();
        return 1;
    }

    if (chunk->GetID() == 0x80034100) {
        ScenerySectionHeader *section_header = 0;
        bChunk *last_chunk = chunk->GetLastChunk();

        for (bChunk *subchunk = chunk->GetFirstChunk(); subchunk != last_chunk; subchunk = subchunk->GetNext()) {
            unsigned int subchunk_id = subchunk->GetID();
            if (subchunk_id == 0x34101) {
                section_header = reinterpret_cast<ScenerySectionHeader *>(subchunk->GetAlignedData(0x10));
                int *section_header_words = reinterpret_cast<int *>(section_header);
                if (section_header_words[2] == 0) {
                    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0xC);
                    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x10);
                    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x14);
                    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x38);
                }

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.AllocateUserInfo(section_header_words[3]);
                user_info->pScenerySectionHeader = section_header;

                if (GetScenerySectionLetter(section_header_words[3]) == 'Z') {
                    ScenerySectionHeaderList.AddHead(section_header);
                } else {
                    ScenerySectionHeaderList.AddTail(section_header);
                }
            } else if (subchunk_id == 0x34102) {
                int *section_header_words = reinterpret_cast<int *>(section_header);
                section_header_words[6] = reinterpret_cast<int>(subchunk->GetData());
                section_header_words[7] = static_cast<unsigned int>(subchunk->Size) / 0x48;
                if (section_header_words[2] == 0) {
                    for (int i = 0; i < section_header_words[7]; i++) {
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[6] + i * 0x48 + 0x40));
                        for (int n = 0; n < 4; n++) {
                            bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[6] + i * 0x48 + 0x18 + n * 4));
                        }
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[6] + i * 0x48 + 0x38));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[6] + i * 0x48 + 0x3C));
                    }
                }

                for (int i = 0; i < section_header_words[7]; i++) {
                    SceneryInfo *scenery_info = reinterpret_cast<SceneryInfo *>(section_header_words[6]) + i;
                    unsigned int hierarchy_name = scenery_info->mHeirarchyNameHash;
                    if (hierarchy_name != 0) {
                        scenery_info->mHeirarchy = FindSceneryHeirarchyByName(hierarchy_name);
                    }
                }
            } else if (subchunk_id == 0x34103) {
                int *section_header_words = reinterpret_cast<int *>(section_header);
                section_header_words[8] = (reinterpret_cast<int>(subchunk) + 0x17) & 0xFFFFFFF0;
                section_header_words[9] =
                    static_cast<unsigned int>(subchunk->Size - (section_header_words[8] - reinterpret_cast<int>(subchunk->GetData()))) >> 6;
                if (section_header_words[2] == 0) {
                    for (int i = 0; i < section_header_words[9]; i++) {
                        unsigned char *instance = reinterpret_cast<unsigned char *>(section_header_words[8] + i * 0x40);
                        bEndianSwap16(instance + 0x3E);
                        bEndianSwap32(instance + 0x18);
                        for (int n = 0; n < 3; n++) {
                            unsigned char *swap = instance + n * 4;
                            bEndianSwap32(swap + 0x20);
                        }
                        for (int n = 0; n < 9; n++) {
                            unsigned char *swap = instance + n * 2;
                            bEndianSwap16(swap + 0x2C);
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
                int *section_header_words = reinterpret_cast<int *>(section_header);
                section_header_words[10] = reinterpret_cast<int>(subchunk->GetData());
                section_header_words[11] = static_cast<unsigned int>(subchunk->Size) / 0x24;
                if (section_header_words[2] == 0) {
                    for (int i = 0; i < section_header_words[11]; i++) {
                        bEndianSwap16(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24 + 0x18));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24 + 0x00));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24 + 0x04));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24 + 0x08));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24 + 0x0C));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24 + 0x10));
                        bEndianSwap32(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24 + 0x14));
                        for (int n = 0; n < 5; n++) {
                            bEndianSwap16(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24 + 0x1A + n * 2));
                        }
                    }
                }
            } else if (subchunk_id == 0x34106) {
                int *section_header_words = reinterpret_cast<int *>(section_header);
                int num_overrides = static_cast<unsigned int>(subchunk->Size) >> 2;
                unsigned short *override_data_base = reinterpret_cast<unsigned short *>(subchunk->GetData());
                for (int i = 0; i < num_overrides; i++) {
                    unsigned short *override_data = reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(override_data_base) + i * 4);
                    bEndianSwap16(&override_data[0]);
                    bEndianSwap16(&override_data[1]);
                    SceneryOverrideInfo *override_info =
                        reinterpret_cast<SceneryOverrideInfo *>(reinterpret_cast<unsigned char *>(SceneryOverrideInfoTable) + override_data[1] * 6);
                    if (override_info->InstanceNumber == override_data[0] && override_info->SectionNumber == section_header_words[3]) {
                        override_info->AssignOverrides(section_header);
                    }
                }
            } else if (subchunk_id == 0x34107) {
                int *section_header_words = reinterpret_cast<int *>(section_header);
                section_header_words[12] = reinterpret_cast<int>(subchunk->GetData());
                int num_override_datas = static_cast<unsigned int>(subchunk->Size) >> 7;
                section_header_words[13] = num_override_datas;
                if (section_header_words[2] == 0) {
                    for (int i = 0; i < num_override_datas; i++) {
                    }
                }
            }
        }

        if (!AreChunksBeingMoved()) {
            int *section_header_words = reinterpret_cast<int *>(section_header);
            SceneryInfo *scenery_infos = reinterpret_cast<SceneryInfo *>(section_header_words[6]);
            for (int n = 0; n < section_header_words[7]; n++) {
                SceneryInfo *scenery_info = &scenery_infos[n];
                for (int detail_level = 0; detail_level < 4; detail_level++) {
                    unsigned int name_hash = scenery_info->NameHash[detail_level];
                    if (name_hash != 0 && name_hash != 0xBE43EDBB && name_hash != 0x90F70174) {
                        eModel *model = 0;
                        for (int i = 0; i < detail_level; i++) {
                            model = scenery_info->pModel[i];
                            if (model && model->NameHash == name_hash) {
                                break;
                            }
                            model = 0;
                        }

                        if (!model) {
                            model = reinterpret_cast<eModel *>(bOMalloc(eModelSlotPool));
                            model->NameHash = 0;
                            model->Solid = 0;
                            model->Init(name_hash);
                            if (ModelConnectionCallback) {
                                ModelConnectionCallback(section_header, n, model);
                            }
                        }
                        scenery_info->pModel[detail_level] = model;
                    }
                }

                eModel *lowest_detail_model = scenery_info->pModel[1];
                if (scenery_info->pModel[2] == 0 && lowest_detail_model != 0) {
                    scenery_info->pModel[2] = lowest_detail_model;
                }
                if (scenery_info->pModel[0] == 0 && lowest_detail_model != 0) {
                    scenery_info->pModel[0] = lowest_detail_model;
                }
            }

            if (SectionConnectionCallback) {
                SectionConnectionCallback(section_header);
            }
        }

        reinterpret_cast<int *>(section_header)[2] = 1;
        return 1;
    }

    if (chunk->GetID() == 0x8003410B) {
        bChunk *last_chunk = chunk->GetLastChunk();
        for (bChunk *subchunk = chunk->GetFirstChunk(); subchunk != last_chunk; subchunk = subchunk->GetNext()) {
            ModelHeirarchy *mH = reinterpret_cast<ModelHeirarchy *>(subchunk->GetData());
            ModelHeirarchy::Node *node = reinterpret_cast<ModelHeirarchy::Node *>(mH + 1);
            bEndianSwap32(&mH->mNameHash);

            unsigned int num_models = mH->mNumNodes;
            for (unsigned int i = 0; i < num_models; i++) {
                bEndianSwap32(&node[i].mNodeName);
                bEndianSwap32(&node[i].mModelHash);
            }

            HeirarchyMap[mH->mNameHash] = mH;
            for (unsigned int i = 0; i < num_models; i++) {
                eModel *model = 0;
                if (node[i].mModelHash != 0) {
                    model = reinterpret_cast<eModel *>(bOMalloc(eModelSlotPool));
                    if (model) {
                        model->NameHash = 0;
                        model->Solid = 0;
                        model->Init(node[i].mModelHash);
                    }
                }
                node[i].mModel = model;
            }
        }
        return 1;
    }

    if (chunk->GetID() == 0x80034115) {
        bChunk *last_chunk = chunk->GetLastChunk();
        for (bChunk *subchunk = chunk->GetFirstChunk(); subchunk != last_chunk; subchunk = subchunk->GetNext()) {
            if (subchunk->GetID() == 0x34116) {
                LightTable = reinterpret_cast<eLight *>(subchunk->GetData());
            } else if (subchunk->GetID() == 0x34117) {
                SceneryLightContextTable = reinterpret_cast<eSceneryLightContext **>(subchunk->GetData());
                MaxSceneryLightContexts = (subchunk->Size + 3) >> 2;
            } else if (subchunk->GetID() == 0x34118) {
                eSceneryLightContext *light_context = reinterpret_cast<eSceneryLightContext *>(subchunk->GetAlignedData(0x10));
                bPlatEndianSwap(&light_context->Type);
                bPlatEndianSwap(&light_context->NumLights);
                bPlatEndianSwap(&light_context->LightingContextNumber);
                light_context->LocalLights = reinterpret_cast<bMatrix4 *>(light_context + 1);
                for (unsigned int i = 0; i < light_context->NumLights; i++) {
                    bPlatEndianSwap(reinterpret_cast<bVector4 *>(reinterpret_cast<char *>(light_context->LocalLights) + i * 0x40 + 0x00));
                    bPlatEndianSwap(reinterpret_cast<bVector4 *>(reinterpret_cast<char *>(light_context->LocalLights) + i * 0x40 + 0x10));
                    bPlatEndianSwap(reinterpret_cast<bVector4 *>(reinterpret_cast<char *>(light_context->LocalLights) + i * 0x40 + 0x20));
                    bPlatEndianSwap(reinterpret_cast<bVector4 *>(reinterpret_cast<char *>(light_context->LocalLights) + i * 0x40 + 0x30));
                }
                SceneryLightContextTable[light_context->LightingContextNumber] = light_context;
            }
        }
        return 1;
    }

    return 0;
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
        TheVisibleSectionManager.GetUserInfo(section_header_words[3])->pScenerySectionHeader = 0;
        TheVisibleSectionManager.UnallocateUserInfo(section_header_words[3]);
        section_header->Remove();

        if (!AreChunksBeingMoved()) {
            for (int i = 0; i < section_header_words[7]; i++) {
                unsigned char *scenery_info = reinterpret_cast<unsigned char *>(section_header_words[6]) + i * 0x48;
                eModel **model_slots = reinterpret_cast<eModel **>(scenery_info + 0x28);
                for (int j = 0; j < 4; j++) {
                    if (AreChunksBeingMoved()) {
                        break;
                    }

                    if (model_slots[j]) {
                        eModel *slot_model = model_slots[j];
                        for (int k = j + 1; k < 4; k++) {
                            if (model_slots[k] == slot_model) {
                                model_slots[k] = 0;
                            }
                        }
                        if (ModelDisconnectionCallback) {
                            ModelDisconnectionCallback(section_header, i, model_slots[j]);
                        }

                        eModel *model = model_slots[j];
                        if (model) {
                            model->UnInit();
                            bFree(eModelSlotPool, model);
                        }
                        model_slots[j] = 0;
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
            ModelHeirarchy *heirarchy = reinterpret_cast<ModelHeirarchy *>(subchunk->GetData());
            ModelHeirarchy::Node *nodes = reinterpret_cast<ModelHeirarchy::Node *>(heirarchy + 1);
            unsigned int num_models = heirarchy->mNumNodes;

            for (unsigned int i = 0; i < num_models; i++) {
                eModel *model = nodes[i].mModel;
                if (model) {
                    model->UnInit();
                    bFree(eModelSlotPool, model);
                    nodes[i].mModel = 0;
                }
            }

            ModelHeirarchyMap::iterator it = HeirarchyMap.find(heirarchy->mNameHash);
            if (it != HeirarchyMap.end()) {
                HeirarchyMap.erase(it);
            }
        }
        return 1;
    }

    if (chunk_id == 0x80034115) {
        MaxSceneryLightContexts = 0;
        LightTable = 0;
        SceneryLightContextTable = 0;
        return 1;
    }

    return 0;
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

void RenderVisibleSectionBoundary(VisibleSectionBoundary *boundary, eView *view) {
    if (boundary->NumPoints <= 0) {
        return;
    }

    float perimeter;
    {
        int n;

        for (n = 0; n < boundary->GetNumPoints(); n++) {
            bVector2 *v1 = boundary->GetPoint(n);
            bVector2 *v2 = boundary->GetPoint((n + 1) % boundary->GetNumPoints());
            float x = v1->x - v2->x;
            float y = v1->y - v2->y;
            perimeter = bSqrt(x * x + y * y);
        }
    }

    bVector3 position;
    TopologyCoordinate topology_coordinate;
    float pos = static_cast<float>((static_cast<int>(WorldTimer.GetSeconds() * 262144.0f) & 0xffff)) * 6.103515625e-05f;
    int point_number;

    for (point_number = 0; point_number < boundary->GetNumPoints(); point_number++) {
        bVector2 normal = *boundary->GetPoint((point_number + 1) % boundary->GetNumPoints()) - *boundary->GetPoint(point_number);
        float length = bLength(&normal);

        bNormalize(&normal, &normal);
        if (pos < length) {
            do {
                bScaleAdd(reinterpret_cast<bVector2 *>(&position), boundary->GetPoint(point_number), &normal, pos);

                if (topology_coordinate.HasTopology(reinterpret_cast<bVector2 *>(&position))) {
                    position.z = 9999.0f;
                    position.z = topology_coordinate.GetElevation(&position, 0, 0, 0);
                    int pixel_size = view->GetPixelSize(&position, 1.0f);
                    if (pixel_size > 0) {
                        unsigned char *matrix_memory = CurrentBufferPos;
                        unsigned char *next_buffer_pos = matrix_memory + sizeof(bMatrix4);
                        if (next_buffer_pos >= CurrentBufferEnd) {
                            FrameMallocFailed = 1;
                            FrameMallocFailAmount += sizeof(bMatrix4);
                            matrix_memory = 0;
                        } else {
                            CurrentBufferPos = next_buffer_pos;
                        }

                        if (matrix_memory) {
                            bMatrix4 *matrix = reinterpret_cast<bMatrix4 *>(matrix_memory);
                            bIdentity(matrix);
                            bCopy(&matrix->v3, &position, 1.0f);
                            reinterpret_cast<eViewPlatInterface *>(view)->Render(pVisibleZoneBoundaryModel, matrix, 0, 0, 0);
                        }
                    }
                }

                pos += 4.0f;
            } while (pos < length);
        }

        pos -= length;
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

int IsInTable(short *section_numbers, int num_sections, int section_number) {
    for (int i = 0; i < num_sections; i++) {
        if (section_numbers[i] == section_number) {
            return i;
        }
    }
    return -1;
}

int ToggleIsInTable(short *section_numbers, int num_sections, int max_sections, int section_number) {
    int section_index = IsInTable(section_numbers, num_sections, section_number);
    if (section_index >= 0) {
        section_numbers[section_index] = -1;
        return num_sections;
    }

    section_numbers[num_sections % max_sections] = static_cast<short>(section_number);
    return num_sections + 1;
}

void ScenerySectionHeader::DrawAScenery(int scenery_instance_number, SceneryCullInfo *scenery_cull_info, int visibility_state) {
    int *section_header_words = reinterpret_cast<int *>(this);
    SceneryInstance *instance = GetSceneryInstance(scenery_instance_number);
    tPrecullerInfo *preculler_info = GetPrecullerInfo(instance->PrecullerInfoIndex);
    int preculler_section_number = scenery_cull_info->PrecullerSectionNumber;
    if (preculler_section_number >= 0) {
        int byte_number = preculler_section_number >> 3;
        int bit_number = preculler_section_number & 7;
        unsigned char visibility_bits = preculler_info->GetBits()[byte_number];
        int visibility_mask = 1 << bit_number;
        if ((visibility_bits & visibility_mask) != 0) {
            return;
        }
    }

    unsigned char instance_exclude_flags = instance->ExcludeFlags;
    short scenery_info_number = instance->SceneryInfoNumber;
    if (((instance_exclude_flags ^ 0x60) & scenery_cull_info->ExcludeFlags) != 0) {
        return;
    }
    int pixel_size_int;
    SceneryInfo *scenery_info = reinterpret_cast<SceneryInfo *>(GetSceneryInfo_Scenery(section_header_words, scenery_info_number));

    if (visibility_state == EVISIBLESTATE_PARTIAL) {
        bVector3 bbox_min;
        bVector3 bbox_max;
        instance->GetBBox(&bbox_min, &bbox_max);
        visibility_state = scenery_cull_info->pView->GetVisibleState(&bbox_min, &bbox_max, 0);
        if (visibility_state == EVISIBLESTATE_NOT) {
            return;
        }
    }

    float radius = scenery_info->Radius + 6.0f;
    pixel_size_int = InlinedViewGetPixelSize(scenery_cull_info, instance->GetPosition(), radius);

    if (pixel_size_int < 2) {
        return;
    }
    unsigned int instance_flags = instance->ExcludeFlags;
    if ((instance_flags & 0x2000000) != 0) {
        pixel_size_int += 10;
    }

    eModel *model = 0;
    if ((scenery_cull_info->ExcludeFlags & 0x1800) != 0) {
        if ((instance_flags & 0x80) != 0) {
            if (pixel_size_int > 0x1F) {
                model = scenery_info->pModel[2];
            }
        } else {
            if (pixel_size_int > 0x1F) {
                if ((instance_flags & 0x1000100) != 0) {
                    model = scenery_info->pModel[0];
                } else {
                    model = scenery_info->pModel[3];
                }
            }
        }
    } else if ((scenery_cull_info->ExcludeFlags & 0x20) != 0) {
        if (pixel_size_int > 0x1F) {
            model = scenery_info->pModel[2];
        }
    } else if (eGetCurrentViewMode() > EVIEWMODE_ONE_RVM) {
        if (pixel_size_int > 0x16) {
            model = scenery_info->pModel[2];
        }
    } else if (pixel_size_int > 0x11) {
        model = scenery_info->pModel[0];
        eSolid *solid = model ? model->GetSolid() : 0;
        if (solid && solid->NumPolys > 0x27) {
            float lod_scale = solid->Density;
            if (lod_scale < 6.0f) {
                lod_scale = 6.0f;
            }
            if ((static_cast<float>(pixel_size_int) / lod_scale) < 8.7f) {
                model = scenery_info->pModel[2];
            }
        }
    }

    if (!model) {
        return;
    }

    if ((instance->ExcludeFlags & 0x200) != 0) {
        SceneryDrawInfo *draw_info = scenery_cull_info->pCurrentDrawInfo;
        if (draw_info >= scenery_cull_info->pTopDrawInfo) {
            return;
        }

        scenery_cull_info->pCurrentDrawInfo = draw_info + 1;
        draw_info->pModel = reinterpret_cast<eModel *>(reinterpret_cast<int>(model) + visibility_state);
        draw_info->pMatrix = 0;
        draw_info->SceneryInst = instance;
        return;
    }

    bMatrix4 *matrix = eFrameMallocMatrix(1);

    if (!matrix) {
        return;
    }

    instance->GetRotation(matrix);
    bFill(&matrix->v3, instance->Position[0], instance->Position[1], instance->Position[2], 1.0f);

    if ((instance->ExcludeFlags & scenery_cull_info->ExcludeFlags & 0x100) != 0) {
        matrix->v3.z += EnvMapShadowExtraHeight;
    }
    if ((scenery_cull_info->ExcludeFlags & 0x800) != 0) {
        matrix->v2.z = -matrix->v2.z;
    }

    SceneryDrawInfo *draw_info = scenery_cull_info->pCurrentDrawInfo;
    if (draw_info >= scenery_cull_info->pTopDrawInfo) {
        return;
    }

    scenery_cull_info->pCurrentDrawInfo = draw_info + 1;
    draw_info->pModel = reinterpret_cast<eModel *>(reinterpret_cast<int>(model) + visibility_state);
    if ((scenery_cull_info->ExcludeFlags & 0x4000) != 0 && model->GetSolid() && (model->GetSolid()->Flags & 0x80) != 0) {
        bMatrix4 windrot;
        int offset = static_cast<int>(matrix->v3.x * 60.0f) % 0x168;
        CreateWindRotMatrix(scenery_cull_info->pView, &windrot, offset, matrix);
        bMulMatrix(matrix, matrix, &windrot);
    }

    draw_info->pMatrix = matrix;
    draw_info->SceneryInst = instance;

    if (scenery_cull_info->pView == eGetView(1, false) || scenery_cull_info->pView == eGetView(2, false)) {
        ePositionMarker *position_marker = 0;
        while ((position_marker = model->GetPostionMarker(position_marker)) != 0) {
            if (model->GetSolid()) {
                unsigned int exclude_view_ids = 2;
                if (scenery_cull_info->pView == eGetView(1, false)) {
                    exclude_view_ids = 1;
                }

                eLightFlare *light_flare = eGetNextLightFlareInPool(exclude_view_ids);
                if (light_flare) {
                    bVector4 ps;
                    ps.x = position_marker->Matrix.v3.x - model->GetSolid()->PivotMatrix.v3.x;
                    ps.y = position_marker->Matrix.v3.y - model->GetSolid()->PivotMatrix.v3.y;
                    ps.z = position_marker->Matrix.v3.z - model->GetSolid()->PivotMatrix.v3.z;
                    ps.w = 1.0f;
                    eMulVector(&ps, draw_info->pMatrix, &ps);

                    if (scenery_cull_info->pView->Precipitation && 0.0f < scenery_cull_info->pView->Precipitation->GetRoadDampness() &&
                        (light_flare->PositionX != ps.x || light_flare->PositionY != ps.y || light_flare->PositionZ != ps.z)) {
                        light_flare->ReflectPosZ = 999.0f;
                    }

                    light_flare->PositionX = ps.x;
                    light_flare->PositionY = ps.y;
                    light_flare->PositionZ = ps.z;
                    light_flare->Type = position_marker->iParam0 + 14;
                    if (static_cast<unsigned int>(position_marker->iParam0 - 3) < 3) {
                        bVector2 dr;
                        light_flare->Flags = 4;
                        dr.x = ps.x - draw_info->pMatrix->v3.x;
                        dr.y = ps.y - draw_info->pMatrix->v3.y;
                        bNormalize(&dr, &dr);
                        light_flare->DirectionX = dr.x;
                        light_flare->DirectionY = dr.y;
                        light_flare->DirectionZ = 0.0f;
                    } else {
                        light_flare->Flags = 2;
                    }
                }
            }
        }
    }
}

void ScenerySectionHeader::TreeCull(SceneryCullInfo *scenery_cull_info) {
    const int max_depth = 64;
    SceneryTreeNode *node_stack[max_depth];
    unsigned char visibility_state_stack[max_depth];
    SceneryTreeNode **pnode = node_stack + 1;
    unsigned char *pvisibility_state = visibility_state_stack + 1;

    node_stack[0] = reinterpret_cast<SceneryTreeNode *>(reinterpret_cast<int *>(this)[10]);
    visibility_state_stack[0] = 1;
    while (pnode != node_stack) {
        pnode -= 1;
        SceneryTreeNode *node = *pnode;
        pvisibility_state -= 1;
        unsigned char visibility_state = *pvisibility_state;
        if (visibility_state == 1) {
            bVector3 bbox_min;
            bVector3 bbox_max;

            node->GetBBox(&bbox_min, &bbox_max);
            visibility_state = scenery_cull_info->pView->GetVisibleState(&bbox_min, &bbox_max, 0);
        }

        if (visibility_state != 0) {
            for (int child_number = 0; child_number < node->NumChildren; child_number++) {
                // TODO
                // short child_code = node->Children[child_number];
                short child_code;
                if (child_code >= 0) {
                    DrawAScenery(child_code, scenery_cull_info, visibility_state);
                } else {
                    int scenery_instance_number = child_code * -1;
                    SceneryTreeNode *child_node;

                    child_node = reinterpret_cast<SceneryTreeNode *>(reinterpret_cast<char *>(reinterpret_cast<int *>(this)[10]) +
                                                                     static_cast<short>(scenery_instance_number) * 0x24);

                    *pnode = child_node;
                    *pvisibility_state = visibility_state;
                    pnode += 1;
                    pvisibility_state += 1;
                }
            }
        }
    }
}

int GrandSceneryCullInfo::WhatSectionsShouldWeDraw(short *sections_to_draw, int max_sections_to_draw, SceneryCullInfo *scenery_cull_info) {
    short *sections = sections_to_draw;
    int max_sections = max_sections_to_draw;
    SceneryCullInfo *cull_info = scenery_cull_info;
    DrivableScenerySection *drivable_scenery_section;
    int iViewID = cull_info->pView->GetID();
    if (iViewID == EVIEW_SHADOWMAP1 || iViewID == EVIEW_SHADOWMAP2) {
        int iViewPlayer = iViewID - 12;
        drivable_scenery_section =
            TheVisibleSectionManager.FindDrivableSection(reinterpret_cast<const bVector2 *>(eViews[iViewPlayer].GetCamera()->GetPosition()));
    } else {
        drivable_scenery_section =
            TheVisibleSectionManager.FindDrivableSection(reinterpret_cast<const bVector2 *>(cull_info->pView->GetCamera()->GetPosition()));
    }

    int num_sections_to_draw = 0;
    if (!drivable_scenery_section) {
        for (ScenerySectionHeader *section_header = reinterpret_cast<ScenerySectionHeader *>(ScenerySectionHeaderList.GetHead());
             section_header != reinterpret_cast<ScenerySectionHeader *>(ScenerySectionHeaderList.EndOfList());
             section_header = section_header->GetNext()) {
            int section_number = section_header->GetSectionNumber();
            int subsection_number = section_number % 100;
            if (subsection_number < 10 && num_sections_to_draw < max_sections) {
                sections[num_sections_to_draw] = static_cast<short>(section_number);
                num_sections_to_draw += 1;
            }
        }
    } else {
        int section_number = 0xA28;
    section_number_loop:
        if (section_number >= 0xA8C) {
            goto end_section_number_loop;
        }

        if (GetScenerySectionHeader(section_number) && num_sections_to_draw < max_sections) {
            sections[num_sections_to_draw] = static_cast<short>(section_number);
            num_sections_to_draw += 1;
        }
        section_number += 1;
        goto section_number_loop;
    end_section_number_loop:

        for (int i = 0; i < drivable_scenery_section->GetNumVisibleSections(); i++) {
            int section_number = drivable_scenery_section->GetVisibleSection(i);
            if (section_number >= 0 && GetScenerySectionHeader(section_number) && num_sections_to_draw < max_sections) {
                sections[num_sections_to_draw] = static_cast<short>(section_number);
                num_sections_to_draw += 1;
            }
        }
    }

    if (cull_info->pView->GetID() == EVIEW_PLAYER1) {
        int current_zone_number = -1;
        if (drivable_scenery_section) {
            current_zone_number = drivable_scenery_section->GetSectionNumber();
        }

        if (current_zone_number != CurrentZoneNumber) {
            CurrentZoneNumber = current_zone_number;
            if (!SeeulatorToolActive) {
                return num_sections_to_draw;
            }
            if (drivable_scenery_section) {
                bFunkCallASync("Seeulator", 1, &CurrentZoneNumber, 4);
            }
        }

        if (SeeulatorToolActive) {
            if (ScenerySectionToBlink != 0 && ((RealTimeFrames / 5) & 1U) != 0) {
                num_sections_to_draw = ToggleIsInTable(sections, num_sections_to_draw, max_sections, ScenerySectionToBlink);
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
        if (sections_to_draw[i] >= 0) {
            ScenerySectionHeader *section_header = GetScenerySectionHeader(sections_to_draw[i]);
            if (section_header && reinterpret_cast<int *>(section_header)[10] != 0) {
                section_header->TreeCull(scenery_cull_info);
            }
        }
    }
}

void GrandSceneryCullInfo::DoCulling() {
    ProfileNode profile_node("TODO", 0);
    int n;
    pFirstDrawInfo = SceneryDrawInfoTable;
    pCurrentDrawInfo = SceneryDrawInfoTable;
    pTopDrawInfo = SceneryDrawInfoTable + 3500;

    for (n = 0; n < NumCullInfos; n++) {
        SceneryCullInfo *scenery_cull_info = &SceneryCullInfos[n];
        bool do_precull = true;

        scenery_cull_info->Position = *scenery_cull_info->pView->GetCamera()->GetPosition();
        scenery_cull_info->Direction = *scenery_cull_info->pView->GetCamera()->GetDirection();
        scenery_cull_info->H = scenery_cull_info->pView->H;

        if (PrecullerMode == 0) {
            do_precull = false;
        } else if (PrecullerMode == 2) {
            int time = RealTimeFrames % 0x3D;
            if (time < 0xF) {
                do_precull = false;
            }
        } else if (PrecullerMode == 3) {
            Camera *camera = scenery_cull_info->pView->GetCamera();
            float speed = bLength(reinterpret_cast<bVector3 *>(reinterpret_cast<unsigned char *>(camera) + 0x1E8));
            if (speed < EnablePrecullingSpeed) {
                do_precull = false;
            }
        }

        if (DisablePrecullerCounter > 0 && PrecullerMode != 2) {
            do_precull = false;
        }

        if (do_precull) {
            if (gPrecullerBooBooManager.IsSet(scenery_cull_info->Position)) {
                do_precull = false;
            }
        }
        scenery_cull_info->PrecullerSectionNumber = -1;
        if (do_precull) {
            scenery_cull_info->PrecullerSectionNumber = GetPrecullerSectionNumber(scenery_cull_info->Position.x, scenery_cull_info->Position.y);
        }
    }

    for (n = 0; n < NumCullInfos; n++) {
        SceneryCullInfo *scenery_cull_info = &SceneryCullInfos[n];
        scenery_cull_info->pFirstDrawInfo = pCurrentDrawInfo;
        scenery_cull_info->pCurrentDrawInfo = pCurrentDrawInfo;
        scenery_cull_info->pTopDrawInfo = pTopDrawInfo;
        CullView(scenery_cull_info);
        pCurrentDrawInfo = scenery_cull_info->pCurrentDrawInfo;
    }

    for (n = 0; n < NumCullInfos; n++) {
    }
}

void GrandSceneryCullInfo::StuffScenery(eView *view, int stuff_flags) {
    unsigned int base_flags = 0;
    unsigned int forbidden_flags = 0;
    unsigned int required_flags = 0;

    if ((stuff_flags & 1) != 0) {
        base_flags = 0x1000;
    }
    if ((stuff_flags & 0x400) != 0) {
        required_flags = 0x100000;
    }
    if ((stuff_flags & 0x80) != 0) {
        base_flags |= 0x100;
    }
    if ((stuff_flags & 0x10) != 0) {
        required_flags = 0x2000;
    } else if ((stuff_flags & 8) != 0) {
        forbidden_flags = 0x2000;
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

        for (SceneryDrawInfo *draw_info = scenery_cull_info->pFirstDrawInfo; draw_info < scenery_cull_info->pCurrentDrawInfo; draw_info++) {
            unsigned int model_word = reinterpret_cast<unsigned int>(draw_info->pModel);
            unsigned int model_type = model_word & 3;
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
            if (model_type == 2) {
                render_flags |= 4;
            }

            bool required_ok = required_flags == 0 || (render_flags & required_flags) != 0;
            bool forbidden_ok = forbidden_flags == 0 || (render_flags & forbidden_flags) == 0;
            if (required_ok && forbidden_ok) {
                bMatrix4 *matrix = draw_info->pMatrix;
                if (!matrix) {
                    reinterpret_cast<eViewPlatInterface *>(view)->Render(pDebugModel, &eMathIdentityMatrix, 0, render_flags, 0);
                } else {
                    reinterpret_cast<eViewPlatInterface *>(view)->Render(pDebugModel, matrix, 0, render_flags, 0);
                }
                pDebugModel = 0;
            }
        }
        return;
    }
}
void ServicePreculler() {}
