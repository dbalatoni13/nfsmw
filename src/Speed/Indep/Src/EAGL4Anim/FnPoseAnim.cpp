#include "FnPoseAnim.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimTypeId.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"
#include "Speed/Indep/Src/EAGL4Anim/PoseAnim.h"

namespace EAGL4Anim {

FnPoseAnim::FnPoseAnim() {
    mType = AnimTypeId::ANIM_POSEANIM;
    mPrevKey = 0;
}

void FnPoseAnim::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
}

bool FnPoseAnim::GetLength(float &timeLength) const {
    PoseAnim *poseAnim = reinterpret_cast<PoseAnim *>(mpAnim);

    timeLength = static_cast<float>(poseAnim->GetNumFrames());

    return true;
}

static inline void PoseLinearBlendF3(float w, const float *d0, const float *d1, float *out) {
    out[0] = (d1[0] - d0[0]) * w + d0[0];
    out[1] = (d1[1] - d0[1]) * w + d0[1];
    out[2] = (d1[2] - d0[2]) * w + d0[2];
}

bool FnPoseAnim::EvalPose(float currTime, const PosePaletteBank *paletteBank, float *sqt) {
    PoseAnim *poseAnim = reinterpret_cast<PoseAnim *>(mpAnim);

    int floorTime = FloatToInt(currTime);
    int floorKey;
    bool slerpReqd = false;
    float scale = 0.0f;
    const PosePalette **palettes;
    int paletteIndex;
    int floorPose;
    int numQs;
    int numTs;
    unsigned short *dofIndices;
    float *poseData;

    if (currTime <= poseAnim->mTimes[0]) {

        floorKey = 0;

    } else if (currTime >= poseAnim->mTimes[poseAnim->mNumKeys - 1]) {

        floorKey = poseAnim->mNumKeys - 1;

    } else {

        if (mPrevKey == 0) {

            floorKey = 0;
        } else {

            floorKey = mPrevKey;
        }

        if (poseAnim->mTimes[floorKey] <= floorTime) {

            while (floorKey < poseAnim->mNumKeys - 1 && poseAnim->mTimes[floorKey + 1] <= floorTime) {
                floorKey++;
            }
        } else {

            while (floorKey > 0 && poseAnim->mTimes[floorKey] > floorTime) {
                floorKey--;
            }
        }

        if (poseAnim->mNumKeys != 0) {

            float floorKeyTime = poseAnim->mTimes[floorKey];

            slerpReqd = currTime != floorKeyTime;

            if (slerpReqd) {

                float ceilKeyTime = poseAnim->mTimes[floorKey + 1];

                scale = (currTime - floorKeyTime) / (ceilKeyTime - floorKeyTime);
            }
        }
    }

    palettes = paletteBank->GetPalettes();

    mPrevKey = floorKey;

    paletteIndex = poseAnim->mPaletteIndex;
    floorPose = poseAnim->mPoseIndices[floorKey];
    numQs = palettes[paletteIndex]->GetNumQs();
    numTs = palettes[paletteIndex]->GetNumTs();
    dofIndices = palettes[paletteIndex]->GetDofIndices();
    poseData = palettes[paletteIndex]->GetPoseData();

    if (slerpReqd) {

        int ceilPose = poseAnim->mPoseIndices[floorKey + 1];
        int floatPerPose = (numQs + numTs) * 4;
        int prevIndex = (floorPose + 1) * floatPerPose - 4;
        int nextIndex = (ceilPose + 1) * floatPerPose - 4;
        int t;
        int q;

        scale = scale * ((3.0f - 2.0f * scale) * scale);

        for (t = numTs - 1; t >= 0; t--) {

            int dofIndex = dofIndices[numQs + t];

            PoseLinearBlendF3(scale, &poseData[prevIndex], &poseData[nextIndex], &sqt[dofIndex]);

            prevIndex -= 4;
            nextIndex -= 4;
        }

        for (q = numQs - 1; q >= 0; q--) {

            int dofIndex = dofIndices[q];

            FastQuatBlendF4(scale, &poseData[prevIndex], &poseData[nextIndex], &sqt[dofIndex]);

            prevIndex -= 4;
            nextIndex -= 4;
        }
    } else {

        int floatPerPose = (numQs + numTs) * 4;
        int index = (floorPose + 1) * floatPerPose - 4;
        int t;
        int q;

        for (t = numTs - 1; t >= 0; t--) {

            int dofIndex = dofIndices[numQs + t];

            sqt[dofIndex] = poseData[index];
            sqt[dofIndex + 1] = poseData[index + 1];
            sqt[dofIndex + 2] = poseData[index + 2];

            index -= 4;
        }

        for (q = numQs - 1; q >= 0; q--) {

            int dofIndex = dofIndices[q];

            sqt[dofIndex] = poseData[index];
            sqt[dofIndex + 1] = poseData[index + 1];
            sqt[dofIndex + 2] = poseData[index + 2];
            sqt[dofIndex + 3] = poseData[index + 3];

            index -= 4;
        }
    }

    return true;
}

}; // namespace EAGL4Anim
