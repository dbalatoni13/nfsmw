#include "AnimEntity_WorldEntity.hpp"
#include "AnimWorldTypes.hpp"
#include "Speed/Indep/Src/Animation/WorldAnimInstanceDirectory.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/WorldModel.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "WorldAnimInstanceDirectory.hpp"

static int NumWorldAnimEntities;           // size: 0x4, address: 0x804156F4
static int MaxNumWorldAnimEntities;        // size: 0x4, address: 0x804156F8
// TODO move? they are static though
static int NumWorldAnimEntityTrees;        // size: 0x4, address: 0x804156FC
static int MaxNumWorldAnimEntityTrees;     // size: 0x4, address: 0x80415700
static int NumWorldAnimEntityTreeInfos;    // size: 0x4, address: 0x80415704
static int MaxNumWorldAnimEntityTreeInfos; // size: 0x4, address: 0x80415708

void *CWorldAnimEntity::operator new(size_t size, const char *debug_name) {
    NumWorldAnimEntities++;
    if (NumWorldAnimEntities > MaxNumWorldAnimEntities) {
        MaxNumWorldAnimEntities = NumWorldAnimEntities;
    }
    return bOMalloc(TheWorldAnimInstanceDirectory.GetWorldAnimEntitySlotPool());
}

void CWorldAnimEntity::operator delete(void *ptr) {
    NumWorldAnimEntities--;
    bFree(TheWorldAnimInstanceDirectory.GetWorldAnimEntitySlotPool(), ptr);
}

void *CWorldAnimEntityTree::operator new(size_t size, const char *debug_name) {
    NumWorldAnimEntityTrees++;
    if (NumWorldAnimEntityTrees > MaxNumWorldAnimEntityTrees) {
        MaxNumWorldAnimEntityTrees = NumWorldAnimEntityTrees;
    }
    return bOMalloc(TheWorldAnimInstanceDirectory.GetWorldAnimEntityTreeSlotPool());
}

void CWorldAnimEntityTree::operator delete(void *ptr) {
    NumWorldAnimEntityTrees--;
    bFree(TheWorldAnimInstanceDirectory.GetWorldAnimEntityTreeSlotPool(), ptr);
}

void *WorldAnimEntityTreeInfo::operator new(size_t size, const char *debug_name) {
    NumWorldAnimEntityTreeInfos++;
    if (NumWorldAnimEntityTreeInfos > MaxNumWorldAnimEntityTreeInfos) {
        MaxNumWorldAnimEntityTreeInfos = NumWorldAnimEntityTreeInfos;
    }
    return bOMalloc(TheWorldAnimInstanceDirectory.GetWorldAnimEntityTreeInfoSlotPool());
}

void WorldAnimEntityTreeInfo::operator delete(void *ptr) {
    NumWorldAnimEntityTreeInfos--;
    bFree(TheWorldAnimInstanceDirectory.GetWorldAnimEntityTreeInfoSlotPool(), ptr);
}

void CWorldAnimEntity::EndianSwapEntityData(void *data, int size) {
    WorldAnimEntityInfo *info = static_cast<WorldAnimEntityInfo *>(data);
    bPlatEndianSwap(&info->mTypeID);
    bPlatEndianSwap(&info->mThisInstanceNameHash);
    bPlatEndianSwap(&info->mParentInstanceNameHash);
    bPlatEndianSwap(&info->mModelHash);
    bPlatEndianSwap(&info->mLODB);
    bPlatEndianSwap(&info->mLODZ);
    bPlatEndianSwap(&info->mAnimTreeHash);
    bPlatEndianSwap(&info->mAnimNameHash);
    bPlatEndianSwap(&info->mLocalMatrix);
}

CWorldAnimEntity::CWorldAnimEntity()
    : mParentInstanceNameHash(0), //
      mTypeID(0),                 //
      mThisInstanceNameHash(0),   //
      mSpaceNode(nullptr),        //
      mWorldModel(nullptr),       //
      mAnimCtrl(nullptr) {}

CWorldAnimEntity::~CWorldAnimEntity() { Purge(); }

