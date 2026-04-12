#ifndef ANIMATION_ANIMENTITY_PROP_H
#define ANIMATION_ANIMENTITY_PROP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimEntity.hpp"

// total size: 0x60
typedef struct {
    uint32 mTypeID;                 // offset 0x0, size 0x4
    uint32 mThisInstanceNameHash;   // offset 0x4, size 0x4
    uint32 mParentInstanceNameHash; // offset 0x8, size 0x4
    uint32 mPlayFlags;              // offset 0xC, size 0x4
    struct bMatrix4 mLocalMatrix;   // offset 0x10, size 0x40
    uint32 mLODNameHash[4];         // offset 0x50, size 0x10
} PropAnimEntityInfo;

// total size: 0x18
class CPropAnimEntity : public IAnimEntity {
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

    static void EndianSwapEntityData(void *data, int size);

    CPropAnimEntity();

    // Overrides: IAnimEntity
    ~CPropAnimEntity() override;

    // Overrides: IAnimEntity
    bool Init(void *init_data, SpaceNode *parent_space_node) override;

    // Overrides: IAnimEntity
    void Purge() override;

    // Overrides: IAnimEntity
    void SetTime(float time) override;

    // Overrides: IAnimEntity
    void UpdateTimeStep(float time_step) override;

  private:
    uint32 mTypeID;               // offset 0x4, size 0x4
    uint32 mThisInstanceNameHash; // offset 0x8, size 0x4
    SpaceNode *mSpaceNode;        // offset 0xC, size 0x4
    WorldModel *mWorldModel;      // offset 0x10, size 0x4
    bool mKeepOnGround;           // offset 0x14, size 0x1
};

#endif
