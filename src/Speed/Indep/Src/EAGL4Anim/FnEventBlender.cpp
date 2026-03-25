#include "FnEventBlender.h"


namespace EAGL4Anim {

void FnEventBlender::Eval(float previousTime, float currentTime, float *data) {
    if (currentTime > mStartTransTime) {
        if (currentTime < mEndTransTime) {
            if (mTriggerType == FIRST_ONLY) {
                mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);
            } else if (mTriggerType == SECOND_ONLY) {
                mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);
            } else {
                mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);
                mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);
            }
            return;
        }
        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);
    } else {
        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);
    }
}

}; // namespace EAGL4Anim