uint32 GetAnimChannelHash(uint32 anim_hash, uint32 dof) {
    if (dof == 0) {
        return bStringHash("_t", anim_hash);
    } else if (dof == 1) {
        return bStringHash("_q", anim_hash);
    } else if (dof == 2) {
        return bStringHash("_s", anim_hash);
    } else {
        return 0;
    }
}

bool CWorldAnimEntity::Init(void *init_data, SpaceNode *parent_space_node) {
    unsigned int play_flags = 0;

    Purge();

    WorldAnimEntityInfo *info = static_cast<WorldAnimEntityInfo *>(init_data);
    mEntityInfo = info;
    mTypeID = info->mTypeID;
    mThisInstanceNameHash = info->mThisInstanceNameHash;
    mParentInstanceNameHash = info->mParentInstanceNameHash;

    mSpaceNode = CreateSpaceNode(nullptr);
    mSpaceNode->SetParent(parent_space_node);
    mSpaceNode->SetNameHash(mThisInstanceNameHash);
    mSpaceNode->SetLocalMatrix(&info->mLocalMatrix);

    if (info->mModelHash != 0 && info->mAnimTreeHash != info->mThisInstanceNameHash) {
        unsigned int lods[4];
        lods[0] = info->mModelHash;
        lods[1] = info->mLODB;
        lods[2] = 0;
        lods[3] = info->mLODZ;
        WorldModel *model = new WorldModel(mSpaceNode, lods, false);
        mWorldModel = model;
    } else {
        mWorldModel = nullptr;
    }

    mAnimCtrl = new ("CWorldAnimCtrl") CWorldAnimCtrl();
    mAnimCtrl->SetNameHash(mSpaceNode->GetNameHash());
    mAnimCtrl->SetTimeScale(info->instance_data->speed);
    if (mAnimCtrl->GetTimeScale() == 0.0f) {
        mAnimCtrl->SetTimeScale(1.0f);
    }

    mAnimCtrl->SetMasterDelayTime(info->instance_data->master_delay);
    mAnimCtrl->SetLocalDelayTime(info->instance_data->loop_delay);

    CAnimPart *anim_part = mAnimCtrl->GetAnimPart();
    CAnimSkeleton *skel = GetSkeletonFromList(bStringHash("ROOT"));
    play_flags = info->instance_data->play_flags;
    anim_part->Init(skel);

    if (play_flags == 0) {
        mAnimCtrl->SetFlags(0x28);
    } else {
        mAnimCtrl->SetFlags(play_flags);
    }

    if (info->mAnimContentFlags & 1) {
        unsigned int TransAnimNameHash = GetAnimChannelHash(info->mAnimNameHash, 0);
        mAnimCtrl->CreateFnAnimFromNamehash(TransAnimNameHash, 0);
    }
    if (info->mAnimContentFlags & 2) {
        unsigned int QuatsAnimNameHash = GetAnimChannelHash(info->mAnimNameHash, 1);
        mAnimCtrl->CreateFnAnimFromNamehash(QuatsAnimNameHash, 1);
    }
    if (info->mAnimContentFlags & 4) {
        unsigned int ScaleAnimNameHash = GetAnimChannelHash(info->mAnimNameHash, 2);
        mAnimCtrl->CreateFnAnimFromNamehash(ScaleAnimNameHash, 2);
    }

    if (mAnimCtrl->GetAllocated() == 0 || skel == nullptr) {
        mAnimCtrl->GetAnimPart()->Purge();
        mAnimCtrl->Cleanup();
        if (mAnimCtrl != nullptr) {
            delete mAnimCtrl;
        }
        mAnimCtrl = nullptr;
    }

    if (mAnimCtrl != nullptr) {
        if (info->instance_data->play_flags & 0x40) {
            mAnimCtrl->SetLoopRange(info->instance_data->begin_range, info->instance_data->end_range);
        }
        if (info->instance_data->play_flags & 0x200) {
            unsigned int seed = 0x69babe69;
            float anim_len_sec = mAnimCtrl->GetAnimLengthInSeconds();
            float rand_delay = bRandom(anim_len_sec * 0.5f, &seed);
            CWorldAnimCtrl *ctrl = mAnimCtrl;
            ctrl->SetLocalDelayTime(rand_delay);
        }
        if (info->instance_data->play_flags & 0x400) {
            unsigned int seed = 0x69babe69;
            float rand_start = bRandom(mAnimCtrl->GetAnimLength(), &seed);
            mAnimCtrl->SetEvalTime(rand_start);
        }
        if (info->instance_data->start_trigger_hash != 0 || info->instance_data->stop_trigger_hash != 0) {
            mAnimCtrl->ClearFlags(0x8);
            mAnimCtrl->SetFlags(0x810);
        }
    }

    return true;
}

