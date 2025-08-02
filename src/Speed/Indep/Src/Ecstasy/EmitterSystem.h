#ifndef ECSTASY_EMITTER_SYSTEM_H
#define ECSTASY_EMITTER_SYSTEM_H

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTLContainer.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitterdata.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emittergroup.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribCollection.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
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

    EmitterControl() {
        this->mState = ECS_NOT_STARTED;
        this->mTime = 0.0f;
    }
};

extern SlotPool *ParticleSlotPool;

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

    void operator delete(void *ptr) {
        bFree(ParticleSlotPool, ptr);
    }

    ~EmitterParticle() {}
};

struct EmitterGroup;

class EmitterDataAttribWrapper {
    // total size: 0x94
  private:
    const Attrib::Gen::emitterdata mStaticData; // offset 0x0, size 0x14
  public:
    bMatrix4 mColourBasis; // offset 0x14, size 0x40
    bMatrix4 mExtraBasis;  // offset 0x54, size 0x40

    EmitterDataAttribWrapper(const Attrib::Collection *spec);
    void CalculateBases();

    const Attrib::Gen::emitterdata &GetAttributes() const {
        return mStaticData;
    }
};

struct EmitterGroupAttribWrapper {
    // total size: 0x14
    const Attrib::Gen::emittergroup mStaticData; // offset 0x0, size 0x14

    EmitterGroupAttribWrapper(const Attrib::Collection *spec);
};

extern SlotPool *EmitterSlotPool;

struct Emitter : public bTNode<Emitter> {
    // total size: 0x90
  private:
    EmitterControl mControl;                    // offset 0x8, size 0x8
    float mParticleAccumulation;                // offset 0x10, size 0x4
    unsigned int mRandomSeed;                   // offset 0x14, size 0x4
    unsigned int mFlags;                        // offset 0x18, size 0x4
    unsigned short mNumParticles;               // offset 0x1C, size 0x2
    unsigned short mListIndex;                  // offset 0x1E, size 0x2
    bMatrix4 mLocalWorld;                       // offset 0x20, size 0x40
    bVector3 mInheritVelocity;                  // offset 0x60, size 0x10
    float mMinIntensity;                        // offset 0x70, size 0x4
    float mMaxIntensity;                        // offset 0x74, size 0x4
    struct TexturePageRange *mTexturePageRange; // offset 0x78, size 0x4
    EmitterDataAttribWrapper *mDynamicData;     // offset 0x7C, size 0x4
    bTList<EmitterParticle> mParticles;         // offset 0x80, size 0x8
    bPNode *mTexPageTokenNode;                  // offset 0x88, size 0x4
    EmitterGroup *mGroup;                       // offset 0x8C, size 0x4

  public:
    Emitter(const Attrib::Collection *spec, EmitterGroup *parent_group);

    void operator delete(void *ptr) {
        bFree(EmitterSlotPool, ptr);
    }

    ~Emitter();
    unsigned short CalcParticleListIndex();
    void GetStandardUVs(unsigned int *mUVStart, unsigned int *mUVEnd);

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

extern SlotPool *EmitterGroupSlotPool;

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
    EmitterGroupAttribWrapper *mDynamicData;         // offset 0x70, size 0x4
    unsigned int mNumZeroParticleFrames;             // offset 0x74, size 0x4
    unsigned int mCreationTimeStamp;                 // offset 0x78, size 0x4
    unsigned int pad;                                // offset 0x7C, size 0x4

    EmitterGroup(const Attrib::Collection *spec, unsigned int creation_context_flags);
    ~EmitterGroup();
    unsigned int GetNumParticles();
    void SubscribeToDeletion(void *subscriber, void (*callback)(void *, struct EmitterGroup *));
    void SetLocalWorld(const bMatrix4 *m);
    bool SetEmitters(unsigned int creation_context_flags);
    void UnloadEmitters(bool kill_particles);

    EmitterGroup() {}
    void *operator new(size_t size) {}
    void operator delete(void *ptr) {
        bFree(EmitterGroupSlotPool, ptr);
    }

