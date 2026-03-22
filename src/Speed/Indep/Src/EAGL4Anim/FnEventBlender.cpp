#include "FnEventBlender.h"


namespace EAGL4Anim {

void FnEventBlender::Eval(float previousTime, float currentTime, float *data) {
    FnAnim *anim = mAnim[0];
    float timeOffset = mTimeOffset[0];

    if (currentTime > mStartTransTime) {
        if (currentTime >= mEndTransTime) {
            anim = mAnim[1];
            timeOffset = mTimeOffset[1];
        } else if (mTriggerType == BOTH) {
            mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);
            anim = mAnim[1];
            timeOffset = mTimeOffset[1];
        } else if (mTriggerType == SECOND_ONLY) {
            anim = mAnim[1];
            timeOffset = mTimeOffset[1];
        }
    }

    anim->Eval(previousTime - timeOffset, currentTime - timeOffset, data);
}

}; // namespace EAGL4Anim
