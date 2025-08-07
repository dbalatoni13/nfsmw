#include "EmitterSystem.h"
#include "EcstasyE.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitterdata.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emittergroup.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribCollection.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "eMath.hpp"
#include "stl/_algo.h"

#include <algorithm>

SlotPool *ParticleSlotPool;
SlotPool *EmitterSlotPool;
SlotPool *EmitterGroupSlotPool;
EmitterSystem gEmitterSystem;

unsigned int CrappyStuffThatCantShip[3];

bool IsAShittyEffect(unsigned int group_key) {
    int num_shitty_effects = 3;
    for (int i = 0; i < num_shitty_effects; i++) {
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

    sv->x = static_cast<char>(norm.x);
    sv->y = static_cast<char>(norm.y);
    sv->z = static_cast<char>(norm.z);
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

// TODO move?
enum EffectParticleConstraint {
    CONSTRAIN_PARTICLE_CAMERA = 8,
    CONSTRAIN_PARTICLE_YZ_AXIS = 3,
    CONSTRAIN_PARTICLE_XZ_AXIS = 5,
    CONSTRAIN_PARTICLE_XY_AXIS = 6,
    CONSTRAIN_PARTICLE_NONE = 0,
};

bool GetConstraintBasis(EffectParticleConstraint constraint, bVector4 &x_basis, bVector4 &y_basis, unsigned short &particle_angle,
                        bMatrix4 *world_view) {
    bool res = true;

    switch (constraint) {
    case CONSTRAIN_PARTICLE_XY_AXIS:
        x_basis = bVector4(1.0f, 0.0f, 0.0f, 0.0f);
        y_basis = bVector4(0.0f, 1.0f, 0.0f, 0.0f);
        if (particle_angle != 0) {
            float cos = bCos(particle_angle);
            float sin = bSin(particle_angle);
            x_basis.x = cos;
            x_basis.y = sin;
            x_basis.z = 0.0f;
            x_basis.w = 0.0f;

            y_basis.x = -sin;
            y_basis.y = cos;
            y_basis.z = 0.0f;
            y_basis.w = 0.0f;
        }
        break;
    case CONSTRAIN_PARTICLE_XZ_AXIS:
        x_basis = bVector4(1.0f, 0.0f, 0.0f, 0.0f);
        y_basis = bVector4(0.0f, 0.0f, 1.0f, 0.0f);
        if (particle_angle != 0) {
            float cos = bCos(particle_angle);
            float sin = bSin(particle_angle);
            x_basis.x = cos;
            x_basis.y = 0.0f;
            x_basis.z = -sin;
            x_basis.w = 0.0f;

            y_basis.x = sin;
            y_basis.y = 0.0f;
            y_basis.z = cos;
            y_basis.w = 0.0f;
        }
        break;
    case CONSTRAIN_PARTICLE_YZ_AXIS:
        x_basis = bVector4(0.0f, 1.0f, 0.0f, 0.0f);
        y_basis = bVector4(0.0f, 0.0f, 1.0f, 0.0f);
        if (particle_angle != 0) {
            float cos = bCos(particle_angle);
            float sin = bSin(particle_angle);
            x_basis.x = 0.0f;
            x_basis.y = cos;
            x_basis.z = sin;
            x_basis.w = 0.0f;

            y_basis.x = 0.0f;
            y_basis.y = -sin;
            y_basis.z = cos;
            y_basis.w = 0.0f;
        }
        break;
    case CONSTRAIN_PARTICLE_CAMERA:
        if (world_view) {
            x_basis = bVector4(world_view->v0.x, world_view->v1.x, world_view->v2.x, 0.0f);
            y_basis = bVector4(world_view->v0.y, world_view->v1.y, world_view->v2.y, 0.0f);
            if (particle_angle != 0) {
                bMatrix4 rz;
                eCreateRotationZ(&rz, particle_angle);
                eMulVector(&x_basis, &rz, &x_basis);
                eMulVector(&y_basis, &rz, &y_basis);
            }
        } else {
            res = false;
        }
        break;
    case CONSTRAIN_PARTICLE_NONE:
    default:
        res = false;
        break;
    }
    return res;
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

void EmitterSystem::KillEverything() {
    for (EmitterGroup *grp = this->mEmitterGroups.GetHead(); grp != this->mEmitterGroups.EndOfList();) {
        EmitterGroup *grp_next = grp->GetNext();
        if (!grp->IsStatic()) {
            delete grp;
            grp = grp_next;
        }
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

    float key0 = this->mStaticData.KeyPositions().x / 3;
    float key1 = this->mStaticData.KeyPositions().y / 3;
    float key2 = this->mStaticData.KeyPositions().z / 3;
    float key3 = this->mStaticData.KeyPositions().w / 3;

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

void Emitter::GetInitialParticleColorAndSize(const bMatrix4 *xtra_basis, const bMatrix4 *clr_basis, EmitterParticle *outParticle) const {
    const bMatrix4 *ExtraBasis = xtra_basis;
    const bMatrix4 *ColourBasis = clr_basis;

    bVector4 extra_params;
    hermite_parameter(&extra_params, ExtraBasis, 0.0f);
    outParticle->mSize = extra_params.x;

    unsigned short pangle = outParticle->mAngle;
    unsigned short adelta = static_cast<unsigned short>(extra_params.y);
    if (pangle % 2 == 0) {
        pangle += adelta;
    } else {
        pangle -= adelta;
    }
    outParticle->mAngle = pangle;

    bVector4 col;
    hermite_parameter(&col, ColourBasis, 0.0f);
    col *= 255.0f;
    if (col.x < 0.0f) {
        col.x = 0.0f;
    }
    if (col.y < 0.0f) {
        col.y = 0.0f;
    }
    if (col.z < 0.0f) {
        col.z = 0.0f;
    }
    if (col.w < 0.0f) {
        col.w = 0.0f;
    }
    if (col.x > 255.0f) {
        col.x = 255.0f;
    }
    if (col.y > 255.0f) {
        col.y = 255.0f;
    }
    if (col.z > 255.0f) {
        col.z = 255.0f;
    }
    if (col.w > 255.0f) {
        col.w = 255.0f;
    }
    unsigned int r = static_cast<int>(col.x);
    unsigned int g = static_cast<int>(col.y);
    unsigned int b = static_cast<int>(col.z);
    unsigned int a = static_cast<int>(col.w);
    outParticle->mColour = r << 24 | g << 16 | b << 8 | a;
}

void Emitter::GetDiscVelocity(float &x, float &y, float &z, unsigned int &rand_seed) const {
    float full_spread_angle = this->mDynamicData->GetAttributes().SpreadAngle();
    float half_spread_angle = full_spread_angle * 0.5f;

    bMatrix4 orientation(this->mLocalWorld);
    orientation.v3.x = 0.0f;
    orientation.v3.w = 1.0f;
    orientation.v3.y = 0.0f;
    orientation.v3.z = 0.0f;

    bMatrix4 *identity = eGetIdentityMatrix();
    bMatrix4 rott;
    bVector4 pdir;
    unsigned short angle_t = bDegToAng(bRandom(full_spread_angle, &rand_seed) - half_spread_angle);
    unsigned short angle_z = bDegToAng(bRandom(360.0f, &rand_seed) - 180.0f);

    pdir.x = bCos(angle_z);
    pdir.y = bSin(angle_z);
    pdir.z = 0.0f;

    eRotateY(&rott, identity, angle_t);
    eMulVector(&pdir, &rott, &pdir);
    eMulVector(&pdir, &orientation, &pdir);

    x = pdir.x;
    y = pdir.y;
    z = pdir.z;
}

void Emitter::GetConeVelocity(float &x, float &y, float &z, unsigned int &rand_seed) const {
    float full_spread_angle = this->mDynamicData->GetAttributes().SpreadAngle();
    float half_spread_angle = full_spread_angle * 0.5f;
    const bVector4 *direction = &this->mLocalWorld.v2;
    bMatrix4 *identity = eGetIdentityMatrix();
    bMatrix4 rot;
    bVector4 pdir;

    unsigned short angle_x = bDegToAng(bRandom(full_spread_angle, &rand_seed) - half_spread_angle);
    unsigned short angle_y = bDegToAng(bRandom(full_spread_angle, &rand_seed) - half_spread_angle);
    unsigned short angle_z = bDegToAng(bRandom(full_spread_angle, &rand_seed) - half_spread_angle);
    eRotateX(&rot, identity, angle_x);
    eRotateY(&rot, &rot, angle_y);
    eRotateZ(&rot, &rot, angle_z);
    eMulVector(&pdir, &rot, direction);

    x = pdir.x;
    y = pdir.y;
    z = pdir.z;
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

bool EmitterGroup::SetEmitters(unsigned int creation_context_flags) {}

void EmitterGroup::UnloadEmitters(bool kill_particles) {
    while (!mEmitters.IsEmpty()) {
        Emitter *emitter = mEmitters.RemoveHead();
        if (kill_particles) {
            gEmitterSystem.KillParticlesFromThisEmitter(emitter);
        } else {
            gEmitterSystem.OrphanParticlesFromThisEmitter(emitter);
        }
        delete emitter;
        this->mNumEmitters--;
        gEmitterSystem.OnDeleteEmitter();
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

// UNSOLVED
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

unsigned short *EmitterLibraryHeader::GetLibraryNumTriggers(int i) {}

// UNSOLVED
EmitterLibrary *EmitterLibraryHeader::GetLibrary(int i) {
    EmitterLibrary *lib = reinterpret_cast<EmitterLibrary *>(&this[1]);

    if (i == 0) {
        return lib;
    }
    int ix;
    for (ix = 0; ix < i;) {
        unsigned short num_trigs = lib->mNumTriggers;
        WorldFXTrigger *trigs = reinterpret_cast<WorldFXTrigger *>(&lib[1]);
        lib = reinterpret_cast<EmitterLibrary *>(&trigs[num_trigs]);
    }
    if (ix == i) {
        return lib;
    }
    return nullptr;
}

void EmitterLibraryHeader::EndianSwap() {
    if (this->EndianSwapped) {
        return;
    }
    WorldFXTrigger *ptrig;
    EmitterLibrary *lib = reinterpret_cast<EmitterLibrary *>(&this[1]);
    bPlatEndianSwap(&this->Version);
    bPlatEndianSwap(&this->NumEmitterLibraries);
    bPlatEndianSwap(&this->SectionNumber);
    for (int i = 0; i < this->NumEmitterLibraries; i++) {
        lib->EndianSwap();
        ptrig = reinterpret_cast<WorldFXTrigger *>(&lib[1]);
        for (int j = 0; j < lib->mNumTriggers; j++) {
            WorldFXTrigger *this_trig = &ptrig[j];
            this_trig->EndianSwap();
        }
        if (lib->mNumTriggers != 0) {
            ptrig = &ptrig[lib->mNumTriggers];
        }
        lib = reinterpret_cast<EmitterLibrary *>(ptrig);
    }
    this->EndianSwapped = true;
}

void EmitterSystem::KillParticle(Emitter *em, EmitterParticle *particle) {
    int listix = em->GetParticleListIndex();
    this->mParticleListCounts[listix]--;
    this->mTotalNumParticles--;
    em->DecrementNumParticles();
    particle->Remove();
    delete particle;
}

bool _EnableParticleSystem;

EmitterGroup *EmitterSystem::CreateEmitterGroup(const Attrib::StringKey &group_name, unsigned int creation_context_flags) {
    if (!_EnableParticleSystem) {
        return nullptr;
    }
    // TODO emittergroup
    const Attrib::Collection *spec = Attrib::FindCollection(0xaba86e60, group_name);
    if (spec) {
        return this->CreateEmitterGroup(spec, creation_context_flags);
    }

    return nullptr;
}

EmitterGroup *EmitterSystem::CreateEmitterGroup(const unsigned int &group_key, unsigned int creation_context_flags) {
    if (!_EnableParticleSystem) {
        return nullptr;
    }
    // TODO emittergroup
    const Attrib::Collection *spec = Attrib::FindCollection(0xaba86e60, group_key);
    if (spec) {
        return this->CreateEmitterGroup(spec, creation_context_flags);
    }

    return nullptr;
}

void EmitterSystem::AddEmitterGroup(EmitterGroup *group) {
    if (!this->mEmitterGroups.IsInList(group)) {
        this->mEmitterGroups.AddTail(group);
        this->mNumEmitterGroups++;
        this->mNumEmitters += group->NumEmitters();
    }
}

void EmitterSystem::RemoveEmitterGroup(EmitterGroup *group) {
    if (this->mEmitterGroups.IsInList(group)) {
        this->mNumEmitterGroups--;
        this->mEmitterGroups.Remove(group);
    }
}

void EmitterSystem::UpdateInterestPoints() {
    {
        eView *view = eGetView(1, false);
        Camera *camera = view->GetCamera();
        if (camera) {
            this->mInterestPoints[0] = *camera->GetPosition();
            this->mInterestVectors[0] = *camera->GetDirection();
        }
    }
    {
        eView *view = eGetView(2, false);
        Camera *camera = view->GetCamera();
        if (camera) {
            this->mInterestPoints[1] = *camera->GetPosition();
            this->mInterestVectors[1] = *camera->GetDirection();
        }
    }
}

EmitterLibrary *EmitterSystem::FindLibrary(unsigned int key) {
    LibEntry e;
    e.Key = key;
    e.Lib = nullptr;
    LibEntry *iter = std::lower_bound(this->mLibs.begin(), this->mLibs.end(), e);
    if (iter != this->mLibs.end() && iter->Key == key) {
        return iter->Lib;
    }
    return nullptr;
}

// UNSOLVED allocator has to be replaced
void EmitterSystem::AddLibrary(EmitterLibrary *lib) {
    LibEntry e;
    e.Key = lib->GroupKey;
    e.Lib = lib;
    this->mLibs.insert(std::upper_bound(this->mLibs.begin(), this->mLibs.end(), e), e);
}

void EmitterSystem::RemoveLibrary(EmitterLibrary *lib) {
    LibEntry e;
    e.Key = lib->GroupKey;
    e.Lib = lib;
    LibEntry *iter = std::lower_bound(this->mLibs.begin(), this->mLibs.end(), e);
    while (iter != this->mLibs.end() && iter->Key == e.Key) {
        if (iter->Lib == lib) {
            this->mLibs.erase(iter);
            break;
        }
        iter++;
    }
}

EmitterSystem::EmitterSystem() {
    this->mNumEmitterGroups = 0;
    this->mNumEmitters = 0;
    this->mCurrentTexture = nullptr;
    this->mLibs.reserve(400);
    this->mTotalNumParticles = 0;
    bMemSet(this->mParticleListCounts, 0, sizeof(this->mParticleListCounts));
}

void EmitterSystem::Init() {
    InitParticleSlotPool();
    InitEmitterSlotPool();
    InitEmitterGroupSlotPool();
}

bool EmitterSystem::IsCloseEnough(const bVector4 *group_pos, float farclip, int frustrum, float cos_angle_fov) const {
    bVector3 vec(group_pos->x, group_pos->y, group_pos->z);
    return this->IsCloseEnough(&vec, farclip, frustrum, cos_angle_fov);
}

bool EmitterSystem::IsCloseEnough(EmitterGroup *group, int frustrum, float cos_angle_fov) const {
    // TODO magic
    if (group->GetFlags() & 0x20000) {
        return true;
    }
    const bVector3 *group_position = group->GetPosition();
    return this->IsCloseEnough(group_position, group->GetFarClip(), frustrum, cos_angle_fov);
}

void UpdateTriggers() {
    float time_now = WorldTimer.GetSeconds();
    bTList<WorldFXTrigger> &trigger_list = gEmitterSystem.GetTriggers();

    for (WorldFXTrigger *trig = trigger_list.GetHead(); trig != trigger_list.EndOfList(); trig = trig->GetNext()) {
        if (trig->mState == 2) {
            if (!trig->mLib->mGroup) {
                trig->mState = 3;
                trig->mLastEnd = time_now;
            }
        } else if (trig->mState == 3 && (time_now - trig->mLastEnd > trig->mResetTime)) {
            trig->mState = 1;
        }
    }
}

int EmitterSystem::TexturePageLoader(bChunk *bchunk) {
    if (bchunk->GetID() == BCHUNK_TPK_SETTINGS) {
        int size = bchunk->GetAlignedSize(16);
        int num = size / sizeof(TexturePageRange);
        TexturePageRange *ranges = reinterpret_cast<TexturePageRange *>(bchunk->GetAlignedData(16));
        EmitterSystem::SetTexturePageRanges(num, ranges);
        for (int i = 0; i < num; i++) {
            TexturePageRange *range = &ranges[i];
            bPlatEndianSwap(&range->flags);
            bPlatEndianSwap(&range->texture_namehash);
            bPlatEndianSwap(&range->u0);
            bPlatEndianSwap(&range->u1);
            bPlatEndianSwap(&range->v0);
            bPlatEndianSwap(&range->v1);
        }
        return true;
    }
    return false;
}

int EmitterSystem::TexturePageUnloader(bChunk *bchunk) {
    if (bchunk->GetID() == BCHUNK_TPK_SETTINGS) {
        EmitterSystem::SetTexturePageRanges(0, nullptr);
        return true;
    }
    return false;
}

void EmitterSystem::SetTexturePageRanges(int num_ranges, TexturePageRange *ranges) {
    EmitterSystem::mNumTextureRanges = num_ranges;
    EmitterSystem::mTextureRanges = ranges;
}

void HandleFXTriggers() {}

void EmitterSystem::KillEffectsMatchingFlag(unsigned int flags_to_match) {
    for (EmitterGroup *grp = this->mEmitterGroups.GetHead(); grp != this->mEmitterGroups.EndOfList();) {
        EmitterGroup *grp_next = grp->GetNext();
        if (grp->IsFlagSet(flags_to_match)) {
            grp->UnloadEmitters(true);
            delete grp;
        }
        grp = grp_next;
    }
}

void CleanParticlesOnRaceRestart() {
    // TODO magic
    gEmitterSystem.KillEffectsMatchingFlag(0xf7000000);
}

void EmitterGroup::SubscribeToDeletion(void *subscriber, void (*callback)(void *, EmitterGroup *)) {
    this->mSubscriber = subscriber;
    this->mDeleteCallback = callback;
}

void EmitterGroup::UnSubscribe() {
    this->mSubscriber = nullptr;
    this->mDeleteCallback = nullptr;
}

void EmitterGroup::DeleteEmitters() {
    while (!this->mEmitters.IsEmpty()) {
        Emitter *emitter = this->mEmitters.GetTail();
        emitter->Remove();
        this->mNumEmitters--;
        if (emitter) {
            delete emitter;
        }
        gEmitterSystem.OnDeleteEmitter();
    }
}

bool Emitter::Update(float dt, float &rollover_time) {
    if (!_EnableParticleSystem || !this->IsEnabled()) {
        return false;
    }
    return this->mControl.Update(dt, this, rollover_time);
}
