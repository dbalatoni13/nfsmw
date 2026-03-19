#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"

#include <float.h>

struct CollisionStream {
    Attrib::RefSpec StreamMoment;
    unsigned char Threshold;
};

struct StitchCollisionVol {
    short Vol1;
    short Vol2;
    short Vol3;
    short Vol4;
};

enum STICH_COLLISION_TYPE {
    CRSH_CAR_2_CAR_FRNT_VLIT1 = 0,
};

#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioimpact.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioscrape.h"

struct CSTATE_Base;
struct EAX_CarState;

extern EAXSound *g_pEAXSound;

namespace Attrib {
struct StringKey;
namespace Gen {
struct audioscrape;
struct audioimpact;
} // namespace Gen
}

namespace Sound {

void AudioEvent::Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag) {
    mParams.position = p;
    mParams.normal = n;
    mParams.velocity = v;
    mParams.magnitude = mag;
}

float DistanceToView(const bVector3 *position) {
    float dist = FLT_MAX;
    eView *view;

    view = eGetView(1, false);
    if (view && view->GetCameraMover()) {
        dist = UMath::Min(view->GetCameraMover()->GetDistanceTo(position), dist);
    }

    view = eGetView(2, false);
    if (view && view->GetCameraMover()) {
        dist = UMath::Min(view->GetCameraMover()->GetDistanceTo(position), dist);
    }

    return dist;
}

bool IsPrimaryTarget(unsigned int object_id) {
    eView *view;

    view = eGetView(1, false);
    if (view) {
        if (view->GetCameraMover()) {
            if (view->GetCameraMover()->GetAnchor()->GetWorldID() == object_id) {
                return true;
            }
        }
    }

    view = eGetView(2, false);
    if (view) {
        if (view->GetCameraMover()) {
            if (view->GetCameraMover()->GetAnchor()->GetWorldID() == object_id) {
                return true;
            }
        }
    }

    return false;
}

unsigned int GetCollisionDescription(const Attrib::StringKey &hash) {
    switch (hash.GetHash32()) {
    case 0x57F6E227:
        return 2;
    case 0x3E275E32:
        return 4;
    case 0xFDC476A6:
        return 8;
    case 0x336FCACF:
        return 0x10;
    case 0xD5ADD1AA:
        return 0x80;
    case 0x8FE44DFE:
        return 0x40;
    case 0x32CCE9F5:
        return 0x20;
    case 0x047645FB:
        return 0x400;
    case 0x15562639:
        return 0x100;
    default:
        return 0;
    }
}

struct CollisionEvent : public AudioEvent {
    Timer CollisionTime;
    bVector3 InitialContactPoint;
    bVector3 CurrentContactPoint;
    bVector3 CurrentVelocity;
    bVector3 ImpulseNormal;
    int mVolume;
    int Intensity;
    SND_Stich *ImpactStich;
    WorldConn::Reference mTarget;
    unsigned int Description;
    CSTATE_Base *Owner;
    int mRefCount;
    const char *mCSISEffect;
    bool mActive;
    unsigned int mAudioFX;
    unsigned int mActor;
    unsigned int mActee;

    CollisionEvent(const AudioEventParams &params, bool isScrape);
    ~CollisionEvent();
    void SetOwner(CSTATE_Base *owner);
    void Pause(bool pause);
    static AudioEvent *PlayScrape(const AudioEventParams &params);
    static AudioEvent *Play(const AudioEventParams &params);
    void Update(const bVector3 &position, const bVector3 &normal, const bVector3 &velocity, float dt);
    void InitAsScrape(const Attrib::Gen::audioscrape &);
    void InitAsImpact(const Attrib::Gen::audioimpact &);
    void Release();
};

CollisionEvent::CollisionEvent(const AudioEventParams &aep, bool impact)
    : AudioEvent(aep) //
    , CollisionTime(WorldTimer.GetPackedTime()) //
    , InitialContactPoint(aep.position) //
    , CurrentContactPoint(aep.position) //
    , ImpulseNormal(aep.normal) //
    , mVolume(0x7FFF) //
    , Intensity(0) //
    , ImpactStich(nullptr) //
    , mTarget(0) //
    , Description(0) //
    , Owner(nullptr) //
    , mRefCount(1) //
    , mCSISEffect(nullptr) //
    , mActive(false) //
    , mAudioFX(0) //
    , mActor(mParams.object) //
    , mActee(mParams.other_object) {
    if (mActor != 0) {
        mTarget.Set(mActor);
    }

    if (mTarget.GetMatrix() != nullptr && mAttributes.IsValid()) {
        Attrib::Attribute descriptionList = mAttributes.Get(0x9925106);
        const unsigned int numDescriptions = descriptionList.GetLength();

        for (unsigned int d = 0; d < numDescriptions; d++) {
            const Attrib::StringKey *hash =
                static_cast<const Attrib::StringKey *>(mAttributes.GetAttributePointer(0x9925106, d));
            if (hash == nullptr) {
                hash = static_cast<const Attrib::StringKey *>(Attrib::DefaultDataArea(0x10));
            }
            Description |= GetCollisionDescription(*hash);
        }

        if (IsPrimaryTarget(mActor) || IsPrimaryTarget(mActee)) {
            Description |= 1;
        }

        float magnitude = mParams.magnitude;
        if (magnitude > 1.0f) {
            magnitude = 1.0f;
        }
        if (magnitude < 0.0f) {
            magnitude = 0.0f;
        }
        Intensity = static_cast<int>(magnitude * 10.0f);

        if (((Description & 6) != 6) || Intensity > 9) {
            Attrib::Instance attributes(mAttributes);
            if (impact) {
                InitAsImpact(*static_cast<const Attrib::Gen::audioimpact *>(static_cast<const void *>(&attributes)));
            } else {
                InitAsScrape(*static_cast<const Attrib::Gen::audioscrape *>(static_cast<const void *>(&attributes)));
            }
        }
    }
}

