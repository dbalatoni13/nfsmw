#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitteruv.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fuelcell_effect.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fuelcell_emitter.h"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

struct XenonEffectDef {
    // total size: 0x58
    UMath::Vector4 vel;                 // offset 0x0, size 0x10
    UMath::Matrix4 mat;                 // offset 0x10, size 0x40
    Attrib::Collection *spec;           // offset 0x50, size 0x4
    EmitterGroup *piggyback_effect;     // offset 0x54, size 0x4
};

struct XenonEffectVec {
    XenonEffectDef *start;          // offset 0x0, size 0x4
    XenonEffectDef *finish;         // offset 0x4, size 0x4
    void *unused;                   // offset 0x8, size 0x4
    XenonEffectDef *end_of_storage; // offset 0xC, size 0x4
};

struct XenonEffectLists {
    XenonEffectVec active;  // offset 0x0, size 0x10
    XenonEffectVec staging; // offset 0x10, size 0x10
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

extern XenonEffectLists gNGEffectList;
extern ParticleList gParticleList;
extern XSpriteManager NGSpriteManager;
extern unsigned int randomSeed;

void reserveXenonEffectVec(void *vec, unsigned int count)
    __asm__("reserve__Q24_STLt6vector2Z14XenonEffectDefZQ33UTL3Stdt9Allocator2Z14XenonEffectDefZ20_type_XenonEffectDefUi");
float bRandom(float range, unsigned int *seed);
unsigned int bStringHash(const char *str);
TextureInfo *GetTextureInfo(unsigned int name_hash, int allow_default, int force_local);

CGEmitter::CGEmitter(const Attrib::Collection *spec, const XenonEffectDef &eDef)
    : mEmitterDef(spec, 0, nullptr) //
    , mTextureUVs(mEmitterDef.emitteruv(), 0, nullptr) //
    , mVel(eDef.vel) //
    , mLocalWorld(eDef.mat) {}

CGEmitter::~CGEmitter() {}

NGParticle *ParticleList::GetNextParticle() {
    if (mNumParticles < 300) {
        return &mParticles[mNumParticles++];
    }
    return 0;
}

void CGEmitter::SpawnParticles(float dt, float intensity) {
    unsigned int seed = randomSeed;

    if (intensity > 0.0f) {
        UMath::Matrix4 local_world = mLocalWorld;
        UMath::Vector4 velocity_base;
        UMath::Vector4 velocity_center;
        UMath::Vector4 volume_extent;
        UMath::Vector4 spawn_point;
        UMath::Vector4 world_spawn_point;
        float age = 0.0f;
        float count = intensity * mEmitterDef.NumParticles();
        float life = mEmitterDef.Life();
        float count_after_variance = count - count * mEmitterDef.NumParticlesVariance() * 100.0f;
        float colour_r = mEmitterDef.Colour1().x;
        float colour_g = mEmitterDef.Colour1().y;
        float colour_b = mEmitterDef.Colour1().z;
        unsigned int colour_a = static_cast<unsigned int>(mEmitterDef.Colour1().w * 255.0f);

        VU0_v4scalexyz(mVel, mEmitterDef.VelocityInherit().x, velocity_base);
        UMath::RotateTranslate(mEmitterDef.VolumeCenter(), local_world, velocity_center);
        velocity_base.x += velocity_center.x;
        velocity_base.y += velocity_center.y;
        velocity_base.z += velocity_center.z;

        if (count_after_variance != 0.0f) {
            float particle_step = dt / count_after_variance;
            while (count_after_variance != 0.0f) {
                NGParticle *particle;
                float length_start;
                float length_clamped;
                float gravity;

                count_after_variance -= 1.0f;
                particle = gParticleList.GetNextParticle();
                if (!particle) {
                    break;
                }

                length_start = mEmitterDef.LengthStart() + bRandom(mEmitterDef.LengthDelta(), &seed);
                if (length_start < 0.0f) {
                    break;
                }

                length_clamped = 1.0f;
                if (length_start < 1.0f) {
                    length_clamped = length_start;
                }

                volume_extent.x = 1.0f - (mEmitterDef.VolumeExtent().x - bRandom(mEmitterDef.VolumeExtent().x, &seed) * 2.0f);
                volume_extent.y = 1.0f - (mEmitterDef.VolumeExtent().y - bRandom(mEmitterDef.VolumeExtent().y, &seed) * 2.0f);
                volume_extent.z = 1.0f - (mEmitterDef.VolumeExtent().z - bRandom(mEmitterDef.VolumeExtent().z, &seed) * 2.0f);
                volume_extent.w = 1.0f;

                spawn_point.x = mEmitterDef.VolumeCenter().x + (bRandom(mEmitterDef.VolumeCenter().x, &seed) - mEmitterDef.VolumeCenter().x * 0.5f);
                spawn_point.y = mEmitterDef.VolumeCenter().y + (bRandom(mEmitterDef.VolumeCenter().y, &seed) - mEmitterDef.VolumeCenter().y * 0.5f);
                spawn_point.z = mEmitterDef.VolumeCenter().z + (bRandom(mEmitterDef.VolumeCenter().z, &seed) - mEmitterDef.VolumeCenter().z * 0.5f);
                spawn_point.w = 1.0f;

                UMath::RotateTranslate(spawn_point, local_world, world_spawn_point);
                VU0_v4scalexyz(velocity_base, volume_extent.x, velocity_center);
                VU0_v3scaleadd(UMath::Vector4To3(velocity_center), age, UMath::Vector4To3(world_spawn_point),
                               *reinterpret_cast<UMath::Vector3 *>(particle));

                gravity = (mEmitterDef.GravityStart() - mEmitterDef.GravityDelta()) + bRandom(mEmitterDef.GravityDelta(), &seed) * 2.0f;
                particle->initialPos = UMath::Vector4To3(world_spawn_point);
                particle->vel = UMath::Vector4To3(velocity_center);
                particle->age = age;
                particle->gravity = gravity;
                particle->life = static_cast<uint16>((life - life * mEmitterDef.LifeVariance()) * 65535.0f);
                particle->color =
                    colour_a << 24 | static_cast<unsigned int>(colour_b * 255.0f) << 16 | static_cast<unsigned int>(colour_g * 255.0f) << 8 |
                    static_cast<unsigned int>(colour_r * 255.0f);
                particle->length = static_cast<uint8>(length_clamped * 255.0f);
                particle->width = static_cast<uint8>(mEmitterDef.HeightStart());

                age += particle_step;
            }
        }

        randomSeed = seed;
    }
}

NGEffect::NGEffect(const XenonEffectDef &eDef)
    : mEffectDef(eDef.spec, 0, nullptr) {
    if (mEffectDef.GetCollection() != 0) {
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
    NGParticle *particle = mParticles;

    if (mNumParticles != 0) {
        if (!mContrail_tex) {
            mContrail_tex = GetTextureInfo(bStringHash("PS2_CONTRAIL"), 0, 0);
            mSparks_tex = GetTextureInfo(bStringHash("PS2_SPARKS"), 0, 0);
        }

        {
            unsigned int i = 0;

            do {
                if (particle->uv[0] == 0x7f) {
                    mCurrentTexture = mContrail_tex;
                } else {
                    mCurrentTexture = mSparks_tex;
                }

                i++;
                NGSpriteManager.AddSpark(*particle, mCurrentTexture);
                particle++;
            } while (i < mNumParticles);
        }
    }
}

void DrawXenonEmitters(eView *view) {}

void ClearXenonEmitters() {
    gNGEffectList.active.finish = gNGEffectList.active.start;
    gNGEffectList.staging.finish = gNGEffectList.staging.start;
}

void AddXenonEffect(EmitterGroup *piggyback_fx, const Attrib::Collection *spec, const UMath::Matrix4 *mat, const UMath::Vector4 *vel) {
    unsigned int size = gNGEffectList.active.finish - gNGEffectList.active.start;
    if (size < 20) {
        unsigned int active_capacity = gNGEffectList.active.end_of_storage - gNGEffectList.active.start;
        if (active_capacity < 20) {
            reserveXenonEffectVec(&gNGEffectList.active, 20);
        }

        unsigned int staging_capacity = gNGEffectList.staging.end_of_storage - gNGEffectList.staging.start;
        if (staging_capacity < 20) {
            reserveXenonEffectVec(&gNGEffectList.staging, 20);
        }

        XenonEffectDef effect;
        effect.mat = UMath::Matrix4::kIdentity;
        effect.mat.v3 = mat->v3;
        effect.spec = const_cast<Attrib::Collection *>(spec);
        effect.piggyback_effect = piggyback_fx;
        effect.vel = *vel;

        *gNGEffectList.active.finish = effect;
        ++gNGEffectList.active.finish;
    }
}

void UpdateXenonEmitters(float dt) {
    gParticleList.AgeParticles(dt);

    XenonEffectDef *effect = gNGEffectList.active.start;
    while (effect != gNGEffectList.active.finish) {
        *gNGEffectList.staging.finish = *effect;
        ++gNGEffectList.staging.finish;
        ++effect;
    }

    gNGEffectList.active.finish = gNGEffectList.active.start;

    effect = gNGEffectList.staging.start;
    while (effect != gNGEffectList.staging.finish) {
        NGEffect ng_effect(*effect);
        ++effect;
    }

    gNGEffectList.staging.finish = gNGEffectList.staging.start;
    gParticleList.GeneratePolys();
}
