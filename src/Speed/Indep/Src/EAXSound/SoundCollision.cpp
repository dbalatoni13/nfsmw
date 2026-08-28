#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Generated/AttribSys/GenericAccessor.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

static const int IsCollisionSoundEnabled = 1; // size: 0x4, Decl: 52

BIND_AUDIO_EVENT(audioscrape, Sound::CollisionEvent::PlayScrape); // Decl: 101

BIND_AUDIO_EVENT(audioimpact, Sound::CollisionEvent::Play); // Decl: 151

static const int THRESHOLD_SMAKEABLE_VS_WORLD = 10; // size: 0x4, Decl: 156

namespace Sound {

float DistanceToView(const bVector3 *position) {
    float dist = 100000.0f;
    eView *view = eGetView(1, false);
    if (view != nullptr && view->GetCameraMover() != nullptr) {
        dist = UMath::Min(view->GetCameraMover()->GetDistanceTo(position), dist);
    }

    view = eGetView(2, false);
    if (view != nullptr && view->GetCameraMover() != nullptr) {
        dist = UMath::Min(view->GetCameraMover()->GetDistanceTo(position), dist);
    }

    return dist;
}

bool IsPrimaryTarget(unsigned int object_id) {
    eView *view = eGetView(1, false);
    if (view != nullptr && view->GetCameraMover() != nullptr) {
        if (view->GetCameraMover()->GetAnchor() != nullptr) {
            if (view->GetCameraMover()->GetAnchor()->GetWorldID() == object_id) {
                return true;
            }
        }
    }

    view = eGetView(2, false);
    if (view != nullptr && view->GetCameraMover() != nullptr) {
        if (view->GetCameraMover()->GetAnchor() != nullptr) {
            if (view->GetCameraMover()->GetAnchor()->GetWorldID() == object_id) {
                return true;
            }
        }
    }

    return false;
}

AudioEvent *CollisionEvent::PlayScrape(const AudioEventParams &aep) {
    if (DistanceToView(&aep.position) > 100.0f) {
        return nullptr;
    }

    if (IsSoundEnabled == 0) {
        return nullptr;
    }

    if (aep.attributes.GetCollectionKey() == 0) {
        return nullptr;
    }

    CollisionEvent *bang = new ("Sound::CollisionEvent", 0) CollisionEvent(aep, false);
    if (bang->GetCSISEffect() == nullptr) {
        delete bang;
        return nullptr;
    }

    CSTATE_Base *StateObj = nullptr;
    if (g_pEAXSound != nullptr && EAXSound::GetStateMgr(eMM_COLLISION) != nullptr) {
        CSTATEMGR_Base *CollisionMgr = EAXSound::GetStateMgr(eMM_COLLISION);
        StateObj = CollisionMgr->GetFreeState(bang);
        if (StateObj != nullptr) {
            StateObj->Attach(bang);
            bang->mRefCount++;
        }
    }

    if (StateObj == nullptr) {
        delete bang;
        return nullptr;
    }

    return bang;
}

AudioEvent *CollisionEvent::Play(const AudioEventParams &aep) {
    if (IsSoundEnabled == 0) {
        return nullptr;
    }

    if (DistanceToView(&aep.position) > 100.0f) {
        return nullptr;
    }

    if (aep.attributes.GetCollectionKey() == 0) {
        return nullptr;
    }

    CollisionEvent *bang = new ("Sound::CollisionEvent", 0) CollisionEvent(aep, true);
    CSTATE_Base *StateObj = nullptr;
    if (bang->GetImpactStich() != nullptr && g_pEAXSound != nullptr && EAXSound::GetStateMgr(eMM_COLLISION) != nullptr) {
        CSTATEMGR_Base *CollisionMgr = EAXSound::GetStateMgr(eMM_COLLISION);
        StateObj = CollisionMgr->GetFreeState(bang);
        if (StateObj != nullptr) {
            StateObj->Attach(bang);
        }
    }

    if (StateObj == nullptr && bang != nullptr) {
        delete bang;
    }

    return nullptr;
}

unsigned int GetCollisionDescription(const Attrib::StringKey &hash);

CollisionEvent::CollisionEvent(const AudioEventParams &aep, bool impact)
    : AudioEvent(aep),                      //
      CollisionTime(WorldTimer),            //
      InitialContactPoint(aep.position),    //
      CurrentContactPoint(aep.position),    //
      ImpulseNormal(aep.normal),            //
      mVolume(0x7FFF),                      //
      Intensity(0),                         //
      ImpactStich(nullptr),                 //
      mTarget(0),                           //
      Description(0),                       //
      Owner(nullptr),                       //
      mRefCount(1),                         //
      mCSISEffect(nullptr),                 //
      mActive(false),                       //
      mActor(this->GetParameters().object), //
      mActee(this->GetParameters().other_object) {
    unsigned int object_id = this->GetParameters().object;

    if (object_id != 0) {
        this->mTarget.Set(object_id);
    }

    if (this->mTarget.IsValid()) {
        if (this->GetAttributes().IsValid()) {
            unsigned int numdesc = this->GetAttributes()->Num_DESCRIPTION();

            for (unsigned int d = 0; d < numdesc; d++) {
                this->Description |= GetCollisionDescription(this->GetAttributes()->DESCRIPTION(d));
            }

            if (IsPrimaryTarget(object_id) || IsPrimaryTarget(this->GetParameters().other_object)) {
                this->Description |= 1;
            }

            float magnitude = UMath::Clamp(this->GetParameters().magnitude, 0.0f, 1.0f);
            this->Intensity = static_cast<int>(magnitude * 127.0f);

            if (((this->Description & 6) != 6) || this->Intensity > 9) {
                if (impact == true) {
                    this->InitAsImpact(Attrib::Gen::audioimpact(this->GetAttributes()));
                } else {
                    this->InitAsScrape(Attrib::Gen::audioscrape(this->GetAttributes()));
                }
            }
        }
    }
}

void CollisionEvent::Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag) {
    AudioEvent::Update(p, n, v, mag);

    float magnitude = UMath::Clamp(mag, 0.0f, 1.0f);
    this->Intensity = static_cast<int>(magnitude * 127.0f);
    this->CurrentContactPoint = p;
    this->CollisionTime = WorldTimer;
}

