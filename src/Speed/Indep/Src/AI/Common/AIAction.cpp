#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Decl: 17
AIAction::AIAction(AIActionParams *params, float score)
    : Sim::Object(0),                //
      mActionParams(params->mOwner), //
      mScore(score) {
    this->mActionParams.mOwner->QueryInterface(&this->mVehicle);
    this->mActionParams.mOwner->QueryInterface(&this->mAI);
}

// Decl: 32
IMPLEMENT_FACTORY(AIAction);

void performance_limiter::init(float speed) {
    this->speed_limit = speed;
}

void performance_limiter::update(float speed, float maxspeed, float maxaccel, float dt) {
    if (speed > 0.0f) {
        if (speed < this->speed_limit) {
            float t = (this->speed_limit - speed) / KPH2MPS(5.0f);
            this->speed_limit -= bMin(1.0f, dt * t * t) * (this->speed_limit - speed);
        }

        this->speed_limit = bMax(speed, this->speed_limit) + maxaccel * dt;
        this->speed_limit = bMin(maxspeed, this->speed_limit);
    } else {
        if (speed > this->speed_limit) {
            float t = (this->speed_limit - speed) / KPH2MPS(5.0f);
            this->speed_limit += bMax(-1.0f, dt * t * t) * (this->speed_limit - speed);
        }

        this->speed_limit = bMin(speed, this->speed_limit) - maxaccel * dt;
        this->speed_limit = bMin(maxspeed, -this->speed_limit);
    }
}
