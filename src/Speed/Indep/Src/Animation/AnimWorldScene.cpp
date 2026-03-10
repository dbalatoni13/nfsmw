#include "AnimWorldScene.hpp"
#include "WorldAnimInstanceDirectory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern int AnimCfg_DisableWorldAnimations;

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
