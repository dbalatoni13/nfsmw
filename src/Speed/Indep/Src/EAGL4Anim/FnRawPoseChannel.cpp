#include "FnRawPoseChannel.h"
#include "Speed/Indep/Src/EAGL4Anim/RawPoseChannel.h"

namespace EAGL4Anim {

void FnRawPoseChannel::Eval(float, float currentTime, float *outputPose) {
    GetRawPoseChannel()->Eval(currentTime, outputPose, mInterp, nullptr);
}

bool FnRawPoseChannel::EvalSQT(float currentTime, float *outputPose, const BoneMask *boneMask) {
    GetRawPoseChannel()->Eval(currentTime, outputPose, mInterp, boneMask);

    return true;
}

}; // namespace EAGL4Anim
