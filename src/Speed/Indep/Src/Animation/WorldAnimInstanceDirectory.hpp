#ifndef ANIMATION_WORLDANIMINSTANCEDIRECTORY_H
#define ANIMATION_WORLDANIMINSTANCEDIRECTORY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimEntity_WorldEntity.hpp"
#include "AnimWorldTypes.hpp"
#include "ControlScenario.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

// total size: 0x10
struct WorldAnimInstanceEntryInfo {
    uint32 mUniqueInstanceID; // offset 0x0, size 0x4
    uint32 mAnimTreeNameHash; // offset 0x4, size 0x4
    uint32 mSectionNumber;    // offset 0x8, size 0x4
    float mEvalTime;          // offset 0xC, size 0x4
};

// total size: 0x10
class WorldAnimInstanceEntry : public bTNode<WorldAnimInstanceEntry> {
  public:
    void *operator new(size_t size, const char *debug_name);

    void operator delete(void *ptr);

    ~WorldAnimInstanceEntry() {}

    WorldAnimInstanceEntry();

    WorldAnimInstanceEntryInfo *mInstanceEntryInfo; // offset 0x8, size 0x4
    CWorldAnimEntityTree *mAnimInstance;            // offset 0xC, size 0x4
};

// total size: 0x50
class WorldAnimInstanceDirectory {
  public:
    WorldAnimInstanceDirectory();

    WorldAnimEntityTreeInfo *GetAnimTreeInfo(uint32 anim_tree_name_hash);

    void AddLoadedAnimTreeInfo(WorldAnimEntityTreeInfo *anim_tree_info);

    WorldAnimEntityInfo *GetAnimEntityInfo(uint32 anim_entity_name_hash);

    void AddLoadedAnimEntityInfo(WorldAnimEntityInfo *anim_entity_info);

    void RemoveAndDeleteAllAnimTreeInfos();

    void RemoveAndDeleteAnimTreeInfo(uint32 tree_name_hash);

    void RemoveEntityInfo(WorldAnimEntityInfo *entity_info);

    void AddLoadedAnimInstance(WorldAnimInstance *instance);

    void RemoveAnimInstance(WorldAnimInstance *instance);

    int GetNumInstanceEntries();

    int GetNumLoadedInstances();

    bPList<WorldAnimInstance> &GetInstanceList();

    void RemoveAndDeleteAllInstanceEntries();

    void AddInstanceEntryFromInfo(WorldAnimInstanceEntryInfo *entry_info);

    void RemoveInstanceEntryAndInfo(WorldAnimInstanceEntryInfo *entry_info);

    void Init();

    ~WorldAnimInstanceDirectory();

    void DeInit(bool full_unload, bool quickrace_drag_restart);

    IControlScenario *GetControlScenario(eControlScenarioType cst);

    CWorldAnimEntityTree *GetAnimationAssociatedWithTriggerZone(uint32 trigger_zone_hash);

    bool IsInitialized() {
        return mInitialized;
    }

    SlotPool *GetWorldAnimEntitySlotPool() {
        return WorldAnimEntitySlotPool;
    }

    SlotPool *GetWorldAnimEntityTreeSlotPool() {
        return WorldAnimEntityTreeSlotPool;
    }

    SlotPool *GetWorldAnimEntityTreeInfoSlotPool() {
        return WorldAnimEntityTreeInfoSlotPool;
    }

    SlotPool *GetWorldAnimCtrlSlotPool() {
        return WorldAnimCtrlSlotPool;
    }

    SlotPool *GetWorldAnimInstanceEntrySlotPool() {
        return WorldAnimInstanceEntrySlotPool;
    }

  private:
    SlotPool *WorldAnimEntitySlotPool;                                       // offset 0x0, size 0x4
    SlotPool *WorldAnimEntityTreeSlotPool;                                   // offset 0x4, size 0x4
    SlotPool *WorldAnimEntityTreeInfoSlotPool;                               // offset 0x8, size 0x4
    SlotPool *WorldAnimCtrlSlotPool;                                         // offset 0xC, size 0x4
    SlotPool *WorldAnimInstanceEntrySlotPool;                                // offset 0x10, size 0x4
    bool mInitialized;                                                       // offset 0x14, size 0x1
    bool mDeInitializing;                                                    // offset 0x18, size 0x1
    IControlScenario *mAnimControlScenarios[3];                              // offset 0x1C, size 0xC
    bPList<WorldAnimEntityInfo> mLoadedWorldAnimEntityInfos;                 // offset 0x28, size 0x8
    bTList<WorldAnimEntityTreeInfo> mLoadedWorldAnimTrees;                   // offset 0x30, size 0x8
    bPList<WorldAnimInstance> mLoadedWorldAnimInstance;                      // offset 0x38, size 0x8
    bPList<WorldAnimInstanceEntryInfo> mResidentWorldAnimInstanceEntryInfos; // offset 0x40, size 0x8
    bTList<WorldAnimInstanceEntry> mResidentWorldAnimInstanceEntries;        // offset 0x48, size 0x8
};

extern WorldAnimInstanceDirectory TheWorldAnimInstanceDirectory;
extern bool DisableWorldAnimations;

#endif
