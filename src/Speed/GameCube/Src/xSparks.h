#ifndef __ECSTASY_ENGINE__XSPARK_H
#define __ECSTASY_ENGINE__XSPARK_H

#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"

struct TextureInfo;

struct NGParticle {
    UMath::Vector3 initialPos;
    unsigned int color;
    UMath::Vector3 vel;
    float gravity;
    unsigned short life;
    unsigned char length;
    unsigned char width;
    unsigned char uv[1];
    float padAlign;
    float age;
};

struct ParticleList {
    unsigned int GetNumParticles() {
        return mNumParticles;
    }

    NGParticle *GetNextParticle();
    void GeneratePolys();
    void AgeParticles(float dt);

    NGParticle mParticles[300];
    unsigned int mNumParticles;
    TextureInfo *mContrail_tex;
    TextureInfo *mSparks_tex;
    TextureInfo *mCurrentTexture;
};

struct XenonEffectDef {
    UMath::Vector4 vel;
    UMath::Matrix4 mat;
    const Attrib::Collection *spec;
    EmitterGroup *piggyback_effect;

    ~XenonEffectDef() {}
};

void AddXenonEffect(EmitterGroup *piggyback_fx, const Attrib::Collection *spec, const UMath::Matrix4 *mat, const UMath::Vector4 *vel);

extern ParticleList gParticleList;

#endif