void CWorldAnimEntity::Purge() {
    if (mAnimCtrl) {
        mAnimCtrl->GetAnimPart()->Purge();
        mAnimCtrl->Cleanup();
        delete mAnimCtrl;
        mAnimCtrl = nullptr;
    }
    if (mWorldModel) {
        delete mWorldModel;
        mWorldModel = nullptr;
    }
    if (mSpaceNode) {
        DeleteSpaceNode(mSpaceNode);
    }
}

void CWorldAnimEntity::Play() {
    if (mAnimCtrl) {
        mAnimCtrl->Play();
    }
}

void CWorldAnimEntity::Pause() {
    if (mAnimCtrl) {
        mAnimCtrl->Pause();
    }
}

void CWorldAnimEntity::Stop() {
    if (mAnimCtrl) {
        mAnimCtrl->Stop();
    }
}

bool CWorldAnimEntity::IsPlaying() {
    if (mAnimCtrl) {
        return mAnimCtrl->IsPlaying();
    }
    return false;
}

void CWorldAnimEntity::SetTime(float time) {
    if (mAnimCtrl) {
        mAnimCtrl->SetEvalTime(0.0f);
    }
    // TODO is this right?
    UpdateTimeStep(time);
}

void CWorldAnimEntity::UpdateTimeStep(float time_step) {
    ProfileNode profile_node("TODO", 0);

    bool visible = true;
    if (mAnimTree && mAnimTree->mInstanceData) {
        int section_number = mAnimTree->mInstanceData->section_number;
        if (!TheTrackStreamer.IsSectionVisible(section_number)) {
            visible = false;
        }
    }
    if (!visible) {
        if (mWorldModel) {
            mWorldModel->SetEnabledFlag(false);
        }
    } else {
        if (mWorldModel && !mAnimCtrl->IsStopped()) {
            mWorldModel->SetEnabledFlag(true);
        }
        if (mAnimCtrl) {
            mAnimCtrl->AdvanceAnimTime(time_step);
            if (mAnimTree && mAnimTree->mInstanceData) {
                mAnimCtrl->UpdateAnimPose();
                bMatrix4 *global_matrices = reinterpret_cast<bMatrix4 *>(mAnimCtrl->GetAnimPart()->GetGlobalMatrices());
                bMatrix4 local_matrix(*mSpaceNode->GetLocalMatrix());
                mSpaceNode->SetBlendingMatrices(nullptr);
                mSpaceNode->SetLocalMatrix(&global_matrices[1]);
            }
        }
    }
}

// STRIPPED
void CWorldAnimEntity::SetCurrentEvalTime(float time, bool in_seconds) {}

// STRIPPED
float CWorldAnimEntity::GetCurrentEvalTime(bool in_seconds) {
    return mAnimCtrl->GetEvalTime();
}

// STRIPPED
float CWorldAnimEntity::GetNumSecondsUntilThisFrame(float frame_number) {}

// STRIPPED
float CWorldAnimEntity::GetNumSecondsBetweenFrames(float start_frame, float end_frame) {}

int CompareParentIndex(bPNode *node_before, bPNode *node_after) {
    WorldAnimEntityInfo *entity_before = reinterpret_cast<WorldAnimEntityInfo *>(node_before->GetObject());
    WorldAnimEntityInfo *entity_after = reinterpret_cast<WorldAnimEntityInfo *>(node_after->GetObject());
    int res = entity_before->mParentIndex >= entity_after->mParentIndex;

    return res;
}

