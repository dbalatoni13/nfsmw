#include "FnPoseBlender.h"

#include "AnimUtil.h"


namespace EAGL4Anim {

namespace {

static inline void ApplyAlignedRootTransform(EAGL4::Transform &tmpTransform, const EAGL4::Transform &alignMatrix,
                                             float *pose, int boneIdx) {
    UMath::Vector4 quat;
    UMath::Vector4 trans;
    float *bonePose = &pose[boneIdx * 12];

    tmpTransform.BuildSQT(bonePose[0], bonePose[1], bonePose[2], bonePose[4], bonePose[5], bonePose[6], bonePose[7],
                          bonePose[8], bonePose[9], bonePose[10]);
    tmpTransform.PostMult(alignMatrix);
    tmpTransform.ExtractQuatTrans(&quat, &trans);

    bonePose[4] = reinterpret_cast<float *>(&quat)[0];
    bonePose[5] = reinterpret_cast<float *>(&quat)[1];
    bonePose[6] = reinterpret_cast<float *>(&quat)[2];
    bonePose[7] = reinterpret_cast<float *>(&quat)[3];
    bonePose[8] = reinterpret_cast<float *>(&trans)[0];
    bonePose[9] = reinterpret_cast<float *>(&trans)[1];
    bonePose[10] = reinterpret_cast<float *>(&trans)[2];
}

static inline void BlendRootTranslation(float w, const float *pose0, const float *pose1, float *out, int boneIdx) {
    int transIdx = boneIdx * 12 + 8;

    out[transIdx + 0] = pose0[transIdx + 0] + w * (pose1[transIdx + 0] - pose0[transIdx + 0]);
    out[transIdx + 1] = pose0[transIdx + 1] + w * (pose1[transIdx + 1] - pose0[transIdx + 1]);
    out[transIdx + 2] = pose0[transIdx + 2] + w * (pose1[transIdx + 2] - pose0[transIdx + 2]);
}

}; // namespace

FnPoseBlender::~FnPoseBlender() {}

void FnPoseBlender::operator delete(void *ptr, size_t size) {
    EAGL4Internal::EAGL4Free(ptr, size);
}

void FnPoseBlender::Blend(int numBones, float w, const float *pose0, const float *pose1, float *result,
                          const BoneMask *boneMask) {
    for (int boneIdx = numBones - 1; boneIdx >= 0; --boneIdx) {
        if (boneMask && !boneMask->GetBone(boneIdx)) {
            continue;
        }

        int poseIdx = boneIdx * 12;

        FastQuatBlendF4(w, &pose0[poseIdx + 4], &pose1[poseIdx + 4], &result[poseIdx + 4]);
        result[poseIdx + 8] = pose0[poseIdx + 8] + w * (pose1[poseIdx + 8] - pose0[poseIdx + 8]);
        result[poseIdx + 9] = pose0[poseIdx + 9] + w * (pose1[poseIdx + 9] - pose0[poseIdx + 9]);
        result[poseIdx + 10] = pose0[poseIdx + 10] + w * (pose1[poseIdx + 10] - pose0[poseIdx + 10]);
    }
}

bool FnPoseBlender::EvalSQT(float currentTime, float *sqtBuffer, const BoneMask *boneMask) {
    if (currentTime <= mStartTransTime) {
        return mAnim[0]->EvalSQT(currentTime - mTimeOffset[0], sqtBuffer, boneMask);
    }

    if (currentTime >= mEndTransTime) {
        bool result = mAnim[1]->EvalSQT(currentTime - mTimeOffset[1], sqtBuffer, boneMask);

        if (result && mAlignRootBoneIdx >= 0 && (!boneMask || boneMask->GetBone(mAlignRootBoneIdx))) {
            EAGL4::Transform rootTransform;
            ApplyAlignedRootTransform(rootTransform, mAlignMatrix, sqtBuffer, mAlignRootBoneIdx);
        }

        return result;
    }

    float w = (currentTime - mStartTransTime) / mDuration;

    if (mResetBuffers[0]) {
        mpSkel->GetStillPose(mPose[0], boneMask);
    }

    if (mResetBuffers[1]) {
        mpSkel->GetStillPose(mPose[1], boneMask);
    }

    if (!mAnim[0]->EvalSQT(currentTime - mTimeOffset[0], mPose[0], boneMask)) {
        return false;
    }

    if (!mAnim[1]->EvalSQT(currentTime - mTimeOffset[1], sqtBuffer, boneMask)) {
        return false;
    }

    if (mAlignRootBoneIdx >= 0 && (!boneMask || boneMask->GetBone(mAlignRootBoneIdx))) {
        EAGL4::Transform rootTransform;
        ApplyAlignedRootTransform(rootTransform, mAlignMatrix, sqtBuffer, mAlignRootBoneIdx);
    }

    int numBones = mpSkel->GetNumBones();
    int transBoneIdx = mAlignRootBoneIdx < 0 ? 0 : mAlignRootBoneIdx;

    for (int boneIdx = numBones - 1; boneIdx >= 0; --boneIdx) {
        if (boneMask && !boneMask->GetBone(boneIdx)) {
            continue;
        }

        int poseIdx = boneIdx * 12;

        FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &sqtBuffer[poseIdx + 4], &sqtBuffer[poseIdx + 4]);
    }

    if (!boneMask || boneMask->GetBone(transBoneIdx)) {
        BlendRootTranslation(w, mPose[0], sqtBuffer, sqtBuffer, transBoneIdx);
    }

    return true;
}

void FnPoseBlender::Eval(float previousTime, float currentTime, float *outputPose) {
    if (currentTime <= mStartTransTime) {
        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], outputPose);
        return;
    }

    if (currentTime >= mEndTransTime) {
        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], outputPose);

        if (mAlignRootBoneIdx >= 0) {
            EAGL4::Transform rootTransform;
            ApplyAlignedRootTransform(rootTransform, mAlignMatrix, outputPose, mAlignRootBoneIdx);
        }

        return;
    }

    float w = (currentTime - mStartTransTime) / mDuration;

    if (mResetBuffers[0]) {
        mpSkel->GetStillPose(mPose[0], 0);
    }

    if (mResetBuffers[1]) {
        mpSkel->GetStillPose(mPose[1], 0);
    }

    mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], mPose[0]);
    mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], outputPose);

    if (mAlignRootBoneIdx >= 0) {
        EAGL4::Transform rootTransform;
        ApplyAlignedRootTransform(rootTransform, mAlignMatrix, outputPose, mAlignRootBoneIdx);
    }

    int numBones = mpSkel->GetNumBones();
    int transBoneIdx = mAlignRootBoneIdx < 0 ? 0 : mAlignRootBoneIdx;

    for (int boneIdx = numBones - 1; boneIdx >= 0; --boneIdx) {
        int poseIdx = boneIdx * 12;

        FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &outputPose[poseIdx + 4], &outputPose[poseIdx + 4]);
    }

    BlendRootTranslation(w, mPose[0], outputPose, outputPose, transBoneIdx);
}

}; // namespace EAGL4Anim
