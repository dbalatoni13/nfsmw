#include "EmitterSystem.h"
#include "EcstasyE.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitterdata.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emittergroup.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribCollection.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

SlotPool *ParticleSlotPool;
SlotPool *EmitterSlotPool;
SlotPool *EmitterGroupSlotPool;
EmitterSystem gEmitterSystem;

unsigned int CrappyStuffThatCantShip[3];

bool IsAShittyEffect(unsigned int group_key) {
    int num_shitty_effects;
    for (int i = 0; i < 3; i++) {
        unsigned int crap_group_key = CrappyStuffThatCantShip[i];
        if (group_key == crap_group_key) {
            return true;
        }
    }
    return false;
}

void ExpandVector(const smVector3 *sv, bVector3 *bv) {
    bv->x = sv->magnitude * (1.0f / 127 * sv->x);
    bv->y = sv->magnitude * (1.0f / 127 * sv->y);
    bv->z = sv->magnitude * (1.0f / 127 * sv->z);
}

void CompressVector(const bVector3 *bv, smVector3 *sv) {
    bVector3 norm;

    float magn = bLength(bv);
    bScale(&norm, &norm, magn * 127.0f);

    sv->x = norm.x;
    sv->y = norm.y;
    sv->z = norm.z;
    sv->magnitude = magn;
}

void NotifyLibOfDeletion(void *lib, EmitterGroup *grp) {
    EmitterLibrary *emlib = reinterpret_cast<EmitterLibrary *>(lib);
    emlib->mGroup = nullptr;
}

unsigned int EmitterGroup::GetNumParticles() {
    if (this->mNumEmitters == 0) {
        return 0;
    }
    unsigned int cnt = 0;
    for (Emitter *em = this->mEmitters.GetHead(); em != this->mEmitters.EndOfList(); em = em->GetNext()) {
        cnt += em->GetNumParticles();
    }
    return cnt;
}

void EmitterSystem::OrphanParticlesFromThisEmitter(Emitter *em) {
    if (em->HasOrphanedParticles()) {
        return;
    }
    bTList<EmitterParticle> &plist = em->GetParticles();

    for (EmitterParticle *particle = plist.GetHead(); particle != plist.EndOfList();) {
        EmitterParticle *next = particle->GetNext();
        particle->mFlags |= 1;
        particle = next;
    }
    em->SetOrphanedParticlesFlag();
}

void EmitterSystem::KillParticlesFromThisEmitter(Emitter *em) {
    bTList<EmitterParticle> &plist = em->GetParticles();

    for (EmitterParticle *particle = plist.GetHead(); particle != plist.EndOfList();) {
        EmitterParticle *next = particle->GetNext();
        this->KillParticle(em, particle);
        particle = next;
    }
}

// TODO move
int GetNumParticleTextures();

// UNSOLVED
void EmitterSystem::KillEverything() {
    for (EmitterGroup *grp = this->mEmitterGroups.GetHead(); grp != this->mEmitterGroups.EndOfList();) {
        EmitterGroup *grp_next = grp->GetNext();
        if (!grp->IsStatic()) {
            delete grp;
        }
        grp = grp_next;
    }
    int total_num_textures = GetNumParticleTextures();
    SetParticleSystemStats(0, 0x400, 0, total_num_textures, gEmitterSystem.GetNumEmitters(), 500, gEmitterSystem.GetNumEmitterGroups(), 200);
}

void InitParticleSlotPool() {
    ParticleSlotPool = bNewSlotPool(0x40, 0x400, "ParticleSlotPool", 0);
    ParticleSlotPool->ClearFlag(SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY);
    ParticleSlotPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
}

void InitEmitterSlotPool() {
    EmitterSlotPool = bNewSlotPool(0x90, 500, "EmitterSlotPool", 0);
    EmitterSlotPool->ClearFlag(SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY);
    EmitterSlotPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
}

void InitEmitterGroupSlotPool() {
    EmitterGroupSlotPool = bNewSlotPool(0x80, 200, "EmitterGroupSlotPool", 0);
    EmitterGroupSlotPool->ClearFlag(SLOTPOOL_FLAG_ZERO_ALLOCATED_MEMORY);
    EmitterGroupSlotPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
}

