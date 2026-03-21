#include "DrawVehicle.h"

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
