#include "AnimWorldScene.hpp"
#include "AnimPlayer.hpp"
#include "WorldAnimInstanceDirectory.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int AnimCfg_DisableWorldAnimations;
extern bool DisableWorldAnimations;
extern bool PrintWorldAnimationStuff;
extern RaceParameters TheRaceParameters;

CAnimWorldScene::CAnimWorldScene() : mHandle(0) {
    mHandle = bStringHash("WorldAnimations");
}

void CAnimWorldScene::ClearAllAnimations() {
    while (!mInstancedAnimTreeList.IsEmpty()) {
        delete mInstancedAnimTreeList.RemoveTail();
    }
}

CWorldAnimEntityTree *CAnimWorldScene::GetAnimTreeFromHash(uint32 instanceHash) {
    for (CWorldAnimEntityTree *tree = mInstancedAnimTreeList.GetHead(); tree != mInstancedAnimTreeList.EndOfList(); tree = tree->GetNext()) {
        if (tree->tree_name_hash == instanceHash) {
            return tree;
        }
    }
    return nullptr;
}

CAnimWorldScene::~CAnimWorldScene() {
    ClearAllAnimations();
}

CWorldAnimEntityTree *CAnimWorldScene::InstantiateAnimTree(WorldAnimInstance *instance) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return nullptr;
    }

    unsigned int track_num_val = instance->track_num;
    if (track_num_val != 0) {
        int min1 = (track_num_val >> 8) & 0xFF;
        int max1 = track_num_val & 0xFF;
        int min2 = track_num_val >> 24;
        int max2 = (track_num_val >> 16) & 0xFF;
        int tracknum = TheRaceParameters.TrackNumber % 100;
        if (tracknum < min1 || tracknum > max1) {
            if (tracknum < min2 || tracknum > max2) {
                return nullptr;
            }
        }
    }

    WorldAnimEntityTreeInfo *treeinfo = TheWorldAnimInstanceDirectory.GetAnimTreeInfo(instance->anim_tree_name_hash);
    bMatrix4 *instance_mat = &instance->instance_matrix;
    if (treeinfo == nullptr) {
        return nullptr;
    }

    uint32 begin_range = 0xFFFFFFFF;
    uint32 end_range = 0xFFFFFFFF;

    if (instance->named_range_hash != 0) {
        for (int i = 0; i < 4; i++) {
            WorldAnimNamedRange *range_array = treeinfo->named_ranges;
            WorldAnimNamedRange &namedrange = range_array[i];
            if (namedrange.name_hash == instance->named_range_hash) {
                uint32 range = namedrange.range;
                begin_range = range >> 16;
                end_range = range & 0xFFFF;
            }
        }
        if (PrintWorldAnimationStuff) {
            if (begin_range == 0xFFFFFFFF || end_range == 0xFFFFFFFF) {
                for (int i = 0; i < 4; i++) {
                }
            }
        }
    }

    CWorldAnimEntityTree *new_tree = new ("CWorldAnimEntityTree") CWorldAnimEntityTree();
    uint32 tree_name_hash = treeinfo->tree_name_hash;
    new_tree->mInstanceData = instance;
    new_tree->tree_name_hash = tree_name_hash;
    new_tree->mControlScenarioType = eCST_ERROR;

    int num_entities = treeinfo->loaded_world_anim_entity_chunks.CountElements();
    CWorldAnimEntity **arr_of_ptrs = new ("CWorldAnimEntity*", 0) CWorldAnimEntity *[num_entities];
    bMemSet(arr_of_ptrs, 0, num_entities * static_cast<int>(sizeof(CWorldAnimEntity *)));

    if (instance->play_flags & 0x400) {
        bRandom(1.0f);
    }
    if (instance->play_flags & 0x200) {
        bRandom(1.0f);
    }

    CWorldAnimEntity *root_entity = nullptr;
    int ix = 0;

    for (bPNode *node = treeinfo->loaded_world_anim_entity_chunks.GetHead(); node != treeinfo->loaded_world_anim_entity_chunks.EndOfList();
         node = node->GetNext()) {
        WorldAnimEntityInfo *entinfo = reinterpret_cast<WorldAnimEntityInfo *>(node->GetObject());
        SpaceNode *parent_space_node = nullptr;

        if (entinfo->mParentIndex != -1) {
            CWorldAnimEntity *parent = arr_of_ptrs[entinfo->mParentIndex];
            SpaceNode *spacenode = parent->GetSpaceNode();
            parent_space_node = arr_of_ptrs[entinfo->mParentIndex]->GetSpaceNode();
        }

        CWorldAnimEntity *new_entity_instantiation = new ("CWorldAnimEntity") CWorldAnimEntity();
        arr_of_ptrs[ix] = new_entity_instantiation;
        new_entity_instantiation->mAnimTree = new_tree;

        WorldAnimEntityInfo override_info;
        bMemCpy(&override_info, entinfo, sizeof(WorldAnimEntityInfo));
        ix++;

        if (instance->named_range_hash != 0) {
            instance->play_flags |= 0x40;
        }
        override_info.mLODB = instance->lodb_hash;
        override_info.mLODZ = instance->lodz_hash;

        uint32 start_trigger = instance->start_trigger_hash;
        override_info.instance_data = instance;
        new_tree->start_trigger_hash = start_trigger;
        new_tree->stop_trigger_hash = instance->stop_trigger_hash;

        if (start_trigger != 0) {
            new_tree->mControlScenarioType = eCST_TriggerZone;
        } else if (instance->track_dir != 0) {
            new_tree->mControlScenarioType = eCST_FwdRevTrack;
        }

        new_entity_instantiation->Init(&override_info, parent_space_node);

        if (entinfo->mThisInstanceNameHash == treeinfo->tree_name_hash) {
            root_entity = new_entity_instantiation;
            SpaceNode *space = root_entity->GetSpaceNode();
            space->SetLocalMatrix(instance_mat);
        }

        new_tree->instantiated_world_anim_entities.AddTail(new_entity_instantiation);

        if (instance->start_trigger_hash == 0 && instance->track_dir == 0) {
            new_entity_instantiation->Play();
        }
    }

    new_tree->root_entity = root_entity;
    mInstancedAnimTreeList.AddTail(new_tree);

    if (arr_of_ptrs != nullptr) {
        delete[] arr_of_ptrs;
    }

    return new_tree;
}

