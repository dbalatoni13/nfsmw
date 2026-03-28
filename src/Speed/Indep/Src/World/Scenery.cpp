#include "Scenery.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "VisibleSection.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/Track.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/bWare/Inc/SpeedScript.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"

int LoaderSceneryGroup(bChunk *chunk);
int UnloaderSceneryGroup(bChunk *chunk);
int LoaderScenery(bChunk *chunk);
int UnloaderScenery(bChunk *chunk);
ScenerySectionHeader *GetScenerySectionHeader(int section_number);

// total size: 0x10
class HashToHeirarchyMap : public UTL::Std::map<unsigned int, ModelHeirarchy *, _type_map> {};

class PrecullerBooBooManager {
  public:
    void Reset() {
        bMemSet(this, 0, 0x800);
    }

    void Set(bVector3 &pos) {
        int n = GetSectionNumber(pos);
        unsigned char *p = GetByte(n);
        *p |= GetBit(n);
    }

    void Clr(bVector3 &pos) {
        int n = GetSectionNumber(pos);
        unsigned char *p = GetByte(n);
        *p &= ~0 - GetBit(n);
    }

    bool IsSet(bVector3 &pos) {
        int n = GetSectionNumber(pos);
        unsigned char *p = GetByte(n);
        return (*p & GetBit(n)) != 0;
    }

    int GetSectionNumber(bVector3 &position) {
        int nx = (static_cast<unsigned int>(static_cast<int>(position.x)) >> 5) & 0x7F;
        int ny = (static_cast<int>(position.y) & 0xFE0) << 2;
        return ny | nx;
    }

  private:
    unsigned char *GetByte(int section_number) {
        return &BitField[section_number >> 3];
    }

    unsigned char GetBit(int section_number) {
        return static_cast<unsigned char>(1 << (section_number & 7));
    }

    unsigned char BitField[0x800];
};

// TODO move these?
extern float EnvMapShadowExtraHeight;
extern int CurrentZoneNumber;
extern int SeeulatorToolActive;
extern int ScenerySectionToBlink;
extern int SeeulatorRefreshTrackStreamer;
extern int ShowSectionBoarder;

bTList<ScenerySectionHeader> ScenerySectionHeaderList;
HashToHeirarchyMap HeirarchyMap;
bTList<SceneryGroup> SceneryGroupList;

bChunkLoader bChunkLoaderSceneryGroup(BCHUNK_SCENERY_BARRIER_GROUPS, LoaderSceneryGroup, UnloaderSceneryGroup);
bChunkLoader bChunkLoaderScenerySection(BCHUNK_SCENERY_SECTION, LoaderScenery, UnloaderScenery);
bChunkLoader bChunkLoaderOverrideInfos(BCHUNK_SCENERY_OVERRIDE_INFOS, LoaderScenery, UnloaderScenery);
bChunkLoader bChunkLoaderSceneryHeirarchy(BCHUNK_MODEL_HIERARCHY_TREE, LoaderScenery, UnloaderScenery);
bChunkLoader bChunkLoaderSceneryLighting(BCHUNK_SCENERY_LIGHT_CONTEXTS, LoaderScenery, UnloaderScenery);

static const float EnablePrecullingSpeed = MPH2MPS(40.0f);

SceneryDetailLevel ForceAllSceneryDetailLevels = SCENERY_DETAIL_NONE;
void (*ModelConnectionCallback)(ScenerySectionHeader *, int, eModel *) = nullptr;
void (*ModelDisconnectionCallback)(ScenerySectionHeader *, int, eModel *) = nullptr;
void (*SectionConnectionCallback)(ScenerySectionHeader *) = nullptr;
void (*SectionDisconnectionCallback)(ScenerySectionHeader *) = nullptr;
eLight *LightTable = nullptr;
int MaxSceneryLightContexts = 0;
eSceneryLightContext **SceneryLightContextTable = nullptr;
SceneryOverrideInfo *SceneryOverrideInfoTable = nullptr;
int NumSceneryOverrideInfos = 0;
signed char SceneryGroupEnabledTable[4096] = {1};
eModel *pVisibleZoneBoundaryModel = nullptr;
int PrecullerMode = 1;
int DisablePrecullerCounter = 0;
eModel *pDebugModel = nullptr;

RegionQuery RegionInfo;
short SceneryOverrideHashTable[257];
PrecullerBooBooManager gPrecullerBooBooManager;
SceneryDrawInfo GrandSceneryCullInfo::SceneryDrawInfoTable[3500];

void BuildSceneryOverrideHashTable() {
    int current_offset = 0;
    for (int hash = 0; hash < 256; hash++) {
        SceneryOverrideHashTable[hash] = static_cast<short>(current_offset);
        while (current_offset < NumSceneryOverrideInfos && (SceneryOverrideInfoTable[current_offset].GetHashIndex() & 0xFF) == hash) {
            current_offset++;
        }
    }
    SceneryOverrideHashTable[256] = static_cast<short>(current_offset);
}

ModelHeirarchy *FindSceneryHeirarchyByName(unsigned int name_hash) {
    HashToHeirarchyMap::iterator iter = HeirarchyMap.find(name_hash);
    if (iter != HeirarchyMap.end()) {
        return (*iter).second;
    }
    return nullptr;
}

SceneryOverrideInfo *GetSceneryOverrideInfo(int override_info_number) {
    return &SceneryOverrideInfoTable[override_info_number];
}

