#include "FnPoseBlender.h"

#include "AnimUtil.h"

static inline void LinearBlendF3(float w, const float *d0, const float *d1, float *out) {
    out[0] = d0[0] + w * (d1[0] - d0[0]);
    out[1] = d0[1] + w * (d1[1] - d0[1]);
    out[2] = d0[2] + w * (d1[2] - d0[2]);
}

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
    if (!boneMask) {
        int q = 0;

        for (int boneIdx = 0; boneIdx < numBones; ++boneIdx) {
            q += 4;
            FastQuatBlendF4(w, &pose0[q], &pose1[q], &result[q]);
            q += 4;
            result[q + 0] = pose0[q + 0] + w * (pose1[q + 0] - pose0[q + 0]);
            result[q + 1] = pose0[q + 1] + w * (pose1[q + 1] - pose0[q + 1]);
            result[q + 2] = pose0[q + 2] + w * (pose1[q + 2] - pose0[q + 2]);
            q += 4;
        }
    } else {
        int q = 0;

        for (int boneIdx = 0; boneIdx < numBones; ++boneIdx) {
            q += 4;
            if (boneMask->GetBone(boneIdx)) {
                FastQuatBlendF4(w, &pose0[q], &pose1[q], &result[q]);
                q += 4;
                result[q + 0] = pose0[q + 0] + w * (pose1[q + 0] - pose0[q + 0]);
                result[q + 1] = pose0[q + 1] + w * (pose1[q + 1] - pose0[q + 1]);
                result[q + 2] = pose0[q + 2] + w * (pose1[q + 2] - pose0[q + 2]);
                q += 4;
            } else {
                q += 8;
            }
        }
    }
}

bool FnPoseBlender::EvalSQT(float currentTime, float *sqtBuffer, const BoneMask *boneMask) {
    if (currentTime <= mStartTransTime) {
        return mAnim[0]->EvalSQT(currentTime - mTimeOffset[0], sqtBuffer, boneMask);
    }

    if (currentTime >= mEndTransTime) {
        bool result = mAnim[1]->EvalSQT(currentTime - mTimeOffset[1], sqtBuffer, boneMask);

        if (result) {
            if (mAlignRootBoneIdx >= 0 && (!boneMask || boneMask->GetBone(mAlignRootBoneIdx))) {
                float *sqt1 = &sqtBuffer[mAlignRootBoneIdx * 12];
                EAGL4::Transform boneMat;
                UMath::Vector4 quat;
                UMath::Vector4 trans;

                boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9], sqt1[10]);
                boneMat.PostMult(mAlignMatrix);
                boneMat.ExtractQuatTrans(&quat, &trans);

                sqt1[4] = quat.x;
                sqt1[5] = quat.y;
                sqt1[6] = quat.z;
                sqt1[7] = quat.w;
                sqt1[8] = trans.x;
                sqt1[9] = trans.y;
                sqt1[10] = trans.z;
            }
            return true;
        }
        return false;
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

        if (mAnim[1]->EvalSQT(currentTime - mTimeOffset[1], mPose[1], boneMask)) {
            if (!boneMask) {
                if (mAlignRootBoneIdx >= 0) {
                    for (int boneIdx = mpSkel->GetNumBones() - 1; boneIdx >= 0; --boneIdx) {
                        if (boneIdx == mAlignRootBoneIdx) {
                            float *sqt1 = &mPose[1][mAlignRootBoneIdx * 12];
                            EAGL4::Transform boneMat;
                            UMath::Vector4 quat;
                            UMath::Vector4 trans;

                            boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9],
                                             sqt1[10]);
                            boneMat.PostMult(mAlignMatrix);
                            boneMat.ExtractQuatTrans(&quat, &trans);

                            sqt1[4] = quat.x;
                            sqt1[5] = quat.y;
                            sqt1[6] = quat.z;
                            sqt1[7] = quat.w;
                            sqt1[8] = trans.x;
                            sqt1[9] = trans.y;
                            sqt1[10] = trans.z;
                            ::LinearBlendF3(w, &mPose[0][mAlignRootBoneIdx * 12 + 8], &mPose[1][mAlignRootBoneIdx * 12 + 8],
                                            &sqtBuffer[mAlignRootBoneIdx * 12 + 8]);
                        }

                    int poseIdx = boneIdx * 12;

                    FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &mPose[1][poseIdx + 4], &sqtBuffer[poseIdx + 4]);
                }
            } else {
                for (int boneIdx = mpSkel->GetNumBones() - 1; boneIdx >= 0; --boneIdx) {
                    int poseIdx = boneIdx * 12;

                    FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &mPose[1][poseIdx + 4], &sqtBuffer[poseIdx + 4]);
                }

                BlendRootTranslation(w, mPose[0], mPose[1], sqtBuffer, 0);
            }
        } else if (mAlignRootBoneIdx >= 0) {
            for (int boneIdx = mpSkel->GetNumBones() - 1; boneIdx >= 0; --boneIdx) {
                if (boneMask->GetBone(boneIdx)) {
                    if (boneIdx == mAlignRootBoneIdx) {
                        float *sqt1 = &mPose[1][mAlignRootBoneIdx * 12];
                        EAGL4::Transform boneMat;
                        UMath::Vector4 quat;
                        UMath::Vector4 trans;

                        boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9],
                                         sqt1[10]);
                        boneMat.PostMult(mAlignMatrix);
                        boneMat.ExtractQuatTrans(&quat, &trans);

                        sqt1[4] = quat.x;
                        sqt1[5] = quat.y;
                        sqt1[6] = quat.z;
                        sqt1[7] = quat.w;
                        sqt1[8] = trans.x;
                        sqt1[9] = trans.y;
                        sqt1[10] = trans.z;
                        ::LinearBlendF3(w, &mPose[0][mAlignRootBoneIdx * 12 + 8], &mPose[1][mAlignRootBoneIdx * 12 + 8],
                                        &sqtBuffer[mAlignRootBoneIdx * 12 + 8]);
                    }

                    int poseIdx = boneIdx * 12;

                    FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &mPose[1][poseIdx + 4], &sqtBuffer[poseIdx + 4]);
                }
            }
        } else {
            for (int boneIdx = mpSkel->GetNumBones() - 1; boneIdx >= 0; --boneIdx) {
                if (boneMask->GetBone(boneIdx)) {
                    int poseIdx = boneIdx * 12;

                    FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &mPose[1][poseIdx + 4], &sqtBuffer[poseIdx + 4]);
                }
            }

            if (boneMask->GetBone(0)) {
                BlendRootTranslation(w, mPose[0], mPose[1], sqtBuffer, 0);
            }
        }
        return true;
    }

    return false;
}

