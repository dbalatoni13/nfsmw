#include "Skeleton.h"

static void MtxMult(EAGL4::Transform *result, const EAGL4::Transform *A, const EAGL4::Transform *B);

namespace EAGL4Anim {

extern void (*MatrixMultiply)(EAGL4::Transform *, const EAGL4::Transform *, const EAGL4::Transform *);

void Skeleton::PoseSQTToGlobal(float *pose, EAGL4::Transform *output, BoneMask *mask) {
    if (mask) {
        float *poseData = pose;
        BoneData *bones = GetBoneData();
        int n = GetNumBones();

        for (int i = 0; i < n; i++) {
            if (mask->GetBone(i)) {
                float *mat = output[i].m.GetElements();

                output[i].BuildSQT(poseData[0], poseData[1], poseData[2], poseData[4], poseData[5], poseData[6], poseData[7], poseData[8],
                                   poseData[9], poseData[10]);
                mat[0] *= poseData[3];
                mat[4] *= poseData[3];
                mat[8] *= poseData[3];

                int p = bones[i].mParentIdx;
                if (p >= 0) {
                    MatrixMultiply(&output[i], &output[p], &output[i]);
                }
            }

            poseData += 12;
        }
    } else {
        float *poseData = pose;
        BoneData *bones = GetBoneData();
        int n = GetNumBones();

        for (int i = 0; i < n; i++) {
            float *mat = output[i].m.GetElements();

            output[i].BuildSQT(poseData[0], poseData[1], poseData[2], poseData[4], poseData[5], poseData[6], poseData[7], poseData[8],
                               poseData[9], poseData[10]);
            mat[0] *= poseData[3];
            mat[4] *= poseData[3];
            mat[8] *= poseData[3];

            int p = bones[i].mParentIdx;
            if (p >= 0) {
                MatrixMultiply(&output[i], &output[p], &output[i]);
            }

            poseData += 12;
        }
    }
}

void Skeleton::GetStillPose(float *pose, const BoneMask *mask) const {
    int numBones = GetNumBones();
    const float *invBoneScales = GetInvBoneScales();

    if (!mask) {
        if (!invBoneScales) {
            for (int i = 0; i < numBones; i++) {
                const BoneData &bone = GetBoneData(i);

                pose[0] = bone.mS.x;
                pose[1] = bone.mS.y;
                pose[2] = bone.mS.z;
                pose[3] = 1.0f;
                pose[4] = bone.mQ.x;
                pose[5] = bone.mQ.y;
                pose[6] = bone.mQ.z;
                pose[7] = bone.mQ.w;
                pose[8] = bone.mT.x;
                pose[9] = bone.mT.y;
                pose[10] = bone.mT.z;
                pose[11] = 1.0f;
                pose += 12;
            }
        } else {
            for (int i = 0; i < numBones; i++) {
                const BoneData &bone = GetBoneData(i);

                pose[0] = bone.mS.x;
                pose[1] = bone.mS.y;
                pose[2] = bone.mS.z;
                pose[3] = invBoneScales[i];
                pose[4] = bone.mQ.x;
                pose[5] = bone.mQ.y;
                pose[6] = bone.mQ.z;
                pose[7] = bone.mQ.w;
                pose[8] = bone.mT.x;
                pose[9] = bone.mT.y;
                pose[10] = bone.mT.z;
                pose[11] = 1.0f;
                pose += 12;
            }
        }
    } else {
        if (!invBoneScales) {
            for (unsigned int i = 0; i < static_cast<unsigned int>(numBones); i++) {
                if (mask->GetBone(i)) {
                    const BoneData &bone = GetBoneData(static_cast<int>(i));

                    pose[0] = bone.mS.x;
                    pose[1] = bone.mS.y;
                    pose[2] = bone.mS.z;
                    pose[3] = 1.0f;
                    pose[4] = bone.mQ.x;
                    pose[5] = bone.mQ.y;
                    pose[6] = bone.mQ.z;
                    pose[7] = bone.mQ.w;
                    pose[8] = bone.mT.x;
                    pose[9] = bone.mT.y;
                    pose[10] = bone.mT.z;
                    pose[11] = 1.0f;
                }
                pose += 12;
            }
        } else {
            for (unsigned int i = 0; i < static_cast<unsigned int>(numBones); i++) {
                if (mask->GetBone(i)) {
                    const BoneData &bone = GetBoneData(static_cast<int>(i));

                    pose[0] = bone.mS.x;
                    pose[1] = bone.mS.y;
                    pose[2] = bone.mS.z;
                    pose[3] = invBoneScales[i];
                    pose[4] = bone.mQ.x;
                    pose[5] = bone.mQ.y;
                    pose[6] = bone.mQ.z;
                    pose[7] = bone.mQ.w;
                    pose[8] = bone.mT.x;
                    pose[9] = bone.mT.y;
                    pose[10] = bone.mT.z;
                    pose[11] = 1.0f;
                }
                pose += 12;
            }
        }
    }
}

void Skeleton::MirrorPose(float *pose, float *mirrorPose, bool local, const BoneMask *mask) {
    int numBones = GetNumBones();

    if (mask) {
        if (pose == mirrorPose) {
            for (unsigned int ibone = 0; static_cast<int>(ibone) < numBones; ibone++) {
                if (mask->GetBone(ibone)) {
                    unsigned int mirrorBone = GetBoneData(static_cast<int>(ibone)).mLeftRightIdx;

                    if (ibone < mirrorBone) {
                        float *dst = &mirrorPose[mirrorBone * 12];
                        float *src = &mirrorPose[ibone * 12];
                        float value = dst[4];

                        dst[4] = -src[4];
                        src[4] = -value;
                        value = dst[5];
                        dst[5] = -src[5];
                        src[5] = -value;
                        value = dst[6];
                        dst[6] = src[6];
                        src[6] = value;
                        value = dst[7];
                        dst[7] = src[7];
                        src[7] = value;
                        value = dst[8];
                        dst[8] = src[8];
                        src[8] = value;
                        value = dst[9];
                        dst[9] = src[9];
                        src[9] = value;
                        value = dst[10];
                        dst[10] = -src[10];
                        src[10] = -value;
                    } else if (mirrorBone == ibone) {
                        float *dst = &mirrorPose[ibone * 12];

                        dst[4] = -dst[4];
                        dst[5] = -dst[5];
                        dst[10] = -dst[10];
                    }
                }
            }
        } else {
            for (unsigned int ibone = 0; static_cast<int>(ibone) < numBones; ibone++) {
                if (mask->GetBone(ibone)) {
                    int mirrorBone = GetBoneData(static_cast<int>(ibone)).mLeftRightIdx;
                    float *src = &pose[ibone * 12];
                    float *dst = &mirrorPose[mirrorBone * 12];

                    dst[0] = src[0];
                    dst[1] = src[1];
                    dst[2] = src[2];
                    dst[3] = src[3];
                    dst[4] = src[4];
                    dst[5] = -src[5];
                    dst[6] = -src[6];
                    dst[7] = src[7];
                    dst[8] = -src[8];
                    dst[9] = src[9];
                    dst[10] = src[10];
                    dst[11] = src[11];

                    if (!local) {
                        dst[4] = -dst[4];
                        dst[7] = -dst[7];
                    }
                }
            }
        }
    } else if (pose == mirrorPose) {
        for (int ibone = 0; ibone < numBones; ibone++) {
            int mirrorBone = GetBoneData(ibone).mLeftRightIdx;

            if (ibone < mirrorBone) {
                float *dst = &mirrorPose[mirrorBone * 12];
                float *src = &mirrorPose[ibone * 12];
                float value = dst[4];

                dst[4] = -src[4];
                src[4] = -value;
                value = dst[5];
                dst[5] = -src[5];
                src[5] = -value;
                value = dst[6];
                dst[6] = src[6];
                src[6] = value;
                value = dst[7];
                dst[7] = src[7];
                src[7] = value;
                value = dst[8];
                dst[8] = src[8];
                src[8] = value;
                value = dst[9];
                dst[9] = src[9];
                src[9] = value;
                value = dst[10];
                dst[10] = -src[10];
                src[10] = -value;
            } else if (mirrorBone == ibone) {
                float *dst = &mirrorPose[ibone * 12];

                dst[4] = -dst[4];
                dst[5] = -dst[5];
                dst[10] = -dst[10];
            }
        }
    } else {
        for (int ibone = 0; ibone < numBones; ibone++) {
            int mirrorBone = GetBoneData(ibone).mLeftRightIdx;
            float *src = &pose[ibone * 12];
            float *dst = &mirrorPose[mirrorBone * 12];

            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            dst[4] = src[4];
            dst[5] = -src[5];
            dst[6] = -src[6];
            dst[7] = src[7];
            dst[8] = -src[8];
            dst[9] = src[9];
            dst[10] = src[10];
            dst[11] = src[11];

            if (!local) {
                dst[4] = -dst[4];
                dst[7] = -dst[7];
            }
        }
    }
}

}; // namespace EAGL4Anim
