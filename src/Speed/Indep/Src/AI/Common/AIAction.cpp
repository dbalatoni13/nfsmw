#include "Speed/Indep/Src/AI/AIAction.h"

AIAction::AIAction(AIActionParams *params, float score)
    : Sim::Object(0),                //
      mActionParams(params->mOwner), //
      mScore(score) {
    this->mActionParams.mOwner->QueryInterface(&this->mVehicle);
    this->mActionParams.mOwner->QueryInterface(&this->mAI);
}

void performance_limiter::init(float speed) {
    this->speed_limit = speed;
}