void FnPoseBlender::Eval(float previousTime, float currentTime, float *outputPose) {
    if (currentTime <= mStartTransTime) {
        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], outputPose);
        return;
    }

    if (currentTime >= mEndTransTime) {
        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], outputPose);

        if (mAlignRootBoneIdx >= 0) {
            float *sqt1 = &outputPose[mAlignRootBoneIdx * 12];
            EAGL4::Transform boneMat;
            UMath::Vector4 quat;
            UMath::Vector4 trans;

            boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9], sqt1[10]);
            boneMat.PostMult(mAlignMatrix);
            boneMat.ExtractQuatTrans(&quat, &trans);

            sqt1[4] = quat.x;
            sqt1[5] = quat.y;
            sqt1[6] = quat.z;
            sqt1[7] = quat.w;
            sqt1[8] = trans.x;
            sqt1[9] = trans.y;
            sqt1[10] = trans.z;
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
    mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], mPose[1]);

    if (mAlignRootBoneIdx >= 0) {
        for (int boneIdx = mpSkel->GetNumBones() - 1; boneIdx >= 0; --boneIdx) {
            if (boneIdx == mAlignRootBoneIdx) {
                float *sqt1 = &mPose[1][mAlignRootBoneIdx * 12];
                EAGL4::Transform boneMat;
                UMath::Vector4 quat;
                UMath::Vector4 trans;

                boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9], sqt1[10]);
                boneMat.PostMult(mAlignMatrix);
                boneMat.ExtractQuatTrans(&quat, &trans);

                sqt1[4] = quat.x;
                sqt1[5] = quat.y;
                sqt1[6] = quat.z;
                sqt1[7] = quat.w;
                sqt1[8] = trans.x;
                sqt1[9] = trans.y;
                sqt1[10] = trans.z;
                ::LinearBlendF3(w, &mPose[0][mAlignRootBoneIdx * 12 + 8], &mPose[1][mAlignRootBoneIdx * 12 + 8],
                                &outputPose[mAlignRootBoneIdx * 12 + 8]);
            }

            int poseIdx = boneIdx * 12;

            FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &mPose[1][poseIdx + 4], &outputPose[poseIdx + 4]);
        }
    } else {
        for (int poseIdx = mpSkel->GetNumBones() * 12 - 8; poseIdx > 3; poseIdx -= 12) {
            FastQuatBlendF4(w, &mPose[0][poseIdx], &mPose[1][poseIdx], &outputPose[poseIdx]);
        }

        BlendRootTranslation(w, mPose[0], mPose[1], outputPose, 0);
    }
}

}; // namespace EAGL4Anim
