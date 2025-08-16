#ifndef ECSTASY_EMITTER_SYSTEM_H
#define ECSTASY_EMITTER_SYSTEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Ecstasy.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitterdata.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emittergroup.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribCollection.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Texture.hpp"

#define BCHUNK_TPK_SETTINGS 0x0003BD00
#define BCHUNK_EMITTER_SYSTEM 0x0003BC00

struct smVector3 {
    // total size: 0x8
    char x;            // offset 0x0, size 0x1
    char y;            // offset 0x1, size 0x1
    char z;            // offset 0x2, size 0x1
    unsigned char pad; // offset 0x3, size 0x1
    float magnitude;   // offset 0x4, size 0x4

    smVector3() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->pad = 0;
        this->magnitude = 0.0f;
    }
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

    bool Update(float dt, struct Emitter *em, float &rollover_time);

    EmitterControl() {
        this->mState = ECS_NOT_STARTED;
        this->mTime = 0.0f;
    }

    EmitterControlState GetState() {
        return this->mState;
    }
};

extern SlotPool *ParticleSlotPool;

struct EmitterParticle : public bTNode<EmitterParticle> {
    // total size: 0x40
    unsigned int mColour;        // offset 0x8, size 0x4
    float mSize;                 // offset 0xC, size 0x4
    smVector3 mVel;              // offset 0x10, size 0x8
    smVector3 mAcc;              // offset 0x18, size 0x8
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

    void *operator new(size_t size) {
        return bOMalloc(ParticleSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(ParticleSlotPool, ptr);
    }

    EmitterParticle() {
        // TODO maybe in the constructor of bNode?
        this->Next = nullptr;
        this->Prev = nullptr;
        this->mColour = 0;
        this->mSize = 0.0f;
        this->mPosX = 0.0f;
        this->mPosY = 0.0f;
        this->mPosZ = 0.0f;
        this->mFlags = 0;
        this->mUVStart = 0;
        this->mUVEnd = 0;
        this->mLife = 0;
        this->mAngle = 0;
        this->mAnimFrame = 0;
        this->mInitialAngle = 0;
        this->mRotOffset = 0;
    }

    ~EmitterParticle() {}
};

struct EmitterGroup;

class EmitterDataAttribWrapper {
    // total size: 0x94
    const Attrib::Gen::emitterdata mStaticData; // offset 0x0, size 0x14
    bMatrix4 mColourBasis;                      // offset 0x14, size 0x40
    bMatrix4 mExtraBasis;                       // offset 0x54, size 0x40

  public:
    EmitterDataAttribWrapper(const Attrib::Collection *spec);
    void CalculateBases();

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    const Attrib::Gen::emitterdata &GetAttributes() const {
        return mStaticData;
    }

    const bMatrix4 *GetColourBasis() const {
        return &this->mColourBasis;
    }

    const bMatrix4 *GetExtraBasis() const {
        return &this->mExtraBasis;
    }
};

class EmitterGroupAttribWrapper {
    // total size: 0x14
    const Attrib::Gen::emittergroup mStaticData; // offset 0x0, size 0x14