void SceneryOverrideInfo::AssignOverrides(ScenerySectionHeader *section_header) {
    SceneryInstance *scenery_instance = section_header->GetSceneryInstance(InstanceNumber);

    if ((scenery_instance->ExcludeFlags & 0x800000) != 0 && ((scenery_instance->ExcludeFlags ^ ExcludeFlags) & 0x400) != 0) {
        bMatrix4 matrix;

        scenery_instance->GetRotation(&matrix);
        bFill(&matrix.v3, 0.0f, 0.0f, 0.0f, 1.0f);
        // TODO what did they actually do with that bDegToAng?
        if (bDegToAng(1.0f) > 0.0f) {
            bMatrix4 flip_matrix;
            bIdentity(&flip_matrix);
            flip_matrix.v0.x = -1.0f;
            bMulMatrix(&matrix, &matrix, &flip_matrix);
        }
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
        char *region;
        if (bStrICmp(script.GetNextArgumentString(), TrackInfo::GetLoadedTrackInfo()->RegionName) == 0) {
            char *section = script.GetNextArgumentString();
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
    LoadPrecullerBooBooScript("TRACKS\\PrecullerBooBooScript.hoo", true);
}

int LoaderSceneryGroup(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_SCENERY_BARRIER_GROUPS) {
        char *table = chunk->GetData();
        int size = chunk->GetSize();
        int pos = 0;

        while (pos < size) {
            SceneryGroup *group = reinterpret_cast<SceneryGroup *>(&table[pos]);
            SceneryGroupList.AddTail(group);

            group->EndianSwap();

            pos += group->GetMemoryImageSize();
        }
        return 1;
    }

    return 0;
}

int UnloaderSceneryGroup(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_SCENERY_BARRIER_GROUPS) {
        bMemSet(SceneryGroupEnabledTable, 0, sizeof(SceneryGroupEnabledTable));
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

void EnableSceneryGroup(unsigned int group_name_hash, bool flip_artwork) {
    SceneryGroup *group = static_cast<SceneryGroup *>(FindSceneryGroup(group_name_hash));
    if (group) {
        group->EnableRendering(flip_artwork);
    }
}

void DisableSceneryGroup(unsigned int name_hash) {
    SceneryGroup *group = FindSceneryGroup(name_hash);
    if (group) {
        group->DisableRendering();
        SceneryGroupEnabledTable[group->GroupNumber] = 0;
    }
}

void DisableAllSceneryGroups() {
    for (SceneryGroup *group = SceneryGroupList.GetHead(); group != SceneryGroupList.EndOfList(); group = group->GetNext()) {
        if (IsSceneryGroupEnabled(group->GroupNumber)) {
            group->DisableRendering();
            SceneryGroupEnabledTable[group->GroupNumber] = 0;
        }
    }
}

ScenerySectionHeader *GetScenerySectionHeader(int section_number) {
    VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(section_number);
    if (user_info) {
        return user_info->pScenerySectionHeader;
    }
    return nullptr;
}

int LoaderScenery(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_SCENERY_OVERRIDE_INFOS) {
        SceneryOverrideInfoTable = reinterpret_cast<SceneryOverrideInfo *>(chunk->GetData());
        NumSceneryOverrideInfos = chunk->GetSize() / sizeof(SceneryOverrideInfo);
        for (int n = 0; n < NumSceneryOverrideInfos; n++) {
            SceneryOverrideInfo *override_info = &SceneryOverrideInfoTable[n];
            override_info->EndianSwap();
        }
        BuildSceneryOverrideHashTable();
        return 1;
    }

    if (chunk->GetID() == BCHUNK_SCENERY_SECTION) {
        ScenerySectionHeader *section_header = nullptr;
        bChunk *first_chunk = chunk->GetFirstChunk();
        bChunk *last_chunk = chunk->GetLastChunk();
        int num_emodels;
        int num_new_emodels;

        for (bChunk *chunk = first_chunk; chunk != last_chunk; chunk = chunk->GetNext()) {
            if (chunk->GetID() == BCHUNK_SCENERY_SECTION_HEADER) {
                section_header = reinterpret_cast<ScenerySectionHeader *>(chunk->GetAlignedData(16));
                if (!section_header->ChunksLoaded) {
                    bPlatEndianSwap(&section_header->SectionNumber);
                    bPlatEndianSwap(&section_header->NumPolygonsInMemory);
                    bPlatEndianSwap(&section_header->NumPolygonsInWorld);
                    bPlatEndianSwap(&section_header->ViewsVisibleThisFrame);
                }

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.AllocateUserInfo(section_header->SectionNumber);
                user_info->pScenerySectionHeader = section_header;

                if (GetScenerySectionLetter(section_header->SectionNumber) == 'Z') {
                    ScenerySectionHeaderList.AddHead(section_header);
                } else {
                    ScenerySectionHeaderList.AddTail(section_header);
                }
            } else if (chunk->GetID() == BCHUNK_SCENERY_INFOS) {
                section_header->pSceneryInfo = reinterpret_cast<SceneryInfo *>(chunk->GetData());
                section_header->NumSceneryInfo = chunk->GetSize() / sizeof(SceneryInfo);
                if (!section_header->ChunksLoaded) {
                    for (int i = 0; i < section_header->NumSceneryInfo; i++) {
                        bPlatEndianSwap(&section_header->pSceneryInfo[i].mHeirarchyNameHash);
                        for (int j = 0; j < 4; j++) {
                            bPlatEndianSwap(&section_header->pSceneryInfo[i].NameHash[j]);
                        }
                        bPlatEndianSwap(&section_header->pSceneryInfo[i].Radius);
                        bPlatEndianSwap(&section_header->pSceneryInfo[i].MeshChecksum);
                    }
                }

                for (int i = 0; i < section_header->NumSceneryInfo; i++) {
                    SceneryInfo *scenery_info = &section_header->pSceneryInfo[i];
                    if (scenery_info->mHeirarchyNameHash != 0) {
                        scenery_info->mHeirarchy = FindSceneryHeirarchyByName(scenery_info->mHeirarchyNameHash);
                    }
                }
            } else if (chunk->GetID() == BCHUNK_SCENERY_INSTANCES) {
                section_header->pSceneryInstance = reinterpret_cast<SceneryInstance *>(chunk->GetAlignedData(16));
                section_header->NumSceneryInstances = chunk->GetAlignedSize(16) / sizeof(SceneryInstance);
                if (!section_header->ChunksLoaded) {
                    for (int i = 0; i < section_header->NumSceneryInstances; i++) {
                        SceneryInstance *scenery_instance = &section_header->pSceneryInstance[i];
                        bPlatEndianSwap(&scenery_instance->SceneryInfoNumber);
                        bPlatEndianSwap(&scenery_instance->ExcludeFlags);
                        int j;
                        for (j = 0; j < 3; j++) {
                            bPlatEndianSwap(&scenery_instance->Position[j]);
                        }
                        for (j = 0; j < 9; j++) {
                            bPlatEndianSwap(&scenery_instance->Rotation[j]);
                        }
                        bPlatEndianSwap(&scenery_instance->BBoxMin[0]);
                        bPlatEndianSwap(&scenery_instance->BBoxMin[1]);
                        bPlatEndianSwap(&scenery_instance->BBoxMin[2]);
                        bPlatEndianSwap(&scenery_instance->BBoxMax[0]);
                        bPlatEndianSwap(&scenery_instance->BBoxMax[1]);
                        bPlatEndianSwap(&scenery_instance->BBoxMax[2]);
                        bPlatEndianSwap(&scenery_instance->PrecullerInfoIndex);
                        bPlatEndianSwap(&scenery_instance->LightingContextNumber);
                    }
                }
            } else if (chunk->GetID() == BCHUNK_SCENERY_TREE_NODES) {
                section_header->SceneryTreeNodeTable = reinterpret_cast<SceneryTreeNode *>(chunk->GetData());
                section_header->NumSceneryTreeNodes = chunk->GetSize() / sizeof(SceneryTreeNode);
                if (!section_header->ChunksLoaded) {
                    for (int i = 0; i < section_header->NumSceneryTreeNodes; i++) {
                        bPlatEndianSwap(&section_header->SceneryTreeNodeTable[i].NumChildren);
                        bPlatEndianSwap(&section_header->SceneryTreeNodeTable[i].BBoxMin[0]);
                        bPlatEndianSwap(&section_header->SceneryTreeNodeTable[i].BBoxMin[1]);
                        bPlatEndianSwap(&section_header->SceneryTreeNodeTable[i].BBoxMin[2]);
                        bPlatEndianSwap(&section_header->SceneryTreeNodeTable[i].BBoxMax[0]);
                        bPlatEndianSwap(&section_header->SceneryTreeNodeTable[i].BBoxMax[1]);
                        bPlatEndianSwap(&section_header->SceneryTreeNodeTable[i].BBoxMax[2]);

                        for (int j = 0; j < 5; j++) {
                            bPlatEndianSwap(&section_header->SceneryTreeNodeTable[i].ChildCodes[j]);
                        }
                    }
                }
            } else if (chunk->GetID() == BCHUNK_SCENERY_OVERRIDE_HOOKS) {
                SceneryOverrideInfoHookup *hookup_table = reinterpret_cast<SceneryOverrideInfoHookup *>(chunk->GetData());
                int num_hookups = chunk->GetSize() / sizeof(SceneryOverrideInfoHookup);
                for (int n = 0; n < num_hookups; n++) {
                    SceneryOverrideInfoHookup *hookup = &hookup_table[n];
                    hookup->EndianSwap();

                    SceneryOverrideInfo *override_info = &SceneryOverrideInfoTable[hookup->OverrideInfoNumber];
                    if (override_info->InstanceNumber == hookup->InstanceNumber &&
                        override_info->SectionNumber == section_header->GetSectionNumber()) {
                        override_info->AssignOverrides(section_header);
                    }
                }
            } else if (chunk->GetID() == BCHUNK_PRECULLER_INFOS) {
                section_header->PrecullerInfoTable = reinterpret_cast<tPrecullerInfo *>(chunk->GetData());
                section_header->NumPrecullerInfos = chunk->GetSize() / sizeof(tPrecullerInfo);
                if (!section_header->ChunksLoaded) {
                    for (int n = 0; n < section_header->NumPrecullerInfos; n++) {
                        tPrecullerInfo *preculler_info = section_header->GetPrecullerInfo(n);
                        preculler_info->EndianSwap();
                    }
                }
            }
        }

        if (!AreChunksBeingMoved()) {
            for (int n = 0; n < section_header->NumSceneryInfo; n++) {
                SceneryInfo *scenery_info = &section_header->pSceneryInfo[n];
                eModel *lowest_detail_model;
                for (int detail_level = 0; detail_level < 4; detail_level++) {
                    unsigned int name_hash = scenery_info->NameHash[detail_level];
                    // TODO magic
                    if (name_hash != 0 && name_hash != 0xBE43EDBB && name_hash != 0x90F70174) {
                        eModel *model = nullptr;
                        for (int i = 0; i < detail_level; i++) {
                            if (scenery_info->pModel[i] && scenery_info->pModel[i]->GetNameHash() == name_hash) {
                                model = scenery_info->pModel[i];
                                break;
                            }
                        }

                        if (!model) {
                            model = new eModel(scenery_info->NameHash[detail_level]);
                            if (ModelConnectionCallback) {
                                ModelConnectionCallback(section_header, n, model);
                            }
                        }
                        scenery_info->pModel[detail_level] = model;
                    }
                }

                if (!scenery_info->pModel[2] && scenery_info->pModel[1]) {
                    scenery_info->pModel[2] = scenery_info->pModel[1];
                }
                if (!scenery_info->pModel[0] && scenery_info->pModel[1]) {
                    scenery_info->pModel[0] = scenery_info->pModel[1];
                }
            }

            if (SectionConnectionCallback) {
                SectionConnectionCallback(section_header);
            }
        }

        section_header->ChunksLoaded = true;
        return 1;
    }

    if (chunk->GetID() == BCHUNK_MODEL_HIERARCHY_TREE) {
        bChunk *last_chunk = chunk->GetLastChunk();
        for (chunk = chunk->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
            ModelHeirarchy *mH = reinterpret_cast<ModelHeirarchy *>(chunk->GetData());
            bPlatEndianSwap(&mH->mNameHash);
            bPlatEndianSwap(&mH->mNumNodes);
            bPlatEndianSwap(&mH->mFlags);

            ModelHeirarchy::Node *node = mH->GetNodes();
            for (unsigned int i = 0; i < mH->mNumNodes; i++) {
                bPlatEndianSwap(&node[i].mNodeName);
                bPlatEndianSwap(&node[i].mFlags);
                bPlatEndianSwap(&node[i].mModelHash);
                bPlatEndianSwap(&node[i].mParent);
                bPlatEndianSwap(&node[i].mNumChildren);
                bPlatEndianSwap(&node[i].mChildIndex);
            }

            HeirarchyMap[mH->mNameHash] = mH;
            for (unsigned int i = 0; i < mH->mNumNodes; i++) {
                node[i].mModel = node[i].mModelHash != 0 ? new eModel(node[i].mModelHash) : nullptr;
            }
        }
        return 1;
    }

    // TODO magic
    if (chunk->GetID() == BCHUNK_SCENERY_LIGHT_CONTEXTS) {
        int numSceneryLightContexts;
        bChunk *last_chunk = chunk->GetLastChunk();
        for (chunk = chunk->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
            if (chunk->GetID() == 0x34116) {
                LightTable = reinterpret_cast<eLight *>(chunk->GetData());
            } else if (chunk->GetID() == 0x34117) {
                SceneryLightContextTable = reinterpret_cast<eSceneryLightContext **>(chunk->GetData());
                MaxSceneryLightContexts = chunk->GetSize() / static_cast<int>(sizeof(eSceneryLightContext *));
            } else if (chunk->GetID() == 0x34118) {
                eSceneryLightContext *sceneryLightContext = reinterpret_cast<eSceneryLightContext *>(chunk->GetAlignedData(16));
                sceneryLightContext->LocalLights = reinterpret_cast<bMatrix4 *>(&sceneryLightContext[1]);
                sceneryLightContext->EndianSwap();
                SceneryLightContextTable[sceneryLightContext->LightingContextNumber] = sceneryLightContext;
            }
        }
        return 1;
    }

    return 0;
}

int UnloaderScenery(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_SCENERY_OVERRIDE_INFOS) {
        SceneryOverrideInfoTable = nullptr;
        NumSceneryOverrideInfos = 0;
        BuildSceneryOverrideHashTable();
        return 1;
    }

    if (chunk->GetID() == BCHUNK_SCENERY_SECTION) {
        ScenerySectionHeader *section_header = reinterpret_cast<ScenerySectionHeader *>(chunk->GetFirstChunk()->GetAlignedData(16));
        VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(section_header->SectionNumber);
        user_info->pScenerySectionHeader = nullptr;
        TheVisibleSectionManager.UnallocateUserInfo(section_header->SectionNumber);
        ScenerySectionHeaderList.Remove(section_header);

        if (!AreChunksBeingMoved()) {
            for (int n = 0; n < section_header->NumSceneryInfo; n++) {
                SceneryInfo *scenery_info = &section_header->pSceneryInfo[n];
                for (int detail_level = 0; detail_level < 4; detail_level++) {
                    if (AreChunksBeingMoved()) {
                        break;
                    }

                    if (scenery_info->pModel[detail_level]) {
                        for (int i = detail_level + 1; i < 4; i++) {
                            if (scenery_info->pModel[i] == scenery_info->pModel[detail_level]) {
                                scenery_info->pModel[i] = nullptr;
                            }
                        }
                        if (ModelDisconnectionCallback) {
                            ModelDisconnectionCallback(section_header, n, scenery_info->pModel[detail_level]);
                        }

                        delete scenery_info->pModel[detail_level];
                        scenery_info->pModel[detail_level] = nullptr;
                    }
                }
            }

            if (SectionDisconnectionCallback) {
                SectionDisconnectionCallback(section_header);
            }
        }

        return 1;
    }

    if (chunk->GetID() == BCHUNK_MODEL_HIERARCHY_TREE) {
        bChunk *last_chunk = chunk->GetLastChunk();
        for (chunk = chunk->GetFirstChunk(); chunk < last_chunk; chunk = chunk->GetNext()) {
            ModelHeirarchy *mH = reinterpret_cast<ModelHeirarchy *>(chunk->GetData());
            ModelHeirarchy::Node *node = mH->GetNodes();

            for (unsigned int i = 0; i < mH->mNumNodes; i++) {
                if (node[i].mModel) {
                    delete node[i].mModel;
                    node[i].mModel = nullptr;
                }
            }

            HashToHeirarchyMap::iterator iter = HeirarchyMap.find(mH->mNameHash);
            HeirarchyMap.erase(iter);
        }
        return 1;
    }

    if (chunk->GetID() == BCHUNK_SCENERY_LIGHT_CONTEXTS) {
        MaxSceneryLightContexts = 0;
        LightTable = nullptr;
        SceneryLightContextTable = nullptr;
        return 1;
    }

    return 0;
}