WorldAnimEntityTreeInfo::WorldAnimEntityTreeInfo(unsigned int treenamehash, bPList<WorldAnimEntityInfo> &temp_list, WorldAnimNamedRange *ranges) {
    tree_name_hash = treenamehash;

    while (!temp_list.IsEmpty()) {
        bPNode *node = temp_list.GetTail();
        WorldAnimEntityInfo *waei = reinterpret_cast<WorldAnimEntityInfo *>(node->GetObject());
        node->Remove();
        delete node;

        loaded_world_anim_entity_chunks.AddSorted(&CompareParentIndex, new bPNode(waei));
    }
    // TODO is the sizeof right?
    bMemCpy(named_ranges, ranges, 4 * sizeof(WorldAnimNamedRange));
}

CWorldAnimEntityTree::CWorldAnimEntityTree() {
    tree_name_hash = 0;
    root_entity = nullptr;
    start_trigger_hash = 0;
    stop_trigger_hash = 0;
}

CWorldAnimEntityTree::~CWorldAnimEntityTree() {
    while (!instantiated_world_anim_entities.IsEmpty()) {
        bPNode *entity_node = instantiated_world_anim_entities.GetTail();
        CWorldAnimEntity *entity = reinterpret_cast<CWorldAnimEntity *>(entity_node->GetObject());

        entity_node->Remove();
        delete entity_node;

        entity->Purge();
        delete entity;
    }
}

WorldAnimEntityTreeInfo::~WorldAnimEntityTreeInfo() {
    while (!loaded_world_anim_entity_chunks.IsEmpty()) {
        bPNode *node = loaded_world_anim_entity_chunks.GetTail();

        node->Remove();
        delete node;
    }
}

// TODO move somewhere in zMisc
extern int AnimCfg_DisableWorldAnimations;

// TODO move?
bPList<WorldAnimEntityInfo> temp_loaded_world_anim_entity_chunks;

int LoaderWorldAnimTreeMarker(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_WORLD_ANIM_TREE_MARKERS) {
        if (!TheWorldAnimInstanceDirectory.IsInitialized()) {
            DisableWorldAnimations = true;
            return 1;
        } else if (DisableWorldAnimations || AnimCfg_DisableWorldAnimations) {
            return 1;
        }

        WorldAnimEntityTreeMarker *marker = reinterpret_cast<WorldAnimEntityTreeMarker *>(chunk->GetAlignedData(16));
        bPlatEndianSwap(&marker->anim_tree_name_hash);
        for (int i = 0; i < 4; i++) {
            bPlatEndianSwap(&marker->named_ranges[i].name_hash);
            bPlatEndianSwap(&marker->named_ranges[i].range);
        }

        unsigned int anim_tree_name_hash = marker->anim_tree_name_hash;

        if (temp_loaded_world_anim_entity_chunks.IsEmpty()) {
            return 1;
        }
        int num_entities = temp_loaded_world_anim_entity_chunks.CountElements();
        WorldAnimEntityInfo **arr_of_ptrs = new ("WorldAnimEntityInfo[]", 0) WorldAnimEntityInfo *[num_entities];
        bMemSet(arr_of_ptrs, 0, num_entities * sizeof(*arr_of_ptrs));

        int ix = 0;
        for (bPNode *node = temp_loaded_world_anim_entity_chunks.GetHead(); node != temp_loaded_world_anim_entity_chunks.EndOfList();
             node = node->GetNext()) {
            WorldAnimEntityInfo *waei = reinterpret_cast<WorldAnimEntityInfo *>(node->GetObject());
            arr_of_ptrs[ix] = waei;
            ix++;
        }

        for (bPNode *node = temp_loaded_world_anim_entity_chunks.GetHead(); node != temp_loaded_world_anim_entity_chunks.EndOfList();
             node = node->GetNext()) {
            WorldAnimEntityInfo *waei = reinterpret_cast<WorldAnimEntityInfo *>(node->GetObject());
            if (waei->mParentIndex >= 0) {
                waei->mParentEntityInfo = arr_of_ptrs[waei->mParentIndex];
            } else {
                waei->mParentEntityInfo = nullptr;
            }
        }

        WorldAnimEntityTreeInfo *anim_tree =
            new ("WorldAnimEntityTreeInfo") WorldAnimEntityTreeInfo(anim_tree_name_hash, temp_loaded_world_anim_entity_chunks, marker->named_ranges);

        TheWorldAnimInstanceDirectory.AddLoadedAnimTreeInfo(anim_tree);
        for (bPNode *node = temp_loaded_world_anim_entity_chunks.GetHead(); node != temp_loaded_world_anim_entity_chunks.EndOfList();
             node = node->GetNext()) {
            WorldAnimEntityInfo *waei = reinterpret_cast<WorldAnimEntityInfo *>(node->GetObject());
            TheWorldAnimInstanceDirectory.AddLoadedAnimEntityInfo(waei);
        }

        while (!temp_loaded_world_anim_entity_chunks.IsEmpty()) {
            bPNode *node = temp_loaded_world_anim_entity_chunks.GetTail();

            node->Remove();
            delete node;
        }
        delete[] arr_of_ptrs;

        return 1;
    } else {
        return 0;
    }
}

