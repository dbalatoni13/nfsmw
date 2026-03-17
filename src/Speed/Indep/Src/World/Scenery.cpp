#include "Scenery.hpp"

#include "VisibleSection.hpp"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/Src/Ecstasy/eSolid.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct ScenerySectionHeader : public bNode {
    void DrawAScenery(int scenery_instance_number, SceneryCullInfo *scenery_cull_info, int visibility_state);
};

struct SceneryOverrideInfo {
    short SectionNumber;
    short OverrideSectionNumber;
    short OverrideIndex;

    void AssignOverrides(ScenerySectionHeader *section_header);
};

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

extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern float EnvMapShadowExtraHeight;
void CreateWindRotMatrix(eView *view, bMatrix4 *matrix, int x, const bMatrix4 *world, int y);

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

static char GetScenerySectionLetter_Scenery(int section_number) {
    return static_cast<char>(section_number / 100 + 'A' - 1);
}

static void EndianSwapSectionHeader_Scenery(int *section_header_words) {
    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0xC);
    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x10);
    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x14);
    bEndianSwap32(reinterpret_cast<char *>(section_header_words) + 0x38);
}

static void EndianSwapSceneryInfo_Scenery(unsigned char *data) {
    for (int i = 0; i < 4; i++) {
        bEndianSwap32(data + 0x18 + i * 4);
    }
    bEndianSwap32(data + 0x38);
    bEndianSwap32(data + 0x3C);
    bEndianSwap32(data + 0x40);
}

