#include "Speed/GameCube/Src/xSparks.h"
#include "Speed/GameCube/Src/Ecstasy/xSprites.hpp"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UTLAllocator.h"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/emitteruv.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fuelcell_effect.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/fuelcell_emitter.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

#include <vector>
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

ParticleList gParticleList;

struct CGEmitter {
    Attrib::Gen::fuelcell_emitter mEmitterDef;
    Attrib::Gen::emitteruv mTextureUVs;
    UMath::Vector4 mVel;
    UMath::Matrix4 mLocalWorld;

    CGEmitter(const Attrib::Collection *spec, const XenonEffectDef &eDef);
    void SpawnParticles(float dt, float intensity);
    ~CGEmitter();

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, 0);
    }

    void operator delete(void *mem, size_t size) {
        if (mem != 0) {
            gFastMem.Free(mem, size, 0);
        }
    }
};

CGEmitter::CGEmitter(const Attrib::Collection *spec, const XenonEffectDef &eDef)
    : mEmitterDef(spec, 0, 0), mTextureUVs(mEmitterDef.emitteruv(), 0, 0) {
    this->mLocalWorld = eDef.mat;
    this->mVel = eDef.vel;
}

NGParticle *ParticleList::GetNextParticle() {
    if (this->mNumParticles < 300) {
        return &this->mParticles[this->mNumParticles++];
    }

    return 0;
}

void ParticleList::GeneratePolys() {
    if (this->mNumParticles == 0) {
        return;
    }

    if (this->mContrail_tex == 0) {
        this->mContrail_tex = GetTextureInfo(bStringHash("PS2_CONTRAIL"), 0, 0);
        this->mSparks_tex = GetTextureInfo(bStringHash("PS2_SPARKS"), 0, 0);
    }

    NGParticle *particle = this->mParticles;
    for (unsigned int i = 0; i < this->mNumParticles; i++) {
        if (particle->uv[0] == 0x7F) {
            this->mCurrentTexture = this->mContrail_tex;
        } else {
            this->mCurrentTexture = this->mSparks_tex;
        }
        NGSpriteManager.AddSpark(*particle, this->mCurrentTexture);
        particle++;
    }
}

struct NGEffect {
    Attrib::Gen::fuelcell_effect mEffectDef;

    NGEffect(const XenonEffectDef &eDef);
};

// Tag del asignador tipado, tal como aparece en el manglado del original:
// UTL::Std::Allocator<XenonEffectDef, _type_XenonEffectDef>.
struct _type_XenonEffectDef {};

typedef _STL::vector<XenonEffectDef, UTL::Std::Allocator<XenonEffectDef, _type_XenonEffectDef> > XenonEffectDefVector;

// [0] recibe los efectos pendientes (AddXenonEffect) y [1] los que suenan este
// frame; UpdateXenonEmitters los traspasa.
struct XenonEffectDefList : XenonEffectDefVector {
    XenonEffectDefList() {
        reserve(0x14);
    }
};

XenonEffectDefList gNGEffectList[2];

// Semilla local del generador de particulas de chispas; se guarda y restaura
// en cada tanda para no desincronizar el aleatorio global.
static unsigned int randomSeed = 0xDEADBEEF;

// Math de vectores del motor (definidas en la unidad de math de VU0).
void VU0_v3scaleadd(const UMath::Vector3 &a, float t, const UMath::Vector3 &b, UMath::Vector3 &out);