void EmitterSystem::ServiceWorldEffects() {
    for (std::vector<LibEntry>::iterator it = this->mLibs.begin(); it != this->mLibs.end(); it++) {
        EmitterLibrary *lib = it->Lib;
        if ((lib->SectionNumber != 0) && (lib->SectionNumber != 0xa28)) {
            const Attrib::Collection *spec = Attrib::FindCollection(0xaba86e60, lib->GroupKey);
            Attrib::Gen::emittergroup atr(spec, 0, nullptr);
            float clip_distance_to_use = atr.FarClip() + 20.0f;
            float cos_fov_angle_to_use = 0.55f;
            bool close_enough_soon =
                this->IsCloseEnough(reinterpret_cast<bVector3 *>(&lib->LocalWorld.v3), clip_distance_to_use, 1, cos_fov_angle_to_use);
            if (close_enough_soon) {
                if (!lib->mGroup) {
                    EmitterGroup *emgroup = gEmitterSystem.CreateEmitterGroup(lib->GroupKey, 0x8000000);
                    if (emgroup) {
                        emgroup->SubscribeToDeletion(lib, NotifyLibOfDeletion);
                        lib->mGroup = emgroup;
                        emgroup->SetLocalWorld(&lib->LocalWorld);
                        emgroup->SetAutoUpdate(true);
                    }
                }
            } else if (lib->mGroup) {
                delete lib->mGroup;
            }
        }
    }
}

void EmitterSystem::RefreshWorldEffects() {
    for (WorldFXTrigger *trig = this->mWorldTriggers.GetHead(); trig != this->mWorldTriggers.EndOfList(); trig = trig->GetNext()) {
        trig->mState = 1;
        trig->mLastEnd = 0.0f;
    }
    for (std::vector<LibEntry>::iterator it = this->mLibs.begin(); it != this->mLibs.end(); it++) {
        EmitterLibrary *lib = it->Lib;
        if (!lib->mGroup && (lib->SectionNumber != 0)) {
            EmitterGroup *emgroup = gEmitterSystem.CreateEmitterGroup(lib->GroupKey, 0x8000000);
            if (emgroup) {
                emgroup->SubscribeToDeletion(lib, NotifyLibOfDeletion);
                lib->mGroup = emgroup;
                emgroup->SetLocalWorld(&lib->LocalWorld);
                emgroup->SetAutoUpdate(true);
            }
        }
    }
    GManager::Get().RefreshWorldParticleEffects();
}

void ExpandFXSlotPools() {
    ParticleSlotPool->SetFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    EmitterSlotPool->SetFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    EmitterGroupSlotPool->SetFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);

    ParticleSlotPool->ClearFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
    EmitterSlotPool->ClearFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
    EmitterGroupSlotPool->ClearFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
}

void CollapseFXSlotPools() {
    ParticleSlotPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    EmitterSlotPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    EmitterGroupSlotPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);

    ParticleSlotPool->SetFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
    EmitterSlotPool->SetFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
    EmitterGroupSlotPool->SetFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);

    if (EmitterGroupSlotPool->HasOverflowed()) {
        EmitterGroupSlotPool->CleanupExpandedSlotPools();
    }
    if (EmitterSlotPool->HasOverflowed()) {
        EmitterSlotPool->CleanupExpandedSlotPools();
    }
    if (ParticleSlotPool->HasOverflowed()) {
        ParticleSlotPool->CleanupExpandedSlotPools();
    }
}

// TODO move
bool IsInNIS;

void EmitterSystem_OnStartNIS() {
    if (!IsInNIS) {
        gEmitterSystem.KillEverything();
        ExpandFXSlotPools();
        gEmitterSystem.RefreshWorldEffects();
        IsInNIS = true;
    }
}

void EmitterSystem_OnEndNis() {
    if (IsInNIS) {
        gEmitterSystem.KillEverything();
        CollapseFXSlotPools();
        gEmitterSystem.RefreshWorldEffects();
        IsInNIS = false;
    }
}

EmitterDataAttribWrapper::EmitterDataAttribWrapper(const Attrib::Collection *spec) : mStaticData(spec, 0, nullptr) {
    bMemSet(&this->mColourBasis, 0, sizeof(this->mColourBasis));
    bMemSet(&this->mExtraBasis, 0, sizeof(mExtraBasis));
    this->CalculateBases();
}

EmitterGroupAttribWrapper::EmitterGroupAttribWrapper(const Attrib::Collection *spec) : mStaticData(spec, 0, nullptr) {}

