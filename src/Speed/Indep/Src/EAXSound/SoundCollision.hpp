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

struct CollisionStream {
    Attrib::RefSpec StreamMoment; // offset 0x0, size 0xC
    unsigned char Threshold;      // offset 0xC, size 0x1
};

struct StitchCollisionVol {
    short Vol1; // offset 0x0, size 0x2
    short Vol2; // offset 0x2, size 0x2
    short Vol3; // offset 0x4, size 0x2
    short Vol4; // offset 0x6, size 0x2
};

enum STICH_COLLISION_TYPE {
    CRSH_CAR_2_CAR_FRNT_VLIT1 = 0,
};

namespace Attrib {
namespace Gen {
struct audioimpact;
struct audioscrape;
} // namespace Gen
} // namespace Attrib

struct CSTATE_Base;
struct SND_Stich;

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
    AudioEventParams mParams;
    Attrib::Instance mAttributes;

    AudioEvent(const AudioEventParams &params)
        : mParams(params) //
        , mAttributes(params.attributes, 0, nullptr)
    {}

    const AudioEventParams &GetParameters() const {
        return mParams;
    }

    const Attrib::Instance &GetAttributes() const {
        return mAttributes;
    }

    virtual ~AudioEvent() {}
    virtual void Release() {}
    virtual void Pause(bool) {}
    virtual void Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag);
};

class CollisionEvent : public AudioEvent {
  public:
    Timer CollisionTime;          // offset 0x64, size 0x4
    bVector3 InitialContactPoint; // offset 0x68, size 0x10
    bVector3 CurrentContactPoint; // offset 0x78, size 0x10
    bVector3 CurrentVelocity;     // offset 0x88, size 0x10
    bVector3 ImpulseNormal;       // offset 0x98, size 0x10
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

    const char *GetCSISEffect() const { return mCSISEffect; }
    SND_Stich *GetImpactStich() const { return ImpactStich; }
    bVector3 *GetInitialContactPoint() { return &InitialContactPoint; }
    bVector3 *GetCurrentContactPoint() { return &CurrentContactPoint; }
    bVector3 *GetCurrentVelocity() { return &CurrentVelocity; }
    bool IsDescribed(unsigned int desc) { return (Description & desc) == desc; }
    float GetAge() { return (WorldTimer - CollisionTime).GetSeconds(); }
    int GetVolume() { return mVolume; }
    int GetIntensity() { return Intensity; }
    int IsStillActive() { return mActive; }
    unsigned int GetAudioFX() { return mAudioFX; }
    unsigned int GetActor() { return mActor; }
    unsigned int GetActee() { return mActee; }
};

} // namespace Sound
#endif
