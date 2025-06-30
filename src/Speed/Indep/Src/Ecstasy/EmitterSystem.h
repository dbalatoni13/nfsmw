#ifndef ECSTASY_EMITTER_SYSTEM_H
#define ECSTASY_EMITTER_SYSTEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Texture.hpp"

struct smVector3 {
    // total size: 0x8
    char x;            // offset 0x0, size 0x1
    char y;            // offset 0x1, size 0x1
    char z;            // offset 0x2, size 0x1
    unsigned char pad; // offset 0x3, size 0x1
    float magnitude;   // offset 0x4, size 0x4

    smVector3() {}
};

enum EmitterControlState {
    ECS_OFF = 6,
    ECS_ON = 5,
    ECS_OFF_CYCLE = 4,
    ECS_ON_CYCLE = 3,
    ECS_DELAYING = 2,
    ECS_NOT_STARTED = 1,
    ECS_ERROR = 0,
};

struct EmitterControl {
    // total size: 0x8
    EmitterControlState mState; // offset 0x0, size 0x4
    float mTime;                // offset 0x4, size 0x4
};

struct EmitterParticle : public bTNode<EmitterParticle> {
    // total size: 0x40
    unsigned int mColour;        // offset 0x8, size 0x4
    float mSize;                 // offset 0xC, size 0x4
    struct smVector3 mVel;       // offset 0x10, size 0x8
    struct smVector3 mAcc;       // offset 0x18, size 0x8
    float mPosX;                 // offset 0x20, size 0x4
    float mPosY;                 // offset 0x24, size 0x4
    float mPosZ;                 // offset 0x28, size 0x4
    unsigned int mFlags;         // offset 0x2C, size 0x4
    unsigned int mUVStart;       // offset 0x30, size 0x4
    unsigned int mUVEnd;         // offset 0x34, size 0x4
    unsigned short mLife;        // offset 0x38, size 0x2
    unsigned short mAngle;       // offset 0x3A, size 0x2
    unsigned short mAnimFrame;   // offset 0x3C, size 0x2
    unsigned char mInitialAngle; // offset 0x3E, size 0x1
    unsigned char mRotOffset;    // offset 0x3F, size 0x1
};

struct EmitterGroup;

struct Emitter : public bTNode<Emitter> {
    // total size: 0x90
  private:
    EmitterControl mControl;                       // offset 0x8, size 0x8
    float mParticleAccumulation;                   // offset 0x10, size 0x4
    unsigned int mRandomSeed;                      // offset 0x14, size 0x4
    unsigned int mFlags;                           // offset 0x18, size 0x4
    unsigned short mNumParticles;                  // offset 0x1C, size 0x2
    unsigned short mListIndex;                     // offset 0x1E, size 0x2
    bMatrix4 mLocalWorld;                          // offset 0x20, size 0x40
    bVector3 mInheritVelocity;                     // offset 0x60, size 0x10
    float mMinIntensity;                           // offset 0x70, size 0x4
    float mMaxIntensity;                           // offset 0x74, size 0x4
    struct TexturePageRange *mTexturePageRange;    // offset 0x78, size 0x4
    struct EmitterDataAttribWrapper *mDynamicData; // offset 0x7C, size 0x4
    bTList<EmitterParticle> mParticles;            // offset 0x80, size 0x8
    bPNode *mTexPageTokenNode;                     // offset 0x88, size 0x4
    EmitterGroup *mGroup;                          // offset 0x8C, size 0x4

  public:
    unsigned int GetNumParticles() {
        return mNumParticles;
    }

    bTList<EmitterParticle> &GetParticles() {
        return mParticles;
    }

    bool HasOrphanedParticles() {
        return this->mFlags & 8;
    }

    void SetOrphanedParticlesFlag() {
        this->mFlags |= 8;
    }
};

struct EmitterGroup : public bTNode<EmitterGroup> {
    // total size: 0x80
    struct bTList<Emitter> mEmitters;                // offset 0x8, size 0x8
    unsigned int mGroupKey;                          // offset 0x10, size 0x4
    unsigned int Padding;                            // offset 0x14, size 0x4
    unsigned int mFlags;                             // offset 0x18, size 0x4
    unsigned short mNumEmitters;                     // offset 0x1C, size 0x2
    unsigned short mSectionNumber;                   // offset 0x1E, size 0x2
    bMatrix4 mLocalWorld;                            // offset 0x20, size 0x40
    void *mSubscriber;                               // offset 0x60, size 0x4
    float mFarClip;                                  // offset 0x64, size 0x4
    float mIntensity;                                // offset 0x68, size 0x4
    void (*mDeleteCallback)(void *, EmitterGroup *); // offset 0x6C, size 0x4
    struct EmitterGroupAttribWrapper *mDynamicData;  // offset 0x70, size 0x4
    unsigned int mNumZeroParticleFrames;             // offset 0x74, size 0x4
    unsigned int mCreationTimeStamp;                 // offset 0x78, size 0x4
    unsigned int pad;                                // offset 0x7C, size 0x4

    unsigned int GetNumParticles();
};

struct EmitterLibrary {
    // total size: 0x50
    unsigned int GroupKey;        // offset 0x0, size 0x4
    unsigned int Padding;         // offset 0x4, size 0x4
    unsigned short SectionNumber; // offset 0x8, size 0x2
    unsigned short mNumTriggers;  // offset 0xA, size 0x2
    EmitterGroup *mGroup;         // offset 0xC, size 0x4
    bMatrix4 LocalWorld;          // offset 0x10, size 0x40
};

struct EmitterLibraryHeader {
    // total size: 0x10
    int EndianSwapped;       // offset 0x0, size 0x4
    int Version;             // offset 0x4, size 0x4
    int NumEmitterLibraries; // offset 0x8, size 0x4
    int SectionNumber;       // offset 0xC, size 0x4
};

struct EmitterSystem {
    // total size: 0x3AC
    int mTotalNumParticles;             // offset 0x0, size 0x4
    int mParticleListCounts[66];        // offset 0x4, size 0x108
    bPList<Emitter> mParticleLists[66]; // offset 0x10C, size 0x210
    bVector3 mInterestPoints[2];        // offset 0x31C, size 0x20
    bVector3 mInterestVectors[2];       // offset 0x33C, size 0x20
    // struct map<unsigned int, EmitterDataAttribWrapper *, _type_map> mEmitterDataMap;   // offset 0x35C, size 0x10
    // struct map<unsigned int, EmitterGroupAttribWrapper *, _type_map> mEmitterGroupMap; // offset 0x36C, size 0x10
    bTList<EmitterGroup> mEmitterGroups; // offset 0x37C, size 0x8
    // bTList<WorldFXTrigger> mWorldTriggers; // offset 0x384, size 0x8
    int mNumTriggers;               // offset 0x38C, size 0x4
    TextureInfo *mCurrentTexture;   // offset 0x390, size 0x4
    unsigned int mNumEmitterGroups; // offset 0x394, size 0x4
    unsigned int mNumEmitters;      // offset 0x398, size 0x4
    // struct vector<EmitterSystem::LibEntry, _type_vector> mLibs;                        // offset 0x39C, size 0x10

    void OrphanParticlesFromThisEmitter(Emitter *em);
    void KillParticlesFromThisEmitter(Emitter *em);
    void KillParticle(Emitter *em, EmitterParticle *particle);
    void KillEverything();
};

#endif
