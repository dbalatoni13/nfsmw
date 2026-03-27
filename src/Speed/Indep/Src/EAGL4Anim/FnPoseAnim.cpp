#include "FnPoseAnim.h"

#include "AnimTypeId.h"
#include "AnimUtil.h"
#include "PoseAnim.h"


namespace EAGL4Anim {

FnPoseAnim::FnPoseAnim() {
    mType = AnimTypeId::ANIM_POSEANIM;
    mPrevKey = 0;
}

void FnPoseAnim::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
}

bool FnPoseAnim::GetLength(float &timeLength) const {
    int zero = 0;

    timeLength = static_cast<float>(zero);
    return true;
}

bool FnPoseAnim::EvalPose(float currTime, const PosePaletteBank *paletteBank, float *sqt) {
    PoseAnim *poseAnim = reinterpret_cast<PoseAnim *>(mpAnim);
    unsigned short *times = poseAnim->mTimes;
    unsigned int key = 0;
    bool interp = false;
    float t = 0.0f;
    int floorTime = FloatToInt(currTime);
    const PosePalette *const *palettes = paletteBank->GetPalettes();

    if (currTime > static_cast<float>(times[0])) {
        unsigned int numKeys = poseAnim->mNumKeys;

        if (currTime >= static_cast<float>(times[numKeys - 1])) {
            key = numKeys - 1;
        } else {
            key = mPrevKey;
            if (times[key] <= floorTime) {
                if (key < numKeys - 1) {
                    if (times[key + 1] <= floorTime) {
                        do {
                            key++;
                        } while (key < numKeys - 2 && times[key + 1] <= floorTime);
                    }
                }
            } else if (key != 0) {
                do {
                    key--;
                } while (key > 0 && times[key] > floorTime);
            }

            if (numKeys != 0) {
                float floorTime = static_cast<float>(times[key]);

                interp = currTime != floorTime;
                if (interp) {
                    t = (currTime - floorTime) / (static_cast<float>(times[key + 1]) - floorTime);
                }
            }
        }
    }

    mPrevKey = static_cast<unsigned short>(key);

    const PosePalette *palette = palettes[poseAnim->mPaletteIndex];
    float *poseData = palette->GetPoseData();
    unsigned short *dofIndices = palette->GetDofIndices();
    unsigned int numQ = static_cast<unsigned int>(palette->GetNumQs());
    unsigned int numT = static_cast<unsigned int>(palette->GetNumTs());
    unsigned int poseIdx = poseAnim->mPoseIndices[key];

    if (interp) {
        int stride = (numQ + numT) * 4;
        int src0 = (poseIdx + 1) * stride - 4;
        int src1 = (poseAnim->mPoseIndices[key + 1] + 1) * stride - 4;
        int transIdx = static_cast<int>(numT) - 1;

        t = (3.0f - (t + t)) * t * t;

        while (transIdx >= 0) {
            float *from = &poseData[src0];
            float *to = &poseData[src1];
            float *out = &sqt[dofIndices[numQ + transIdx]];

            out[0] = from[0] + (to[0] - from[0]) * t;
            out[1] = from[1] + (to[1] - from[1]) * t;
            out[2] = from[2] + (to[2] - from[2]) * t;

            src0 -= 4;
            src1 -= 4;
            transIdx--;
        }

        int quatIdx = static_cast<int>(numQ) - 1;

        while (quatIdx >= 0) {
            FastQuatBlendF4(t, &poseData[src0], &poseData[src1], &sqt[dofIndices[quatIdx]]);
            src0 -= 4;
            src1 -= 4;
            quatIdx--;
        }
    } else {
        int stride = (numQ + numT) * 4;
        int src = (poseIdx + 1) * stride - 4;
        int transIdx = static_cast<int>(numT) - 1;

        while (transIdx >= 0) {
            float *from = &poseData[src];
            float *out = &sqt[dofIndices[numQ + transIdx]];

            out[0] = from[0];
            out[1] = from[1];
            out[2] = from[2];

            src -= 4;
            transIdx--;
        }

        int quatIdx = static_cast<int>(numQ) - 1;

        while (quatIdx >= 0) {
            float *from = &poseData[src];
            float *out = &sqt[dofIndices[quatIdx]];

            out[0] = from[0];
            out[1] = from[1];
            out[2] = from[2];
            out[3] = from[3];

            src -= 4;
            quatIdx--;
        }
    }

    return true;
}

unsigned short FnPoseAnim::GetTargetCheckSum() const {
    return mpAnim->GetTargetCheckSum();
}

}; // namespace EAGL4Anim
