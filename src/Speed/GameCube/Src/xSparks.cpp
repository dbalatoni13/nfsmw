#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitteruv.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fuelcell_effect.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fuelcell_emitter.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

DECLARE_CONTAINER_TYPE(XenonEffectDef);

struct XenonEffectDef {
    // total size: 0x58
    UMath::Vector4 vel;                 // offset 0x0, size 0x10
    UMath::Matrix4 mat;                 // offset 0x10, size 0x40
    const Attrib::Collection *spec;     // offset 0x50, size 0x4
    EmitterGroup *piggyback_effect;     // offset 0x54, size 0x4
    ~XenonEffectDef() {}
};

typedef UTL::Std::vector<XenonEffectDef, _type_XenonEffectDef> XenonEffectStdVector;

struct XenonEffectLists {
    enum { ACTIVE = 0, STAGING = 1 };
    XenonEffectStdVector lists[2]; // [0]=active, [1]=staging
};

struct CGEmitter {
    Attrib::Gen::fuelcell_emitter mEmitterDef;
    Attrib::Gen::emitteruv mTextureUVs;
    UMath::Vector4 mVel;
    UMath::Matrix4 mLocalWorld;

    CGEmitter(const Attrib::Collection *spec, const XenonEffectDef &eDef);
    ~CGEmitter();
    void SpawnParticles(float, float);

    void operator delete(void *ptr) {
        if (ptr) {
            gFastMem.Free(ptr, 0x78, 0);
        }
    }
};

struct NGEffect {
    Attrib::Gen::fuelcell_effect mEffectDef;

    NGEffect(const XenonEffectDef &eDef);
};

class ParticleList {
    NGParticle mParticles[300];
    uint32 mNumParticles;
    TextureInfo *mContrail_tex;
    TextureInfo *mSparks_tex;
    TextureInfo *mCurrentTexture;

  public:
    void GeneratePolys();
    void AgeParticles(float dt);
    NGParticle *GetNextParticle();
    uint32 GetNumParticles();
};

XenonEffectLists gNGEffectList;
extern ParticleList gParticleList;
extern XSpriteManager NGSpriteManager;
extern unsigned int randomSeed;
float bRandom(float range, unsigned int *seed);
unsigned int bStringHash(const char *str);
TextureInfo *GetTextureInfo(unsigned int name_hash, int allow_default, int force_local);

CGEmitter::CGEmitter(const Attrib::Collection *spec, const XenonEffectDef &eDef)
    : mEmitterDef(spec, 0, nullptr) //
    , mTextureUVs(mEmitterDef.emitteruv(), 0, nullptr) //
    , mLocalWorld(eDef.mat) {
    mVel = eDef.vel;
}

CGEmitter::~CGEmitter() {}

NGParticle *ParticleList::GetNextParticle() {
    if (mNumParticles < 300) {
        return &mParticles[mNumParticles++];
    }
    return 0;
}

