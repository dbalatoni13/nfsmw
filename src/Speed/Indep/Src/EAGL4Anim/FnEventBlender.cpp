#include "FnEventBlender.h"


namespace EAGL4Anim {

void FnEventBlender::Eval(float previousTime, float currentTime, float *data) {
    if (!(currentTime > mStartTransTime)) {
        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);
        return;
    }

    if (!(currentTime < mEndTransTime)) {
        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);
        return;
    }

    switch (mTriggerType) {
    case FIRST_ONLY:
        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);
        return;
    case SECOND_ONLY:
        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);
        return;
    default:
        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], data);
        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], data);
        return;
    }
}

}; // namespace EAGL4Anim
