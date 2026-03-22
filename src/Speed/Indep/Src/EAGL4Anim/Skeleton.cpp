#include "Skeleton.h"

static void MtxMult(EAGL4::Transform *result, const EAGL4::Transform *A, const EAGL4::Transform *B);

namespace EAGL4Anim {

void Skeleton::PoseSQTToGlobal(float *pose, EAGL4::Transform *output, BoneMask *mask) {
    int numBones = GetNumBones();

    if (!mask) {
        for (int i = 0; i < numBones; i++) {
            float *bonePose = pose;
            float *mat = output[i].m.GetElements();

            output[i].BuildSQT(bonePose[0], bonePose[1], bonePose[2], bonePose[4], bonePose[5], bonePose[6], bonePose[7], bonePose[8],
                               bonePose[9], bonePose[10]);
            mat[0] *= bonePose[3];
            mat[4] *= bonePose[3];
            mat[8] *= bonePose[3];

            int parentIdx = GetBoneData(i).mParentIdx;
            if (parentIdx > -1) {
                MtxMult(&output[i], &output[parentIdx], &output[i]);
            }

            pose += 12;
        }
    } else {
        for (int i = 0; i < numBones; i++) {
            if (mask->GetBone(i)) {
                float *bonePose = pose;
                float *mat = output[i].m.GetElements();

                output[i].BuildSQT(bonePose[0], bonePose[1], bonePose[2], bonePose[4], bonePose[5], bonePose[6], bonePose[7], bonePose[8],
                                   bonePose[9], bonePose[10]);
                mat[0] *= bonePose[3];
                mat[4] *= bonePose[3];
                mat[8] *= bonePose[3];

                int parentIdx = GetBoneData(i).mParentIdx;
                if (parentIdx > -1) {
                    MtxMult(&output[i], &output[parentIdx], &output[i]);
                }
            }

            pose += 12;
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
    for (int ibone = 0; ibone < GetNumBones(); ibone++) {
        if (!mask || mask->GetBone(ibone)) {
            int mirrorBone = GetBoneData(ibone).mLeftRightIdx;

            if (mirrorBone < 0 || mirrorBone >= GetNumBones()) {
                mirrorBone = ibone;
            }

            float *dst = &mirrorPose[ibone * 12];
            const float *src = &pose[mirrorBone * 12];

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
