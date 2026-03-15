#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"

struct CSTATE_Base;
struct EAX_CarState;
struct EAXSound;

extern EAXSound *g_pEAXSound;

namespace Attrib {
struct StringKey;
namespace Gen {
struct audioscrape;
struct audioimpact;
} // namespace Gen
}

namespace Sound {

struct AudioEventParams {
    bVector3 position;
    bVector3 normal;
    bVector3 velocity;
    float magnitude;
    Attrib::RefSpec attributes;
    unsigned int object;
    unsigned int other_object;
};

struct CollisionDescription {
    int _pad;
};

struct AudioEvent {
    unsigned int mFactoryPad;
    AudioEventParams mParams;
    Attrib::Instance mAttributes;
    void *mVTableAudioEvent;

    AudioEvent(const AudioEventParams &params)
        : mFactoryPad(0) //
        , mParams(params) //
        , mAttributes(params.attributes, 0, nullptr) //
        , mVTableAudioEvent(nullptr)
    {}

    const AudioEventParams &GetParameters() const {
        return mParams;
    }

    const Attrib::Instance &GetAttributes() const {
        return mAttributes;
    }
};

float DistanceToView(const bVector3 *vPosition) {
    (void)vPosition;
    return 0.0f;
}

bool IsPrimaryTarget(unsigned int target) {
    (void)target;
    return false;
}

unsigned int GetCollisionDescription(const Attrib::StringKey &key) {
    (void)key;
    return 0;
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
    char _padActive[3];
    unsigned int mAudioFX;
    unsigned int mActor;
    unsigned int mActee;

    CollisionEvent(const AudioEventParams &params, bool isScrape);
    ~CollisionEvent();
    void SetOwner(CSTATE_Base *owner);
    void Pause(bool pause);
    int PlayScrape(const AudioEventParams &params);
    int Play(const AudioEventParams &params);
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
                InitAsImpact(*reinterpret_cast<const Attrib::Gen::audioimpact *>(&attributes));
            } else {
                InitAsScrape(*reinterpret_cast<const Attrib::Gen::audioscrape *>(&attributes));
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

int CollisionEvent::PlayScrape(const AudioEventParams &params) {
    (void)params;
    return 0;
}

int CollisionEvent::Play(const AudioEventParams &params) {
    (void)params;
    return 0;
}

void CollisionEvent::Update(const bVector3 &position, const bVector3 &normal, const bVector3 &velocity, float dt) {
    (void)position;
    (void)normal;
    (void)velocity;
    (void)dt;
}

void CollisionEvent::InitAsScrape(const Attrib::Gen::audioscrape &) {}

void CollisionEvent::InitAsImpact(const Attrib::Gen::audioimpact &audioFx) {
    static unsigned int sCollisionCounter;

    const Attrib::Instance *instance = reinterpret_cast<const Attrib::Instance *>(&audioFx);
    int levels[5];

    *reinterpret_cast<Attrib::Key *>(reinterpret_cast<char *>(this) + 0x100) = instance->GetCollection();

    {
        Attrib::Attribute attribute = instance->Get(0xc15856df);
        levels[0] = static_cast<int>(attribute.GetLength());
    }

    {
        Attrib::Attribute attribute = instance->Get(0xdadb5580);
        levels[1] = static_cast<int>(attribute.GetLength());
    }

    {
        Attrib::Attribute attribute = instance->Get(0xc9218f8c);
        levels[2] = static_cast<int>(attribute.GetLength());
    }

    {
        Attrib::Attribute attribute = instance->Get(0xefbca3c9);
        levels[3] = static_cast<int>(attribute.GetLength());
    }

    levels[4] = 0;

    int numLevels = 0;
    while (numLevels < 5 && levels[numLevels] != 0) {
        numLevels++;
    }

    if (numLevels == 0) {
        return;
    }

    float magnitude = *reinterpret_cast<float *>(reinterpret_cast<char *>(this) + 0x40);
    if (magnitude > 1.0f) {
        magnitude = 1.0f;
    }
    if (magnitude < 0.0f) {
        magnitude = 0.0f;
    }

    const int maxLevel = numLevels - 1;
    int selectedLevel = static_cast<int>(magnitude * static_cast<float>(maxLevel) + 0.5f);
    if (selectedLevel < 0) {
        selectedLevel = 0;
    } else if (selectedLevel > maxLevel) {
        selectedLevel = maxLevel;
    }

    const int count = levels[selectedLevel];
    if (count == 0) {
        return;
    }

    sCollisionCounter++;
    const unsigned int entry = sCollisionCounter % static_cast<unsigned int>(count);

    const short *volumes = static_cast<const short *>(instance->GetAttributePointer(0xfcc8e754, 0));
    if (volumes == nullptr) {
        volumes = static_cast<const short *>(Attrib::DefaultDataArea(8));
    }

    int stitchIndex;
    int volume;

    if (selectedLevel == 1) {
        const int *stitches = static_cast<const int *>(instance->GetAttributePointer(0xdadb5580, entry));
        if (stitches == nullptr) {
            stitches = static_cast<const int *>(Attrib::DefaultDataArea(4));
        }
        stitchIndex = *stitches;
        volume = static_cast<int>(volumes[1]);
    } else if (selectedLevel == 2) {
        const int *stitches = static_cast<const int *>(instance->GetAttributePointer(0xc9218f8c, entry));
        if (stitches == nullptr) {
            stitches = static_cast<const int *>(Attrib::DefaultDataArea(4));
        }
        stitchIndex = *stitches;
        volume = static_cast<int>(volumes[2]);
    } else if (selectedLevel == 3) {
        const int *stitches = static_cast<const int *>(instance->GetAttributePointer(0xefbca3c9, entry));
        if (stitches == nullptr) {
            stitches = static_cast<const int *>(Attrib::DefaultDataArea(4));
        }
        stitchIndex = *stitches;
        volume = static_cast<int>(volumes[3]);
    } else {
        const int *stitches = static_cast<const int *>(instance->GetAttributePointer(0xc15856df, entry));
        if (stitches == nullptr) {
            stitches = static_cast<const int *>(Attrib::DefaultDataArea(4));
        }
        stitchIndex = *stitches;
        volume = static_cast<int>(volumes[0]);
    }

    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0xd0) = volume;

    cSTICH_PlayBack *playback = *reinterpret_cast<cSTICH_PlayBack **>(reinterpret_cast<char *>(g_pEAXSound) + 0x9c);
    *reinterpret_cast<SND_Stich **>(reinterpret_cast<char *>(this) + 0xd8) = &playback->GetStich(STICH_TYPE_COLLISION, stitchIndex);
}

void CollisionEvent::Release() {}

} // namespace Sound

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
