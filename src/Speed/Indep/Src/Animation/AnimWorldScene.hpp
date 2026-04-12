#ifndef ANIMATION_ANIMWORLDSCENE_H
#define ANIMATION_ANIMWORLDSCENE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimEntity_WorldEntity.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Animation/AnimScene.hpp"

// total size: 0x18
class CAnimWorldScene : public bTNode<CAnimWorldScene> {
  public:
    CAnimWorldScene();

    void ClearAllAnimations();

    CWorldAnimEntityTree *GetAnimTree(int instance_id);

    CWorldAnimEntityTree *GetAnimTreeFromHash(uint32 instanceHash);

    virtual ~CAnimWorldScene();

    void UpdateTime(float time_step);

    bMatrix4 *GetAnimLocation(int SceneID);

    CWorldAnimEntityTree *InstantiateAnimTree(WorldAnimInstance *instance);

    void DoSnapshot(ReplaySnapshot *snapshot);

  private:
    AnimHandle mHandle;                                  // offset 0x8, size 0x4
    bTList<CWorldAnimEntityTree> mInstancedAnimTreeList; // offset 0xC, size 0x8
};

#endif
