#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Sim/SimServer.h"

// TODO move
void bRotateVector(bVector3 *dest, const bMatrix4 *m, bVector3 *v);

namespace WorldConn {

static Server *_Server;
static int world_refcount = 0;

Server::Server() {}

Server::~Server() {}

Server::Body *Server::LockID(unsigned int id) {
    BodyMap::iterator iter = mBodies.find(id);
    if (iter == mBodies.end()) {
        Body *body = new Body();
        mBodies[id] = body;
        return body;
    }
    iter->second->refcount++;
    return iter->second;
}

void Server::UnlockID(unsigned int id) {
    BodyMap::iterator iter = mBodies.find(id);
    iter->second->refcount--;
    if (iter->second->refcount == 0) {
        delete iter->second;
        mBodies.erase(iter);
    }
}

Reference::Reference(unsigned int worldid)
    : mWorldID(worldid), //
      mMatrix(nullptr),  //
      mVelocity(nullptr) {
    Lock();
    world_refcount++;
}

Reference::~Reference() {
    Unlock();
    world_refcount--;
}

void Reference::Set(unsigned int worldid) {
    if (worldid != mWorldID) {
        Unlock();
        mWorldID = worldid;
    }
    Lock();
}

void Reference::Lock() {
    if ((mMatrix == nullptr) && mWorldID != 0) {
        const Server::Body *body = _Server->LockID(mWorldID);
        mMatrix = &body->matrix;
        mVelocity = &body->velocity;
        mAcceleration = &body->acceleration;
    }
}

void Reference::Unlock() {
    if ((mMatrix != nullptr) && mWorldID != 0) {
        _Server->UnlockID(mWorldID);
        mMatrix = nullptr;
        mVelocity = nullptr;
        mAcceleration = nullptr;
    }
}

}; // namespace WorldConn

int *World_UpdateBody(Sim::Packet *pkt) {
    WorldConn::Pkt_Body_Send *data = Sim::Packet::Cast<WorldConn::Pkt_Body_Send>(pkt);
    WorldConn::Server::Body *body = WorldConn::_Server->LockID(data->mID);
    eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(data->mMatrix), body->matrix);
    WorldConn::_Server->UnlockID(data->mID);
    return nullptr;
}

class WorldBodyConn : public Sim::Connection, public bTNode<WorldBodyConn> {
  public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    WorldBodyConn(const Sim::ConnectionData &data);
    ~WorldBodyConn() override;

    void OnClose() override {
        delete this;
    }

    Sim::ConnStatus OnStatusCheck() override {
        return Sim::CONNSTATUS_READY;
    }

    void Update(float dT);
    static void FetchData(float dT);

  private:
    WUID mID;                       // offset 0x18, size 0x4
    WorldConn::Server::Body *mDest; // offset 0x1C, size 0x4
    static bTList<WorldBodyConn> mList;
};

BIND_SIM_CONN(WorldBodyConn);

Sim::Connection *WorldBodyConn::Construct(const Sim::ConnectionData &data) {
    return new WorldBodyConn(data);
}

WorldBodyConn::WorldBodyConn(const Sim::ConnectionData &data)
    : Connection(data), //
      mID(0) {
    mList.AddTail(this);
    WorldConn::Pkt_Body_Open *oc = Sim::Packet::Cast<WorldConn::Pkt_Body_Open>(data.pkt);
    mID = oc->mID;
    mDest = WorldConn::_Server->LockID(mID);
    eSwizzleWorldMatrix(reinterpret_cast<bMatrix4 &>(oc->mMatrix), mDest->matrix);
}

WorldBodyConn::~WorldBodyConn() {
    WorldConn::_Server->UnlockID(mID);
    mList.Remove(this);
}

void WorldBodyConn::Update(float dT) {
    WorldConn::Pkt_Body_Service pkt;
    pkt.mMatrix = UMath::Matrix4::kIdentity;
    if (!Service(&pkt)) {
        bFill(&mDest->acceleration, 0.0f, 0.0f, 0.0f);
        mDest->time = 0.0f;
        return;
    }
    bVector3 prevvel(mDest->velocity);
    eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(pkt.mMatrix), mDest->matrix);
    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(pkt.mVelocity), mDest->velocity);
    if (0.0f < mDest->time) {
        mDest->acceleration.x = (mDest->velocity.x - prevvel.x) / dT;
        mDest->acceleration.y = (mDest->velocity.y - prevvel.y) / dT;
        mDest->acceleration.z = (mDest->velocity.z - prevvel.z) / dT;
    }
    mDest->time = mDest->time + dT;
}

void WorldBodyConn::FetchData(float dT) {
    for (WorldBodyConn *pconn = mList.GetHead(); pconn != mList.EndOfList(); pconn = pconn->GetNext()) {
        pconn->Update(dT);
    }
}