SceneryInfo *FindSceneryInfo(unsigned int name_hash) {
    for (ScenerySectionHeader *section_header = ScenerySectionHeaderList.GetHead(); section_header != ScenerySectionHeaderList.EndOfList();
         section_header = section_header->GetNext()) {
        for (int scenery_number = 0; scenery_number < section_header->NumSceneryInfo; scenery_number++) {
            SceneryInfo *scenery_info = &section_header->pSceneryInfo[scenery_number];
            if (scenery_info->pModel[0] && scenery_info->pModel[0]->GetNameHash() == name_hash) {
                return scenery_info;
            }
        }
    }
    return 0;
}

SceneryInstance *FindSceneryInstance(unsigned int name_hash) {
    for (ScenerySectionHeader *section_header = ScenerySectionHeaderList.GetHead(); section_header != ScenerySectionHeaderList.EndOfList();
         section_header = section_header->GetNext()) {
        int scenery_instance_number;
        for (scenery_instance_number = 0; scenery_instance_number < section_header->NumSceneryInstances; scenery_instance_number++) {
            SceneryInstance *scenery_instance = &section_header->pSceneryInstance[scenery_instance_number];
            SceneryInfo *scenery_info = section_header->GetSceneryInfo(scenery_instance->SceneryInfoNumber);

            if (scenery_info->pModel[0] && scenery_info->pModel[0]->GetNameHash() == name_hash) {
                return scenery_instance;
            }
        }
    }
    return 0;
}

