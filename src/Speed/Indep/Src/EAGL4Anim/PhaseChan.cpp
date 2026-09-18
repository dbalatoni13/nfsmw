#include "PhaseChan.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

bool FnPhaseChan::GetLength(float &timeLength) const {
    PhaseChan *phaseChan = reinterpret_cast<PhaseChan *>(mpAnim);
    timeLength = static_cast<float>(phaseChan->mNumFrames);
    return true;
}

void FnPhaseChan::Eval(float, float currTime, float *phaseValue) {

    PhaseValue *value = reinterpret_cast<PhaseValue *>(phaseValue);
    PhaseChan *a = reinterpret_cast<PhaseChan *>(mpAnim);
    int n;
    int l = a->mNumFrames - 1;
    int frame;
    float w = currTime;

    if (a->IsCycleAnim()) {

        if (w < 0.0f) {

            n = FloatToInt(w / l);
            w = l - (w - n * l);
        } else if (w > l) {

            float tmp = w - l;

            n = FloatToInt(tmp / l);
            w = tmp - n * l;
        }
    }

    frame = FloatToInt(w) / mSampleRate;

    float t = (w - frame * mSampleRate) / mSampleRate;
    w = t;

    value->mAngle = a->GetAngle(frame);

    if (frame < l / mSampleRate + 1) {

        value->mAngle = (1.0f - t) * value->mAngle + t * a->GetAngle(frame + 1);
    } else {

        value->mAngle = (t + 1.0f) * value->mAngle - t * a->GetAngle(frame - 1);
    }
}

void FnPhaseChan::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;

    PhaseChan *phaseChan = reinterpret_cast<PhaseChan *>(anim);

    mIdx = 0;
    mCurrentFrame = phaseChan->mStartTime;
    mRight = !phaseChan->StartWithRight();

    mSampleRate = phaseChan->GetAngleSampleRate();
}

}; // namespace EAGL4Anim
