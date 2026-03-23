#include "Skeleton.h"

static void MtxMult(EAGL4::Transform *result, const EAGL4::Transform *A, const EAGL4::Transform *B);

namespace EAGL4Anim {

extern void (*MatrixMultiply)(EAGL4::Transform *, const EAGL4::Transform *, const EAGL4::Transform *);

void Skeleton::PoseSQTToGlobal(float *pose, EAGL4::Transform *output, BoneMask *mask) {
    if (mask) {
        int i;
        int p;
        float *poseData = pose;
        BoneData *bones = GetBoneData();
        int n = GetNumBones();

        for (i = 0; i < n; i++) {
            if (mask->GetBone(i)) {
                output[i].BuildSQT(poseData[0], poseData[1], poseData[2], poseData[4], poseData[5], poseData[6], poseData[7], poseData[8],
                                   poseData[9], poseData[10]);
                output[i].m.v0.x *= poseData[3];
                output[i].m.v1.x *= poseData[3];
                output[i].m.v2.x *= poseData[3];

                p = bones[i].mParentIdx;
                if (p >= 0) {
                    MatrixMultiply(&output[i], &output[p], &output[i]);
                }
            }

            poseData += 12;
        }
    } else {
        int i;
        int p;
        float *poseData = pose;
        BoneData *bones = GetBoneData();
        int n = GetNumBones();

        for (i = 0; i < n; i++) {
            output[i].BuildSQT(poseData[0], poseData[1], poseData[2], poseData[4], poseData[5], poseData[6], poseData[7], poseData[8],
                               poseData[9], poseData[10]);
            output[i].m.v0.x *= poseData[3];
            output[i].m.v1.x *= poseData[3];
            output[i].m.v2.x *= poseData[3];

            p = bones[i].mParentIdx;
            if (p >= 0) {
                MatrixMultiply(&output[i], &output[p], &output[i]);
            }

            poseData += 12;
        }
    }
}

void Skeleton::GetStillPose(float *pose, const BoneMask *mask) const {
    int n = GetNumBones();
    int i;

    if (mask) {
        if (GetInvBoneScales()) {
            for (i = 0; i < n; i++) {
                if (mask->GetBone(i)) {
                    const BoneData &bd = GetBoneData(i);

                    pose[0] = bd.mS.x;
                    pose[1] = bd.mS.y;
                    pose[2] = bd.mS.z;
                    pose[3] = GetInvBoneScales()[i];
                    pose[4] = bd.mQ.x;
                    pose[5] = bd.mQ.y;
                    pose[6] = bd.mQ.z;
                    pose[7] = bd.mQ.w;
                    pose[8] = bd.mT.x;
                    pose[9] = bd.mT.y;
                    pose[10] = bd.mT.z;
                    pose[11] = 1.0f;
                }
                pose += 12;
            }
        } else {
            for (i = 0; i < n; i++) {
                if (mask->GetBone(i)) {
                    const BoneData &bd = GetBoneData(i);
                    const float one = 1.0f;
                    float z;

                    pose[0] = bd.mS.x;
                    pose[1] = bd.mS.y;
                    z = bd.mS.z;
                    pose[2] = (pose[3] = one, z);
                    pose[4] = bd.mQ.x;
                    pose[5] = bd.mQ.y;
                    pose[6] = bd.mQ.z;
                    pose[7] = bd.mQ.w;
                    pose[8] = bd.mT.x;
                    pose[9] = bd.mT.y;
                    pose[10] = bd.mT.z;
                    pose[11] = one;
                }
                pose += 12;
            }
        }
    } else if (GetInvBoneScales()) {
        for (i = 0; i < n; i++) {
            const BoneData &bd = GetBoneData(i);

            pose[0] = bd.mS.x;
            pose[1] = bd.mS.y;
            pose[2] = bd.mS.z;
            pose[3] = GetInvBoneScales()[i];
            pose[4] = bd.mQ.x;
            pose[5] = bd.mQ.y;
            pose[6] = bd.mQ.z;
            pose[7] = bd.mQ.w;
            pose[8] = bd.mT.x;
            pose[9] = bd.mT.y;
            pose[10] = bd.mT.z;
            pose[11] = 1.0f;
            pose += 12;
        }
    } else {
        for (i = 0; i < n; i++) {
            const BoneData &bd = GetBoneData(i);
            const float one = 1.0f;
            float z;

            pose[0] = bd.mS.x;
            pose[1] = bd.mS.y;
            z = bd.mS.z;
            pose[2] = (pose[3] = one, z);
            pose[4] = bd.mQ.x;
            pose[5] = bd.mQ.y;
            pose[6] = bd.mQ.z;
            pose[7] = bd.mQ.w;
            pose[8] = bd.mT.x;
            pose[9] = bd.mT.y;
            pose[10] = bd.mT.z;
            pose[11] = one;
            pose += 12;
        }
    }
}

void Skeleton::MirrorPose(float *pose, float *mirrorPose, bool local, const BoneMask *mask) {
    const int DOF = 12;
    const int n = GetNumBones();
    int i;
    float *ps;
    float *pd;

    if (mask) {
        if (pose == mirrorPose) {
            for (i = 0; i < n; i++) {
                if (mask->GetBone(i)) {
                    int lrIdx = GetBoneData(i).mLeftRightIdx;

                    if (lrIdx > i) {
                        float tmp;

                        pd = &mirrorPose[lrIdx * DOF];
                        ps = &mirrorPose[i * DOF];
                        tmp = pd[4];
                        pd[4] = -ps[4];
                        ps[4] = -tmp;
                        tmp = pd[5];
                        pd[5] = -ps[5];
                        ps[5] = -tmp;
                        tmp = pd[6];
                        pd[6] = ps[6];
                        ps[6] = tmp;
                        tmp = pd[7];
                        pd[7] = ps[7];
                        ps[7] = tmp;
                        tmp = pd[8];
                        pd[8] = ps[8];
                        ps[8] = tmp;
                        tmp = pd[9];
                        pd[9] = ps[9];
                        ps[9] = tmp;
                        tmp = pd[10];
                        pd[10] = -ps[10];
                        ps[10] = -tmp;
                    } else if (lrIdx == i) {
                        pd = &mirrorPose[i * DOF];
                        pd[4] = -pd[4];
                        pd[5] = -pd[5];
                        pd[10] = -pd[10];
                    }
                }
            }
        } else {
            for (i = 0; i < n; i++) {
                if (mask->GetBone(i)) {
                    int lrIdx = GetBoneData(i).mLeftRightIdx;

                    ps = &pose[i * DOF];
                    pd = &mirrorPose[lrIdx * DOF];
                    pd[0] = ps[0];
                    pd[1] = ps[1];
                    pd[2] = ps[2];
                    pd[4] = -ps[4];
                    pd[5] = -ps[5];
                    pd[6] = ps[6];
                    pd[7] = ps[7];
                    pd[8] = ps[8];
                    pd[9] = ps[9];
                    pd[10] = -ps[10];
                }
            }
        }
        if (!local) {
            pd = mirrorPose;
            UMath::Vector4 quat = *reinterpret_cast<UMath::Vector4 *>(&pd[4]);

            pd[4] = quat.z;
            pd[5] = quat.w;
            pd[6] = -quat.x;
            pd[8] = -pd[8];
            pd[10] = -pd[10];
            pd[7] = -quat.y;
        }
    } else {
        if (pose == mirrorPose) {
            for (i = 0; i < n; i++) {
                int lrIdx = GetBoneData(i).mLeftRightIdx;

                if (lrIdx > i) {
                    float tmp;

                    pd = &mirrorPose[lrIdx * DOF];
                    ps = &mirrorPose[i * DOF];
                    tmp = pd[4];
                    pd[4] = -ps[4];
                    ps[4] = -tmp;
                    tmp = pd[5];
                    pd[5] = -ps[5];
                    ps[5] = -tmp;
                    tmp = pd[6];
                    pd[6] = ps[6];
                    ps[6] = tmp;
                    tmp = pd[7];
                    pd[7] = ps[7];
                    ps[7] = tmp;
                    tmp = pd[8];
                    pd[8] = ps[8];
                    ps[8] = tmp;
                    tmp = pd[9];
                    pd[9] = ps[9];
                    ps[9] = tmp;
                    tmp = pd[10];
                    pd[10] = -ps[10];
                    ps[10] = -tmp;
                } else if (lrIdx == i) {
                    pd = &mirrorPose[i * DOF];
                    pd[4] = -pd[4];
                    pd[5] = -pd[5];
                    pd[10] = -pd[10];
                }
            }
        } else {
            for (i = 0; i < n; i++) {
                int lrIdx = GetBoneData(i).mLeftRightIdx;

                ps = &pose[i * DOF];
                pd = &mirrorPose[lrIdx * DOF];
                pd[0] = ps[0];
                pd[1] = ps[1];
                pd[2] = ps[2];
                pd[4] = -ps[4];
                pd[5] = -ps[5];
                pd[6] = ps[6];
                pd[7] = ps[7];
                pd[8] = ps[8];
                pd[9] = ps[9];
                pd[10] = -ps[10];
            }
        }
        if (!local) {
            pd = mirrorPose;
            UMath::Vector4 quat = *reinterpret_cast<UMath::Vector4 *>(&pd[4]);

            pd[4] = quat.z;
            pd[5] = quat.w;
            pd[6] = -quat.x;
            pd[8] = -pd[8];
            pd[10] = -pd[10];
            pd[7] = -quat.y;
        }
    }
}

}; // namespace EAGL4Anim
