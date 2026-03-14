#include "Speed/Indep/Src/AI/AIAction.h"

AIAction::AIAction(AIActionParams *params, float score)
    : Sim::Object(0),                //
      mActionParams(params->mOwner), //
      mScore(score) {
    mActionParams.mOwner->QueryInterface(&mVehicle);
    mActionParams.mOwner->QueryInterface(&mAI);
}

void performance_limiter::init(float speed) {
    speed_limit = speed;
}

void performance_limiter::update(float speed, float maxspeed, float maxaccel, float dt) {
    if (speed > KPH2MPS(5.0f)) {
        if (speed < speed_limit) {
            float t = (speed_limit - speed) / (KPH2MPS(100.0f) * KPH2MPS(100.0f));
            float frac = bMin(0.85f, dt * t * t);
            speed_limit = speed_limit - frac * (speed_limit - speed);
        }
        speed_limit = bMax(speed, speed_limit);
        speed_limit = bMin(speed_limit, maxaccel * dt + speed);
        speed_limit = bMin(speed_limit, maxspeed);
    } else {
        if (speed_limit < speed) {
            float t = (speed_limit - speed) / (KPH2MPS(100.0f) * KPH2MPS(100.0f));
            float frac = bMax(0.95f, dt * t * t);
            speed_limit = speed_limit + frac * (speed_limit - speed);
        }
        speed_limit = bMin(speed, speed_limit);
        speed_limit = bMax(speed_limit, speed_limit - maxaccel * dt);
        speed_limit = bMax(speed_limit, -maxspeed);
    }
}