    bool IsStatic() {
        return this->mFlags & 2;
    }

    void SetAutoUpdate(bool val) {
        if (val) {
            this->mFlags |= 1;
        } else {
            this->mFlags &= ~1;
        }
    }

    void SetEnabledFlag() {
        this->mFlags |= 16;
    }

    void SetLoadedFlag() {
        this->mFlags |= 8;
    }

    void ClearLoadedFlag() {
        this->mFlags &= ~8;
    }

    unsigned int GetFlags() {
        return this->mFlags;
    }
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

// TODO right place?
struct WorldFXTrigger : public bTNode<WorldFXTrigger> {
    // total size: 0x30
    EmitterLibrary *mLib;        // offset 0x8, size 0x4
    float mTriggerRadius;        // offset 0xC, size 0x4
    bVector3 mWorldPos;          // offset 0x10, size 0x10
    float mResetTime;            // offset 0x20, size 0x4
    unsigned short mProbability; // offset 0x24, size 0x2
    unsigned char mState;        // offset 0x26, size 0x1
    unsigned char mFlags;        // offset 0x27, size 0x1
    float mLastEnd;              // offset 0x28, size 0x4
    unsigned int pad;            // offset 0x2C, size 0x4
};

class EmitterSystem {
    struct LibEntry {
        unsigned int Key;
        EmitterLibrary *Lib;
    };

    // total size: 0x3AC
    int mTotalNumParticles;                                                                        // offset 0x0, size 0x4
    int mParticleListCounts[66];                                                                   // offset 0x4, size 0x108
    bPList<Emitter> mParticleLists[66];                                                            // offset 0x10C, size 0x210
    bVector3 mInterestPoints[2];                                                                   // offset 0x31C, size 0x20
    bVector3 mInterestVectors[2];                                                                  // offset 0x33C, size 0x20
    UTL::Container::map<unsigned int, EmitterDataAttribWrapper *, _type_vector> mEmitterDataMap;   // offset 0x35C, size 0x10
    UTL::Container::map<unsigned int, EmitterGroupAttribWrapper *, _type_vector> mEmitterGroupMap; // offset 0x36C, size 0x10
    bTList<EmitterGroup> mEmitterGroups;                                                           // offset 0x37C, size 0x8
    bTList<WorldFXTrigger> mWorldTriggers;                                                         // offset 0x384, size 0x8
    int mNumTriggers;                                                                              // offset 0x38C, size 0x4
    TextureInfo *mCurrentTexture;                                                                  // offset 0x390, size 0x4
    unsigned int mNumEmitterGroups;                                                                // offset 0x394, size 0x4
    unsigned int mNumEmitters;                                                                     // offset 0x398, size 0x4
    UTL::Container::vector<EmitterSystem::LibEntry, _type_vector> mLibs;                           // offset 0x39C, size 0x10

  public:
    void OrphanParticlesFromThisEmitter(Emitter *em);
    void KillParticlesFromThisEmitter(Emitter *em);
    void KillParticle(Emitter *em, EmitterParticle *particle);
    void KillEverything();
    void ServiceWorldEffects();
    void RefreshWorldEffects();
    bool IsCloseEnough(const bVector3 *group_pos, float farclip, int frustrum, float cos_angle_fov);
    EmitterGroup *CreateEmitterGroup(const Attrib::StringKey &group_name, unsigned int creation_context_flags);
    EmitterGroup *CreateEmitterGroup(const unsigned int &group_key, unsigned int creation_context_flags);
    EmitterGroup *CreateEmitterGroup(const Attrib::Collection *group_spec, unsigned int creation_context_flags);
    EmitterGroupAttribWrapper *GetEmitterGroup(const Attrib::Collection *spec);
    void RemoveEmitterGroup(EmitterGroup *group);
    EmitterDataAttribWrapper *GetEmitterData(const Attrib::Collection *spec);

    int GetNumEmitters() {
        return this->mNumEmitters;
    }

    int GetNumEmitterGroups() {
        return this->mNumEmitterGroups;
    }
};

#endif