  public:
    EmitterGroupAttribWrapper(const Attrib::Collection *spec);

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    const Attrib::Gen::emittergroup &GetAttributes() const {
        return this->mStaticData;
    }
};

extern SlotPool *EmitterSlotPool;

// TODO move?
struct TexturePageRange {
    // total size: 0x20
    float u0;                      // offset 0x0, size 0x4
    float u1;                      // offset 0x4, size 0x4
    float v0;                      // offset 0x8, size 0x4
    float v1;                      // offset 0xC, size 0x4
    unsigned int flags;            // offset 0x10, size 0x4
    unsigned int texture_namehash; // offset 0x14, size 0x4
    unsigned int pad1;             // offset 0x18, size 0x4
    unsigned int pad2;             // offset 0x1C, size 0x4
};

enum EmitterFlags {
    TRACKED_EMITTER = 32,
    ALREADY_ORPHANED_PARTICLES = 8,
    START_DELAY_ALREADY_DONE = 4,
    DELAYING = 2,
    ONE_SHOT = 1,
};

enum EmitterGroupFlags {
    ENABLED = 16,
    LOADED = 8,
    TRACKED_GROUP = 4,
    IS_STATIC = 2,
    AUTO_UPDATE = 1,
};

struct Emitter : public bTNode<Emitter> {
    // total size: 0x90
    EmitterControl mControl;                // offset 0x8, size 0x8
    float mParticleAccumulation;            // offset 0x10, size 0x4
    unsigned int mRandomSeed;               // offset 0x14, size 0x4
    unsigned int mFlags;                    // offset 0x18, size 0x4
    unsigned short mNumParticles;           // offset 0x1C, size 0x2
    unsigned short mListIndex;              // offset 0x1E, size 0x2
    bMatrix4 mLocalWorld;                   // offset 0x20, size 0x40
    bVector3 mInheritVelocity;              // offset 0x60, size 0x10
    float mMinIntensity;                    // offset 0x70, size 0x4
    float mMaxIntensity;                    // offset 0x74, size 0x4
    TexturePageRange *mTexturePageRange;    // offset 0x78, size 0x4
    EmitterDataAttribWrapper *mDynamicData; // offset 0x7C, size 0x4
    bTList<EmitterParticle> mParticles;     // offset 0x80, size 0x8
    bPNode *mTexPageTokenNode;              // offset 0x88, size 0x4
    EmitterGroup *mGroup;                   // offset 0x8C, size 0x4

  public:
    Emitter(const Attrib::Collection *spec, EmitterGroup *parent_group);

    void *operator new(std::size_t size) {
        return bOMalloc(EmitterSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(EmitterSlotPool, ptr);
    }

    ~Emitter();
    void GetInitialParticleColorAndSize(const bMatrix4 *xtra_basis, const bMatrix4 *clr_basis, EmitterParticle *outParticle) const;
    void GetDiscVelocity(float &x, float &y, float &z, unsigned int &rand_seed) const;
    void GetConeVelocity(float &x, float &y, float &z, unsigned int &rand_seed) const;
    unsigned short CalcParticleListIndex();
    void GetStandardUVs(unsigned int *mUVStart, unsigned int *mUVEnd);
    const Attrib::Gen::emitterdata &GetAttributes() const;
    bool Update(float dt, float &rollover_time);
    void SpawnParticles(float dt, float intensity);

    unsigned int GetNumParticles() {
        return this->mNumParticles;
    }

    bTList<EmitterParticle> &GetParticles() {
        return this->mParticles;
    }

    unsigned short GetParticleListIndex() {
        return this->mListIndex;
    }

    void DecrementNumParticles() {
        this->mNumParticles--;
    }

    bool HasOrphanedParticles() {
        return this->mFlags & ALREADY_ORPHANED_PARTICLES;
    }

    void SetOrphanedParticlesFlag() {
        this->mFlags |= ALREADY_ORPHANED_PARTICLES;
    }

    bool IsEnabled() const {
        return this->mFlags & ENABLED;
    }

    void Enable() {
        this->mFlags |= ENABLED;
    }

    void Disable() {
        this->mFlags &= ~ENABLED;
    }

    unsigned int GetFlags() {
        return this->mFlags;
    }

    void SetInheritVelocity(const bVector3 *vel) {
        this->mInheritVelocity = *vel;
    }

    void SetLocalWorld(const bMatrix4 *local_world) {
        this->mLocalWorld = *local_world;
    }

    void SetIntensityRange(float min, float max) {
        this->mMinIntensity = min;
        this->mMaxIntensity = max;
    }

    void MakeOneShot() {}

    EmitterControlState GetControlState() {
        return this->mControl.GetState();
    };

    EmitterGroup *GetEmitterGroup() {
        return this->mGroup;
    }

    EmitterDataAttribWrapper *GetEmitterData() {
        return this->mDynamicData;
    }
};

extern SlotPool *EmitterGroupSlotPool;

class EmitterGroup : public bTNode<EmitterGroup> {
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

