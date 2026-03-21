#include "DrawVehicle.h"

#include "Speed/Indep/Src/Sim/SimConn.h"

namespace RenderConn {

class Pkt_Heli_Service : public Sim::Packet {
  public:
    bool mInView;           // offset 0x4, size 0x1
    float mDistanceToView;  // offset 0x8, size 0x4
    float mShadowScale;     // offset 0xC, size 0x4
    float mDustStorm;       // offset 0x10, size 0x4
};

} // namespace RenderConn

Behavior *DrawHeli::Construct(const BehaviorParams &params) {
    return new DrawHeli(params);
}

void DrawHeli::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mCollisionBody);
    }
}

DrawHeli::~DrawHeli() {
    RemoveTask(mWashTask);
    CloseService(mRenderService);
}

void DrawHeli::OnService(RenderConn::Pkt_Heli_Service &pkt) {
    mInView = pkt.mInView;
    mDistanceToView = pkt.mDistanceToView;
    if (mIVehicle) {
        mIVehicle->QueryInterface(&mIAIHelicopter);
        if (mIAIHelicopter) {
            pkt.mShadowScale = mIAIHelicopter->GetShadowScale();
            pkt.mDustStorm = mIAIHelicopter->GetDustStormIntensity();
        }
    }
}

bool DrawHeli::OnService(HSIMSERVICE hCon, Sim::Packet *pkt) {
    if (hCon == mRenderService) {
        if (!static_cast<IModel *>(this)->GetParentModel()) {
            OnService(*static_cast<RenderConn::Pkt_Heli_Service *>(pkt));
            return true;
        }
        mInView = false;
    }
    return false;
}

void DrawHeli::HidePart(const UCrc32 &name) {}

void DrawHeli::ShowPart(const UCrc32 &name) {}

bool DrawHeli::IsPartVisible(const UCrc32 &name) const {
    return true;
}

bool DrawHeli::InView() const {
    return mInView;
}

bool DrawHeli::IsRenderable() const {
    return CheckService(mRenderService) == Sim::CONNSTATUS_READY;
}

float DrawHeli::DistanceToView() const {
    return mDistanceToView;
}

void DrawHeli::Reset() {}

void DrawHeli::OnTaskSimulate(float dT) {}
