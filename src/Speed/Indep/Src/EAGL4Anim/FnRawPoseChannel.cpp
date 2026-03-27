#include "FnRawPoseChannel.h"


namespace EAGL4Anim {

void FnRawPoseChannel::Eval(float prevTime, float currentTime, float *outputPose) {
    GetRawPoseChannel()->Eval(currentTime, outputPose, mInterp, nullptr);
}

bool FnRawPoseChannel::EvalSQT(float currentTime, float *outputPose, const BoneMask *boneMask) {
    GetRawPoseChannel()->Eval(currentTime, outputPose, mInterp, boneMask);
    return true;
}

}; // namespace EAGL4Anim