  public:
    EmitterGroup(const Attrib::Collection *spec, unsigned int creation_context_flags);
    ~EmitterGroup();
    unsigned int GetNumParticles();
    void SetLocalWorld(const bMatrix4 *m);
    bool SetEmitters(unsigned int creation_context_flags);
    void UnloadEmitters(bool kill_particles);
    unsigned int NumEmitters() const;
    bool MakeOneShot(bool force_all);
    void SetInheritVelocity(const bVector3 *vel);
    void Enable();
    void Disable();
    void SubscribeToDeletion(void *subscriber, void (*callback)(void *, struct EmitterGroup *));
    void UnSubscribe();
    void DeleteEmitters();
    void Update(float dt);

    EmitterGroup() {}

    void *operator new(std::size_t size) {
        return bOMalloc(EmitterGroupSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(EmitterGroupSlotPool, ptr);
    }

    void SetAutoUpdate(bool val) {
        if (val) {
            this->mFlags |= AUTO_UPDATE;
        } else {
            this->mFlags &= ~AUTO_UPDATE;
        }
    }

    bool IsAutoUpdate() {
        return this->mFlags & AUTO_UPDATE;
    }

    bool IsStatic() {
        return this->mFlags & IS_STATIC;
    }

    bool IsEnabled() {
        return this->mFlags & ENABLED;
    }

    bool IsOldSurfaceEffect() {
        return this->mFlags & 0x80000;
    }

    void SetLoadedFlag() {
        this->mFlags |= LOADED;
    }

    void ClearLoadedFlag() {
        this->mFlags &= ~LOADED;
    }

    void SetEnabledFlag() {
        this->mFlags |= ENABLED;
    }

    void ClearEnabledFlag() {
        this->mFlags &= ~ENABLED;
    }

    unsigned int GetFlags() {
        return this->mFlags;
    }

    bool IsFlagSet(unsigned int flag) {
        return this->mFlags & flag;
    }

    const Attrib::Gen::emittergroup &GetAttribs() const {
        return this->mDynamicData->GetAttributes();
    }

    const bVector3 *GetPosition() {
        return reinterpret_cast<const bVector3 *>(&this->mLocalWorld.v3);
    }

    float GetFarClip() {
        return mFarClip;
    }

    void IncZeroParticleFrame() {
        this->mNumZeroParticleFrames++;
    }

    unsigned int GetNumZeroParticleFrames() {
        return this->mNumZeroParticleFrames;
    }

    unsigned int CurrentNumEmitters() const {
        return this->mNumEmitters;
    }

    bTList<Emitter> &GetEmitters() {
        return this->mEmitters;
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

    void EndianSwap();
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

    void EndianSwap() {
        bPlatEndianSwap(&this->mTriggerRadius);
        bPlatEndianSwap(&this->mWorldPos);
        bPlatEndianSwap(&this->mResetTime);
        bPlatEndianSwap(&this->mProbability);
        bPlatEndianSwap(&this->mState); // or mFlags?
    }
};

struct EmitterLibraryHeader {
    // total size: 0x10
    BOOL EndianSwapped;      // offset 0x0, size 0x4
    int Version;             // offset 0x4, size 0x4
    int NumEmitterLibraries; // offset 0x8, size 0x4
    int SectionNumber;       // offset 0xC, size 0x4

    unsigned short *GetLibraryNumTriggers(int i);
    WorldFXTrigger *GetLibraryTriggers(int i);
    EmitterLibrary *GetLibrary(int i);
    void EndianSwap();
};

struct EmitterPackHeader {
    // total size: 0x10
    int SectionNumber;    // offset 0x0, size 0x4
    int EndianSwapped;    // offset 0x4, size 0x4
    int Version;          // offset 0x8, size 0x4
    int NumEmitterGroups; // offset 0xC, size 0x4
};

class EmitterSystem {
    struct LibEntry {
        unsigned int Key;
        EmitterLibrary *Lib;

        bool operator<(const LibEntry &other) const {
            return this->Key < other.Key;
        }
    };

    static TexturePageRange *mTextureRanges;
    static int mNumTextureRanges;

    // total size: 0x3AC
    int mTotalNumParticles;                                                               // offset 0x0, size 0x4
    int mParticleListCounts[66];                                                          // offset 0x4, size 0x108
    bPList<Emitter> mParticleLists[66];                                                   // offset 0x10C, size 0x210
    bVector3 mInterestPoints[2];                                                          // offset 0x31C, size 0x20
    bVector3 mInterestVectors[2];                                                         // offset 0x33C, size 0x20
    UTL::Std::map<unsigned int, EmitterDataAttribWrapper *, _type_map> mEmitterDataMap;   // offset 0x35C, size 0x10
    UTL::Std::map<unsigned int, EmitterGroupAttribWrapper *, _type_map> mEmitterGroupMap; // offset 0x36C, size 0x10
    bTList<EmitterGroup> mEmitterGroups;                                                  // offset 0x37C, size 0x8
    bTList<WorldFXTrigger> mWorldTriggers;                                                // offset 0x384, size 0x8
    int mNumTriggers;                                                                     // offset 0x38C, size 0x4
    TextureInfo *mCurrentTexture;                                                         // offset 0x390, size 0x4
    unsigned int mNumEmitterGroups;                                                       // offset 0x394, size 0x4
    unsigned int mNumEmitters;                                                            // offset 0x398, size 0x4
    UTL::Std::vector<EmitterSystem::LibEntry, _type_vector> mLibs;                        // offset 0x39C, size 0x10

  public:
    static BOOL Loader(bChunk *bchunk);
    static BOOL Unloader(bChunk *bchunk);
    static int TexturePageLoader(bChunk *bchunk);
    static int TexturePageUnloader(bChunk *bchunk);
    static void SetTexturePageRanges(int num_ranges, TexturePageRange *ranges);

    EmitterSystem();
    void OrphanParticlesFromThisEmitter(Emitter *em);
    void KillParticlesFromThisEmitter(Emitter *em);
    EmitterParticle *GetNewParticle(Emitter *spawning_emitter);
    void KillParticle(Emitter *em, EmitterParticle *particle);
    void KillEverything();
    void ServiceWorldEffects();
    void RefreshWorldEffects();
    EmitterGroup *CreateEmitterGroup(const Attrib::StringKey &group_name, unsigned int creation_context_flags);
    EmitterGroup *CreateEmitterGroup(const unsigned int &group_key, unsigned int creation_context_flags);
    EmitterGroup *CreateEmitterGroup(const Attrib::Collection *group_spec, unsigned int creation_context_flags);
    void AddEmitterGroup(EmitterGroup *group);
    void RemoveEmitterGroup(EmitterGroup *group);
    void UpdateInterestPoints();
    void UpdateParticles(float dt);
    EmitterLibrary *FindLibrary(unsigned int key);
    void AddLibrary(EmitterLibrary *lib);
    void RemoveLibrary(EmitterLibrary *lib);
    void Render(eView *view);
    void Init();
    EmitterDataAttribWrapper *GetEmitterData(const Attrib::Collection *spec);
    EmitterGroupAttribWrapper *GetEmitterGroup(const Attrib::Collection *spec);
    bool IsCloseEnough(const bVector3 *group_pos, float farclip, int frustrum, float cos_angle_fov) const;
    bool IsCloseEnough(const bVector4 *group_pos, float farclip, int frustrum, float cos_angle_fov) const;
    bool IsCloseEnough(EmitterGroup *group, int frustrum, float cos_angle_fov) const;
    void KillEffectsMatchingFlag(unsigned int flags_to_match);
    void Update(float dt);

    int GetNumEmitters() {
        return this->mNumEmitters;
    }

    int GetNumEmitterGroups() {
        return this->mNumEmitterGroups;
    }

    void OnDeleteEmitter() {
        this->mNumEmitters--;
    }

    bTList<WorldFXTrigger> &GetTriggers() {
        return this->mWorldTriggers;
    }

    void AddTrigger(WorldFXTrigger *trig) {
        this->mWorldTriggers.AddTail(trig);
        this->mNumTriggers++;
    }

    void KillTrigger(WorldFXTrigger *trig) {
        trig->Remove();
        this->mNumTriggers--;
    }
};

#endif