static Attrib::RefSpec ChooseAudioAttributes(const Attrib::Gen::effects &effect, const bMatrix4 *matrix, const bVector3 *normal) {
    if ((matrix != nullptr) && (normal != nullptr)) {
        Attrib::RefSpec zone_spec;
        float AngleDiff_Front = bDot(reinterpret_cast<const bVector3 *>(&matrix->v0), normal);
        float AngleDiff_Top = bDot(reinterpret_cast<const bVector3 *>(&matrix->v2), normal);

        if (AngleDiff_Front < -0.707f) {
            zone_spec = effect.AudioFX_FRONT();
        } else if (AngleDiff_Front > 0.707f) {
            zone_spec = effect.AudioFX_REAR();
        } else if (AngleDiff_Top < -0.707f) {
            zone_spec = effect.AudioFX_TOP();
        } else if (AngleDiff_Top > 0.707f) {
            zone_spec = effect.AudioFX_BOTTOM();
        } else {
            zone_spec = effect.AudioFX_SIDE();
        }

        if (zone_spec.GetCollectionKey() != 0 && zone_spec.GetClassKey() != 0) {
            return zone_spec;
        }
    }

    return effect.AudioFX_DEFAULT();
}

class WorldEffectConn : public Sim::Connection, public bTNode<WorldEffectConn> {
  public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    WorldEffectConn(const Sim::ConnectionData &data, const WorldConn::Pkt_Effect_Open *oc);
    ~WorldEffectConn() override;

    void OnClose() override {
        delete this;
    }

    Sim::ConnStatus OnStatusCheck() override {
        return Sim::CONNSTATUS_READY;
    }

    void ResetEmitterGroup() {
        mEmitters = nullptr;
    }

    void Update(float dT);
    static void FetchData(float dT);

  private:
    Attrib::Gen::effects mAttributes; // offset 0x18, size 0x14
    WorldConn::Reference mOwnerRef;   // offset 0x2C, size 0x10
    EmitterGroup *mEmitters;          // offset 0x3C, size 0x4
    bool mPaused;                     // offset 0x40, size 0x1
    bool mSilent;                     // offset 0x44, size 0x1
    Sound::AudioEvent *mAudioEvent;   // offset 0x48, size 0x4
    static bTList<WorldEffectConn> mList;
    WUID mActee; // offset 0x4C, size 0x4
};

BIND_SIM_CONN(WorldEffectConn);

Sim::Connection *WorldEffectConn::Construct(const Sim::ConnectionData &data) {
    WorldConn::Pkt_Effect_Open *oc = Sim::Packet::Cast<WorldConn::Pkt_Effect_Open>(data.pkt);
    return new WorldEffectConn(data, oc);
}

void HandleWorldEffectEmitterGroupDelete(void *subscriber, EmitterGroup *grp) {
    WorldEffectConn *fx_conn = static_cast<WorldEffectConn *>(subscriber);
    fx_conn->ResetEmitterGroup();
}

WorldEffectConn::WorldEffectConn(const Sim::ConnectionData &data, const WorldConn::Pkt_Effect_Open *oc)
    : Connection(data),                          //
      mAttributes(oc->mEffectGroup, 0, nullptr), //
      mOwnerRef(oc->mOwner),                     //
      mPaused(false),                            //
      mSilent(false),                            //
      mAudioEvent(nullptr),                      //
      mActee(oc->mActee) {
    unsigned int effect_creation_flags = 0;

    Attrib::Instance owner_attribs(oc->mOwnerAttributes, 0, nullptr);
    switch (owner_attribs.GetClass()) {
        case 0x4a97ec8f:
            effect_creation_flags = 0x10000000;
            break;
        case 0xce70d7db:
            effect_creation_flags = 0x20000000;
            break;
    }

    Attrib::Instance context_attribs(oc->mContext, 0, nullptr);
    if (context_attribs.GetClass() == 0xfb111fef) {
        effect_creation_flags |= 0x01000000;
    } else {
        effect_creation_flags |= 0x02000000;
    }

    mList.AddTail(this);

    const Attrib::Collection *fxspec = mAttributes.emittergroup().GetCollection();
    mEmitters = nullptr;
    if (fxspec != nullptr) {
        mEmitters = gEmitterSystem.CreateEmitterGroup(fxspec, effect_creation_flags | 0x800000);
        if (mEmitters != nullptr) {
            mEmitters->SubscribeToDeletion(this, HandleWorldEffectEmitterGroupDelete);
            mEmitters->Disable();
        }
    }
}

