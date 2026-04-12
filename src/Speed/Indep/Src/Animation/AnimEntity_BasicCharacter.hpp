#ifndef ANIMATION_ANIMENTITY_BASICCHARACTER_H
#define ANIMATION_ANIMENTITY_BASICCHARACTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimCtrl.hpp"
#include "AnimEntity.hpp"

typedef struct {
    uint32 mTypeID;                  // offset 0x0, size 0x4
    uint32 mThisInstanceNameHash;    // offset 0x4, size 0x4
    uint32 mParentInstanceNameHash;  // offset 0x8, size 0x4
    uint32 mPlayFlags;               // offset 0xC, size 0x4
    bMatrix4 mLocalMatrix;           // offset 0x10, size 0x40
    uint32 mLODNameHash[4];          // offset 0x50, size 0x10
    uint32 mTransAnimNameHash;       // offset 0x60, size 0x4
    uint32 mQuatsAnimNameHash;       // offset 0x64, size 0x4
    uint32 mScaleAnimNameHash;       // offset 0x68, size 0x4
    uint32 mSkelAnimNameHash;        // offset 0x6C, size 0x4
    uint32 mSkelNameHash;            // offset 0x70, size 0x4
    float mPlaySpeed;                // offset 0x74, size 0x4
    float mPlayDelay;                // offset 0x78, size 0x4
    uint32 mWorldObjectTypeNameHash; // offset 0x7C, size 0x4
    uint32 mLoopRangeStart;          // offset 0x80, size 0x4
    uint32 mLoopRangeEnd;            // offset 0x84, size 0x4
    uint32 mPad1;                    // offset 0x88, size 0x4
    uint32 mPad2;                    // offset 0x8C, size 0x4
} BasicCharacterAnimEntityInfo;

// total size: 0x38
class CBasicCharacterAnimEntity : public IAnimEntity {
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

    CAnimCtrl *GetAnimCtrl() {
        return mAnimCtrl;
    }

    static void EndianSwapEntityData(void *data, int size);

    CBasicCharacterAnimEntity();

    // Overrides: IAnimEntity
    ~CBasicCharacterAnimEntity() override;

    // Overrides: IAnimEntity
    bool Init(void *init_data, SpaceNode *parent_space_node) override;

    // Overrides: IAnimEntity
    void Purge() override;

    void Play();

    void Stop();

    bool IsPlaying();

    // Overrides: IAnimEntity
    void SetTime(float time) override;

    // Overrides: IAnimEntity
    void UpdateTimeStep(float time_step) override;

    // Overrides: IAnimEntity
    void RenderEffects(eView *view, int is_reflection) override;

    void SetCurrentEvalTime(float time, bool in_seconds);

    float GetCurrentEvalTime(bool in_seconds);

    float GetNumSecondsUntilThisFrame(float frame_number);

    float GetNumSecondsBetweenFrames(float start_frame, float end_frame);

    void FindWorldBonePosition(int bone, bVector3 *position);

  private:
    uint32 mTypeID;                                    // offset 0x4, size 0x4
    uint32 mThisInstanceNameHash;                      // offset 0x8, size 0x4
    SpaceNode *mSpaceNode;                             // offset 0xC, size 0x4
    WorldModel *mWorldModel;                           // offset 0x10, size 0x4
    CAnimCtrl *mAnimCtrl;                              // offset 0x14, size 0x4
    bool mKeepOnGround;                                // offset 0x18, size 0x1
    bool mHavePreviousElevation;                       // offset 0x1C, size 0x1
    bool mDrawShadow;                                  // offset 0x20, size 0x1
    float mPreviousElevation;                          // offset 0x24, size 0x4
    int mBoneMapType;                                  // offset 0x28, size 0x4
    eReplacementTextureTable mReplacementTextureTable; // offset 0x2C, size 0xC
};

void InitCharacterEffects();

void CloseCharacterEffects();

#endif