void CollisionEvent::InitAsScrape(const Attrib::Gen::audioscrape &audioFx) {
    Attrib::StringKey effect(audioFx.CSIS_EFFECT());
    if (!effect.IsEmpty()) {
        this->mCSISEffect = effect.GetString();
        this->Description |= SD_IS_SCRAPE;
        this->mActive = true;
    }
}

void CollisionEvent::InitAsImpact(const Attrib::Gen::audioimpact &audioFx) {
    this->mAudioFX = audioFx.GetCollection();

    int levels[5];
    levels[0] = static_cast<int>(audioFx.Num_STITCH_LEVEL_0());
    levels[1] = static_cast<int>(audioFx.Num_STITCH_LEVEL_1());
    levels[2] = static_cast<int>(audioFx.Num_STITCH_LEVEL_2());
    levels[3] = static_cast<int>(audioFx.Num_STITCH_LEVEL_3());
    levels[4] = 0;

    int num_levels = 0;

    for (int i = 0; i < NUM_ELEMENTS(levels) && levels[i] != 0; i++) {
        num_levels++;
    }

    float magnitude = UMath::Clamp(this->GetParameters().magnitude, 0.0f, 1.0f);

    if (num_levels == 0) {
        return;
    } else {
        float max_level = static_cast<float>(num_levels - 1);
        int selected_level = UMath::Clamp(static_cast<int>(magnitude * max_level + 0.5f), 0, num_levels - 1);

        {
            static int counter = 0;

            if (levels[selected_level] == 0) {
                return;
            }

            counter++;
            int index = counter % levels[selected_level];

            STICH_COLLISION_TYPE stich_id;
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

    if (this->Owner != nullptr) {
        this->Owner->Detach();
    }

    this->mRefCount--;
    if (this->mRefCount == 0) {
        delete this;
    }
}

CollisionEvent::~CollisionEvent() {
    this->mTarget.Set(0);
}

void CollisionEvent::SetOwner(CSTATE_Base *owner) {
    this->Owner = owner;
}

unsigned int GetCollisionDescription(const Attrib::StringKey &hash) {
    switch (hash.GetHash32()) {
        case UCRC32_SMOKABLE:
            return SD_SMOKABLE_INVOLVED;
        case UCRC32_WALL:
            return SD_WALL_INVOLVED;
        case UCRC32_TWO_CAR:
            return SD_TWO_CAR_COLLISION;
        case UCRC32_CAR:
            return SD_IS_CAR_INVOLVED;
        case UCRC32_FRONT:
            return SD_IS_FRONT;
        case UCRC32_BOTTOMOUT:
            return SD_IS_BOTTOMOUT;
        case UCRC32_ROLLOVER:
            return SD_IS_ROLLOVER;
        case UCRC32_EVENT:
            return SD_IS_EVENT;
        case UCRC32_SIDE:
            return SD_IS_SIDE;
        default:
            return 0;
    }
}

}; // namespace Sound
