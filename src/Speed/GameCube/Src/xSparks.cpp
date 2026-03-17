#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitteruv.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fuelcell_effect.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fuelcell_emitter.h"
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
    void SpawnParticles(float, float);
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
    uint32 GetNumParticles();
};

extern XenonEffectLists gNGEffectList;
extern ParticleList gParticleList;

void reserveXenonEffectVec(void *vec, unsigned int count)
    __asm__("reserve__Q24_STLt6vector2Z14XenonEffectDefZQ33UTL3Stdt9Allocator2Z14XenonEffectDefZ20_type_XenonEffectDefUi");

CGEmitter::CGEmitter(const Attrib::Collection *spec, const XenonEffectDef &eDef)
    : mEmitterDef(spec, 0, nullptr) //
    , mTextureUVs(mEmitterDef.emitteruv(), 0, nullptr) //
    , mVel(eDef.vel) //
    , mLocalWorld(eDef.mat) {}

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
