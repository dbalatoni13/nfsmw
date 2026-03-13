struct CSTATE_Base;
struct bVector3;
struct EAX_CarState;

namespace Attrib {
struct StringKey;
namespace Gen {
struct audioscrape;
struct audioimpact;
} // namespace Gen
}

namespace Sound {

struct AudioEventParams {
    int _pad;
};

struct CollisionDescription {
    int _pad;
};

float DistanceToView(const bVector3 *vPosition) {
    (void)vPosition;
    return 0.0f;
}

bool IsPrimaryTarget(unsigned int target) {
    (void)target;
    return false;
}

CollisionDescription *GetCollisionDescription(const Attrib::StringKey &key) {
    (void)key;
    return nullptr;
}

struct CollisionEvent {
    char _pad[0xC8];
    CSTATE_Base *Owner;

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

CollisionEvent::CollisionEvent(const AudioEventParams &params, bool isScrape)
    : Owner(nullptr) {
    (void)params;
    (void)isScrape;
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

void CollisionEvent::InitAsImpact(const Attrib::Gen::audioimpact &) {}

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
