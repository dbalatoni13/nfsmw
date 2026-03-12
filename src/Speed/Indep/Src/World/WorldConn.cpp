#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"

extern unsigned int eFrameCounter;
extern EmitterSystem gEmitterSystem;

void bRotateVector(bVector3 *dest, const bMatrix4 *m, bVector3 *v);

namespace Sound {

struct AudioEventParams {
    bVector3 position;          // offset 0x0, size 0x10
    bVector3 normal;            // offset 0x10, size 0x10
    bVector3 velocity;          // offset 0x20, size 0x10
    float magnitude;            // offset 0x30, size 0x4
    Attrib::RefSpec attributes; // offset 0x34, size 0xC
    unsigned int object;        // offset 0x40, size 0x4
    unsigned int other_object;  // offset 0x44, size 0x4
};

class AudioEvent : public UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int> {
    AudioEventParams mParams;       // offset 0x4, size 0x48
    Attrib::Instance mAttributes;   // offset 0x4C, size 0x14

  public:
    static AudioEvent *CreateInstance(const AudioEventParams &params) {
        return UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int>::CreateInstance(
            params.attributes.GetClassKey(), params);
    }

    virtual ~AudioEvent() {}
    virtual void Stop() = 0;
    virtual void _unk() = 0;
    virtual void Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag) = 0;
};

float DistanceToView(const bVector3 *position);

} // namespace Sound

inline float Quadratic(float x, float A, float B, float C, float D) {
    return A + B * x + C * x * x + D * x * x * x;
}

inline float SolveEffectQuadratic(float x, const UMath::Vector4 &v) {
    float y = Quadratic(x, v[0], v[1], v[2], v[3]);
    return UMath::Clamp(y, 0.0f, 1.0f);
}

static Attrib::RefSpec ChooseAudioAttributes(const Attrib::Gen::effects &effect, const bMatrix4 *matrix, const bVector3 *normal);

