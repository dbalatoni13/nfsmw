#include "FnPoseBlender.h"

#include "AnimUtil.h"


namespace EAGL4 {

void MultMatrix(const UMath::Matrix4 *pm1, const UMath::Matrix4 *pm2, UMath::Matrix4 *presult) {
    const float *m1 = pm1->GetElements();
    const float *m2 = pm2->GetElements();
    float *result = presult->GetElements();

    result[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
    result[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
    result[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
    result[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];
    result[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
    result[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
    result[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
    result[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];
    result[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
    result[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
    result[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
    result[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];
    result[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
    result[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
    result[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
    result[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];
}

void Transform::PostMult(const Transform &second, Transform *pOutput) const {
    MultMatrix(&m, &second.m, &pOutput->m);
}

void Transform::PostMult(const Transform &second) {
    Transform result;

    MultMatrix(&m, &second.m, &result.m);
    m = result.m;
}

void Transform::ExtractQuatTrans(UMath::Vector4 *retQuat, UMath::Vector4 *retTrans) const {
    const float *mat = m.GetElements();
    float xx = mat[0];
    float yy = mat[5];
    float zz = mat[10];
    float xy = mat[1];
    float xz = mat[2];
    float yx = mat[4];
    float yz = mat[6];
    float zx = mat[8];
    float zy = mat[9];
    float trace = xx + yy + zz;

    if (trace > 0.0f) {
        float s = EAGL4Anim::FastSqrt(trace + 1.0f);
        float invS = 0.5f / s;

        retQuat->w = s * 0.5f;
        retQuat->z = (xy - yx) * invS;
        retQuat->x = (yz - zy) * invS;
        retQuat->y = (zx - xz) * invS;
    } else {
        float maxDiag = yy;

        if (xx >= yy) {
            maxDiag = xx;
        }

        if (zz > maxDiag) {
            float s = EAGL4Anim::FastSqrt((zz - (xx + yy)) + 1.0f);

            retQuat->z = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            retQuat->w = (xy - yx) * s;
            retQuat->y = (zy + yz) * s;
            retQuat->x = (zx + xz) * s;
        } else if (xx >= yy) {
            float s = EAGL4Anim::FastSqrt((xx - (yy + zz)) + 1.0f);

            retQuat->x = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            retQuat->w = (yz - zy) * s;
            retQuat->y = (xy + yx) * s;
            retQuat->z = (xz + zx) * s;
        } else {
            float s = EAGL4Anim::FastSqrt((yy - (zz + xx)) + 1.0f);

            retQuat->y = s * 0.5f;
            if (s != 0.0f) {
                s = 0.5f / s;
            }
            retQuat->w = (zx - xz) * s;
            retQuat->x = (yx + xy) * s;
            retQuat->z = (yz + zy) * s;
        }
    }

    retTrans->x = mat[12];
    retTrans->y = mat[13];
    retTrans->z = mat[14];
    retTrans->w = mat[15];
}

void Transform::BuildSQT(float sx, float sy, float sz, float qx, float qy, float qz, float qw, float tx, float ty, float tz) {
    float *mat = m.GetElements();
    float qy2 = qy + qy;
    float qz2 = qz + qz;
    float qx2qx = qx * (qx + qx);
    float qw2qx = qw * (qx + qx);

    mat[12] = tx;
    mat[13] = ty;
    mat[15] = 1.0f;
    mat[11] = 0.0f;
    mat[3] = 0.0f;
    mat[7] = 0.0f;
    mat[14] = tz;
    mat[2] = sx * (qw * qz2 - qw * qy2);
    mat[6] = sy * (qy * qz2 + qw2qx);
    mat[10] = sz * (1.0f - (qx2qx + qy * qy2));
    mat[0] = sx * (1.0f - (qy * qy2 + qz * qz2));
    mat[4] = sy * (qw * qy2 - qz * qz2);
    mat[8] = sz * (qw * qz2 + qz * qy2);
    mat[1] = sx * (qw * qy2 + qz * qz2);
    mat[5] = sy * (1.0f - (qx2qx + qz * qz2));
    mat[9] = sz * (qy * qz2 - qw2qx);
}

}; // namespace EAGL4

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
        if (mAnim[0]->EvalSQT(currentTime - mTimeOffset[0], sqtBuffer, boneMask)) {
            return true;
        }
        return false;
    }

    if (currentTime >= mEndTransTime) {
        bool result = mAnim[1]->EvalSQT(currentTime - mTimeOffset[1], sqtBuffer, boneMask);

        if (result) {
            if (mAlignRootBoneIdx >= 0 && (!boneMask || boneMask->GetBone(mAlignRootBoneIdx))) {
                EAGL4::Transform rootTransform;
                ApplyAlignedRootTransform(rootTransform, mAlignMatrix, sqtBuffer, mAlignRootBoneIdx);
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

    int numBones = mpSkel->GetNumBones();
    if (mAnim[1]->EvalSQT(currentTime - mTimeOffset[1], mPose[1], boneMask)) {
        if (!boneMask) {
            if (mAlignRootBoneIdx >= 0) {
                for (int boneIdx = numBones - 1; boneIdx >= 0; --boneIdx) {
                    if (boneIdx == mAlignRootBoneIdx) {
                        EAGL4::Transform rootTransform;

                        ApplyAlignedRootTransform(rootTransform, mAlignMatrix, mPose[1], boneIdx);
                        BlendRootTranslation(w, mPose[0], mPose[1], sqtBuffer, boneIdx);
                    }

                    int poseIdx = boneIdx * 12;

                    FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &mPose[1][poseIdx + 4], &sqtBuffer[poseIdx + 4]);
                }
            } else {
                for (int boneIdx = numBones - 1; boneIdx >= 0; --boneIdx) {
                    int poseIdx = boneIdx * 12;

                    FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &mPose[1][poseIdx + 4], &sqtBuffer[poseIdx + 4]);
                }

                BlendRootTranslation(w, mPose[0], mPose[1], sqtBuffer, 0);
            }
        } else if (mAlignRootBoneIdx >= 0) {
            for (int boneIdx = numBones - 1; boneIdx >= 0; --boneIdx) {
                if (boneMask->GetBone(boneIdx)) {
                    if (boneIdx == mAlignRootBoneIdx) {
                        EAGL4::Transform rootTransform;

                        ApplyAlignedRootTransform(rootTransform, mAlignMatrix, mPose[1], boneIdx);
                        BlendRootTranslation(w, mPose[0], mPose[1], sqtBuffer, boneIdx);
                    }

                    int poseIdx = boneIdx * 12;

                    FastQuatBlendF4(w, &mPose[0][poseIdx + 4], &mPose[1][poseIdx + 4], &sqtBuffer[poseIdx + 4]);
                }
            }
        } else {
            for (int boneIdx = numBones - 1; boneIdx >= 0; --boneIdx) {
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
