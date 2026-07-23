#ifndef SOUND_COLLISION_HPP_
#define SOUND_COLLISION_HPP_

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/EAXSound/STICH_Struct.h"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioimpact.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioscrape.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Sound {

// Decl: 57
struct AudioEventParams {
    bVector3 position;          // offset 0x0, size 0x10
    bVector3 normal;            // offset 0x10, size 0x10
    bVector3 velocity;          // offset 0x20, size 0x10
    float magnitude;            // offset 0x30, size 0x4
    Attrib::RefSpec attributes; // offset 0x34, size 0xC
    WUID object;                // offset 0x40, size 0x4
    WUID other_object;          // offset 0x44, size 0x4
};

// Decl: 69
class AudioEvent : public UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int> {
  public:
    static AudioEvent *CreateInstance(const AudioEventParams &params) {
        return UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int>::CreateInstance(params.attributes.GetClassKey(), params);
    }

    virtual ~AudioEvent() {}
    virtual void Release() = 0;
    virtual void Pause(bool pause) = 0;
    virtual void Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag) {}

    const AudioEventParams &GetParameters() const {
        return mParams;
    }

    const Attrib::Instance &GetAttributes() const {
        return mAttributes;
    }

  private:
    AudioEventParams mParams;     // offset 0x4, size 0x48
    Attrib::Instance mAttributes; // offset 0x4C, size 0x14
};

// total size: 0xE4
// Decl: 92
class CollisionEvent : public AudioEvent {
  public:
    static AudioEvent *Play(const AudioEventParams &aep);
    static AudioEvent *PlayScrape(const AudioEventParams &aep);

    // Overrides: AudioEvent
    void Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag) override;

    int IsStillActive() {
        return static_cast<int>(this->mActive);
    }
    float GetAge() {
        return (WorldTimer - CollisionTime).GetSeconds();
    }
    bVector3 *GetInitialContactPoint() {
        return &this->InitialContactPoint;
    }
    bVector3 *GetCurrentContactPoint() {
        return &this->CurrentContactPoint;
    }
    bVector3 *GetImpulseNormal() {
        return &this->ImpulseNormal;
    }
    bVector3 *GetCurrentVelocity() {
        return &this->CurrentVelocity;
    }

    // Overrides: AudioEvent
    void Release() override;
    void Pause(bool pause) override {} // Decl: 107

    SND_Stich *GetImpactStich() const {
        return this->ImpactStich;
    }

    const char *GetCSISEffect() const {
        return this->mCSISEffect;
    }

    int GetIntensity() {
        return this->Intensity;
    }

    int GetVolume() {
        return this->mVolume;
    }

    uint32 GetDescription() {
        return this->Description;
    }

    bool IsDescribed(uint32 desc) {
        return (this->Description & desc) == desc;
    }

    void SetOwner(CSTATE_Base *owner);

    Attrib::Key GetAudioFX() {
        return this->mAudioFX;
    }

    WUID GetActor() {
        return this->mActor;
    }
    WUID GetActee() {
        return this->mActee;
    }

  private:
    // Overrides: AudioEvent
    ~CollisionEvent() override;

    void InitAsImpact(const Attrib::Gen::audioimpact &audioFx);

    void InitAsScrape(const Attrib::Gen::audioscrape &audioFx);

  public:
    CollisionEvent(const AudioEventParams &aep, bool impact);

  private:
    Timer CollisionTime; // offset 0x64, size 0x4, Decl: 147

    bVector3 InitialContactPoint; // offset 0x68, size 0x10, Decl: 150
    bVector3 CurrentContactPoint; // offset 0x78, size 0x10, Decl: 151
    bVector3 CurrentVelocity;     // offset 0x88, size 0x10, Decl: 152

    bVector3 ImpulseNormal; // offset 0x98, size 0x10, Decl: 156
    int mVolume;            // offset 0xA8, size 0x4, Decl: 157
    int Intensity;          // offset 0xAC, size 0x4, Decl: 158

    SND_Stich *ImpactStich;       // offset 0xB0, size 0x4, Decl: 160
    WorldConn::Reference mTarget; // offset 0xB4, size 0x10, Decl: 161
    uint32 Description;           // offset 0xC4, size 0x4, Decl: 163
    CSTATE_Base *Owner;           // offset 0xC8, size 0x4, Decl: 164
    int mRefCount;                // offset 0xCC, size 0x4, Decl: 165
    const char *mCSISEffect;      // offset 0xD0, size 0x4, Decl: 166
    bool mActive;                 // offset 0xD4, size 0x1, Decl: 167

    Attrib::Key mAudioFX; // offset 0xD8, size 0x4, Decl: 169

    WUID mActor; // offset 0xDC, size 0x4, Decl: 171
    WUID mActee; // offset 0xE0, size 0x4, Decl: 172
};

float DistanceToView(const bVector3 *position);

} // namespace Sound

#endif