void CAnimWorldScene::UpdateTime(float time_step) {
    if (!AnimCfg_DisableWorldAnimations && !DisableWorldAnimations) {
        for (CWorldAnimEntityTree *tree = mInstancedAnimTreeList.GetHead(); tree != mInstancedAnimTreeList.EndOfList(); tree = tree->GetNext()) {
            for (bPNode *node = tree->instantiated_world_anim_entities.GetHead(); node != tree->instantiated_world_anim_entities.EndOfList();
                 node = node->GetNext()) {
                reinterpret_cast<IAnimEntity *>(node->GetpObject())->UpdateTimeStep(time_step);
            }
        }
    }
}

void ControlWorldAnim(unsigned int fAnimTreeNameHash, float fTimeSet, bool fAnimPause, bool fAnimHide) {
    if (TheAnimPlayer.GetWorldAnimScene() != nullptr) {
        CWorldAnimEntityTree *animTree = TheAnimPlayer.GetWorldAnimScene()->GetAnimTreeFromHash(fAnimTreeNameHash);
        if (animTree != nullptr) {
            if (fTimeSet >= 0.0f) {
                animTree->SetTime(fTimeSet);
            }
            if (fAnimHide) {
                animTree->Stop();
            } else if (fAnimPause) {
                animTree->Pause();
            } else {
                animTree->Play();
            }
        }
    }
}

void StartWorldAnimations() {
    if (!DisableWorldAnimations && !AnimCfg_DisableWorldAnimations) {
        TheWorldAnimInstanceDirectory.Init();
        CAnimWorldScene *pscene = TheAnimPlayer.GetWorldAnimScene();
        if (pscene == nullptr) {
            TheAnimPlayer.InitWorldAnimScene();
        }
        pscene = TheAnimPlayer.GetWorldAnimScene();
        if (pscene != nullptr) {
            bPList<WorldAnimInstance> &directory_list = TheWorldAnimInstanceDirectory.GetInstanceList();
            bPNode *node = directory_list.GetHead();
            while (node != directory_list.EndOfList()) {
                WorldAnimInstance *instance = reinterpret_cast<WorldAnimInstance *>(node->GetObject());
                bMatrix4 location_matrix;
                bIdentity(&location_matrix);
                location_matrix.v3 = instance->instance_matrix.v3;
                CWorldAnimEntityTree *tree_instance = pscene->InstantiateAnimTree(instance);
                node = node->GetNext();
            }
            ControlWorldAnim(bStringHash("EN_TollBoothArm_01"), 0.0f, true, true);
            ControlWorldAnim(bStringHash("EN_TollBoothArm_02"), 0.0f, true, true);
            ControlWorldAnim(bStringHash("EN_TollBoothArm_03"), 0.0f, true, true);
            ControlWorldAnim(bStringHash("EN_TollBoothArm_04"), 0.0f, true, true);
        }
    }
}

void ResetWorldAnimations() {
    ControlWorldAnim(bStringHash("EN_TollBoothArm_01"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_TollBoothArm_02"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_TollBoothArm_03"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_TollBoothArm_04"), 0.0f, true, false);
}

void CloseAllGarageDoors() {
    ControlWorldAnim(bStringHash("EN_Chopshop_30"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_13"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_20"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_14"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_40"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_50"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_45"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_90"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_11"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_60"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_70"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_35"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_80"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_12"), 0.0f, true, false);
    ControlWorldAnim(bStringHash("EN_Chopshop_55"), 0.0f, true, false);
}
