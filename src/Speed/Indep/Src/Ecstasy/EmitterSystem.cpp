#include "EmitterSystem.h"
#include "EcstasyE.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitterdata.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emittergroup.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "eMath.hpp"

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
    for (LibList::iterator it = this->mLibs.begin(); it != this->mLibs.end(); it++) {
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
    for (LibList::iterator it = this->mLibs.begin(); it != this->mLibs.end(); it++) {
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

    bVector4 partSize(this->mStaticData.Size());
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

void GetAnimatedUVs(EffectParticleAnimation texture_layout_type, int frame, unsigned int *uvS, unsigned int *uvE) {
    if (texture_layout_type != ANIMATE_PARTICLE_NONE) {
        float startu = (*uvS >> 16) / 65535.0f;
        float startv = (*uvS & 0xffff) / 65535.0f;
        float endu = (*uvE >> 16) / 65535.0f;
        float endv = (*uvE & 0xffff) / 65535.0f;
        // TODO some names might be wrong
        int square_width;
        int num_sub_squares = frame / texture_layout_type;
        float xdiff = endu - startu;
        float fsquarewidth = xdiff / texture_layout_type;
        int i = frame - num_sub_squares * texture_layout_type;
        int j = num_sub_squares;
        *uvS = (int)((i * fsquarewidth + startu) * 65535.0f) << 16 | (int)((j * fsquarewidth + startv) * 65535.0f);
        *uvE = (int)(((i + 1) * fsquarewidth + startu) * 65535.0f) << 16 | (int)(((j + 1) * fsquarewidth + startv) * 65535.0f);
    }
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

bool EmitterGroup::SetEmitters(unsigned int creation_context_flags) {
    const Attrib::Gen::emittergroup &grpatr = this->GetAttribs();
    if (grpatr.IsValid()) {
        int num_emitters = grpatr.Num_Emitters();
        int num_ranges = grpatr.Num_IntensityRanges();
        bool setRanges = num_ranges == num_emitters;
        if (num_emitters == 0) {
            return false;
        }
        for (int i = 0; i < num_emitters; i++) {
            const Attrib::Collection *emspec = grpatr.Emitters(i).GetCollection();
            if (emspec) {
                Attrib::Gen::emitterdata atr(emspec, 0, nullptr);
                Emitter *emitter = new Emitter(emspec, this);
                if (!emitter) {
                    static bool warn_once = false;
                    if (!warn_once) {
                        warn_once = true;
                    }
                    return false;
                }
                emitter->SetLocalWorld(&this->mLocalWorld);
                if (setRanges) {
                    UMath::Vector2 range = grpatr.IntensityRanges(i);
                    if (range.x == range.y || range.x > range.y) {
                        range.x = 0.0f;
                        range.y = 1.0f;
                    }
                    emitter->SetIntensityRange(range.x, range.y);
                } else {
                    emitter->SetIntensityRange(0.0f, 999999.0f);
                }
                this->mEmitters.AddTail(emitter);
                this->mNumEmitters++;
            }
        }
        this->mFarClip = grpatr.FarClip();
        return true;
    }
    return false;
}

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
unsigned short *EmitterLibraryHeader::GetLibraryNumTriggers(int i) {
    EmitterLibrary *plib = reinterpret_cast<EmitterLibrary *>(&this[1]);
    EmitterLibrary *lib = plib;

    if (i == 0) {
        return &plib->mNumTriggers;
    }
    int ix;
    for (ix = 0; ix < i; i++) {
        unsigned short num_trigs = lib->mNumTriggers;
        WorldFXTrigger *trigs = reinterpret_cast<WorldFXTrigger *>(&lib[1]);
        lib = reinterpret_cast<EmitterLibrary *>(&trigs[num_trigs]);
    }
    if (ix == i) {
        return &lib->mNumTriggers;
    }
    return nullptr;
}

// UNSOLVED
EmitterLibrary *EmitterLibraryHeader::GetLibrary(int i) {
    EmitterLibrary *plib = reinterpret_cast<EmitterLibrary *>(&this[1]);
    EmitterLibrary *lib = plib;

    if (i == 0) {
        return plib;
    }
    int ix;
    for (ix = 0; ix < i; i++) {
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

// UNSOLVED (lazy)
EmitterParticle *EmitterSystem::GetNewParticle(Emitter *spawning_emitter) {
    unsigned short num_emitters;
    bool done;
    bool bVar3;
    unsigned int uVar4;
    bNode *pbVar5;
    EmitterGroup *grp;
    EmitterGroup *this_00;
    EmitterGroup *local_r31_188;

    if (this->mTotalNumParticles == 1024) {
        bool high_priority = spawning_emitter->GetEmitterGroup()->GetFlags() & 0x40000;
        if (high_priority) {
            done = false;
            grp = this->mEmitterGroups.GetHead();
            while (this_00 = grp, bVar3 = !done, bVar3) {
                if (this_00 == this->mEmitterGroups.EndOfList()) {
                    if (bVar3) {
                        grp = this->mEmitterGroups.GetHead();
                        while ((bVar3 && (grp != this->mEmitterGroups.EndOfList()))) {
                            local_r31_188 = grp->GetNext();
                            if (((grp->GetFlags() & 0x40000) == 0) && (done = true)) {
                                delete grp;
                            }
                            bVar3 = !done;
                            grp = local_r31_188;
                        }
                    }
                    break;
                }
                grp = this_00->GetNext();
                uVar4 = this_00->GetNumParticles();
                if ((uVar4 != 0) && (uVar4 = this_00->GetFlags(), (uVar4 & 0x8000000) != 0) && ((uVar4 & 0x40000) == 0) &&
                    ((uVar4 & 0x4000000) == 0 && (done = true))) {
                    delete this_00;
                }
            }
        }
    }
    EmitterParticle *new_particle = new EmitterParticle();
    if (new_particle) {
        new_particle->mFlags = spawning_emitter->mFlags & 0xFFFF0000;
        spawning_emitter->GetParticles().AddTail(new_particle);
        int listix = spawning_emitter->GetParticleListIndex();
        this->mTotalNumParticles++;
        this->mParticleListCounts[listix]++;
    }
    return new_particle;
}

void EmitterSystem::KillParticle(Emitter *em, EmitterParticle *particle) {
    int listix = em->GetParticleListIndex();
    this->mParticleListCounts[listix]--;
    this->mTotalNumParticles--;
    em->DecrementNumParticles();
    particle->Remove();
    delete particle;
}

bool EnableParticleSystem;

EmitterGroup *EmitterSystem::CreateEmitterGroup(const Attrib::StringKey &group_name, unsigned int creation_context_flags) {
    if (!EnableParticleSystem) {
        return nullptr;
    }
    // TODO emittergroup
    const Attrib::Collection *spec = Attrib::FindCollection(0xaba86e60, group_name);
    if (spec) {
        return this->CreateEmitterGroup(spec, creation_context_flags);
    }

    return nullptr;
}

EmitterGroup *EmitterSystem::CreateEmitterGroup(const Attrib::Key &group_key, unsigned int creation_context_flags) {
    if (!EnableParticleSystem) {
        return nullptr;
    }
    // TODO emittergroup
    const Attrib::Collection *spec = Attrib::FindCollection(0xaba86e60, group_key);
    if (spec) {
        return this->CreateEmitterGroup(spec, creation_context_flags);
    }

    return nullptr;
}

EmitterGroup *EmitterSystem::CreateEmitterGroup(const Attrib::Collection *group_spec, unsigned int creation_context_flags) {
    int ThisIsNISCondition;
    EmitterGroup *grp;

    if (!EnableParticleSystem) {
        return nullptr;
    } else {
        if (group_spec) {
            if (IsAShittyEffect(Attrib::GetCollectionKey(group_spec))) {
                return nullptr;
            }
        }
        grp = nullptr;
        ThisIsNISCondition = IsInNIS;
        if (group_spec) {

            Attrib::Gen::emittergroup atr(group_spec, 0, nullptr);
            if (atr.GetCollection() != 0xeec2271a && atr.GetCollection() != 0) {
                bool all_good = true;
                int numEmitters = atr.Num_Emitters();
                for (int i = 0; all_good && i < numEmitters; i++) {
                    const struct Attrib::Collection *emspec = atr.Emitters(i).GetCollection();
                    if (!emspec) {
                        all_good = false;
                    }
                }
                if (!all_good) {
                    return nullptr;
                }
                static bool warn_once = false;
                bool is_full = false;
                if ((this->mNumEmitters + numEmitters > 500) || (this->mNumEmitterGroups > 199)) {
                    is_full = true;
                }
                bool high_priority = creation_context_flags & 0x40000;
                if (is_full && high_priority) {
                    int need_emitters = this->mNumEmitters + numEmitters - 500;
                    int need_groups = 1;
                    bool done = false;
                    for (EmitterGroup *grp = this->mEmitterGroups.GetHead(); !done && grp != this->mEmitterGroups.EndOfList();) {
                        EmitterGroup *grpnext = grp->GetNext();
                        if ((grp->GetFlags() & 0x8000000) && !(grp->GetFlags() & 0x40000) && !(grp->GetFlags() & 0x4000000)) {
                            need_groups--;
                            need_emitters -= grp->CurrentNumEmitters();
                            delete grp;
                            if ((need_emitters < 1) && (need_groups < 1)) {
                                done = true;
                            }
                        }
                        grp = grpnext;
                    }
                    if (!done) {
                        for (EmitterGroup *grp = this->mEmitterGroups.GetHead(); !done && grp != this->mEmitterGroups.EndOfList();) {
                            EmitterGroup *grpnext = grp->GetNext();
                            if (!(grp->GetFlags() & 0x40000)) {
                                done = true;
                                delete grp;
                            }
                            grp = grpnext;
                        }
                    }
                }

                if (ThisIsNISCondition || (this->mNumEmitters + numEmitters < 501 && this->mNumEmitterGroups < 200)) {
                    grp = new EmitterGroup(group_spec, creation_context_flags);
                    if (grp) {
                        grp->Enable();
                        bVector3 zero(0.0f, 0.0f, 0.0f);
                        grp->SetInheritVelocity(&zero);

                        if (EmitterLibrary *lib = this->FindLibrary(atr.GetCollection())) {
                            grp->SetLocalWorld(&lib->LocalWorld);
                        }

                        this->mNumEmitters += numEmitters;
                        this->mNumEmitterGroups++;
                        this->mEmitterGroups.AddTail(grp);
                    } else {
                        // allocation failed → warn+return
                        if (!warn_once) {
                            warn_once = true;
                        }
                        return nullptr;
                    }
                } else {
                    // condition failed → warn+return
                    if (!warn_once) {
                        warn_once = true;
                    }
                    return nullptr;
                }
            }
        }
    }

    return grp;
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

void EmitterSystem::UpdateParticles(float dt) {
    if (!EnableParticleSystem || this->mTotalNumParticles == 0) {
        return;
    }
    int time_step = static_cast<int>(dt * 1024.0f);
    float ed_drag = 0.0f;
    float ed_gravity = 0.0f;
    float ed_life = 0.0f;
    const bMatrix4 *ExtraBasis = nullptr;
    const bMatrix4 *ColourBasis = nullptr;
    bool texture_animation = false;
    EffectParticleAnimation anim_type = ANIMATE_PARTICLE_NONE;
    float fAnimFPS = 0.0f;
    const EmitterDataAttribWrapper *last_emitter_data = nullptr;

    for (EmitterGroup *grp = this->mEmitterGroups.GetHead(); grp != this->mEmitterGroups.EndOfList(); grp = grp->GetNext()) {
        bTList<Emitter> &emitters = grp->GetEmitters();
        if (!this->IsCloseEnough(grp, 0, 0.7f)) {
            for (Emitter *em = emitters.GetHead(); em != emitters.EndOfList(); em = em->GetNext()) {
                gEmitterSystem.KillParticlesFromThisEmitter(em);
            }
        } else {
            for (Emitter *em = emitters.GetHead(); em != emitters.EndOfList(); em = em->GetNext()) {
                bTList<EmitterParticle> &particles = em->GetParticles();
                if (particles.IsEmpty()) {
                    continue;
                }
                EmitterDataAttribWrapper *this_emmiter_data = em->GetEmitterData();
                const Attrib::Gen::emitterdata *this_emitter_data_atr = &this_emmiter_data->GetAttributes();
                bool emitter_data_switch = this_emmiter_data != last_emitter_data;
                if (emitter_data_switch) {
                    ed_drag = this_emitter_data_atr->Drag();
                    ed_gravity = this_emitter_data_atr->Gravity();
                    texture_animation = true;
                    ed_life = this_emitter_data_atr->Life();
                    ExtraBasis = this_emmiter_data->GetExtraBasis();
                    ColourBasis = this_emmiter_data->GetColourBasis();
                    const ParticleAnimationInfo &animinfo = this_emitter_data_atr->TextureAnimation();
                    if (animinfo.AnimType == ANIMATE_PARTICLE_NONE) {
                        texture_animation = false;
                    }
                    anim_type = animinfo.AnimType;
                    fAnimFPS = animinfo.FPS;
                    last_emitter_data = this_emmiter_data;
                }
                for (EmitterParticle *particle = particles.GetHead(); particle != particles.EndOfList(); particle = particle->GetNext()) {
                    bVector3 pvel;
                    bVector3 pacc;
                    bVector3 ppos;
                    UMath::Vector4 t;
                    UMath::Vector4 extra_params;
                    UMath::Vector4 col;
                    if (time_step < particle->mLife) {
                        if (texture_animation) {
                            // TODO
                            // const unsigned int i_num_frames;
                            const float f_num_frames = dt * fAnimFPS;
                            const float f_max_frame_index = anim_type * anim_type;
                            unsigned int frame_index = (int)((f_num_frames / f_max_frame_index) * 65535.0f);
                            unsigned int cur_frame = particle->mAnimFrame;
                            cur_frame += frame_index;
                            unsigned int delta_frames = cur_frame + frame_index;
                            if (delta_frames > 65535) {
                                cur_frame -= 65535;
                            }
                            frame_index = (int)((float)((unsigned short)cur_frame) * (f_max_frame_index - 1.0f) / 65535.0f);
                            em->GetStandardUVs(&particle->mUVStart, &particle->mUVEnd);
                            GetAnimatedUVs(anim_type, frame_index, &particle->mUVStart, &particle->mUVEnd);
                            particle->mAnimFrame = cur_frame;
                        }
                        ppos.x = particle->mPosX;
                        ppos.y = particle->mPosY;
                        ppos.z = particle->mPosZ;
                        ExpandVector(&particle->mVel, &pvel);
                        ExpandVector(&particle->mAcc, &pacc);
                        if (ed_drag > 0.0f) {
                            float vLen = bLength(&pvel);
                            float testDrag = -dt * ed_drag * vLen;
                            float dragC; // TODO
                            bScaleAdd(&pvel, &pvel, &pvel, bMax(-1.0f, testDrag));
                        }
                        if ((ed_gravity < 0.0f) || (ed_gravity > 0.0f)) {
                            pvel.z -= ed_gravity * dt;
                        } else {
                            bScaleAdd(&pvel, &pvel, &pacc, dt);
                        }
                        bScaleAdd(&ppos, &ppos, &pvel, dt);
                        particle->mPosX = ppos.x;
                        particle->mPosY = ppos.y;
                        particle->mPosZ = ppos.z;
                        CompressVector(&pvel, &particle->mVel);
                        CompressVector(&pacc, &particle->mAcc);
                        particle->mLife -= time_step;
                        float tlife = static_cast<int>(particle->mLife);
                        t.w = 1.0f;
                        t.z = 1.0f - tlife / (ed_life * 1024.0f);
                        t.y = t.z * t.z;
                        t.x = t.z * t.y;
                        RotateTranslate(t, *reinterpret_cast<const UMath::Matrix4 *>(ExtraBasis), extra_params);
                        particle->mSize = extra_params.x;
                        unsigned short pangle = static_cast<unsigned int>(particle->mInitialAngle) * 257.0f;
                        unsigned short adelta = extra_params.y / 255 * static_cast<float>(particle->mRotOffset) + extra_params.y;
                        if (pangle % 2 == 0) {
                            pangle += adelta;
                        } else {
                            pangle -= adelta;
                        }
                        RotateTranslate(t, *reinterpret_cast<const UMath::Matrix4 *>(ColourBasis), col);
                        particle->mAngle = pangle;
                        Scale(col, 255.0f, col);
                        unsigned int r = bClamp(static_cast<int>(col.x), 0, 255);
                        unsigned int g = bClamp(static_cast<int>(col.y), 0, 255);
                        unsigned int b = bClamp(static_cast<int>(col.z), 0, 255);
                        unsigned int a = bClamp(static_cast<int>(col.w), 0, 255);
                        particle->mColour = r << 24 | g << 16 | b << 8 | a;
                        // unsigned int alpha_value_to_kill_at = em->GetAttributes().AlphaToKillAt();
                        if (!em->GetAttributes().NoKillAtAlpha() && (em->GetAttributes().AlphaToKillAt() > a)) {
                            // EmitterParticle *to_kill = particle;
                            // particle = particle->GetNext();
                            // this->KillParticle(em, to_kill);
                            // TODO according to the dwarf they wrote the same thing twice
                            // but if not, gotta change the branching
                            continue;
                        }
                    } else {
                        EmitterParticle *to_kill = particle;
                        particle = particle->GetNext();
                        this->KillParticle(em, to_kill);
                    }
                }
            }
        }
    }
}

EmitterLibrary *EmitterSystem::FindLibrary(Attrib::Key key) {
    LibEntry e;
    e.Key = key;
    e.Lib = nullptr;
    LibEntry *iter = std::lower_bound(this->mLibs.begin(), this->mLibs.end(), e);
    if (iter != this->mLibs.end() && iter->Key == key) {
        return iter->Lib;
    }
    return nullptr;
}

// UNSOLVED diff in Allocator::allocate
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

// UNSOLVED Allocator::allocate or _STLP_alloc_proxy's constructor causes the diff
EmitterSystem::EmitterSystem() {
    this->mNumEmitterGroups = 0;
    this->mNumEmitters = 0;
    this->mCurrentTexture = nullptr;
    this->mLibs.reserve(400);
    this->mTotalNumParticles = 0;
    bMemSet(this->mParticleListCounts, 0, sizeof(this->mParticleListCounts));
}

// UNSOLVED
void PlatRotateScaleParticle(EmitterParticle *particle, UMath::Vector3 &rightVec, UMath::Vector3 &upVec, UMath::Vector3 &fwdVec,
                             UMath::Vector3 &newUpVec, UMath::Vector3 &newRightVec) {
    float angle = bAngToDeg(particle->mAngle);

    UMath::Matrix4 rotMat;
    BuildRotate(rotMat, angle, fwdVec.x, fwdVec.y, fwdVec.z);
    RotateTranslate(upVec, rotMat, newUpVec);
    RotateTranslate(rightVec, rotMat, newRightVec);
    newRightVec = newRightVec * particle->mSize;
    newUpVec = newUpVec * particle->mSize;
}

void afxGetWorldViewMatrix(eView *view, bMatrix4 *world_view) {
    Camera *camera = view->GetCamera();
    bCopy(world_view, camera->GetCameraMatrix());
}

// TODO move
void PlatGetViewVectors(eView *view, UMath::Vector3 &rightVec, UMath::Vector3 &upVec, UMath::Vector3 &fwdVec);
bool PlatStartParticleRender(eView *view, TextureInfo *mTextureInfo, unsigned int mNumParticles);
void PlatEndParticleRender();
void PlatAddParticle(const EmitterParticle &particle, const UMath::Vector3 &upVec, const UMath::Vector3 &rightVec, unsigned int hack_flags,
                     bVector4 *x_constrain_basis, bVector4 *y_constrain_basis);
void DrawXenonEmitters(eView *view);

// UNSOLVED
void EmitterSystem::Render(eView *view) {
    if (!EnableParticleSystem) {
        return;
    }
    int num_textures;
    int total_num_textures;
    if (this->mTotalNumParticles > 0) {
        bMatrix4 world_view;
        num_textures = 0;
        afxGetWorldViewMatrix(view, &world_view);
        for (EmitterGroup *grp = this->mEmitterGroups.GetHead(); grp != this->mEmitterGroups.EndOfList(); grp = grp->GetNext()) {
            bTList<Emitter> &elist = grp->GetEmitters();
            for (Emitter *em = elist.GetHead(); em != elist.EndOfList(); em = em->GetNext()) {
                bTList<EmitterParticle> *plist = &em->GetParticles();
                if (plist->IsEmpty()) {
                    continue;
                }
                num_textures++;
                unsigned int texture_hash = em->GetAttributes().Texture().mEnum;
                this->mCurrentTexture = GetTextureInfo(texture_hash, 1, 0);
                if (!this->mCurrentTexture) {
                    continue;
                }
                unsigned int sprite_hack_flags = 0;
                UMath::Vector3 rightVec;
                UMath::Vector3 upVec;
                UMath::Vector3 fwdVec;
                UMath::Vector3 newUpVec;
                UMath::Vector3 newRightVec;
                PlatGetViewVectors(view, rightVec, upVec, fwdVec);
                bool submitParticles = PlatStartParticleRender(view, this->mCurrentTexture, em->GetNumParticles());
                if (submitParticles) {
                    EmitterDataAttribWrapper *last_emitter_data_atr = nullptr;
                    for (EmitterParticle *particle = plist->GetHead(); particle != plist->EndOfList(); particle = particle->GetNext()) {
                        EmitterDataAttribWrapper *this_emitter_data = em->GetEmitterData();
                        const Attrib::Gen::emitterdata *this_emitter_data_atr = &this_emitter_data->GetAttributes();
                        bool emitter_data_switch = this_emitter_data != last_emitter_data_atr;
                        if (emitter_data_switch) {
                            last_emitter_data_atr = this_emitter_data;
                        }
                        const EffectParticleConstraint &constraint = this_emitter_data_atr->AxisConstraint();
                        bool axis_constrained = constraint != CONSTRAIN_PARTICLE_NONE;
                        bVector4 xbasis;
                        bVector4 ybasis;
                        if (axis_constrained) {
                            GetConstraintBasis(constraint, xbasis, ybasis, particle->mAngle, &world_view);
                            bVector4 vposition(particle->mPosX, particle->mPosY, particle->mPosZ, 1.0f);
                            eMulVector(&vposition, &world_view, &vposition);
                            float world_size = particle->mSize;
                            sprite_hack_flags = 2;
                            xbasis *= world_size;
                            ybasis *= world_size;
                        }
                        PlatRotateScaleParticle(particle, rightVec, upVec, fwdVec, newUpVec, newRightVec);
                        PlatAddParticle(*particle, newRightVec, newUpVec, sprite_hack_flags, axis_constrained ? &xbasis : nullptr,
                                        axis_constrained ? &ybasis : nullptr);
                    }
                }
                PlatEndParticleRender();
            }
        }
        total_num_textures = num_textures;
        SetParticleSystemStats(this->mTotalNumParticles, 0x400, total_num_textures, GetNumParticleTextures(), gEmitterSystem.GetNumEmitters(), 500,
                               gEmitterSystem.GetNumEmitterGroups(), 200);
    }
    DrawXenonEmitters(view);
}

void EmitterSystem::Init() {
    InitParticleSlotPool();
    InitEmitterSlotPool();
    InitEmitterGroupSlotPool();
}

EmitterDataAttribWrapper *EmitterSystem::GetEmitterData(const Attrib::Collection *spec) {
    unsigned int key = Attrib::GetCollectionKey(spec);
    EmitterDataMap::iterator iter = this->mEmitterDataMap.find(key);
    if (iter != this->mEmitterDataMap.end()) {
        return iter->second;
    }
    EmitterDataAttribWrapper *ddata = new EmitterDataAttribWrapper(spec);
    this->mEmitterDataMap[key] = ddata;
    return ddata;
}

EmitterGroupAttribWrapper *EmitterSystem::GetEmitterGroup(const Attrib::Collection *spec) {
    unsigned int key = Attrib::GetCollectionKey(spec);
    EmitterGroupMap::iterator iter = this->mEmitterGroupMap.find(key);
    if (iter != this->mEmitterGroupMap.end()) {
        return iter->second;
    }
    EmitterGroupAttribWrapper *ddata = new EmitterGroupAttribWrapper(spec);
    this->mEmitterGroupMap[key] = ddata;
    return ddata;
}

bool EmitterSystem::IsCloseEnough(const bVector3 *group_pos, float farclip, int frustrum, float cos_angle_fov) const {
    const bVector3 *group_position = group_pos;
    int num_points = 1;

    if (eGetCurrentViewMode() == EVIEWMODE_TWOH) {
        num_points = 2;
    }
    if (frustrum == 0) {
        for (int i = 0; i < num_points; i++) {
            float distance = bDistBetween(group_position, &this->mInterestPoints[i]);
            if (distance < farclip) {
                return true;
            }
        }
    } else {
        for (int i = 0; i < num_points; i++) {
            float distance = bDistBetween(group_position, &this->mInterestPoints[i]);
            if (distance < farclip) {
                bVector3 toGroup = *group_position - this->mInterestPoints[i];
                toGroup /= distance;
                if (bDot(&toGroup, &this->mInterestVectors[i]) > cos_angle_fov) {
                    return true;
                }
            }
        }
    }
    return false;
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

int GetEmitterGroupsToTrigger(bVector3 &pos, EmitterLibrary **lib_buffer_out) {
    int ix = 0;
    int num_ret = 0;
    bTList<WorldFXTrigger> &trigger_list = gEmitterSystem.GetTriggers();
    for (WorldFXTrigger *trig = trigger_list.GetHead(); trig != trigger_list.EndOfList(); trig = trig->GetNext()) {
        EmitterLibrary *lib = trig->mLib;
        bool actually_trigger = false;
        if (trig->mState == 1) {
            float event_x = trig->mWorldPos.x;
            float event_y = trig->mWorldPos.y;
            float event_z = trig->mWorldPos.z;
            float dx = bAbs(pos.x - event_x);
            float dy = bAbs(pos.y - event_y);
            float dz = bAbs(pos.z - event_z);
            float r2 = trig->mTriggerRadius * trig->mTriggerRadius;
            float dist2 = dx * dx + dy * dy + dz * dz;
            // TODO magic numbers
            if (dist2 < r2) {
                bool last_was_intersecting = trig->mFlags & 1;
                if (!last_was_intersecting) {
                    if (trig->mProbability != 0) {
                        if (trig->mProbability == 0xffff) {
                            actually_trigger = true;
                        } else {
                            static unsigned int random_seed;
                            unsigned int rand_value = bRandom(0xffff, &random_seed);
                            if (rand_value > trig->mProbability) {
                                actually_trigger = true;
                                trig->mFlags |= 2;
                            } else {
                                trig->mFlags &= ~2;
                            }
                        }
                    }
                    if (actually_trigger) {
                        trig->mState = 2;
                    }
                    trig->mFlags |= 1;
                }
            } else {
                trig->mFlags &= ~(1 | 2);
            }
        }
        if (actually_trigger) {
            num_ret++;
            lib_buffer_out[ix] = lib;
            ix++;
        }
    }
    return num_ret;
}

int EmitterSystem::Loader(bChunk *bchunk) {
    if (bchunk->GetID() == 0x3bb00) {
        return true;
    } else if (bchunk->GetID() == BCHUNK_EMITTER_SYSTEM) {
        EmitterLibraryHeader *header = reinterpret_cast<EmitterLibraryHeader *>(bchunk->GetAlignedData(16));
        header->EndianSwap();
        for (int i = 0; i < header->NumEmitterLibraries; i++) {
            EmitterLibrary *lib = header->GetLibrary(i);
            bPlatEndianSwap(&lib->GroupKey);
            unsigned short *pnum_triggers = header->GetLibraryNumTriggers(i);
            int num_triggers = *pnum_triggers;
            WorldFXTrigger *triggers = header->GetLibraryTriggers(i);

            gEmitterSystem.AddLibrary(lib);
            if (lib->SectionNumber != 0) {
                if (num_triggers == 0) {
                    EmitterGroup *emgroup;
                    if (lib->SectionNumber == 0xa28) {
                        emgroup = gEmitterSystem.CreateEmitterGroup(lib->GroupKey, 0x8020000);
                    } else {
                        emgroup = nullptr;
                    }
                    if (emgroup) {
                        emgroup->SubscribeToDeletion(lib, NotifyLibOfDeletion);
                        lib->mGroup = emgroup;
                        emgroup->SetLocalWorld(&lib->LocalWorld);
                        emgroup->SetAutoUpdate(true);
                    }
                } else if (num_triggers > 0) {
                    for (int i = 0; i < num_triggers; i++) {
                        WorldFXTrigger *fx_trigger = &triggers[i];
                        fx_trigger->mState = 1;
                        fx_trigger->mLib = lib;
                        gEmitterSystem.AddTrigger(fx_trigger);
                    }
                }
            }
        }
        return true;
    }
    return false;
}

BOOL EmitterSystem::Unloader(bChunk *bchunk) {
    // TODO hash
    if (bchunk->GetID() == 0x3bb00) {
        EmitterPackHeader *pack_header = reinterpret_cast<EmitterPackHeader *>(bchunk->GetAlignedData(16));
        EmitterGroup *emitter_group = reinterpret_cast<EmitterGroup *>(&pack_header[1]);
        for (int num_emitter_groups = pack_header->NumEmitterGroups; num_emitter_groups != 0; num_emitter_groups--) {
            gEmitterSystem.RemoveEmitterGroup(emitter_group);
            emitter_group++;
        }
        return true;
    } else if (bchunk->GetID() == BCHUNK_EMITTER_SYSTEM) {
        EmitterLibraryHeader *header = reinterpret_cast<EmitterLibraryHeader *>(bchunk->GetAlignedData(16));
        for (int i = 0; i < header->NumEmitterLibraries; i++) {
            EmitterLibrary *lib = header->GetLibrary(i);
            EmitterGroup *group = lib->mGroup;
            delete group;
            bTList<WorldFXTrigger> &trigs = gEmitterSystem.GetTriggers();
            for (WorldFXTrigger *trig = trigs.GetHead(); trig != trigs.EndOfList();) {
                WorldFXTrigger *next = trig->GetNext();
                if (trig->mLib == lib) {
                    gEmitterSystem.KillTrigger(trig);
                }
                trig = next;
            }
            gEmitterSystem.RemoveLibrary(lib);
        }
        return true;
    }
    return false;
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

// TODO move
void UpdateXenonEmitters(float dt);

void EmitterSystem::Update(float dt) {
    if (!EnableParticleSystem) {
        return;
    }
    this->ServiceWorldEffects();
    HandleFXTriggers();
    this->UpdateInterestPoints();

    EmitterGroup *next_group;
    for (EmitterGroup *group = this->mEmitterGroups.GetHead(); group != this->mEmitterGroups.EndOfList(); group = next_group) {
        next_group = group->GetNext();
        if (group->IsAutoUpdate()) {
            group->Update(dt);
        } else {
            if ((group->GetFlags() & 0x40000000) && !group->IsOldSurfaceEffect() && group->GetNumParticles() == 0) {
                group->IncZeroParticleFrame();
                if (group->GetNumZeroParticleFrames() > 10)
                    group->DeleteEmitters();
            }
        }
        if (group->CurrentNumEmitters() == 0 && !group->IsStatic()) {
            delete group;
        }
        if (group->IsOldSurfaceEffect() && group->GetNumParticles() == 0) {
            delete group;
        }
    }
    this->UpdateParticles(dt);
    UpdateXenonEmitters(dt);
}

void EmitterGroup::Update(float dt) {
    if (!EnableParticleSystem || !this->IsEnabled()) {
        return;
    }
    bool closeEnough2spawn = gEmitterSystem.IsCloseEnough(this, !(this->GetFlags() & 0x40000), 0.7f);
    Emitter *next_emitter;
    for (Emitter *emitter = this->mEmitters.GetHead(); emitter != this->mEmitters.EndOfList(); emitter = next_emitter) {
        next_emitter = emitter->GetNext();
        float rollover_time = 0.0f;
        bool will_require_respawn = emitter->Update(dt, rollover_time);
        EmitterControlState state = emitter->GetControlState();
        if (will_require_respawn && closeEnough2spawn && (dt + rollover_time > 0.0f)) {
            emitter->SpawnParticles(dt + rollover_time, this->mIntensity);
        }
        if (state == ECS_OFF) {
            gEmitterSystem.OrphanParticlesFromThisEmitter(emitter);
            if (emitter->GetNumParticles() == 0) {
                emitter->Remove();
                this->mNumEmitters--;
                delete emitter;
                gEmitterSystem.OnDeleteEmitter();
            }
        }
        if (this->IsOldSurfaceEffect() && emitter->GetNumParticles() == 0) {
            emitter->Remove();
            this->mNumEmitters--;
            delete emitter;
            gEmitterSystem.OnDeleteEmitter();
        }
    }
}

bool EmitterControl::Update(float dt, Emitter *em, float &rollover_time) {
    bool will_require_spawn;

    if (!EnableParticleSystem || !em->IsEnabled()) {
        return false;
    }
    rollover_time = 0.0f;
    float start_delay = em->GetAttributes().StartDelay();
    bool random_start_delay = em->GetAttributes().StartDelayRandomVariance();
    if (random_start_delay) {
        start_delay = bRandom(start_delay);
    }
    float on_cycle = em->GetAttributes().OnCycle();
    float on_cycle_variance = em->GetAttributes().OnCycleVariance();
    float off_cycle = em->GetAttributes().OffCycle();
    float off_cycle_variance = em->GetAttributes().OffCycleVariance();
    bool forced_to_oneshot = em->GetFlags() & 0x400000;
    bool is_one_shot = false;

    if (em->GetAttributes().IsOneShot() || forced_to_oneshot) {
        is_one_shot = true;
    }
    bool has_start_delay = false;
    if (start_delay > 0.0f || start_delay < 0.0f) {
        has_start_delay = true;
    }
    bool has_on_cycle = false;
    if (on_cycle > 0.0f || on_cycle < 0.0f || on_cycle_variance > 0.0f || on_cycle_variance < 0.0f) {
        has_on_cycle = true;
    }
    bool has_off_cycle = false;
    if (off_cycle > 0.0f || off_cycle < 0.0f || off_cycle_variance > 0.0f || off_cycle_variance < 0.0f) {
        has_off_cycle = true;
    }
    // what?...
    if (has_off_cycle ? !has_on_cycle : !has_on_cycle) {
    } else if (!is_one_shot && !has_off_cycle) {
        has_on_cycle = false;
    }
    static unsigned int aseed;
    switch (this->mState) {
        case ECS_NOT_STARTED:
            if (has_start_delay) {
                this->mState = ECS_DELAYING;
                this->mTime = start_delay - dt;
                will_require_spawn = false;
                break;
            }
            if (has_on_cycle) {
                this->mState = ECS_ON_CYCLE;
                this->mTime = (on_cycle + bRandom(on_cycle_variance, &aseed)) - dt;
                if (this->mTime < 0.0f) {
                    this->mTime = 0.0f;
                }
                will_require_spawn = true;
                break;
            }
            will_require_spawn = true;
            this->mTime = 0.0f;
            this->mState = ECS_ON;
            break;
        case ECS_DELAYING:
            this->mTime -= dt;
            if (this->mTime > 0.0f) {
                will_require_spawn = false;
                break;
            }
            rollover_time = this->mTime;
            will_require_spawn = true;
            if (has_on_cycle) {
                this->mState = ECS_ON_CYCLE;
                this->mTime += on_cycle + bRandom(on_cycle_variance, &aseed);
                break;
            }
            this->mTime = 0.0f;
            this->mState = ECS_ON;
            break;
        case ECS_ON_CYCLE:
            will_require_spawn = true;
            this->mTime -= dt;
            if (this->mTime > 0.0f) {
                break;
            }
            rollover_time = this->mTime + dt;
            if (is_one_shot) {
                this->mState = ECS_OFF;
                this->mTime = 0.0f;
                break;
            }
            this->mState = ECS_OFF_CYCLE;
            this->mTime += off_cycle + bRandom(off_cycle_variance, &aseed);
            break;
        case ECS_OFF_CYCLE:
            this->mTime -= dt;
            if (this->mTime > 0.0f) {
                will_require_spawn = false;
                break;
            }
            rollover_time = this->mTime;
            this->mState = ECS_ON_CYCLE;
            this->mTime += on_cycle + bRandom(on_cycle_variance, &aseed);
            will_require_spawn = true;
            break;
        case ECS_ON:
            will_require_spawn = true;
            break;
        case ECS_OFF:
            will_require_spawn = false;
            break;
        case ECS_ERROR:
        default:
            will_require_spawn = false;
            break;
    }
    return will_require_spawn;
}

bool Emitter::Update(float dt, float &rollover_time) {
    if (!EnableParticleSystem || !this->IsEnabled()) {
        return false;
    }
    return this->mControl.Update(dt, this, rollover_time);
}
