#include "PhaseChan.h"


namespace EAGL4Anim {

bool FnPhaseChan::GetLength(float &timeLength) const {
    const PhaseChan *phaseChan = reinterpret_cast<const PhaseChan *>(mpAnim);

    timeLength = static_cast<float>(phaseChan->mNumFrames);
    return true;
}

void FnPhaseChan::SetAnimMemoryMap(AnimMemoryMap *anim) {
    PhaseChan *phaseChan = reinterpret_cast<PhaseChan *>(anim);

    mpAnim = anim;
    mIdx = 0;
    mCurrentFrame = phaseChan->mStartTime;
    mRight = !phaseChan->StartWithRight();
    mSampleRate = phaseChan->GetAngleSampleRate();
}

void FnPhaseChan::Eval(float prevTime, float currTime, float *phaseValue) {
    PhaseChan *phaseChan = reinterpret_cast<PhaseChan *>(mpAnim);
    int lastFrame = phaseChan->mNumFrames - 1;

    if (phaseChan->IsCycleAnim()) {
        if (currTime < 0.0f) {
            float cycleLength = static_cast<float>(lastFrame);

            currTime = currTime - static_cast<float>(static_cast<int>(currTime / cycleLength) * lastFrame);
        } else {
            float cycleLength = static_cast<float>(lastFrame);

            if (currTime > cycleLength) {
                float wrapped = currTime - cycleLength;

                currTime = wrapped - static_cast<float>(static_cast<int>(wrapped / cycleLength) * lastFrame);
            }
        }
    }

    int frame = static_cast<int>(currTime) / static_cast<int>(mSampleRate);
    float t = (currTime - static_cast<float>(frame * mSampleRate)) / static_cast<float>(mSampleRate);
    int numCycles = phaseChan->mNumCycles > 1 ? phaseChan->mNumCycles : 2;
    float angle0 = static_cast<float>(phaseChan->GetAngles()[numCycles + frame]) * 1.4117647f - 180.0f;

    if (frame < lastFrame / static_cast<int>(mSampleRate) + 1) {
        float angle1 = static_cast<float>(phaseChan->GetAngles()[numCycles + frame + 1]) * 1.4117647f - 180.0f;

        *phaseValue = (1.0f - t) * angle0 + t * angle1;
    } else {
        float angle1 = static_cast<float>(phaseChan->GetAngles()[numCycles + frame - 1]) * 1.4117647f - 180.0f;

        *phaseValue = (t + 1.0f) * angle0 - t * angle1;
    }
}

}; // namespace EAGL4Anim
