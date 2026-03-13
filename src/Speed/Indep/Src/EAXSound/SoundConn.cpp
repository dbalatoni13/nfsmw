#include "Speed/Indep/Src/EAXSound/SoundConn.h"

namespace SoundConn {

void InitServices() {}
void RestoreServices() {}

void UpdateServices(float dT) {
    typedef UTL::Collections::Listable<CarSoundConn, 10> CarList;
    for (CarSoundConn *const *iter = CarList::GetList().begin(); iter != CarList::GetList().end(); ++iter) {
        (*iter)->UpdateState(dT);
    }

    typedef UTL::Collections::Listable<HeliSoundConn, 10> HeliList;
    for (HeliSoundConn *const *iter = HeliList::GetList().begin(); iter != HeliList::GetList().end(); ++iter) {
        (*iter)->UpdateState(dT);
    }
}

} // namespace SoundConn

CarSoundConn::CarSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data) //
    , mConnected(false) //
    , mState(nullptr) {}

CarSoundConn::~CarSoundConn() {}

HeliSoundConn::HeliSoundConn(const Sim::ConnectionData &data)
    : Sim::Connection(data) //
    , mState(nullptr) {}

HeliSoundConn::~HeliSoundConn() {}

Sim::ConnStatus CarSoundConn::OnStatusCheck() {
    if (mConnected && mState != nullptr && mState->mAssetsLoaded) {
        return Sim::CONNSTATUS_READY;
    }
    return Sim::CONNSTATUS_CONNECTING;
}

void CarSoundConn::OnReceive(Sim::Packet *) {}

void CarSoundConn::OnClose() {
    mConnected = false;
    mState = nullptr;
}

void HeliSoundConn::OnReceive(Sim::Packet *) {}

void HeliSoundConn::OnClose() {
    mState = nullptr;
}

Sim::ConnStatus HeliSoundConn::OnStatusCheck() {
    return Sim::CONNSTATUS_READY;
}

Sim::Connection *CarSoundConn::Construct(const Sim::ConnectionData &data) {
    return new CarSoundConn(data);
}

Sim::Connection *HeliSoundConn::Construct(const Sim::ConnectionData &data) {
    return new HeliSoundConn(data);
}

void CarSoundConn::UpdateState(float dT) {
    if (mState == nullptr) {
        mConnected = false;
        return;
    }

    if (!mState->mAssetsLoaded) {
        mConnected = false;
        return;
    }

    mConnected = true;
}

void HeliSoundConn::UpdateState(float dT) { (void)dT; }
