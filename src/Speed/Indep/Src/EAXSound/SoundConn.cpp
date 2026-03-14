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

Pkt_Car_Service::~Pkt_Car_Service() {}

UCrc32 Pkt_Car_Service::ConnectionClass() {
    static UCrc32 hash = "CarSoundConn";
    return hash;
}

unsigned int Pkt_Car_Service::Size() {
    return sizeof(*this);
}

unsigned int Pkt_Car_Service::Type() {
    return SType();
}

unsigned int Pkt_Car_Service::SType() {
    static UCrc32 hash = "Pkt_Car_Service";
    return hash.GetValue();
}

Pkt_Heli_Service::~Pkt_Heli_Service() {}

UCrc32 Pkt_Heli_Service::ConnectionClass() {
    static UCrc32 hash = "HeliSoundConn";
    return hash;
}

unsigned int Pkt_Heli_Service::Size() {
    return sizeof(*this);
}

unsigned int Pkt_Heli_Service::Type() {
    return SType();
}

unsigned int Pkt_Heli_Service::SType() {
    static UCrc32 hash = "Pkt_Heli_Service";
    return hash.GetValue();
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

void CarSoundConn::SetAssetsLoaded(CarSoundConn *conn) {
    if (conn != nullptr && conn->mState != nullptr) {
        conn->mState->mAssetsLoaded = true;
    }
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