WorldEffectConn::~WorldEffectConn() {
    if (mEmitters != nullptr) {
        mEmitters->UnSubscribe();
        delete mEmitters;
    }
    if (mAudioEvent != nullptr) {
        mAudioEvent->Release();
        mAudioEvent = nullptr;
    }
    mList.Remove(this);
}

inline float Quadratic(float x, float A, float B, float C, float D) {
    return A + B * x + C * x * x + D * x * x * x;
}

inline float SolveEffectQuadratic(float x, const UMath::Vector4 &v) {
    float y = Quadratic(x, v[0], v[1], v[2], v[3]);
    return UMath::Clamp(y, 0.0f, 1.0f);
}

void WorldEffectConn::Update(float dT) {
    WorldConn::Pkt_Effect_Service pkt;
    pkt.mPosition = UMath::Vector3::kZero;
    pkt.mTracking = false;
    pkt.mMagnitude = UMath::Vector3::kZero;

    if (!Service(&pkt)) {
        if (!mPaused) {
            if (mAudioEvent != nullptr) {
                mAudioEvent->Release();
                mAudioEvent = nullptr;
            }
            mPaused = true;
            if (mEmitters != nullptr) {
                mEmitters->Disable();
            }
        }
        return;
    }
    mPaused = false;
    UMath::Vector3 simnormal = pkt.mMagnitude;
    float intensity = UMath::Normalize(simnormal);
    float emitter_intensity = SolveEffectQuadratic(intensity, mAttributes.EmitterQuadratic());
    float audio_intensity = SolveEffectQuadratic(intensity, mAttributes.AudioQuadratic());
    bVector3 velocity(0.0f, 0.0f, 0.0f);
    bVector3 normal;
    bVector3 position;
    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(pkt.mPosition), position);
    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(simnormal), normal);

    if (mOwnerRef.GetVelocity() != nullptr) {
        if (0.0f < mAttributes.InheritVelocity()) {
            bScale(&velocity, mOwnerRef.GetVelocity(), mAttributes.InheritVelocity());
        }
    }
    if (pkt.mTracking && mOwnerRef.IsValid()) {
        bVector3 world_pos;
        bVector3 world_mag;
        bMulMatrix(&world_pos, mOwnerRef.GetMatrix(), &position);
        bRotateVector(&world_mag, mOwnerRef.GetMatrix(), &normal);
        position = world_pos;
        normal = world_mag;
    }
    float distance = Sound::DistanceToView(&position);
    if (mEmitters != nullptr) {
        UQuat quat;
        bMatrix4 matrix;
        bVector3 sim_dir;
        static const UMath::Vector3 up = {0.0f, 1.0f, 0.0f};

        quat.BuildDeltaAxis(up, reinterpret_cast<const UMath::Vector3 &>(normal));
        UMath::QuaternionToMatrix4(quat, reinterpret_cast<UMath::Matrix4 &>(matrix));
        bCopy(&matrix.v3, &position, 1.0f);

        if (mEmitters != nullptr) {
            if (0.0f < emitter_intensity && distance < mAttributes.VisualCullDist()) {
                mEmitters->Enable();
                mEmitters->SetLocalWorld(&matrix);
                mEmitters->SetInheritVelocity(&velocity);
                mEmitters->SetIntensity(emitter_intensity);
                mEmitters->Update(dT);
            } else {
                mEmitters->Disable();
            }
        }
    }
    if (mAudioEvent == nullptr) {
        if (!mSilent && 0.0f < audio_intensity && distance < mAttributes.AudioCullDist()) {
            Sound::AudioEventParams params;
            params.position = position;
            params.normal = normal;
            params.velocity = velocity;
            params.magnitude = audio_intensity;
            params.object = mOwnerRef.GetWorldID();
            params.attributes = ChooseAudioAttributes(mAttributes, mOwnerRef.GetMatrix(), &normal);
            params.other_object = mActee;
            mAudioEvent = Sound::AudioEvent::CreateInstance(params);
            if (mAudioEvent == nullptr) {
                mSilent = true;
            }
        }
    } else {
        if (0.0f < audio_intensity && distance < mAttributes.AudioCullDist()) {
            mAudioEvent->Update(position, normal, velocity, audio_intensity);
        } else {
            mAudioEvent->Release();
            mAudioEvent = nullptr;
        }
    }
}

void WorldEffectConn::FetchData(float dT) {
    for (WorldEffectConn *pconn = mList.GetHead(); pconn != mList.EndOfList(); pconn = pconn->GetNext()) {
        pconn->Update(dT);
    }
}

