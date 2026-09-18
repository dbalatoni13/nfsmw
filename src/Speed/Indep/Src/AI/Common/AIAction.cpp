#include "Speed/Indep/Src/AI/AIAction.h"

IMPLEMENT_FACTORY(AIAction);

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
    if (speed > 0.0f) {

        if (speed < speed_limit) {

            float t = (speed_limit - speed) / KPH2MPS(5.0f);
            speed_limit -= bMin(1.0f, dt * t * t) * (speed_limit - speed);
        }

        speed_limit = bMax(speed, speed_limit) + maxaccel * dt;
        speed_limit = bMin(maxspeed, speed_limit);

    } else {


        if (speed > speed_limit) {

            float t = (speed_limit - speed) / KPH2MPS(5.0f);
            speed_limit += bMax(-1.0f, dt * t * t) * (speed_limit - speed);
        }

        speed_limit = bMin(speed, speed_limit) - maxaccel * dt;
        speed_limit = bMin(maxspeed, -speed_limit);
    }
}
