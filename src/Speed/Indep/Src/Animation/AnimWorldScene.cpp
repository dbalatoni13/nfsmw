#include "AnimWorldScene.hpp"
#include "AnimPlayer.hpp"
#include "WorldAnimInstanceDirectory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern int AnimCfg_DisableWorldAnimations;
extern bool DisableWorldAnimations;

CAnimWorldScene::CAnimWorldScene()
    : mHandle(0) {
    mHandle = bStringHash("WorldAnimations");
}

void CAnimWorldScene::ClearAllAnimations() {
    while (!mInstancedAnimTreeList.IsEmpty()) {
        delete mInstancedAnimTreeList.RemoveTail();
    }
}

CWorldAnimEntityTree *CAnimWorldScene::GetAnimTreeFromHash(unsigned int instanceHash) {
    for (CWorldAnimEntityTree *tree = mInstancedAnimTreeList.GetHead(); tree != mInstancedAnimTreeList.EndOfList(); tree = tree->GetNext()) {
        if (tree->tree_name_hash == instanceHash) {
            return tree;
        }
    }
    return nullptr;
}

CAnimWorldScene::~CAnimWorldScene() {
    ClearAllAnimations();
    while (!mInstancedAnimTreeList.IsEmpty()) {
        delete mInstancedAnimTreeList.RemoveHead();
    }
}

void CAnimWorldScene::UpdateTime(float time_step) {
    if (!AnimCfg_DisableWorldAnimations && !DisableWorldAnimations) {
        for (CWorldAnimEntityTree *tree = mInstancedAnimTreeList.GetHead(); tree != mInstancedAnimTreeList.EndOfList(); tree = tree->GetNext()) {
            for (bPNode *node = tree->instantiated_world_anim_entities.GetHead(); node != tree->instantiated_world_anim_entities.EndOfList();
                 node = node->GetNext()) {
                reinterpret_cast< IAnimEntity * >(node->GetpObject())->UpdateTimeStep(time_step);
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