void GetFloatColor(unsigned int clr, bVector4 &clr_out) {
    unsigned int rr = clr >> 0x18;
    unsigned int gg = clr >> 0x10 & 0xff;
    unsigned int bb = clr >> 8 & 0xff;
    unsigned int aa = clr & 0xff;

    clr_out.x = rr * 0.003921569f;
    clr_out.y = gg * 0.003921569f;
    clr_out.z = bb * 0.003921569f;
    clr_out.w = aa * 0.003921569f;
}

void EmitterDataAttribWrapper::CalculateBases() {
    if (!this->mStaticData.IsValid()) {
        return;
    }
    bMatrix4 ColourMatrix;
    unsigned int clr1 = this->mStaticData.Color1();
    unsigned int clr2 = this->mStaticData.Color2();
    unsigned int clr3 = this->mStaticData.Color3();
    unsigned int clr4 = this->mStaticData.Color4();

    GetFloatColor(clr1, ColourMatrix.v0);
    GetFloatColor(clr2, ColourMatrix.v1);
    GetFloatColor(clr3, ColourMatrix.v2);
    GetFloatColor(clr4, ColourMatrix.v3);

    bMatrix4 extra_control_matrix;
    bMemSet(&extra_control_matrix, 0, sizeof(extra_control_matrix));

    float key0 = this->mStaticData.KeyPositions().x * 0.33333334f;
    float key1 = this->mStaticData.KeyPositions().y * 0.33333334f;
    float key2 = this->mStaticData.KeyPositions().z * 0.33333334f;
    float key3 = this->mStaticData.KeyPositions().w * 0.33333334f;

    bVector4 partSize(*reinterpret_cast<bVector4 *>(&this->mStaticData.Size()));
    partSize *= 0.5f;
    extra_control_matrix.v0.x = partSize.x;
    extra_control_matrix.v1.x = partSize.y;
    extra_control_matrix.v2.x = partSize.z;
    extra_control_matrix.v3.x = partSize.w;

    extra_control_matrix.v0.y = this->mStaticData.RelativeAngle().x * 182.04445f;
    extra_control_matrix.v1.y = this->mStaticData.RelativeAngle().y * 182.04445f;
    extra_control_matrix.v2.y = this->mStaticData.RelativeAngle().z * 182.04445f;
    extra_control_matrix.v3.y = this->mStaticData.RelativeAngle().w * 182.04445f;

    hermite_basis(&this->mColourBasis, &ColourMatrix, key0, key1, key2, key3);
    hermite_basis(&this->mExtraBasis, &extra_control_matrix, key0, key1, key2, key3);
}

unsigned int EmitterRandomSeed;

Emitter::Emitter(const Attrib::Collection *spec, EmitterGroup *parent_group) {
    this->mDynamicData = gEmitterSystem.GetEmitterData(spec);
    this->mParticleAccumulation = 0.0f;
    this->mRandomSeed = 0;
    eIdentity(&this->mLocalWorld);
    this->mRandomSeed = static_cast<int>(bRandom(2.1474836e+09f, &EmitterRandomSeed));
    this->mFlags = parent_group->GetFlags() & 0xffff0000;
    this->mMinIntensity = 0.0f;
    this->mGroup = parent_group;
    this->mTexPageTokenNode = nullptr;
    this->mNumParticles = 0;
    this->mMaxIntensity = 1.0f;
    this->mListIndex = this->CalcParticleListIndex();
    this->mTexturePageRange = nullptr;
}

Emitter::~Emitter() {
    this->mParticles.DeleteAllElements();
    if (this->mTexPageTokenNode) {
        this->mTexPageTokenNode->Remove();
        delete this->mTexPageTokenNode;
        this->mTexPageTokenNode = nullptr;
    }
}

unsigned short Emitter::CalcParticleListIndex() {
    unsigned short list_index = this->mDynamicData->GetAttributes().Texture().mIndex;
    return list_index + 2;
}

void Emitter::GetStandardUVs(unsigned int *mUVStart, unsigned int *mUVEnd) {
    *mUVStart = 0;
    *mUVEnd = 0xffffffff;
}