static void EndianSwapSceneryInstance_Scenery(SceneryInstance *instance) {
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

static void EndianSwapPrecullerInfo_Scenery(unsigned char *data) {
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

static SceneryOverrideInfo *FindMatchingOverrideInfo_Scenery(int section_number, int override_index) {
    for (int i = 0; i < NumSceneryOverrideInfos; i++) {
        SceneryOverrideInfo *override_info = &SceneryOverrideInfoTable[i];
        if (override_info->SectionNumber == section_number && override_info->OverrideIndex == override_index) {
            return override_info;
        }
    }
    return 0;
}

static eModel *FindExistingModel_Scenery(unsigned char *scenery_info, int model_slot) {
    unsigned int name_hash = *reinterpret_cast<unsigned int *>(scenery_info + 0x18 + model_slot * 4);
    for (int i = 0; i < model_slot; i++) {
        eModel *model = *reinterpret_cast<eModel **>(scenery_info + 0x28 + i * 4);
        if (model && model->NameHash == name_hash) {
            return model;
        }
    }
    return 0;
}

static unsigned char *GetSceneryInfo_Scenery(int *section_header_words, short scenery_info_number) {
    return reinterpret_cast<unsigned char *>(section_header_words[6]) + scenery_info_number * 0x48;
}

static eModel *GetSceneryModel_Scenery(unsigned char *scenery_info, int model_slot) {
    return *reinterpret_cast<eModel **>(scenery_info + 0x28 + model_slot * 4);
}

static float GetSceneryRadius_Scenery(unsigned char *scenery_info) {
    return *reinterpret_cast<float *>(scenery_info + 0x38);
}

void BuildSceneryOverrideHashTable() {}

unsigned int FindSceneryHeirarchyByName(unsigned int name_hash) {
    return name_hash;
}

void SceneryOverrideInfo::AssignOverrides(ScenerySectionHeader *section_header) {
    int *section_header_words = reinterpret_cast<int *>(section_header);
    SceneryInstance *instance = reinterpret_cast<SceneryInstance *>(section_header_words[8]) +
                                *reinterpret_cast<short *>(reinterpret_cast<char *>(this) + 2);
    unsigned short override_flags = *reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(this) + 4);

    if ((instance->ExcludeFlags & 0x800000) != 0 && ((instance->ExcludeFlags ^ override_flags) & 0x400) != 0) {
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

        instance->Rotation[0] = static_cast<short>(rotation.v0.x * 8192.0f);
        instance->Rotation[1] = static_cast<short>(rotation.v0.y * 8192.0f);
        instance->Rotation[2] = static_cast<short>(rotation.v0.z * 8192.0f);
        instance->Rotation[3] = static_cast<short>(rotation.v1.x * 8192.0f);
        instance->Rotation[4] = static_cast<short>(rotation.v1.y * 8192.0f);
        instance->Rotation[5] = static_cast<short>(rotation.v1.z * 8192.0f);
        instance->Rotation[6] = static_cast<short>(rotation.v2.x * 8192.0f);
        instance->Rotation[7] = static_cast<short>(rotation.v2.y * 8192.0f);
        instance->Rotation[8] = static_cast<short>(rotation.v2.z * 8192.0f);
    }

    instance->ExcludeFlags = (instance->ExcludeFlags & 0xFFFF0000) | override_flags;
}

void InitVisibleZones() {}

void CloseVisibleZones() {}

void ServicePreculler() {}

void ScenerySectionHeader::DrawAScenery(int scenery_instance_number, SceneryCullInfo *scenery_cull_info, int visibility_state) {
    int *section_header_words = reinterpret_cast<int *>(this);
    SceneryInstance *instances = reinterpret_cast<SceneryInstance *>(section_header_words[8]);
    int num_instances = section_header_words[9];

    if (!instances || scenery_instance_number < 0 || scenery_instance_number >= num_instances) {
        return;
    }

    SceneryInstance *instance = &instances[scenery_instance_number];
    if (((instance->ExcludeFlags ^ 0x60) & scenery_cull_info->ExcludeFlags) != 0) {
        return;
    }

    if (visibility_state >= 0 && section_header_words[12] && instance->PrecullerInfoIndex >= 0) {
        unsigned char *visibility_table = reinterpret_cast<unsigned char *>(section_header_words[12]) + instance->PrecullerInfoIndex * 0x80;
        if ((visibility_table[visibility_state >> 3] & (1 << (visibility_state & 7))) != 0) {
            return;
        }
    }

    unsigned char *scenery_info = GetSceneryInfo_Scenery(section_header_words, instance->SceneryInfoNumber);
    if (visibility_state == EVISIBLESTATE_PARTIAL) {
        bVector3 bbox_min(instance->BBoxMin[0], instance->BBoxMin[1], instance->BBoxMin[2]);
        bVector3 bbox_max(instance->BBoxMax[0], instance->BBoxMax[1], instance->BBoxMax[2]);
        visibility_state = scenery_cull_info->pView->GetVisibleState(&bbox_min, &bbox_max, 0);
        if (visibility_state == EVISIBLESTATE_NOT) {
            return;
        }
    }

    bVector3 to_instance(
        instance->Position[0] - scenery_cull_info->Position.x,
        instance->Position[1] - scenery_cull_info->Position.y,
        instance->Position[2] - scenery_cull_info->Position.z
    );
    float radius = GetSceneryRadius_Scenery(scenery_info) + 6.0f;
    float forward_distance = to_instance.x * scenery_cull_info->Direction.x + to_instance.y * scenery_cull_info->Direction.y +
                             to_instance.z * scenery_cull_info->Direction.z;
    if (forward_distance < -radius) {
        return;
    }

    float distance = bSqrt(to_instance.x * to_instance.x + to_instance.y * to_instance.y + to_instance.z * to_instance.z);
    float pixel_size = scenery_cull_info->H;
    if (radius < distance - radius) {
        pixel_size = (radius * pixel_size) / (distance - radius);
    }

    int pixel_size_int = static_cast<int>(pixel_size);
    if (pixel_size_int < 2) {
        return;
    }
    if ((instance->ExcludeFlags & 0x2000000) != 0) {
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
    } else if ((instance->ExcludeFlags & 0x80) == 0) {
        if (pixel_size_int > 0x1F) {
            if ((instance->ExcludeFlags & 0x1000100) == 0) {
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

int LoaderScenery(bChunk *chunk) {
    unsigned int chunk_id = chunk->GetID();

    if (chunk_id == 0x34108) {
        SceneryOverrideInfoTable = reinterpret_cast<SceneryOverrideInfo *>(chunk->GetData());
        NumSceneryOverrideInfos = chunk->Size / 6;
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
            switch (subchunk->GetID()) {
                case 0x34101: {
                    section_header = reinterpret_cast<ScenerySectionHeader *>(subchunk->GetAlignedData(0x10));
                    section_header_words = reinterpret_cast<int *>(section_header);
                    if (section_header_words[2] == 0) {
                        EndianSwapSectionHeader_Scenery(section_header_words);
                    }

                    VisibleSectionUserInfo *user_info = TheVisibleSectionManager.AllocateUserInfo(section_header_words[3]);
                    if (user_info) {
                        user_info->pScenerySectionHeader = section_header;
                    }

                    if (GetScenerySectionLetter_Scenery(section_header_words[3]) == 'Z') {
                        ScenerySectionHeaderList.AddHead(section_header);
                    } else {
                        ScenerySectionHeaderList.AddTail(section_header);
                    }
                    break;
                }

                case 0x34102: {
                    if (!section_header_words) {
                        break;
                    }

                    section_header_words[6] = reinterpret_cast<int>(subchunk->GetData());
                    section_header_words[7] = subchunk->Size / 0x48;
                    if (section_header_words[2] == 0) {
                        for (int i = 0; i < section_header_words[7]; i++) {
                            EndianSwapSceneryInfo_Scenery(reinterpret_cast<unsigned char *>(section_header_words[6] + i * 0x48));
                        }
                    }

                    for (int i = 0; i < section_header_words[7]; i++) {
                        unsigned char *scenery_info = reinterpret_cast<unsigned char *>(section_header_words[6] + i * 0x48);
                        unsigned int hierarchy_name = *reinterpret_cast<unsigned int *>(scenery_info + 0x40);
                        if (hierarchy_name != 0) {
                            *reinterpret_cast<unsigned int *>(scenery_info + 0x44) = FindSceneryHeirarchyByName(hierarchy_name);
                        }
                    }
                    break;
                }

                case 0x34103: {
                    if (!section_header_words) {
                        break;
                    }

                    section_header_words[8] = reinterpret_cast<int>(subchunk->GetAlignedData(0x10));
                    section_header_words[9] = subchunk->GetAlignedSize(0x10) / sizeof(SceneryInstance);
                    if (section_header_words[2] == 0) {
                        SceneryInstance *instances = reinterpret_cast<SceneryInstance *>(section_header_words[8]);
                        for (int i = 0; i < section_header_words[9]; i++) {
                            EndianSwapSceneryInstance_Scenery(&instances[i]);
                        }
                    }
                    break;
                }

                case 0x34105: {
                    if (!section_header_words) {
                        break;
                    }

                    section_header_words[10] = reinterpret_cast<int>(subchunk->GetData());
                    section_header_words[11] = subchunk->Size / 0x24;
                    if (section_header_words[2] == 0) {
                        for (int i = 0; i < section_header_words[11]; i++) {
                            EndianSwapPrecullerInfo_Scenery(reinterpret_cast<unsigned char *>(section_header_words[10] + i * 0x24));
                        }
                    }
                    break;
                }

                case 0x34106: {
                    if (!section_header_words) {
                        break;
                    }

                    int num_overrides = subchunk->Size >> 2;
                    for (int i = 0; i < num_overrides; i++) {
                        unsigned char *override_data = reinterpret_cast<unsigned char *>(subchunk->GetData()) + i * 4;
                        bEndianSwap16(override_data + 0);
                        bEndianSwap16(override_data + 2);
                        SceneryOverrideInfo *override_info =
                            FindMatchingOverrideInfo_Scenery(section_header_words[3], *reinterpret_cast<unsigned short *>(override_data));
                        if (override_info) {
                            override_info->AssignOverrides(section_header);
                        }
                    }
                    break;
                }

                case 0x34107: {
                    if (!section_header_words) {
                        break;
                    }

                    section_header_words[12] = reinterpret_cast<int>(subchunk->GetData());
                    section_header_words[13] = subchunk->Size >> 7;
                    break;
                }

                default:
                    break;
            }
        }

        if (ChunkMovementOffset == 0 && section_header_words) {
            unsigned char *scenery_infos = reinterpret_cast<unsigned char *>(section_header_words[6]);
            for (int i = 0; i < section_header_words[7]; i++) {
                unsigned char *scenery_info = scenery_infos + i * 0x48;
                for (int j = 0; j < 4; j++) {
                    unsigned int name_hash = *reinterpret_cast<unsigned int *>(scenery_info + 0x18 + j * 4);
                    if (name_hash != 0 && name_hash != 0xBE43EDBB && name_hash != 0x90F70174) {
                        eModel *model = FindExistingModel_Scenery(scenery_info, j);
                        if (!model) {
                            model = reinterpret_cast<eModel *>(bOMalloc(eModelSlotPool));
                            if (model) {
                                model->Solid = 0;
                                model->pReplacementTextureTable = 0;
                                model->NumReplacementTextures = 0;
                                model->Init(name_hash);
                                if (ModelConnectionCallback) {
                                    ModelConnectionCallback(section_header, i, model);
                                }
                            }
                        }
                        *reinterpret_cast<eModel **>(scenery_info + 0x28 + j * 4) = model;
                    }
                }

                if (*reinterpret_cast<eModel **>(scenery_info + 0x30) == 0 && *reinterpret_cast<eModel **>(scenery_info + 0x2C) != 0) {
                    *reinterpret_cast<eModel **>(scenery_info + 0x30) = *reinterpret_cast<eModel **>(scenery_info + 0x2C);
                }
                if (*reinterpret_cast<eModel **>(scenery_info + 0x28) == 0 && *reinterpret_cast<eModel **>(scenery_info + 0x2C) != 0) {
                    *reinterpret_cast<eModel **>(scenery_info + 0x28) = *reinterpret_cast<eModel **>(scenery_info + 0x2C);
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
        int *section_header_words = reinterpret_cast<int *>(chunk->GetAlignedData(0x10));
        VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(section_header_words[3]);
        if (user_info) {
            user_info->pScenerySectionHeader = 0;
        }
        TheVisibleSectionManager.UnallocateUserInfo(section_header_words[3]);
        reinterpret_cast<bNode *>(section_header_words)->Remove();

        if (ChunkMovementOffset == 0) {
            unsigned char *scenery_infos = reinterpret_cast<unsigned char *>(section_header_words[6]);
            for (int i = 0; i < section_header_words[7]; i++) {
                unsigned char *scenery_info = scenery_infos + i * 0x48;
                for (int j = 0; j < 4; j++) {
                    eModel **model_slot = reinterpret_cast<eModel **>(scenery_info + 0x28 + j * 4);
                    eModel *model = *model_slot;
                    if (model) {
                        for (int k = j + 1; k < 4; k++) {
                            eModel **duplicate_slot = reinterpret_cast<eModel **>(scenery_info + 0x28 + k * 4);
                            if (*duplicate_slot == model) {
                                *duplicate_slot = 0;
                            }
                        }
                        if (ModelDisconnectionCallback) {
                            ModelDisconnectionCallback(reinterpret_cast<ScenerySectionHeader *>(section_header_words), i, model);
                        }
                        model->UnInit();
                        bFree(eModelSlotPool, model);
                        *model_slot = 0;
                    }
                }
            }

            if (SectionDisconnectionCallback) {
                SectionDisconnectionCallback(reinterpret_cast<ScenerySectionHeader *>(section_header_words));
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

    if (chunk_id == 0x8003410B) {
        return 1;
    }

    return 0;
}
