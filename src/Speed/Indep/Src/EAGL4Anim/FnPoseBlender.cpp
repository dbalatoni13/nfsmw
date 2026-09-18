#include "FnPoseBlender.h"
#include "Speed/Indep/Src/EAGL4Anim/AnimUtil.h"

namespace EAGL4Anim {

void FnPoseBlender::Blend(int numBones, float w, const float *pose0, const float *pose1, float *result, const BoneMask *boneMask) {

    int i;
    int j;

    if (!boneMask) {

        j = 0;

        for (i = 0; i < numBones; i++) {

            j += 4;
            FastQuatBlendF4(w, &pose0[j], &pose1[j], &result[j]);

            j += 4;
            EAGL4Anim::LinearBlendF3(w, &pose0[j], &pose1[j], &result[j]);

            j += 4;
        }
    } else {

        j = 0;

        for (i = 0; i < numBones; i++) {

            j += 4;

            if (boneMask->GetBone(i)) {

                FastQuatBlendF4(w, &pose0[j], &pose1[j], &result[j]);

                j += 4;
                EAGL4Anim::LinearBlendF3(w, &pose0[j], &pose1[j], &result[j]);

                j += 4;

            } else {

                j += 8;
            }
        }
    }
}

bool FnPoseBlender::EvalSQT(float currentTime, float *sqtBuffer, const BoneMask *boneMask) {

    const int DOF = 12;
    int q;
    int i;

    if (currentTime <= mStartTransTime) {

        if (!mAnim[0]->EvalSQT(currentTime - mTimeOffset[0], sqtBuffer, boneMask)) {

            return false;
        }

    } else if (currentTime >= mEndTransTime) {

        if (!mAnim[1]->EvalSQT(currentTime - mTimeOffset[1], sqtBuffer, boneMask)) {

            return false;
        }

        if (mAlignRootBoneIdx >= 0) {

            if (!boneMask || boneMask->GetBone(mAlignRootBoneIdx)) {

                float *sqt1 = &sqtBuffer[mAlignRootBoneIdx * DOF];
                EAGL4::Transform boneMat;
                UMath::Vector4 quat;
                UMath::Vector4 trans;

                boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9], sqt1[10]);
                boneMat.PostMult(mAlignMatrix);
                boneMat.ExtractQuatTrans(&quat, &trans);

                sqt1[4] = quat.x; sqt1[5] = quat.y; sqt1[6] = quat.z;
                sqt1[7] = quat.w;

                sqt1[8] = trans.x; sqt1[9] = trans.y; sqt1[10] = trans.z;
            }
        }
    } else {

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

        if (!mAnim[1]->EvalSQT(currentTime - mTimeOffset[1], mPose[1], boneMask)) {

            return false;
        }

        if (!boneMask) {

            if (mAlignRootBoneIdx >= 0) {

                for (i = mpSkel->GetNumBones() - 1; i >= 0; i--) {

                    if (i == mAlignRootBoneIdx) {

                        float *sqt1 = &mPose[1][i * DOF];
                        EAGL4::Transform boneMat;
                        UMath::Vector4 quat;
                        UMath::Vector4 trans;

                        boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9], sqt1[10]);
                        boneMat.PostMult(mAlignMatrix);
                        boneMat.ExtractQuatTrans(&quat, &trans);

                        sqt1[4] = quat.x; sqt1[5] = quat.y; sqt1[6] = quat.z;
                        sqt1[7] = quat.w;

                        sqt1[8] = trans.x; sqt1[9] = trans.y; sqt1[10] = trans.z;

                        q = mAlignRootBoneIdx;
                        EAGL4Anim::LinearBlendF3(w, &mPose[0][q + 8], &mPose[1][q + 8], &sqtBuffer[q + 8]);
                    }

                    q = i * DOF + 4;
                    FastQuatBlendF4(w, &mPose[0][q], &mPose[1][q], &sqtBuffer[q]);
                }
            } else {

                for (i = mpSkel->GetNumBones() - 1; i >= 0; i--) {

                    q = i * DOF + 4;
                    FastQuatBlendF4(w, &mPose[0][q], &mPose[1][q], &sqtBuffer[q]);
                }

                EAGL4Anim::LinearBlendF3(w, &mPose[0][8], &mPose[1][8], &sqtBuffer[8]);
            }
        } else {

            if (mAlignRootBoneIdx >= 0) {

                for (i = mpSkel->GetNumBones() - 1; i >= 0; i--) {

                    if (boneMask->GetBone(i)) {

                        if (i == mAlignRootBoneIdx) {

                            float *sqt1 = &mPose[1][i * DOF];
                            EAGL4::Transform boneMat;
                            UMath::Vector4 quat;
                            UMath::Vector4 trans;

                            boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9], sqt1[10]);
                            boneMat.PostMult(mAlignMatrix);
                            boneMat.ExtractQuatTrans(&quat, &trans);

                            sqt1[4] = quat.x; sqt1[5] = quat.y; sqt1[6] = quat.z;
                            sqt1[7] = quat.w;

                            sqt1[8] = trans.x; sqt1[9] = trans.y; sqt1[10] = trans.z;

                            q = mAlignRootBoneIdx;
                            EAGL4Anim::LinearBlendF3(w, &mPose[0][q + 8], &mPose[1][q + 8], &sqtBuffer[q + 8]);
                        }

                        q = i * DOF + 4;
                        FastQuatBlendF4(w, &mPose[0][q], &mPose[1][q], &sqtBuffer[q]);
                    }
                }
            } else {

                for (i = mpSkel->GetNumBones() - 1; i >= 0; i--) {

                    if (boneMask->GetBone(i)) {

                        q = i * DOF + 4;
                        FastQuatBlendF4(w, &mPose[0][q], &mPose[1][q], &sqtBuffer[q]);
                    }
                }

                if (boneMask->GetBone(0)) {

                    EAGL4Anim::LinearBlendF3(w, &mPose[0][8], &mPose[1][8], &sqtBuffer[8]);
                }
            }
        }
    }

    return true;
}