// UNSOLVED
void RenderVisibleSectionBoundary(VisibleSectionBoundary *boundary, eView *view) {
    float perimeter;
    for (int n = 0; n < boundary->GetNumPoints(); n++) {
        perimeter = bDistBetween(boundary->GetPoint(n), boundary->GetPoint((n + 1) % boundary->GetNumPoints()));
    }

    bVector3 position;
    TopologyCoordinate topology_coordinate;
    float pos = static_cast<float>((static_cast<int>(WorldTimer.GetSeconds() * 262144.0f) & 0xffff)) * 6.103515625e-05f;

    int point_number;
    for (point_number = 0; point_number < boundary->GetNumPoints(); point_number++) {
        bVector2 normal = *boundary->GetPoint((point_number + 1) % boundary->GetNumPoints()) - *boundary->GetPoint(point_number);
        float length = bLength(&normal);

        bNormalize(&normal, &normal);
        while (pos < length) {
            bScaleAdd(reinterpret_cast<bVector2 *>(&position), boundary->GetPoint(point_number), &normal, pos);

            if (topology_coordinate.HasTopology(reinterpret_cast<bVector2 *>(&position))) {
                position.z = 9999.0f;
                position.z = topology_coordinate.GetElevation(&position, nullptr, nullptr, nullptr);
                int pixel_size = view->GetPixelSize(&position, 1.0f);
                if (pixel_size > 0) {
                    bMatrix4 *matrix = eFrameMallocMatrix(1);
                    if (matrix) {
                        bIdentity(matrix);
                        bCopy(&matrix->v3, &position, 1.0f);
                        view->Render(pVisibleZoneBoundaryModel, matrix, nullptr, 0, nullptr);
                    }
                }
            }

            pos += 4.0f;
        }

        pos -= length;
    }
}