int UnloaderWorldAnimTreeMarker(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_WORLD_ANIM_TREE_MARKERS) {
        if (!DisableWorldAnimations && !AnimCfg_DisableWorldAnimations) {
            WorldAnimEntityTreeMarker *tree_marker = reinterpret_cast<WorldAnimEntityTreeMarker *>(chunk->GetAlignedData(16));
            TheWorldAnimInstanceDirectory.RemoveAndDeleteAnimTreeInfo(tree_marker->anim_tree_name_hash);
        }

        return 1;
    } else {
        return 0;
    }
}

int LoaderWorldAnimEntityData(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_WORLD_ANIM_ENTITIES) {
        return 0;
    }

    int num_objects = chunk->GetAlignedSize(16) / sizeof(WorldAnimEntityInfo);
    for (int n = 0; n < num_objects; n++) {
        if (DisableWorldAnimations || AnimCfg_DisableWorldAnimations) {
            break;
        }
        WorldAnimEntityInfo *entity_info = &reinterpret_cast<WorldAnimEntityInfo *>(chunk->GetAlignedData(16))[n];
        CWorldAnimEntity::EndianSwapEntityData(entity_info, sizeof(WorldAnimEntityInfo));

        temp_loaded_world_anim_entity_chunks.AddTail(entity_info);
    }

    return 1;
}

int UnloaderWorldAnimEntityData(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_WORLD_ANIM_ENTITIES) {
        return 0;
    }

    int num_objects = chunk->GetAlignedSize(16) / sizeof(WorldAnimEntityInfo);
    for (int n = 0; n < num_objects; n++) {
        if (DisableWorldAnimations || AnimCfg_DisableWorldAnimations) {
            break;
        }
        WorldAnimEntityInfo *entity_info_to_unload = &reinterpret_cast<WorldAnimEntityInfo *>(chunk->GetAlignedData(16))[n];
        TheWorldAnimInstanceDirectory.RemoveEntityInfo(entity_info_to_unload);
    }
    return 1;
}