void CGEmitter::SpawnParticles(float dt, float intensity) {
    UMath::Matrix4 local_world;
    UMath::Matrix4 local_orientation;
    unsigned int random_seed;
    float life_variance;
    float life;
    int r;
    int g;
    int b;
    int a;
    unsigned int particleColor;
    float num_particles_variance;
    float num_particles;
    float particle_age_factor;
    float current_particle_age;

    random_seed = randomSeed;

    if (intensity > 0.0f) {
        local_world = mLocalWorld;
        local_orientation = local_world;
        local_orientation.v3.x = 0.0f;
        local_orientation.v3.y = 0.0f;
        local_orientation.v3.z = 0.0f;
        local_orientation.v3.w = 1.0f;
        r = static_cast<int>(mEmitterDef.Colour1().x * 255.0f);
        g = static_cast<int>(mEmitterDef.Colour1().y * 255.0f);
        b = static_cast<int>(mEmitterDef.Colour1().z * 255.0f);
        a = static_cast<int>(mEmitterDef.Colour1().w * 255.0f);
        life = mEmitterDef.Life();
        life_variance = life * mEmitterDef.LifeVariance();
        num_particles = intensity * mEmitterDef.NumParticles();
        num_particles_variance = num_particles * mEmitterDef.NumParticlesVariance() * 100.0f;
        current_particle_age = 0.0f;
        particleColor = a << 24 | b << 16 | g << 8 | r;
        num_particles -= num_particles_variance;
        life -= life_variance;

        if (num_particles != 0.0f) {
            particle_age_factor = dt / num_particles;
            while (num_particles != 0.0f) {
                NGParticle *particle;
                float sparkLength;
                float ld;
                UMath::Vector4 pvel;
                UMath::Vector4 rand;
                UMath::Vector4 rotatedVel;
                float gravity;
                UMath::Vector4 ppos;

                num_particles -= 1.0f;
                particle = gParticleList.GetNextParticle();
                if (!particle) {
                    break;
                }

                sparkLength = mEmitterDef.LengthStart() + bRandom(mEmitterDef.LengthDelta(), &random_seed);
                if (sparkLength < 0.0f) {
                    break;
                }

                ld = bMin(sparkLength, 1.0f);

                rand.x = 1.0f - (mEmitterDef.VelocityDelta().x - bRandom(mEmitterDef.VelocityDelta().x, &random_seed) * 2.0f);
                rand.y = 1.0f - (mEmitterDef.VelocityDelta().y - bRandom(mEmitterDef.VelocityDelta().y, &random_seed) * 2.0f);
                rand.z = 1.0f - (mEmitterDef.VelocityDelta().z - bRandom(mEmitterDef.VelocityDelta().z, &random_seed) * 2.0f);
                rand.w = 1.0f;

                VU0_v4scalexyz(mEmitterDef.VelocityInherit(), mVel, pvel);
                VU0_MATRIX3x4_vect4mult(mEmitterDef.VelocityStart(), local_orientation, rotatedVel);
                VU0_v4add(pvel, rotatedVel, pvel);
                VU0_v4scalexyz(pvel, rand, pvel);

                gravity = (mEmitterDef.GravityStart() - mEmitterDef.GravityDelta()) + bRandom(mEmitterDef.GravityDelta(), &random_seed) * 2.0f;

                ppos.x = mEmitterDef.VolumeCenter().x + (bRandom(mEmitterDef.VolumeExtent().x, &random_seed) - mEmitterDef.VolumeExtent().x * 0.5f);
                ppos.y = mEmitterDef.VolumeCenter().y + (bRandom(mEmitterDef.VolumeExtent().y, &random_seed) - mEmitterDef.VolumeExtent().y * 0.5f);
                ppos.z = mEmitterDef.VolumeCenter().z + (bRandom(mEmitterDef.VolumeExtent().z, &random_seed) - mEmitterDef.VolumeExtent().z * 0.5f);
                ppos.w = 1.0f;

                UMath::RotateTranslate(ppos, local_world, ppos);
                UMath::ScaleAdd(
                    reinterpret_cast<const UMath::Vector3 &>(pvel),
                    current_particle_age,
                    reinterpret_cast<const UMath::Vector3 &>(ppos),
                    particle->initialPos);

                particle->initialPos.z += gravity * current_particle_age * current_particle_age;
                particle->vel.x = pvel.x;
                particle->vel.y = pvel.y;
                particle->vel.z = pvel.z;
                particle->age = current_particle_age;
                particle->gravity = gravity;
                particle->life = static_cast<uint16>(life * 65535.0f);
                particle->color = particleColor;
                particle->length = static_cast<uint8>(ld * 255.0f);
                particle->width = static_cast<uint8>(mEmitterDef.HeightStart());

                current_particle_age += particle_age_factor;
            }
        }

        randomSeed = random_seed;
    }
}

