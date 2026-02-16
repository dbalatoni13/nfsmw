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
