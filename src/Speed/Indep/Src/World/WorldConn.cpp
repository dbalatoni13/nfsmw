#include "Speed/Indep/Src/World/WorldConn.h"

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
        body->velocity = bVector3(0.0f, 0.0f, 0.0f);
        body->acceleration = bVector3(0.0f, 0.0f, 0.0f);
        iter = mBodies.insert(std::pair<const unsigned int, Body*>(id, nullptr)).first;
        iter->second = body;
    } else {
        iter->second->refcount++;
    }
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
