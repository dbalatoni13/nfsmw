#include "Speed/Indep/Src/EAXSound/SoundConn.h"

namespace SoundConn {

void InitServices() {}
void RestoreServices() {}

} // namespace SoundConn

Sim::ConnStatus CarSoundConn::OnStatusCheck() {
    if (mConnected && mState != nullptr && mState->mAssetsLoaded) {
        return Sim::CONNSTATUS_READY;
    }
    return Sim::CONNSTATUS_CONNECTING;
}

void CarSoundConn::OnReceive(Sim::Packet *) {}

void HeliSoundConn::OnReceive(Sim::Packet *) {}

Sim::ConnStatus HeliSoundConn::OnStatusCheck() {
    return Sim::CONNSTATUS_READY;
}

Sim::Connection *CarSoundConn::Construct(const Sim::ConnectionData &data) {
    return new CarSoundConn(data);
}

Sim::Connection *HeliSoundConn::Construct(const Sim::ConnectionData &data) {
    return new HeliSoundConn(data);
}
