#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"

#include <float.h>

#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioimpact.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioscrape.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

extern EAXSound *g_pEAXSound;

namespace Sound {

inline void AudioEvent::Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag) {
    this->mParams.position = p;
    this->mParams.normal = n;
    this->mParams.velocity = v;
    this->mParams.magnitude = mag;
}

float DistanceToView(const bVector3 *position) {
    float dist = 100000.0f;
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
            if (view->GetCameraMover()->GetAnchor()) {
                if (view->GetCameraMover()->GetAnchor()->GetWorldID() == object_id) {
                    return true;
                }
            }
        }
    }

    view = eGetView(2, false);
    if (view) {
        if (view->GetCameraMover()) {
            if (view->GetCameraMover()->GetAnchor()) {
                if (view->GetCameraMover()->GetAnchor()->GetWorldID() == object_id) {
                    return true;
                }
            }
        }
    }

    return false;
}

unsigned int GetCollisionDescription(const Attrib::StringKey &hash);

AudioEvent *CollisionEvent::PlayScrape(const AudioEventParams &aep) {
    CollisionEvent *bang;
    CSTATE_Base *StateObj;

    if (DistanceToView(&aep.position) > 100.0f) {
        return nullptr;
    }

    if (IsSoundEnabled == 0) {
        return nullptr;
    }

    if (aep.attributes.GetCollectionKey() == 0) {
        return nullptr;
    }

    bang = new ("Sound::CollisionEvent", 0) CollisionEvent(aep, false);
    if (!bang->GetCSISEffect()) {
        if (bang) {
            delete bang;
        }
        return nullptr;
    }
    StateObj = nullptr;
    if (g_pEAXSound && EAXSound::GetStateMgr(eMM_COLLISION)) {
        CSTATEMGR_Base *CollisionMgr = EAXSound::GetStateMgr(eMM_COLLISION);
        if ((StateObj = CollisionMgr->GetFreeState(bang))) {
            StateObj->Attach(bang);
            bang->mRefCount = bang->mRefCount + 1;
        }
    }
    if (!StateObj) {
        if (bang) {
            delete bang;
        }
        return nullptr;
    }
    return bang;
}

AudioEvent *CollisionEvent::Play(const AudioEventParams &aep) {
    CollisionEvent *bang;
    CSTATE_Base *StateObj;

    if (IsSoundEnabled == 0) {
        return nullptr;
    }

    if (DistanceToView(&aep.position) > 100.0f) {
        return nullptr;
    }

    if (aep.attributes.GetCollectionKey() == 0) {
        return nullptr;
    }

    bang = new ("Sound::CollisionEvent", 0) CollisionEvent(aep, true);
    StateObj = nullptr;
    if (bang->GetImpactStich() && g_pEAXSound && EAXSound::GetStateMgr(eMM_COLLISION)) {
        CSTATEMGR_Base *CollisionMgr = EAXSound::GetStateMgr(eMM_COLLISION);
        if ((StateObj = CollisionMgr->GetFreeState(bang))) {
            StateObj->Attach(bang);
        }
    }

    if (!StateObj && bang) {
        delete bang;
    }

    return nullptr;
}

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
    , mActor(mParams.object) //
    , mActee(mParams.other_object) {
    unsigned int actor = this->mActor;

    if (actor != 0) {
        this->mTarget.Set(actor);
    }

    if (this->mTarget.IsValid()) {
        if (this->mAttributes.IsValid()) {
            unsigned int numDescriptions = this->GetAttributes()->Num_DESCRIPTION();

            for (unsigned int d = 0; d < numDescriptions; d++) {
                this->Description |= GetCollisionDescription(this->GetAttributes()->DESCRIPTION(d));
            }

            if (IsPrimaryTarget(actor) || IsPrimaryTarget(this->mParams.other_object)) {
                this->Description |= 1;
            }

            float magnitude = UMath::Clamp(this->GetParameters().magnitude, 0.0f, 1.0f);
            this->Intensity = static_cast<int>(magnitude * 127.0f);

            if (((this->Description & 6) != 6) || this->Intensity > 9) {
                if (impact == true) {
                    Attrib::Gen::audioimpact attributes(this->GetAttributes());
                    this->InitAsImpact(attributes);
                } else {
                    Attrib::Gen::audioscrape attributes(this->GetAttributes());
                    this->InitAsScrape(attributes);
                }
            }
        }
    }
}

