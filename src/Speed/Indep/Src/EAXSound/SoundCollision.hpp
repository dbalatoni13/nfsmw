#ifndef EAXSOUND_SOUNDCOLLISION_H
#define EAXSOUND_SOUNDCOLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_EnumAttributes.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioimpact.h"



namespace Attrib {
namespace Gen {
struct audioimpact;
struct audioscrape;
} // namespace Gen
} // namespace Attrib

struct CSTATE_Base;
struct EAXCar;
struct EAX_CarState;
struct SND_Stich;

EAX_CarState *GetClosestCopCarToCamera();
EAXCar *GetPlayerCarInRadius(bVector3 &objectpos, float distance);

namespace Sound {

struct AudioEventParams {
    bVector3 position;            // offset 0x0, size 0x10
    bVector3 normal;              // offset 0x10, size 0x10
    bVector3 velocity;            // offset 0x20, size 0x10
    float magnitude;              // offset 0x30, size 0x4
    Attrib::RefSpec attributes;   // offset 0x34, size 0xC
    unsigned int object;          // offset 0x40, size 0x4
    unsigned int other_object;    // offset 0x44, size 0x4
};

struct CollisionDescription {
    int _pad;
};

struct AudioEvent : public UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int> {
    ALIGNVEC AudioEventParams mParams;
    Attrib::Instance mAttributes;

    AudioEvent(const AudioEventParams &params)
        : mParams(params) //
        , mAttributes(params.attributes, 0, nullptr)
    {}

    // Sobrecarga de conveniencia que usa WorldConn.cpp (zWorld2): saca la clave
    // de clase de los propios parametros. La cabecera de eaxsound-clean no la
    // trae y sin ella esa unidad no compila.
    static AudioEvent *CreateInstance(const AudioEventParams &params) {
        return UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int>::CreateInstance(params.attributes.GetClassKey(), params);
    }

    const AudioEventParams &GetParameters() const {
        return this->mParams;
    }

    const Attrib::Instance &GetAttributes() const {
        return this->mAttributes;
    }

    virtual ~AudioEvent() {}
    virtual void Release() = 0;
    virtual void Pause(bool) = 0;
    virtual void Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag);
};

class CollisionEvent : public AudioEvent {
  public:
    bVector3 *GetImpulseNormal() {
        return &this->ImpulseNormal;
    }

    Timer CollisionTime;          // offset 0x64, size 0x4
    ALIGNVEC bVector3 InitialContactPoint; // offset 0x68, size 0x10
    ALIGNVEC bVector3 CurrentContactPoint; // offset 0x78, size 0x10
    ALIGNVEC bVector3 CurrentVelocity;     // offset 0x88, size 0x10
    ALIGNVEC bVector3 ImpulseNormal;       // offset 0x98, size 0x10
    int mVolume;                  // offset 0xA8, size 0x4
    int Intensity;                // offset 0xAC, size 0x4
    SND_Stich *ImpactStich;       // offset 0xB0, size 0x4
    WorldConn::Reference mTarget; // offset 0xB4, size 0x10
    unsigned int Description;     // offset 0xC4, size 0x4
    CSTATE_Base *Owner;           // offset 0xC8, size 0x4
    int mRefCount;                // offset 0xCC, size 0x4
    const char *mCSISEffect;      // offset 0xD0, size 0x4
    bool mActive;                 // offset 0xD4, size 0x1
    unsigned int mAudioFX;        // offset 0xD8, size 0x4
    unsigned int mActor;          // offset 0xDC, size 0x4
    unsigned int mActee;          // offset 0xE0, size 0x4

    CollisionEvent(const AudioEventParams &params, bool impact);
    ~CollisionEvent() override;
    void SetOwner(CSTATE_Base *owner);
    void Pause(bool pause) override;
    static AudioEvent *PlayScrape(const AudioEventParams &params);
    static AudioEvent *Play(const AudioEventParams &params);
    void Update(const bVector3 &position, const bVector3 &normal, const bVector3 &velocity, float dt) override;
    void InitAsScrape(const Attrib::Gen::audioscrape &audioFx);
    void InitAsImpact(const Attrib::Gen::audioimpact &audioFx);
    void Release() override;

    const char *GetCSISEffect() const { return this->mCSISEffect; }
    SND_Stich *GetImpactStich() const { return this->ImpactStich; }
    bVector3 *GetInitialContactPoint() { return &this->InitialContactPoint; }
    bVector3 *GetCurrentContactPoint() { return &this->CurrentContactPoint; }
    bVector3 *GetCurrentVelocity() { return &this->CurrentVelocity; }
    bool IsDescribed(unsigned int desc) { return (this->Description & desc) == desc; }
    float GetAge() { return (WorldTimer - this->CollisionTime).GetSeconds(); }
    int GetVolume() { return this->mVolume; }
    int GetIntensity() { return this->Intensity; }
    int IsStillActive() { return this->mActive; }
    unsigned int GetAudioFX() { return this->mAudioFX; }
    unsigned int GetActor() { return this->mActor; }
    unsigned int GetActee() { return this->mActee; }
};

float DistanceToView(const bVector3 *position);

} // namespace Sound
#endif