int LoaderWorldAnimDirectoryData(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_WORLD_ANIM_INSTANCES) {
        return 0;
    }

    int num_objects = chunk->GetAlignedSize(16) / sizeof(WorldAnimInstance);
    for (int n = 0; n < num_objects; n++) {
        if (DisableWorldAnimations || AnimCfg_DisableWorldAnimations) {
            break;
        }
        WorldAnimInstance *wai = &reinterpret_cast<WorldAnimInstance *>(chunk->GetAlignedData(16))[n];
        bPlatEndianSwap(&wai->unique_instance_id);
        bPlatEndianSwap(&wai->anim_tree_name_hash);
        bPlatEndianSwap(&wai->section_number);
        bPlatEndianSwap(&wai->play_flags);
        bPlatEndianSwap(&wai->speed);
        bPlatEndianSwap(&wai->master_delay);
        bPlatEndianSwap(&wai->loop_delay);
        bPlatEndianSwap(&wai->begin_range);
        bPlatEndianSwap(&wai->end_range);
        bPlatEndianSwap(&wai->named_range_hash);
        bPlatEndianSwap(&wai->event_track_hash);
        bPlatEndianSwap(&wai->track_num);
        bPlatEndianSwap(&wai->track_dir);
        bPlatEndianSwap(&wai->bbox_max);
        bPlatEndianSwap(&wai->bbox_min);
        bPlatEndianSwap(&wai->start_trigger_hash);
        bPlatEndianSwap(&wai->stop_trigger_hash);
        bPlatEndianSwap(&wai->lodb_hash);
        bPlatEndianSwap(&wai->lodz_hash);
        bPlatEndianSwap(&wai->instance_matrix);

        TheWorldAnimInstanceDirectory.AddLoadedAnimInstance(wai);
    }

    return 1;
}

int UnloaderWorldAnimDirectoryData(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_WORLD_ANIM_INSTANCES) {
        return 0;
    }

    int num_objects = chunk->GetAlignedSize(16) / sizeof(WorldAnimInstance);
    for (int n = 0; n < num_objects; n++) {
        if (DisableWorldAnimations || AnimCfg_DisableWorldAnimations) {
            break;
        }
        WorldAnimInstance *wai = &reinterpret_cast<WorldAnimInstance *>(chunk->GetAlignedData(16))[n];
        TheWorldAnimInstanceDirectory.RemoveAnimInstance(wai);
    }
    return 1;
}

CWorldAnimEntity *CWorldAnimEntityTree::GetEntityByNameHash(unsigned int namehash) {
    // TODO
    for (bPNode *node = instantiated_world_anim_entities.GetHead(); node != instantiated_world_anim_entities.EndOfList(); node = node->GetNext()) {
        CWorldAnimEntity *entity = reinterpret_cast<CWorldAnimEntity *>(node->GetObject());
        if (entity->GetInstanceNameHash() == namehash) {
            return entity;
        }
    }
    return nullptr;
}

void CWorldAnimEntityTree::SetTime(float time) {
    for (bPNode *node = instantiated_world_anim_entities.GetHead(); node != instantiated_world_anim_entities.EndOfList(); node = node->GetNext()) {
        CWorldAnimEntity *entity = reinterpret_cast<CWorldAnimEntity *>(node->GetObject());
        entity->SetTime(time);
    }
}

void CWorldAnimEntityTree::Pause() {
    for (bPNode *node = instantiated_world_anim_entities.GetHead(); node != instantiated_world_anim_entities.EndOfList(); node = node->GetNext()) {
        CWorldAnimEntity *entity = reinterpret_cast<CWorldAnimEntity *>(node->GetObject());
        entity->Pause();
        if (entity->GetWorldModel()) {
            entity->GetWorldModel()->SetEnabledFlag(true);
        }
    }
}

void CWorldAnimEntityTree::Play() {
    for (bPNode *node = instantiated_world_anim_entities.GetHead(); node != instantiated_world_anim_entities.EndOfList(); node = node->GetNext()) {
        CWorldAnimEntity *entity = reinterpret_cast<CWorldAnimEntity *>(node->GetObject());
        if (!entity->IsPlaying()) {
            entity->Play();
        }
        if (entity->GetWorldModel()) {
            entity->GetWorldModel()->SetEnabledFlag(true);
        }
    }
}

void CWorldAnimEntityTree::Stop() {
    for (bPNode *node = instantiated_world_anim_entities.GetHead(); node != instantiated_world_anim_entities.EndOfList(); node = node->GetNext()) {
        CWorldAnimEntity *entity = reinterpret_cast<CWorldAnimEntity *>(node->GetObject());
        entity->Stop();
        if (entity->GetWorldModel()) {
            entity->GetWorldModel()->SetEnabledFlag(false);
        }
    }
}