void CollisionEvent::Update(const bVector3 &position, const bVector3 &normal, const bVector3 &velocity, float dt) {
    float magnitude;

    this->mParams.position = position;
    this->mParams.normal = normal;
    this->mParams.velocity = velocity;
    this->mParams.magnitude = dt;
    magnitude = UMath::Clamp(dt, 0.0f, 1.0f);
    this->Intensity = static_cast<int>(magnitude * 127.0f);
    this->CurrentContactPoint = position;
    this->CollisionTime = WorldTimer;
}

void CollisionEvent::InitAsScrape(const Attrib::Gen::audioscrape &audioFx) {
    Attrib::StringKey effect(audioFx.CSIS_EFFECT());
    if (!effect.IsEmpty()) {
        const char *effectString = effect.GetString();

        this->mActive = true;
        this->mCSISEffect = effectString;
        this->Description |= 0x200;
    }
}

void CollisionEvent::InitAsImpact(const Attrib::Gen::audioimpact &audioFx) {
    int levels[5];
    int num_levels;
    float magnitude;

    this->mAudioFX = audioFx.GetCollection();
    levels[0] = static_cast<int>(audioFx.Num_STITCH_LEVEL_0());
    levels[1] = static_cast<int>(audioFx.Num_STITCH_LEVEL_1());
    levels[2] = static_cast<int>(audioFx.Num_STITCH_LEVEL_2());
    levels[3] = static_cast<int>(audioFx.Num_STITCH_LEVEL_3());
    levels[4] = 0;

    {
        int i;

        num_levels = 0;
        for (i = 0; i < 5 && levels[i] != 0; i++) {
            num_levels++;
        }
    }

    magnitude = UMath::Clamp(this->GetParameters().magnitude, 0.0f, 1.0f);

    if (num_levels == 0) {
        return;
    }

    {
        float max_level = static_cast<float>(num_levels - 1);
        int selected_level = UMath::Clamp(static_cast<int>(magnitude * max_level + 0.5f), 0, num_levels - 1);

        {
            // El inicializador decide .data contra .bss en GCC 2.9 aunque el
            // valor sea cero, y el objetivo lo tiene en .data (r48).
            static int counter = 0;
            int index;
            STICH_COLLISION_TYPE stich_id;

            if (levels[selected_level] == 0) {
                return;
            }

            counter = counter + 1;
            index = counter % levels[selected_level];

            switch (selected_level) {
                case 0:
                default:
                    stich_id = audioFx.STITCH_LEVEL_0(index);
                    this->mVolume = static_cast<int>(audioFx.Volumes().Vol1);
                    break;

                case 1:
                    stich_id = audioFx.STITCH_LEVEL_1(index);
                    this->mVolume = static_cast<int>(audioFx.Volumes().Vol2);
                    break;

                case 2:
                    stich_id = audioFx.STITCH_LEVEL_2(index);
                    this->mVolume = static_cast<int>(audioFx.Volumes().Vol3);
                    break;

                case 3:
                    stich_id = audioFx.STITCH_LEVEL_3(index);
                    this->mVolume = static_cast<int>(audioFx.Volumes().Vol4);
                    break;
            }
            this->ImpactStich = &g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_COLLISION, stich_id);
        }
    }
}

void CollisionEvent::Release() {
    this->mActive = false;

    if (this->Owner) {
        this->Owner->Detach();
    }

    this->mRefCount = this->mRefCount - 1;
    if (this->mRefCount == 0 && this) {
        delete this;
    }
}

CollisionEvent::~CollisionEvent() {
    this->mTarget.Set(0);
}

void CollisionEvent::SetOwner(CSTATE_Base *owner) {
    this->Owner = owner;
}

inline void CollisionEvent::Pause(bool pause) {
    (void)pause;
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

} // namespace Sound

template <>
UTL::COM::Factory<Sound::AudioEventParams const &, Sound::AudioEvent, unsigned int>::Prototype *
    UTL::COM::Factory<Sound::AudioEventParams const &, Sound::AudioEvent, unsigned int>::Prototype::mHead = nullptr;

UTL::COM::Factory<Sound::AudioEventParams const &, Sound::AudioEvent, unsigned int>::Prototype __audioscrape(
    Attrib::Gen::audioscrape::ClassKey(), Sound::CollisionEvent::PlayScrape);
UTL::COM::Factory<Sound::AudioEventParams const &, Sound::AudioEvent, unsigned int>::Prototype __audioimpact(
    Attrib::Gen::audioimpact::ClassKey(), Sound::CollisionEvent::Play);
