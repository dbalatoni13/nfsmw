#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

extern unsigned int eFrameCounter;

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
    pkt.SetMatrix(UMath::Matrix4::kIdentity);
    int result = Service(&pkt);
    if (result == 0) {
        mDest->acceleration.x = 0.0f;
        mDest->acceleration.y = 0.0f;
        mDest->acceleration.z = 0.0f;
        mDest->time = 0.0f;
    } else {
        bVector3 prevvel(mDest->velocity);
        bConvertFromBond(mDest->matrix, *reinterpret_cast<const bMatrix4 *>(&pkt.mMatrix));
        eSwizzleWorldVector(*reinterpret_cast<const bVector3 *>(&pkt.mVelocity), mDest->velocity);
        if (0.0f < mDest->time) {
            mDest->acceleration.x = (mDest->velocity.x - prevvel.x) / dT;
            mDest->acceleration.y = (mDest->velocity.y - prevvel.y) / dT;
            mDest->acceleration.z = (mDest->velocity.z - prevvel.z) / dT;
        }
        mDest->time = mDest->time + dT;
    }
}

void WorldBodyConn::FetchData(float dT) {
    for (WorldBodyConn *pconn = mList.GetHead(); pconn != mList.EndOfList(); pconn = pconn->GetNext()) {
        pconn->Update(dT);
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

class EmitterGroup;

void HandleWorldEffectEmitterGroupDelete(void *subscriber, EmitterGroup *grp) {
    WorldEffectConn *fx_conn = static_cast<WorldEffectConn *>(subscriber);
    fx_conn->ResetEmitterGroup();
}

int *World_UpdateBody(Sim::Packet *pkt) {
    WorldConn::Pkt_Body_Open *data = static_cast<WorldConn::Pkt_Body_Open *>(pkt);
    WorldConn::Server::Body *body = WorldConn::_Server->LockID(data->mID);
    eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(data->mMatrix), body->matrix);
    WorldConn::_Server->UnlockID(data->mID);
    return 0;
}
