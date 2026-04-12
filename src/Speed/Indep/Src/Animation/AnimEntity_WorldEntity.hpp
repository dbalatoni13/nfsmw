#ifndef ANIMATION_ANIMENTITY_WORLDENTITY_H
#define ANIMATION_ANIMENTITY_WORLDENTITY_H

#include "WorldAnimCtrl.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimEntity.hpp"
#include "Speed/Indep/Src/Animation/AnimWorldTypes.hpp"

class CWorldAnimEntityTree;

// total size: 0x24
class CWorldAnimEntity : public IAnimEntity {
  public:
    virtual uint32 GetTypeID() {
        return mTypeID;
    }

    // Overrides: IAnimEntity
    uint32 GetInstanceNameHash() override {
        return mThisInstanceNameHash;
    }

    // Overrides: IAnimEntity
    SpaceNode *GetSpaceNode() override {
        return mSpaceNode;
    }

    // Overrides: IAnimEntity
    WorldModel *GetWorldModel() override {
        return mWorldModel;
    }

    CWorldAnimCtrl *GetAnimCtrl() {
        return mAnimCtrl;
    }

    uint32 GetParentInstanceNameHash() {
        return mParentInstanceNameHash;
    }

    CWorldAnimEntityTree *GetAnimTree() {
        return mAnimTree;
    }

    void *operator new(size_t size, const char *debug_name);

    void operator delete(void *ptr);

    static void EndianSwapEntityData(void *data, int size);

    CWorldAnimEntity();

    // Overrides: IAnimEntity
    ~CWorldAnimEntity() override;

    // Overrides: IAnimEntity
    bool Init(void *init_data, SpaceNode *parent_space_node) override;

    // Overrides: IAnimEntity
    void Purge() override;

    void Play();

    void Pause();

    void Stop();

    bool IsPlaying();

    // Overrides: IAnimEntity
    void SetTime(float time) override;

    // Overrides: IAnimEntity
    void UpdateTimeStep(float time_step) override;

    void SetCurrentEvalTime(float time, bool in_seconds);

    float GetCurrentEvalTime(bool in_seconds);

    float GetNumSecondsUntilThisFrame(float frame_number);

    float GetNumSecondsBetweenFrames(float start_frame, float end_frame);

  private:
    friend class CAnimWorldScene;
    uint32 mTypeID;                   // offset 0x4, size 0x4
    uint32 mThisInstanceNameHash;     // offset 0x8, size 0x4
    SpaceNode *mSpaceNode;            // offset 0xC, size 0x4
    WorldModel *mWorldModel;          // offset 0x10, size 0x4
    CWorldAnimCtrl *mAnimCtrl;        // offset 0x14, size 0x4
    WorldAnimEntityInfo *mEntityInfo; // offset 0x18, size 0x4
    uint32 mParentInstanceNameHash;   // offset 0x1C, size 0x4
    CWorldAnimEntityTree *mAnimTree;  // offset 0x20, size 0x4
};

// total size: 0x2C
class CWorldAnimEntityTree : public bTNode<CWorldAnimEntityTree> {
  public:
    void *operator new(size_t size, const char *debug_name);

    void operator delete(void *ptr);

    CWorldAnimEntityTree();

    virtual ~CWorldAnimEntityTree();

    CWorldAnimEntity *GetEntityByNameHash(uint32 namehash);

    void SetTime(float time);

    void Pause();

    void Play();

    void Stop();

    float GetAnimLengthInSeconds();

    uint32 tree_name_hash;                                     // offset 0x8, size 0x4
    CWorldAnimEntity *root_entity;                             // offset 0xC, size 0x4
    bPList<CWorldAnimEntity> instantiated_world_anim_entities; // offset 0x10, size 0x8
    uint32 start_trigger_hash;                                 // offset 0x18, size 0x4
    uint32 stop_trigger_hash;                                  // offset 0x1C, size 0x4
    WorldAnimInstance *mInstanceData;                          // offset 0x20, size 0x4
    eControlScenarioType mControlScenarioType;                 // offset 0x24, size 0x4
};

// total size: 0x58
class WorldAnimEntityTreeInfo : public bTNode<WorldAnimEntityTreeInfo> {
  public:
    void *operator new(size_t size, const char *debug_name);
    void operator delete(void *ptr);

    WorldAnimEntityTreeInfo() {}

    WorldAnimEntityTreeInfo(uint32 treenamehash, bPList<WorldAnimEntityInfo> &temp_list, WorldAnimNamedRange *ranges);

    virtual ~WorldAnimEntityTreeInfo();

  private:
    friend class WorldAnimInstanceDirectory;
    friend class CAnimWorldScene;

    uint32 tree_name_hash;                                       // offset 0x8, size 0x4
    bPList<WorldAnimEntityInfo> loaded_world_anim_entity_chunks; // offset 0xC, size 0x8
    WorldAnimNamedRange named_ranges[4];                         // offset 0x14, size 0x40
};

// total size: 0x50
struct WorldAnimEntityTreeMarker {
    uint32 anim_tree_name_hash;          // offset 0x0, size 0x4
    uint32 pad0;                         // offset 0x4, size 0x4
    uint32 pad1;                         // offset 0x8, size 0x4
    uint32 pad2;                         // offset 0xC, size 0x4
    WorldAnimNamedRange named_ranges[4]; // offset 0x10, size 0x40
};

#endif