CollisionEvent::~CollisionEvent() {}

void CollisionEvent::SetOwner(CSTATE_Base *owner) {
    Owner = owner;
}

void CollisionEvent::Pause(bool pause) {
    (void)pause;
}

AudioEvent *CollisionEvent::PlayScrape(const AudioEventParams &params) {
    return new CollisionEvent(params, false);
}

AudioEvent *CollisionEvent::Play(const AudioEventParams &params) {
    return new CollisionEvent(params, true);
}

void CollisionEvent::Update(const bVector3 &position, const bVector3 &normal, const bVector3 &velocity, float dt) {
    (void)position;
    (void)normal;
    (void)velocity;
    (void)dt;
}

void CollisionEvent::InitAsScrape(const Attrib::Gen::audioscrape &) {}

void CollisionEvent::InitAsImpact(const Attrib::Gen::audioimpact &audioFx) {
    int levels[5];
    int num_levels;
    float magnitude;

    mAudioFX = audioFx.GetCollection();
    levels[0] = static_cast<int>(audioFx.Num_STITCH_LEVEL_0());
    levels[1] = static_cast<int>(audioFx.Num_STITCH_LEVEL_1());
    levels[2] = static_cast<int>(audioFx.Num_STITCH_LEVEL_2());
    levels[3] = static_cast<int>(audioFx.Num_STITCH_LEVEL_3());
    levels[4] = 0;

    {
        int i;

        num_levels = 0;
        i = 0;
        if (levels[0] != 0) {
            do {
                i = i + 1;
                num_levels = num_levels + 1;
                if (i > 4) {
                    break;
                }
            } while (levels[i] != 0);
        }
    }

    magnitude = UMath::Clamp(GetParameters().magnitude, 0.0f, 1.0f);

    if (num_levels == 0) {
        return;
    }

    {
        float max_level = static_cast<float>(num_levels - 1);
        int selected_level = UMath::Clamp(static_cast<int>(magnitude * max_level + 0.5f), 0, num_levels - 1);

        {
            static int counter;
            int index;
            STICH_COLLISION_TYPE stich_id;

            if (levels[selected_level] == 0) {
                return;
            }

            counter = counter + 1;
            index = counter % levels[selected_level];

            if (selected_level == 1) {
                stich_id = audioFx.STITCH_LEVEL_1(index);
                mVolume = static_cast<int>(audioFx.Volumes().Vol2);
            } else {
                if (selected_level > 1) {
                    if (selected_level == 2) {
                        stich_id = audioFx.STITCH_LEVEL_2(index);
                        mVolume = static_cast<int>(audioFx.Volumes().Vol3);
                        goto set_stich;
                    }

                    if (selected_level == 3) {
                        stich_id = audioFx.STITCH_LEVEL_3(index);
                        mVolume = static_cast<int>(audioFx.Volumes().Vol4);
                        goto set_stich;
                    }
                }

                stich_id = audioFx.STITCH_LEVEL_0(index);
                mVolume = static_cast<int>(audioFx.Volumes().Vol1);
            }

        set_stich:
            ImpactStich = &g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_COLLISION, stich_id);
        }
    }
}

void CollisionEvent::Release() {}

} // namespace Sound

template <>
UTL::COM::Factory<Sound::AudioEventParams const &, Sound::AudioEvent, unsigned int>::Prototype *
    UTL::COM::Factory<Sound::AudioEventParams const &, Sound::AudioEvent, unsigned int>::Prototype::mHead = nullptr;

UTL::COM::Factory<Sound::AudioEventParams const &, Sound::AudioEvent, unsigned int>::Prototype __audioscrape(
    Attrib::Gen::audioscrape::ClassKey(), Sound::CollisionEvent::PlayScrape);
UTL::COM::Factory<Sound::AudioEventParams const &, Sound::AudioEvent, unsigned int>::Prototype __audioimpact(
    Attrib::Gen::audioimpact::ClassKey(), Sound::CollisionEvent::Play);

EAX_CarState *GetClosestPlayerCar(const bVector3 *vPosition, bool CameraRelative, int &CarID) {
    (void)vPosition;
    (void)CameraRelative;
    CarID = -1;
    return nullptr;
}

EAX_CarState *GetClosestCopCarToCamera() {
    return nullptr;
}

EAX_CarState *GetPlayerCarInRadius(bVector3 &vPos, float fRadius) {
    (void)vPos;
    (void)fRadius;
    return nullptr;
}