EmitterGroup::EmitterGroup(const Attrib::Collection *spec, unsigned int creation_context_flags) {
    this->mDynamicData = gEmitterSystem.GetEmitterGroup(spec);
    this->mNumEmitters = 0;
    this->mIntensity = 1.0f;
    this->mFlags = creation_context_flags;
    this->SetEnabledFlag();
    eIdentity(&this->mLocalWorld);
    this->mSubscriber = nullptr;
    this->mDeleteCallback = nullptr;
    this->mNumZeroParticleFrames = 0;
    this->mCreationTimeStamp = bGetTicker();
    this->pad = 0;
    if (spec) {
        if (this->SetEmitters(creation_context_flags)) {
            this->SetLoadedFlag();
        } else {
            this->ClearLoadedFlag();
        }
    }
}

EmitterGroup::~EmitterGroup() {
    this->UnloadEmitters(true);
    gEmitterSystem.RemoveEmitterGroup(this);
    if (this->mSubscriber && this->mDeleteCallback) {
        this->mDeleteCallback(this->mSubscriber, this);
    }
}

unsigned int EmitterGroup::NumEmitters() const {
    return this->GetAttribs().Num_Emitters();
}

const Attrib::Gen::emitterdata &Emitter::GetAttributes() const {
    return this->mDynamicData->GetAttributes();
}

void EmitterGroup::SetLocalWorld(const bMatrix4 *m) {
    this->mLocalWorld = *m;
    for (Emitter *em = this->mEmitters.GetHead(); em != this->mEmitters.EndOfList(); em = em->GetNext()) {
        em->SetLocalWorld(m);
    }
}

void EmitterGroup::SetInheritVelocity(const bVector3 *vel) {
    for (Emitter *em = this->mEmitters.GetHead(); em != this->mEmitters.EndOfList(); em = em->GetNext()) {
        em->SetInheritVelocity(vel);
    }
}

void EmitterGroup::Enable() {
    for (Emitter *emitter = this->mEmitters.GetHead(); emitter != this->mEmitters.EndOfList(); emitter = emitter->GetNext()) {
        emitter->Enable();
    }
    this->SetEnabledFlag();
}

void EmitterGroup::Disable() {
    for (Emitter *emitter = this->mEmitters.GetHead(); emitter != this->mEmitters.EndOfList(); emitter = emitter->GetNext()) {
        emitter->Disable();
    }
    this->ClearEnabledFlag();
}

void EmitterLibrary::EndianSwap() {
    bPlatEndianSwap(&this->LocalWorld);
    bPlatEndianSwap(&this->SectionNumber);
    bPlatEndianSwap(&this->mNumTriggers);
}

bool EmitterGroup::MakeOneShot(bool force_all) {
    bool at_least_one_was_oneshot = false;
    bool bVar2;

    Emitter *emitter = this->mEmitters.GetHead();
    do {
        if (emitter == (Emitter *)&this->mEmitters) {
            bVar2 = false;
            if ((force_all) || (at_least_one_was_oneshot)) {
                bVar2 = true;
            }
            return bVar2;
        }
        if (force_all) {
        LAB_80111078:
            emitter->mFlags = emitter->mFlags | 1;
        } else {
            const Attrib::Gen::emitterdata atr = emitter->GetAttributes();
            bool one_shot = force_all;
            if (atr.IsValid() && atr.IsOneShot()) {
                at_least_one_was_oneshot = true;
                one_shot = true;
            }
            if (one_shot)
                goto LAB_80111078;
        }
        emitter = emitter->GetNext();
    } while (true);
}

unsigned short *EmitterLibraryHeader::GetLibraryNumTriggers(int i) {
    // EmitterLibrary *local_r3_44;
    // EmitterLibrary *lib;
    // int ix;

    // local_r3_44 = (EmitterLibrary *)&lib->LocalWorld;
    // if (i == 0) {
    //     return (unsigned short *)((int)&(lib->LocalWorld).v0.z + 2);
    // }
    // for (ix = 0; ix < i; ix++) {
    //     if (ix == i) {
    //         return &local_r3_44->mNumTriggers;
    //     }
    //     local_r3_44 = (EmitterLibrary *)(&local_r3_44[1].GroupKey + (unsigned int)local_r3_44->mNumTriggers * 0xc);
    //     if (ix == i) {
    //         return &local_r3_44->mNumTriggers;
    //     }
    // }
    // return nullptr;
}

EmitterSystem::EmitterSystem() {
    this->mNumEmitterGroups = 0;
    this->mNumEmitters = 0;
    this->mCurrentTexture = nullptr;
    this->mLibs.reserve(400);
    this->mTotalNumParticles = 0;
    bMemSet(this->mParticleListCounts, 0, sizeof(this->mParticleListCounts));
}
