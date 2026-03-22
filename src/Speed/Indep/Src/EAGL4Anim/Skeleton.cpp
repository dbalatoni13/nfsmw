#include "Skeleton.h"


namespace EAGL4Anim {

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