NGEffect::NGEffect(const XenonEffectDef &eDef)
    : mEffectDef(eDef.spec, 0, nullptr) {
    if (mEffectDef.IsValid()) {
        int i = 0;
        int length = mEffectDef.Num_NGEmitter();
        while (i < length) {
            CGEmitter emitter(mEffectDef.NGEmitter(i).GetCollection(), eDef);
            emitter.SpawnParticles(1.0f / 30.0f, 1.0f);
            i++;
        }
    }
}

void ParticleList::AgeParticles(float dt) {
    int alive = 0;
    int i = 0;

    if (static_cast<int>(mNumParticles) > 0) {
        NGParticle *src = mParticles;
        NGParticle *dst = mParticles;
        do {
            if (dt * 8191.0f <= static_cast<float>(src->life)) {
                alive++;
                *dst = *src;
                dst->age += dt;
                dst->life = static_cast<uint16>(static_cast<float>(src->life) - dt * 8191.0f);
                dst++;
            }
            src++;
            i++;
        } while (i < static_cast<int>(mNumParticles));
    }

    mNumParticles = alive;
}

void ParticleList::GeneratePolys() {
    if (mNumParticles != 0) {
        if (!mContrail_tex) {
            mContrail_tex = GetTextureInfo(bStringHash("PS2_CONTRAIL"), 0, 0);
            mSparks_tex = GetTextureInfo(bStringHash("PS2_SPARKS"), 0, 0);
        }

        NGParticle *particle = mParticles;
        for (unsigned int i = 0; i < mNumParticles; i++) {
            if (particle->uv[0] == 0x7f) {
                mCurrentTexture = mContrail_tex;
            } else {
                mCurrentTexture = mSparks_tex;
            }

            NGSpriteManager.AddSpark(*particle, mCurrentTexture);
            particle++;
        }
    }
}

void DrawXenonEmitters(eView *view) {}

void ClearXenonEmitters() {
    gNGEffectList.lists[XenonEffectLists::ACTIVE].clear();
    gNGEffectList.lists[XenonEffectLists::STAGING].clear();
}

void AddXenonEffect(EmitterGroup *piggyback_fx, const Attrib::Collection *spec, const UMath::Matrix4 *mat, const UMath::Vector4 *vel) {
    XenonEffectDef eDef;

    if (gNGEffectList.lists[XenonEffectLists::ACTIVE].size() < 20) {
        if (gNGEffectList.lists[XenonEffectLists::ACTIVE].capacity() < 20) {
            gNGEffectList.lists[XenonEffectLists::ACTIVE].reserve(20);
        }

        if (gNGEffectList.lists[XenonEffectLists::STAGING].capacity() < 20) {
            gNGEffectList.lists[XenonEffectLists::STAGING].reserve(20);
        }

        eDef.mat = UMath::Matrix4::kIdentity;
        eDef.mat.v3 = mat->v3;
        eDef.piggyback_effect = piggyback_fx;
        eDef.spec = spec;
        eDef.vel = *vel;

        gNGEffectList.lists[XenonEffectLists::ACTIVE].push_back(eDef);
    }
}

void UpdateXenonEmitters(float dt) {
    XenonEffectDef *iter;
    XenonEffectDef eDef;

    gParticleList.AgeParticles(dt);

    iter = gNGEffectList.lists[XenonEffectLists::ACTIVE].begin();
    while (iter != gNGEffectList.lists[XenonEffectLists::ACTIVE].end()) {
        eDef = *iter;
        gNGEffectList.lists[XenonEffectLists::STAGING].push_back(eDef);
        ++iter;
    }

    gNGEffectList.lists[XenonEffectLists::ACTIVE].clear();

    iter = gNGEffectList.lists[XenonEffectLists::STAGING].begin();
    while (iter != gNGEffectList.lists[XenonEffectLists::STAGING].end()) {
        eDef = *iter;
        NGEffect anEffect(eDef);
        ++iter;
    }

    gNGEffectList.lists[XenonEffectLists::STAGING].clear();
    gParticleList.GeneratePolys();
}