// Una tanda de particulas: la cantidad sale de NumParticles*intensidad menos su
// varianza (en porcentaje), y cada particula nace en un punto aleatorio del
// volumen, hereda velocidad del emisor y del evento, y vive Life*(1-varianza).
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

    if (intensity > 0.0f) {
        local_world = this->mLocalWorld;
        local_orientation = local_world;
        local_orientation.v3.x = 0.0f;
        local_orientation.v3.y = 0.0f;
        local_orientation.v3.z = 0.0f;
        local_orientation.v3.w = 1.0f;

        random_seed = randomSeed;

        life_variance = this->mEmitterDef.Life() * this->mEmitterDef.LifeVariance();
        life = this->mEmitterDef.Life() - life_variance;

        r = (int)(this->mEmitterDef.Colour1().x * 255.0f);
        g = (int)(this->mEmitterDef.Colour1().y * 255.0f);
        b = (int)(this->mEmitterDef.Colour1().z * 255.0f);
        a = (int)(this->mEmitterDef.Colour1().w * 255.0f);
        particleColor = (a << 24) | (b << 16) | (g << 8) | r;

        num_particles_variance = intensity * this->mEmitterDef.NumParticles() * this->mEmitterDef.NumParticlesVariance() * 100.0f;
        num_particles = intensity * this->mEmitterDef.NumParticles() - num_particles_variance;

        particle_age_factor = dt / num_particles;
        current_particle_age = 0.0f;

        while (num_particles-- != 0.0f) {
            NGParticle *particle;
            float sparkLength;
            float ld;
            UMath::Vector4 pvel;
            UMath::Vector4 rand;
            UMath::Vector4 rotatedVel;
            float gravity;
            UMath::Vector4 ppos;

            particle = gParticleList.GetNextParticle();
            if (particle == 0) {
                break;
            }

            sparkLength = this->mEmitterDef.LengthStart();
            ld = bRandom(this->mEmitterDef.LengthDelta(), &random_seed);
            sparkLength = sparkLength + ld;
            if (sparkLength < 0.0f) {
                break;
            }

            sparkLength = bMin(sparkLength, 255.0f);

            rand.x = 1.0f - (this->mEmitterDef.VelocityDelta().x - 2.0f * bRandom(this->mEmitterDef.VelocityDelta().x, &random_seed));
            rand.y = 1.0f - (this->mEmitterDef.VelocityDelta().y - 2.0f * bRandom(this->mEmitterDef.VelocityDelta().y, &random_seed));
            rand.z = 1.0f - (this->mEmitterDef.VelocityDelta().z - 2.0f * bRandom(this->mEmitterDef.VelocityDelta().z, &random_seed));
            UMath::Scalexyz(this->mEmitterDef.VelocityInherit(), this->mVel, pvel);
            UMath::Rotate(this->mEmitterDef.VelocityStart(), this->mLocalWorld, rotatedVel);
            UMath::Add(pvel, rotatedVel, pvel);
            UMath::Scalexyz(pvel, rand, pvel);

            gravity = this->mEmitterDef.GravityStart() - this->mEmitterDef.GravityDelta() + 2.0f * bRandom(this->mEmitterDef.GravityDelta(), &random_seed);

            ppos.x = this->mEmitterDef.VolumeCenter().x +
                     (bRandom(this->mEmitterDef.VolumeExtent().x, &random_seed) - this->mEmitterDef.VolumeExtent().x * 0.5f);
            ppos.y = this->mEmitterDef.VolumeCenter().y +
                     (bRandom(this->mEmitterDef.VolumeExtent().y, &random_seed) - this->mEmitterDef.VolumeExtent().y * 0.5f);
            ppos.z = this->mEmitterDef.VolumeCenter().z +
                     (bRandom(this->mEmitterDef.VolumeExtent().z, &random_seed) - this->mEmitterDef.VolumeExtent().z * 0.5f);
            ppos.w = 1.0f;
            UMath::RotateTranslate(ppos, local_world, ppos);

            UMath::ScaleAdd((const UMath::Vector3 &)pvel, current_particle_age, (const UMath::Vector3 &)ppos, particle->initialPos);
            particle->initialPos.z = particle->initialPos.z + gravity * current_particle_age * current_particle_age;
            particle->vel.x = pvel.x;
            particle->vel.y = pvel.y;
            particle->vel.z = pvel.z;
            particle->life = (unsigned short)(int)(life * 8191.0f);
            particle->age = current_particle_age;
            particle->gravity = gravity;
            particle->uv[0] = (unsigned char)(int)(this->mTextureUVs.StartU() * 255.0f);
            particle->length = (unsigned char)(int)sparkLength;
            particle->width = (unsigned char)(int)this->mEmitterDef.HeightStart();
            particle->color = particleColor;

            current_particle_age += particle_age_factor;
        }

        randomSeed = random_seed;
    }
}