void FnPoseBlender::Eval(float previousTime, float currentTime, float *outputPose) {

    const int DOF = 12;
    int q;

    if (currentTime <= mStartTransTime) {

        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], outputPose);

    } else if (currentTime >= mEndTransTime) {

        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], outputPose);

        if (mAlignRootBoneIdx >= 0) {

            float *sqt1 = &outputPose[mAlignRootBoneIdx * DOF];
            EAGL4::Transform boneMat;
            UMath::Vector4 quat;
            UMath::Vector4 trans;

            boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9], sqt1[10]);
            boneMat.PostMult(mAlignMatrix);
            boneMat.ExtractQuatTrans(&quat, &trans);

            sqt1[4] = quat.x; sqt1[5] = quat.y; sqt1[6] = quat.z;
            sqt1[7] = quat.w;

            sqt1[8] = trans.x; sqt1[9] = trans.y; sqt1[10] = trans.z;
        }
    } else {

        float w = (currentTime - mStartTransTime) / mDuration;

        if (mResetBuffers[0]) {
            mpSkel->GetStillPose(mPose[0], nullptr);
        }
        if (mResetBuffers[1]) {
            mpSkel->GetStillPose(mPose[1], nullptr);
        }

        mAnim[0]->Eval(previousTime - mTimeOffset[0], currentTime - mTimeOffset[0], mPose[0]);

        mAnim[1]->Eval(previousTime - mTimeOffset[1], currentTime - mTimeOffset[1], mPose[1]);

        if (mAlignRootBoneIdx >= 0) {

            for (int i = mpSkel->GetNumBones() - 1; i >= 0; i--) {

                if (i == mAlignRootBoneIdx) {

                    float *sqt1 = &mPose[1][i * DOF];
                    EAGL4::Transform boneMat;
                    UMath::Vector4 quat;
                    UMath::Vector4 trans;

                    boneMat.BuildSQT(sqt1[0], sqt1[1], sqt1[2], sqt1[4], sqt1[5], sqt1[6], sqt1[7], sqt1[8], sqt1[9], sqt1[10]);
                    boneMat.PostMult(mAlignMatrix);
                    boneMat.ExtractQuatTrans(&quat, &trans);

                    sqt1[4] = quat.x; sqt1[5] = quat.y; sqt1[6] = quat.z;
                    sqt1[7] = quat.w;

                    sqt1[8] = trans.x; sqt1[9] = trans.y; sqt1[10] = trans.z;

                    EAGL4Anim::LinearBlendF3(w, &mPose[0][mAlignRootBoneIdx + 8], &mPose[1][mAlignRootBoneIdx + 8], &outputPose[mAlignRootBoneIdx + 8]);
                }

                q = i * DOF + 4;
                FastQuatBlendF4(w, &mPose[0][q], &mPose[1][q], &outputPose[q]);
            }
        } else {

            for (q = mpSkel->GetNumBones() * DOF - 8; q >= 4; q -= DOF) {

                FastQuatBlendF4(w, &mPose[0][q], &mPose[1][q], &outputPose[q]);
            }

            EAGL4Anim::LinearBlendF3(w, &mPose[0][8], &mPose[1][8], &outputPose[8]);
        }
    }
}

}; // namespace EAGL4Anim