int *World_OneShotEffect(Sim::Packet *pkt) {
    WorldConn::Pkt_Effect_Send *pe = Sim::Packet::Cast<WorldConn::Pkt_Effect_Send>(pkt);
    Attrib::Gen::effects effects(pe->mEffectGroup, 0, nullptr);

    if (!effects.IsValid()) {
        return nullptr;
    }

    const char *effects_name = effects.CollectionName();
    unsigned int effect_creation_flags = 0;

    Attrib::Instance owner_attribs(pe->mOwnerAttributes, 0, nullptr);
    switch (owner_attribs.GetClass()) {
        case 0x4a97ec8f:
            effect_creation_flags = 0x10000000;
            break;
        case 0xce70d7db:
            effect_creation_flags = 0x20000000;
            break;
    }

    Attrib::Instance context_attribs(pe->mContext, 0, nullptr);
    if (context_attribs.GetClass() == 0xfb111fef) {
        effect_creation_flags |= 0x01000000;
    } else {
        effect_creation_flags |= 0x02000000;
    }

    bVector3 position;
    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(pe->mPosition), position);
    float distance = Sound::DistanceToView(&position);
    bool noaudio = distance > effects.AudioCullDist();
    bool novideo = distance > effects.VisualCullDist();

    if (noaudio && novideo) {
        return nullptr;
    }

    UMath::Vector4 mEmitterQuadratic = UMath::Vector4Make(0.0f, 1.0f, 0.0f, 0.0f);
    UMath::Vector4 mAudioQuadratic = UMath::Vector4Make(0.0f, 1.0f, 0.0f, 0.0f);
    Attrib::TAttrib<UMath::Vector4> attrib;
    if (effects.EmitterQuadratic(attrib)) {
        mEmitterQuadratic = attrib.Get(0);
    }
    if (effects.AudioQuadratic(attrib)) {
        mAudioQuadratic = attrib.Get(0);
    }

    UMath::Vector3 simnormal = pe->mMagnitude;

    float intensity = UMath::Normalize(simnormal);
    float emitter_intensity = SolveEffectQuadratic(intensity, mEmitterQuadratic);
    float audio_intensity = SolveEffectQuadratic(intensity, mAudioQuadratic);
    if (0.0f >= emitter_intensity && 0.0f >= audio_intensity) {
        return nullptr;
    }

    WorldConn::Reference effect_object(pe->mOwner);
    bVector3 velocity(0.0f, 0.0f, 0.0f);
    float inherit = effects.InheritVelocity();

    if (effect_object.IsValid() && 0.0f < inherit) {
        bScale(&velocity, effect_object.GetVelocity(), inherit);
    }

    bVector3 normal;
    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(simnormal), normal);
    if (0.0f < emitter_intensity && !novideo) {
        const Attrib::Collection *emitter_group_spec = effects.emittergroup().GetCollection();
        EmitterGroup *emitter_group = gEmitterSystem.CreateEmitterGroup(emitter_group_spec, effect_creation_flags | 0x400000);
        if (emitter_group != nullptr) {
            UQuat quat;
            static const UMath::Vector3 up = {0.0f, 1.0f, 0.0f};
            quat.BuildDeltaAxis(up, reinterpret_cast<const UMath::Vector3 &>(simnormal));

            UMath::Matrix4 mat;
            UMath::QuaternionToMatrix4(quat, mat);

            bMatrix4 matrix;
            eSwizzleWorldMatrix(reinterpret_cast<bMatrix4 &>(mat), matrix);
            matrix.v3 = bVector4(position.x, position.y, position.z, 1.0f);

            emitter_group->SetIntensity(emitter_intensity);
            emitter_group->MakeOneShot(true);
            emitter_group->SetAutoUpdate(true);
            emitter_group->SetLocalWorld(&matrix);
            emitter_group->SetInheritVelocity(&velocity);
        }
    }
    if (0.0f < audio_intensity && !noaudio) {
        Sound::AudioEventParams params;
        params.position = position;
        params.normal = normal;
        params.magnitude = audio_intensity;
        params.object = pe->mOwner;
        params.attributes = ChooseAudioAttributes(effects, effect_object.GetMatrix(), &normal);
        params.other_object = pe->mActee;
        Sound::AudioEvent *audio = Sound::AudioEvent::CreateInstance(params);
        if (audio != nullptr) {
        }
        audio->Release();
    }

    return nullptr;
}

UTL::COM::Factory<Sim::Packet *, int, UCrc32>::Prototype _World_OneShotEffect("World_OneShotEffect", World_OneShotEffect);
UTL::COM::Factory<Sim::Packet *, int, UCrc32>::Prototype _World_UpdateBody("World_UpdateBody", World_UpdateBody);

void WorldConn::UpdateServices(float dT) {
    WorldBodyConn::FetchData(dT);
    WorldEffectConn::FetchData(dT);
}

void WorldConn::InitServices() {
    _Server = new Server();
}

void WorldConn::RestoreServices() {
    delete _Server;
    _Server = nullptr;
}
