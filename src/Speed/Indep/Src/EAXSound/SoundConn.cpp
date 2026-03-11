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