namespace WorldConn {

Server *_Server;
int world_refcount;

Server::Server() {
}

Server::~Server() {
    mBodies.clear();
}

Server::Body* Server::LockID(unsigned int id) {
    BodyMap::iterator iter = mBodies.find(id);
    if (iter == mBodies.end()) {
        Body *body = new Body();
        body->refcount = 1;
        body->time = 0.0f;
        bIdentity(&body->matrix);
        body->velocity.x = 0.0f;
        body->velocity.y = 0.0f;
        body->velocity.z = 0.0f;
        body->acceleration.x = 0.0f;
        body->acceleration.y = 0.0f;
        body->acceleration.z = 0.0f;
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

unsigned int Server::GetFrame() const {
    return eFrameCounter;
}

void InitServices() {
    _Server = new Server();
}

void RestoreServices() {
    if (_Server != nullptr) {
        delete _Server;
    }
    _Server = nullptr;
}

void UpdateServices(float dT) {
    WorldBodyConn::FetchData(dT);
    WorldEffectConn::FetchData(dT);
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
    if (mMatrix == nullptr && mWorldID != 0) {
        const Server::Body *body = _Server->LockID(mWorldID);
        mMatrix = &body->matrix;
        mVelocity = &body->velocity;
        mAcceleration = &body->acceleration;
    }
}

void Reference::Unlock() {
    if (mMatrix != nullptr && mWorldID != 0) {
        _Server->UnlockID(mWorldID);
        mMatrix = nullptr;
        mVelocity = nullptr;
        mAcceleration = nullptr;
    }
}

} // namespace WorldConn

bTList<WorldBodyConn> WorldBodyConn::mList;
bTList<WorldEffectConn> WorldEffectConn::mList;

Sim::Connection *WorldBodyConn::Construct(const Sim::ConnectionData &data) {
    return new WorldBodyConn(data);
}

void WorldBodyConn::OnClose() {
    delete this;
}

WorldBodyConn::~WorldBodyConn() {
    WorldConn::_Server->UnlockID(mID);
    mList.Remove(this);
}

WorldBodyConn::WorldBodyConn(const Sim::ConnectionData &data)
    : Connection(data), //
      mID(0) {
    mList.AddTail(this);
    WorldConn::Pkt_Body_Open *oc = Sim::Packet::Cast<WorldConn::Pkt_Body_Open>(data.pkt);
    mID = oc->mID;
    mDest = WorldConn::_Server->LockID(mID);
    bConvertFromBond(mDest->matrix, *reinterpret_cast<const bMatrix4 *>(&oc->mMatrix));
}

void WorldBodyConn::Update(float dT) {
    WorldConn::Pkt_Body_Service pkt;
    pkt.mMatrix = UMath::Matrix4::kIdentity;
    int result = Service(&pkt);
    if (result == 0) {
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

void WorldEffectConn::Update(float dT) {
    WorldConn::Pkt_Effect_Service pkt;
    pkt.mPosition = UMath::Vector3::kZero;
    pkt.mTracking = false;
    pkt.mMagnitude = UMath::Vector3::kZero;
    int result = Service(&pkt);
    if (result == 0) {
        if (!mPaused) {
            if (mAudioEvent != nullptr) {
                static_cast<Sound::AudioEvent *>(mAudioEvent)->Stop();
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
    UMath::Vector3 simnormal;
    simnormal = pkt.mMagnitude;
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
        static const UMath::Vector3 up = {0.0f, 1.0f, 0.0f};
        UQuat quat;
        bMatrix4 matrix;
        bVector3 sim_dir;
        quat.BuildDeltaAxis(up, reinterpret_cast<const UMath::Vector3 &>(normal));
        UMath::QuaternionToMatrix4(quat, reinterpret_cast<UMath::Matrix4 &>(matrix));
        bCopy(&matrix.v3, &position, 1.0f);
        if (0.0f < emitter_intensity) {
            if (distance < mAttributes.VisualCullDist()) {
                mEmitters->Enable();
                mEmitters->SetLocalWorld(&matrix);
                mEmitters->SetInheritVelocity(&velocity);
                mEmitters->SetIntensity(emitter_intensity);
                mEmitters->Update(dT);
                goto audio_section;
            }
        }
        mEmitters->Disable();
    }
audio_section:
    if (mAudioEvent == nullptr) {
        if (!mSilent && 0.0f < audio_intensity) {
            if (distance < mAttributes.AudioCullDist()) {
                Sound::AudioEventParams params;
                params.position = position;
                params.normal = normal;
                params.velocity = velocity;
                params.magnitude = audio_intensity;
                params.attributes = ChooseAudioAttributes(mAttributes, mOwnerRef.GetMatrix(), &normal);
                params.object = mOwnerRef.GetWorldID();
                params.other_object = mActee;
                mAudioEvent = Sound::AudioEvent::CreateInstance(params);
                if (mAudioEvent == nullptr) {
                    mSilent = true;
                }
            }
        }
    } else {
        if (0.0f < audio_intensity) {
            if (distance < mAttributes.AudioCullDist()) {
                static_cast<Sound::AudioEvent *>(mAudioEvent)->Update(position, normal, velocity, audio_intensity);
                return;
            }
        }
        static_cast<Sound::AudioEvent *>(mAudioEvent)->Stop();
        mAudioEvent = nullptr;
    }
}

void WorldEffectConn::FetchData(float dT) {
    for (WorldEffectConn *pconn = mList.GetHead(); pconn != mList.EndOfList(); pconn = pconn->GetNext()) {
        pconn->Update(dT);
    }
}

Sim::Connection *WorldEffectConn::Construct(const Sim::ConnectionData &data) {
    WorldConn::Pkt_Effect_Open *oc = Sim::Packet::Cast<WorldConn::Pkt_Effect_Open>(data.pkt);
    return new WorldEffectConn(data, oc);
}

void WorldEffectConn::OnClose() {
    delete this;
}

void HandleWorldEffectEmitterGroupDelete(void *subscriber, EmitterGroup *grp) {
    WorldEffectConn *fx_conn = static_cast<WorldEffectConn *>(subscriber);
    fx_conn->ResetEmitterGroup();
}

int *World_OneShotEffect(Sim::Packet *pkt) {
    WorldConn::Pkt_Effect_Send *pe = Sim::Packet::Cast<WorldConn::Pkt_Effect_Send>(pkt);
    Attrib::Gen::effects effects(pe->mEffectGroup, 0, nullptr);
    if (effects.IsValid()) {
        Attrib::Instance owner_attribs(pe->mOwnerAttributes, 0, nullptr);
        unsigned int effect_creation_flags = 0;
        unsigned int owner_class = owner_attribs.GetClass();
        if (owner_class == 0x4a97ec8f) {
            effect_creation_flags = 0x10000000;
        } else if (owner_class == 0xce70d7db) {
            effect_creation_flags = 0x20000000;
        }
        Attrib::Instance context_attribs(pe->mContext, 0, nullptr);
        unsigned int context_class = context_attribs.GetClass();
        if (context_class == 0xfb111fef) {
            effect_creation_flags |= 0x01000000;
        } else {
            effect_creation_flags |= 0x02000000;
        }
        bVector3 position;
        eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(pe->mPosition), position);
        float distance = Sound::DistanceToView(&position);
        float audioCullDist = effects.AudioCullDist();
        bool noaudio = audioCullDist < distance;
        float visualCullDist = effects.VisualCullDist();
        if (!noaudio || distance <= visualCullDist) {
            UMath::Vector4 mEmitterQuadratic = UMath::Vector4Make(0.0f, 1.0f, 0.0f, 0.0f);
            UMath::Vector4 mAudioQuadratic = UMath::Vector4Make(0.0f, 1.0f, 0.0f, 0.0f);
            Attrib::TAttrib<UMath::Vector4> attrib(effects.Get(0xa9402c33));
            if (attrib.IsValid()) {
                const UMath::Vector4 *p = reinterpret_cast<const UMath::Vector4 *>(attrib.GetDataAddress());
                mEmitterQuadratic = *p;
            }
            {
                Attrib::TAttrib<UMath::Vector4> attrib2(effects.Get(0x15e6552f));
                if (attrib2.IsValid()) {
                    const UMath::Vector4 *p = reinterpret_cast<const UMath::Vector4 *>(attrib2.GetDataAddress());
                    mAudioQuadratic = *p;
                }
            }
            UMath::Vector3 simnormal;
            simnormal.x = pe->mMagnitude.x;
            simnormal.y = pe->mMagnitude.z;
            simnormal.z = pe->mMagnitude.y;
            float intensity = UMath::Normalize(simnormal);
            float emitter_intensity = SolveEffectQuadratic(intensity, mEmitterQuadratic);
            float audio_intensity = SolveEffectQuadratic(intensity, mAudioQuadratic);
            if (0.0f < emitter_intensity || 0.0f < audio_intensity) {
                WorldConn::Reference effect_object(pe->mOwner);
                bVector3 velocity(0.0f, 0.0f, 0.0f);
                float inherit = effects.InheritVelocity();
                if (effect_object.IsValid() && 0.0f < inherit) {
                    bScale(&velocity, effect_object.GetVelocity(), inherit);
                }
                bVector3 normal;
                eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(simnormal), normal);
                if (0.0f < emitter_intensity && distance <= visualCullDist) {
                    const Attrib::Collection *emitter_group_spec = effects.emittergroup().GetCollection();
                    EmitterGroup *emitter_group = gEmitterSystem.CreateEmitterGroup(emitter_group_spec, effect_creation_flags | 0x400000);
                    if (emitter_group != nullptr) {
                        static const UMath::Vector3 up = {0.0f, 1.0f, 0.0f};
                        UQuat quat;
                        UMath::Matrix4 mat;
                        bMatrix4 matrix;
                        quat.BuildDeltaAxis(up, reinterpret_cast<const UMath::Vector3 &>(normal));
                        UMath::QuaternionToMatrix4(quat, mat);
                        bConvertFromBond(matrix, reinterpret_cast<const bMatrix4 &>(mat));
                        bCopy(&matrix.v3, &position, 1.0f);
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
                    params.velocity = velocity;
                    params.magnitude = audio_intensity;
                    params.attributes = ChooseAudioAttributes(effects, effect_object.IsValid() ? effect_object.GetMatrix() : nullptr, &normal);
                    params.object = pe->mOwner;
                    params.other_object = pe->mActee;
                    Sound::AudioEvent *event = Sound::AudioEvent::CreateInstance(params);
                    if (event != nullptr) {
                        event->Stop();
                    }
                }
            }
        }
    }
    return 0;
}

int *World_UpdateBody(Sim::Packet *pkt) {
    WorldConn::Pkt_Body_Open *data = static_cast<WorldConn::Pkt_Body_Open *>(pkt);
    WorldConn::Server::Body *body = WorldConn::_Server->LockID(data->mID);
    eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(data->mMatrix), body->matrix);
    WorldConn::_Server->UnlockID(data->mID);
    return 0;
}

WorldEffectConn::WorldEffectConn(const Sim::ConnectionData &data, const WorldConn::Pkt_Effect_Open *oc)
    : Connection(data), //
      mAttributes(oc->mEffectGroup, 0, nullptr), //
      mOwnerRef(oc->mOwner)
{
    mPaused = false;
    mSilent = false;
    mAudioEvent = nullptr;
    mActee = oc->mActee;

    unsigned int effect_creation_flags = 0;

    Attrib::Instance owner_attribs(oc->mOwnerAttributes, 0, nullptr);
    unsigned int owner_class = owner_attribs.GetClass();
    switch (owner_class) {
    case 0x4a97ec8f:
        effect_creation_flags = 0x10000000;
        break;
    case 0xce70d7db:
        effect_creation_flags = 0x20000000;
        break;
    }

    Attrib::Instance context_attribs(oc->mContext, 0, nullptr);
    unsigned int context_class = context_attribs.GetClass();
    if (context_class == 0xfb111fef) {
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
        if (mEmitters != nullptr) {
            delete mEmitters;
        }
    }
    if (mAudioEvent != nullptr) {
        static_cast<Sound::AudioEvent *>(mAudioEvent)->Stop();
        mAudioEvent = nullptr;
    }
    mList.Remove(this);
}

static Attrib::RefSpec ChooseAudioAttributes(const Attrib::Gen::effects &effect, const bMatrix4 *matrix, const bVector3 *normal) {
    if (matrix != nullptr && normal != nullptr) {
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
