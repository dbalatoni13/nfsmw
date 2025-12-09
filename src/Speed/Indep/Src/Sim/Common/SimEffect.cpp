#include "../SimEffect.h"
#include "../SimServer.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/World/WorldConn.h"

namespace Sim {

Effect::Effect(unsigned int owner, const Attrib::Collection *participant)
    : UTL::COM::Object(1),               //
      IServiceable(this),                //
      mPosition(UMath::Vector3::kZero),  //
      mTime(0.0f),                       //
      mMagnitude(UMath::Vector3::kZero), //
      mParticipant(participant),         //
      mOwner(owner),                     //
      mService(nullptr),                 //
      mContext(nullptr),                 //
      mEffect(nullptr),                  //
      mPaused(false),                    //
      mTracking(false),                  //
      mActee(0) {}

void Effect::Fire(const Attrib::Collection *effect, const UMath::Vector3 &position, const UMath::Vector3 &magnitude, WUID owner,
                  const Attrib::Collection *participant, const Attrib::Collection *context, WUID actee) {
    if (effect) {
        WorldConn::Pkt_Effect_Send pkt(effect, position, magnitude, owner, participant, context, actee);
        SendService(UCrc32(0x998c21c0), &pkt); // TODO hash
    }
}

void Effect::Stop() {
    if (mService) {
        CloseService(mService);
        mService = nullptr;
        mEffect = nullptr;
    }
}

// bool Effect::OnService(HSIMSERVICE hCon, Packet *pkt) {}

}; // namespace Sim