void ParticleList::AgeParticles(float dt) {
    NGParticle *inParticle = this->mParticles;
    NGParticle *outParticle = this->mParticles;
    int numOutParticles = 0;

    {
        int i;
        for (i = 0; i < static_cast<int>(this->mNumParticles); i++) {
        if (static_cast<float>(static_cast<int>(inParticle->life)) < dt * 8191.0f) {
            inParticle++;
        } else {
            *outParticle = *inParticle;
            outParticle->life = static_cast<unsigned short>(static_cast<float>(static_cast<int>(inParticle->life)) - dt * 8191.0f);
            outParticle->age += dt;
            inParticle++;
            outParticle++;
            numOutParticles++;
        }
        }
    }

    this->mNumParticles = numOutParticles;
}

NGEffect::NGEffect(const XenonEffectDef &eDef) : mEffectDef(eDef.spec, 0, 0) {
    if (!this->mEffectDef.IsValid()) {
        return;
    }

    int numEmitters;
    {
        Attrib::Attribute attr = this->mEffectDef.Get(0xb0d98a89);
        numEmitters = attr.GetLength();
    }

    for (int i = 0; i < numEmitters; i++) {
        const Attrib::RefSpec *ref = static_cast<const Attrib::RefSpec *>(this->mEffectDef.GetAttributePointer(0xb0d98a89, i));
        if (ref == 0) {
            ref = static_cast<const Attrib::RefSpec *>(Attrib::DefaultDataArea(0xC));
        }
        CGEmitter anEmitter(ref->GetCollection(), eDef);
        anEmitter.SpawnParticles(1.0f / 30.0f, 1.0f);
    }
}

// Envejece las chispas vivas, ejecuta los efectos Xenon pendientes (los que
// siguen enganchados a un EmitterGroup activo) y genera los quads.
void UpdateXenonEmitters(float dt) {
    gParticleList.AgeParticles(dt);

    for (XenonEffectDefVector::iterator it = gNGEffectList[0].begin(); it != gNGEffectList[0].end(); ++it) {
        XenonEffectDef def = *it;
        gNGEffectList[1].push_back(def);
    }

    gNGEffectList[0].clear();

    for (XenonEffectDefVector::iterator it = gNGEffectList[1].begin(); it != gNGEffectList[1].end(); ++it) {
        XenonEffectDef def = *it;

        if (def.piggyback_effect == 0 || def.piggyback_effect->IsEnabled()) {
            NGEffect anEffect(def);
        }
    }

    gNGEffectList[1].clear();

    gParticleList.GeneratePolys();
}

void DrawXenonEmitters(eView *view) {}

void ClearXenonEmitters() {
    gNGEffectList[0].clear();
    gNGEffectList[1].clear();
}

// Encola un efecto de luces Xenon: matriz identidad con solo la fila de
// traslacion del parametro, velocidad tal cual y referencia al grupo al que se
// engancha. Tope de 19 efectos pendientes (reserva de 20 en ambas listas).
void AddXenonEffect(EmitterGroup *piggyback_fx, const Attrib::Collection *spec, const UMath::Matrix4 *mat, const UMath::Vector4 *vel) {
    XenonEffectDef eDef;

    if (gNGEffectList[0].size() <= 19) {
        if (gNGEffectList[0].capacity() <= 19) {
            gNGEffectList[0].reserve(20);
        }

        if (gNGEffectList[1].capacity() <= 19) {
            gNGEffectList[1].reserve(20);
        }

        eDef.mat = UMath::Matrix4::kIdentity;
        eDef.mat.v3 = mat->v3;
        eDef.piggyback_effect = piggyback_fx;
        eDef.spec = spec;
        eDef.vel = *vel;

        gNGEffectList[0].push_back(eDef);
    }
}

inline CGEmitter::~CGEmitter() {}