void RenderVisibleZones(eView *view) {
    if (ShowSectionBoarder && pVisibleZoneBoundaryModel) {
        DrivableScenerySection *scenery_section =
            TheVisibleSectionManager.FindDrivableSection(reinterpret_cast<const bVector2 *>(view->GetCamera()->GetPosition()));
        if (scenery_section) {
            RenderVisibleSectionBoundary(scenery_section->GetBoundary(), view);
        }
    }
}

void InitVisibleZones() {
    if (!pVisibleZoneBoundaryModel) {
        pVisibleZoneBoundaryModel = new eModel(bStringHash("MARKER_BOUNDARY"));
    }
}

void CloseVisibleZones() {
    delete pVisibleZoneBoundaryModel;
    pVisibleZoneBoundaryModel = nullptr;
    if (SeeulatorToolActive) {
        unsigned int address = 0;
        bFunkCallASync("Seeulator", 4, &address, 4);
        bFunkCallASync("Seeulator", 5, &address, 4);
        bFunkCallASync("Seeulator", 6, &address, 4);
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

int ToggleIsInTable(short *table, int num_entries, int max_entries, int entry) {
    int index = IsInTable(table, num_entries, entry);

    if (index < 0) {
        table[num_entries % max_entries] = static_cast<short>(entry);
        num_entries++;
    } else {
        table[index] = -1;
    }

    return num_entries;
}

void ScenerySectionHeader::DrawAScenery(int scenery_instance_number, SceneryCullInfo *scenery_cull_info, int visibility_state) {
    SceneryInstance *scenery_instance = &pSceneryInstance[scenery_instance_number];
    tPrecullerInfo *preculler_info = GetPrecullerInfo(scenery_instance->PrecullerInfoIndex);
    if (scenery_cull_info->PrecullerSectionNumber >= 0) {
        if (!preculler_info->IsVisible(scenery_cull_info->PrecullerSectionNumber)) {
            return;
        }
    }

    SceneryInfo *my_scenery_info = &pSceneryInfo[scenery_instance->SceneryInfoNumber];
    static bool bPrintName = false;
    int exclude_bits = scenery_instance->ExcludeFlags & 0xFF;
    int exclude_mask = 0x60;
    int exclude_flagxor = exclude_bits ^ exclude_mask;
    if (exclude_flagxor & scenery_cull_info->ExcludeFlags) {
        return;
    }
    SceneryInfo *scenery_info = &pSceneryInfo[scenery_instance->SceneryInfoNumber];

    if (bPrintName) {
    }

    if (visibility_state == EVISIBLESTATE_PARTIAL) {
        bVector3 bbox_min;
        bVector3 bbox_max;
        scenery_instance->GetBBox(&bbox_min, &bbox_max);
        visibility_state = scenery_cull_info->pView->GetVisibleState(&bbox_min, &bbox_max, 0);
        if (visibility_state == EVISIBLESTATE_NOT) {
            return;
        }
    }

    bVector3 *position = scenery_instance->GetPosition();
    int pixel_size = InlinedViewGetPixelSize(scenery_cull_info, position, scenery_info->Radius + 6.0f);

    if (pixel_size < 2) {
        return;
    }
    if (scenery_instance->ExcludeFlags & 0x2000000) {
        pixel_size += 10;
    }

    eModel *model = nullptr;
    if (scenery_cull_info->ExcludeFlags & 0x1800) {
        int far_clip_size = 0x20;
        if (scenery_instance->ExcludeFlags & 0x80) {
            if (pixel_size >= far_clip_size) {
                model = scenery_info->pModel[2];
            }
        } else {
            if (pixel_size >= far_clip_size) {
                if (scenery_instance->ExcludeFlags & 0x1000100) {
                    model = scenery_info->pModel[0];
                } else {
                    model = scenery_info->pModel[3];
                }
            }
        }
    } else if (scenery_cull_info->ExcludeFlags & 0x20) {
        int far_clip_size = 0x20;
        if (pixel_size >= far_clip_size) {
            model = scenery_info->pModel[2];
        }
    } else if (eGetCurrentViewMode() > EVIEWMODE_ONE_RVM) {
        if (pixel_size > 0x16) {
            model = scenery_info->pModel[2];
        }
    } else if (pixel_size > 0x11) {
        float density = 8.7f;

        if (scenery_info->pModel[0]) {
            eSolid *solid = scenery_info->pModel[0] ? scenery_info->pModel[0]->GetSolid() : nullptr;
            if (solid && solid->NumPolys > 39) {
                float ratio = solid->Density;
                if (ratio < 6.0f) {
                    ratio = 6.0f;
                }
                density = static_cast<float>(pixel_size) / ratio;
            }
        }
        if (density < 8.7f) {
            model = scenery_info->pModel[2];
        } else {
            model = scenery_info->pModel[0];
        }
    }

    if (!model) {
        return;
    }

    if (scenery_instance->ExcludeFlags & 0x200) {
        SceneryDrawInfo *draw_info = scenery_cull_info->pCurrentDrawInfo;
        if (draw_info >= scenery_cull_info->pTopDrawInfo) {
            return;
        }

        scenery_cull_info->pCurrentDrawInfo = draw_info + 1;
        draw_info->pModel = reinterpret_cast<eModel *>(reinterpret_cast<uintptr_t>(model) + visibility_state);
        draw_info->pMatrix = 0;
        draw_info->SceneryInst = scenery_instance;
        return;
    }

    {
        bMatrix4 *matrix = eFrameMallocMatrix(1);
        if (!matrix) {
            return;
        }

        scenery_instance->GetRotation(matrix);
        scenery_instance->GetPosition(&matrix->v3);

        if (scenery_instance->ExcludeFlags & scenery_cull_info->ExcludeFlags & 0x100) {
            matrix->v3.z += EnvMapShadowExtraHeight;
        }
        if (scenery_cull_info->ExcludeFlags & 0x800) {
            matrix->v2.z = -matrix->v2.z;
        }

        {
            SceneryDrawInfo *draw_info = scenery_cull_info->pCurrentDrawInfo;
            if (draw_info >= scenery_cull_info->pTopDrawInfo) {
                return;
            }

            scenery_cull_info->pCurrentDrawInfo = draw_info + 1;
            draw_info->pModel = reinterpret_cast<eModel *>(reinterpret_cast<uintptr_t>(model) + visibility_state);
            if ((scenery_cull_info->ExcludeFlags & 0x4000) && model->GetSolid() && (model->GetSolid()->Flags & 0x80)) {
                bMatrix4 windrot;
                int offset = static_cast<int>(matrix->v3.x * 60.0f) % 0x168; // TODO magic
                CreateWindRotMatrix(scenery_cull_info->pView, &windrot, offset, matrix);
                bMulMatrix(matrix, matrix, &windrot);
            }

            draw_info->pMatrix = matrix;
            draw_info->SceneryInst = scenery_instance;

            if (scenery_cull_info->pView == eGetView(1, false) || scenery_cull_info->pView == eGetView(2, false)) {
                ePositionMarker *position_marker = 0;
                while ((position_marker = model->GetPostionMarker(position_marker)) != 0) {
                    if (model->GetSolid()) {
                        eLightFlare *light_flare = eGetNextLightFlareInPool(scenery_cull_info->pView == eGetView(1, false) ? 1 : 2);
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
                                light_flare->Flags = 4;
                                bVector2 dr(ps.x - draw_info->pMatrix->v3.x, ps.y - draw_info->pMatrix->v3.y);
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
    }
}

void ScenerySectionHeader::TreeCull(SceneryCullInfo *scenery_cull_info) {
    const int max_depth = 64;
    SceneryTreeNode *node_stack[max_depth];
    unsigned char visibility_state_stack[max_depth];
    SceneryTreeNode **pnode = node_stack + 1;
    unsigned char *pvisibility_state = visibility_state_stack + 1;

    node_stack[0] = SceneryTreeNodeTable;
    visibility_state_stack[0] = 1;
    while (pnode != node_stack) {
        pnode--;
        SceneryTreeNode *node = *pnode;
        pvisibility_state--;
        unsigned char visibility_state = *pvisibility_state;
        if (visibility_state == 1) {
            bVector3 bbox_min;
            bVector3 bbox_max;

            node->GetBBox(&bbox_min, &bbox_max);
            visibility_state = scenery_cull_info->pView->GetVisibleState(&bbox_min, &bbox_max, 0);
        }

        if (visibility_state != 0) {
            for (int child_number = 0; child_number < node->NumChildren; child_number++) {
                short child_code = node->ChildCodes[child_number];
                if (child_code >= 0) {
                    DrawAScenery(child_code, scenery_cull_info, visibility_state);
                } else {
                    // TODO, DebugBreak() stuff
                    {
                        int scenery_instance_number = -child_code;
                    }
                    {
                        child_code = -child_code;
                        SceneryTreeNode *child_node = &SceneryTreeNodeTable[child_code];
                        *pnode = child_node;
                    }
                    *pvisibility_state = visibility_state;
                    pnode++;
                    pvisibility_state++;
                }
            }
        }
    }
}

int GrandSceneryCullInfo::WhatSectionsShouldWeDraw(short *sections_to_draw, int max_sections_to_draw, SceneryCullInfo *scenery_cull_info) {
    DrivableScenerySection *drivable_scenery_section;
    int iViewID = scenery_cull_info->pView->GetID();
    if (iViewID == EVIEW_SHADOWMAP1 || iViewID == EVIEW_SHADOWMAP2) {
        int iViewPlayer = iViewID - 12;
        drivable_scenery_section = TheVisibleSectionManager.FindDrivableSection(
            reinterpret_cast<const bVector2 *>(eGetView(iViewPlayer, false)->GetCamera()->GetPosition()));
    } else {
        drivable_scenery_section =
            TheVisibleSectionManager.FindDrivableSection(reinterpret_cast<const bVector2 *>(scenery_cull_info->pView->GetCamera()->GetPosition()));
    }

    int num_sections_to_draw = 0;
    if (!drivable_scenery_section) {
        for (ScenerySectionHeader *section_header = ScenerySectionHeaderList.GetHead(); section_header != ScenerySectionHeaderList.EndOfList();
             section_header = section_header->GetNext()) {
            int section_number = section_header->GetSectionNumber();
            int subsection_number = GetScenerySubsectionNumber(section_number);
            if (subsection_number < 10 && num_sections_to_draw < max_sections_to_draw) {
                sections_to_draw[num_sections_to_draw] = static_cast<short>(section_number);
                num_sections_to_draw++;
            }
        }
    } else {
        for (int section_number = MakeScenerySectionNumber('Z', 0); section_number < MakeScenerySectionNumber('Z', 100); section_number++) {
            if (GetScenerySectionHeader(section_number) && num_sections_to_draw < max_sections_to_draw) {
                ScenerySectionHeader *section_header; // ?
                sections_to_draw[num_sections_to_draw] = static_cast<short>(section_number);
                num_sections_to_draw++;
            }
        }

        for (int i = 0; i < drivable_scenery_section->GetNumVisibleSections(); i++) {
            int section_number = drivable_scenery_section->GetVisibleSection(i);
            if (section_number >= 0 && GetScenerySectionHeader(section_number) && num_sections_to_draw < max_sections_to_draw) {
                ScenerySectionHeader *section_header; // ?
                sections_to_draw[num_sections_to_draw] = static_cast<short>(section_number);
                num_sections_to_draw++;
            }
        }
    }

    if (scenery_cull_info->pView->GetID() == EVIEW_PLAYER1) {
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
                num_sections_to_draw = ToggleIsInTable(sections_to_draw, num_sections_to_draw, max_sections_to_draw, ScenerySectionToBlink);
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
    ProfileNode profile_node("TODO", 0);
    const int max_sections_to_draw = 128;
    short sections_to_draw[max_sections_to_draw];
    int num_sections_to_draw = WhatSectionsShouldWeDraw(sections_to_draw, max_sections_to_draw, scenery_cull_info);

    for (int i = 0; i < num_sections_to_draw; i++) {
        int section_number = sections_to_draw[i];
        if (section_number >= 0) {
            ScenerySectionHeader *section_header = GetScenerySectionHeader(section_number);
            if (section_header && section_header->SceneryTreeNodeTable) {
                section_header->TreeCull(scenery_cull_info);
            }
        }
    }
}

void GrandSceneryCullInfo::DoCulling() {
    ProfileNode profile_node("TODO", 0);
    pFirstDrawInfo = SceneryDrawInfoTable;
    pCurrentDrawInfo = SceneryDrawInfoTable;
    pTopDrawInfo = SceneryDrawInfoTable + 3500;

    int n;
    for (n = 0; n < NumCullInfos; n++) {
        SceneryCullInfo *scenery_cull_info = &SceneryCullInfos[n];
        bool enable_preculling = true;

        scenery_cull_info->Position = *scenery_cull_info->pView->GetCamera()->GetPosition();
        scenery_cull_info->Direction = *scenery_cull_info->pView->GetCamera()->GetDirection();
        scenery_cull_info->H = scenery_cull_info->pView->H;

        if (PrecullerMode == 0) {
            enable_preculling = false;
        } else if (PrecullerMode == 2) {
            int time = RealTimeFrames % 0x3D;
            if (time < 0xF) {
                enable_preculling = false;
            }
        } else if (PrecullerMode == 3) {
            Camera *camera = scenery_cull_info->pView->GetCamera();
            if (bLength(camera->GetVelocityPosition()) < EnablePrecullingSpeed) {
                enable_preculling = false;
            }
        }

        if (DisablePrecullerCounter > 0 && PrecullerMode != 2) {
            enable_preculling = false;
        }

        if (gPrecullerBooBooManager.IsSet(scenery_cull_info->Position)) {
            enable_preculling = false;
        }
        scenery_cull_info->PrecullerSectionNumber = -1;
        if (enable_preculling) {
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

    // TODO
    {
        SceneryCullInfo *scenery_cull_info;
    }
    {
        SceneryCullInfo *scenery_cull_info;
    }

    for (n = 0; n < NumCullInfos; n++) {
        SceneryCullInfo *scenery_cull_info = &SceneryCullInfos[n];
        // TODO
        if (false) {
            for (SceneryDrawInfo *info = scenery_cull_info->pFirstDrawInfo; info < scenery_cull_info->pCurrentDrawInfo; info++) {
                eModel *model = reinterpret_cast<eModel *>(reinterpret_cast<uintptr_t>(info->pModel) & ~3);
                if (model->HasSolid()) {
                    model->GetSolid();
                }
            }
        }
    }
}

void GrandSceneryCullInfo::StuffScenery(eView *view, int stuff_flags) {
    ProfileNode profile_node("TODO", 0);
    Camera *camera = view->GetCamera();
    bVector4 *cpos = reinterpret_cast<bVector4 *>(camera->GetPosition());
    unsigned int extra_flags = 0;
    unsigned int exclude_flags = 0;
    unsigned int exclusive_flags = 0;

    if (stuff_flags & 1) {
        extra_flags = 0x1000;
    }
    if (stuff_flags & 0x400) {
        exclusive_flags = 0x100000;
    }
    if (stuff_flags & 0x80) {
        extra_flags |= 0x100;
    }
    if (stuff_flags & 0x10) {
        exclusive_flags = 0x2000;
    } else if (stuff_flags & 8) {
        exclude_flags = 0x2000;
    }
    if (stuff_flags & 0x300) {
        exclusive_flags = 0x10000;
    }
    if (stuff_flags & 0x800) {
        exclude_flags |= 0x400000;
    }
    if (stuff_flags & 0x1000) {
        exclusive_flags = 0x1000000;
    }

    for (int n = 0; n < NumCullInfos; n++) {
        SceneryCullInfo *scenery_cull_info = &SceneryCullInfos[n];
        if (scenery_cull_info->pView != view) {
            continue;
        }

        for (SceneryDrawInfo *info = scenery_cull_info->pFirstDrawInfo; info < scenery_cull_info->pCurrentDrawInfo; info++) {
            // They store data in the lower bits of the pointer
            // *free real estate*
            eVisibleState vis_state = static_cast<eVisibleState>(reinterpret_cast<uintptr_t>(info->pModel) & 3);
            unsigned int flags = extra_flags;
            bMatrix4 *positMarkMatrix;

            pDebugModel = reinterpret_cast<eModel *>(reinterpret_cast<uintptr_t>(info->pModel) & ~3);
            eSceneryLightContext *sceneryLightContext = nullptr;

            if (info->SceneryInst->ExcludeFlags & 0x80) {
                flags |= 0x2000;
            }
            if (info->SceneryInst->ExcludeFlags & 0x1000000) {
                flags |= 0x100000;
            }
            if (info->SceneryInst->ExcludeFlags & 0x100) {
                flags |= 0x1020000;
            }
            if (info->SceneryInst->ExcludeFlags & 0x400000) {
                flags |= 0x40000;
            }
            if (info->SceneryInst->ExcludeFlags & 0x20000) {
                flags |= 0x800000;
            }
            if (info->SceneryInst->ExcludeFlags & 0x80000) {
                flags |= 0x400000;
            }
            if (stuff_flags & 0x200) {
                bool isShadowMapMesh = (info->SceneryInst->ExcludeFlags & 0x4000000) != 0;
                if ((info->SceneryInst->ExcludeFlags & 0x200000) != 0 && !isShadowMapMesh) {
                    flags |= 0x10000;
                }
                if ((info->SceneryInst->ExcludeFlags & 0x40000000) != 0) {
                    flags |= 0x10000;
                }
            }
            if (stuff_flags & 0x100) {
                if ((info->SceneryInst->ExcludeFlags & 0x200000) != 0) {
                    flags |= 0x10000;
                }
                if ((info->SceneryInst->ExcludeFlags & 0x40000000) != 0) {
                    flags |= 0x10000;
                }
            }
            if (vis_state == EVISIBLESTATE_FULL) {
                flags |= 4;
            }

            bool A = false;
            if (exclusive_flags == 0) {
                A = exclude_flags == 0;
            }

            bool B = false;
            if (exclusive_flags != 0) {
                if (flags & exclusive_flags) {
                    B = true;
                }
            }

            bool C = false;
            if (exclude_flags != 0) {
                C = (flags & exclude_flags) == 0;
            }
            if (A || B || C) {
                if (!info->pMatrix) {
                    bMatrix4 *local_world = eGetIdentityMatrix();
                    view->Render(pDebugModel, local_world, 0, flags, 0);
                } else {
                    view->Render(pDebugModel, info->pMatrix, 0, flags, 0);
                }
                pDebugModel = reinterpret_cast<eModel *>(sceneryLightContext);
            }
        }
        return;
    }
}

void ServicePreculler() {}
