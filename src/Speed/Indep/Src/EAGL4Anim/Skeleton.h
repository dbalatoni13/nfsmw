#ifndef EAGL4ANIM_SKELETON_H
#define EAGL4ANIM_SKELETON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "BoneMask.h"
#include "SkeletonData.h"
#include "eagl4runtimetransform.h"

namespace EAGL4Anim {

// total size: 0x10
class Skeleton : public SkeletonData {
    void GetOrthoScale(int boneId, Transform &orthoScalingMatrix) const {}

    void RestoreOrthoScale(int boneId, const Transform &orthoScalingMatrix) {}

    static int ComputePoseBufferLength(int numBones) {}

    int GetPoseBufferLength() const {}

    void PoseLocal(float *pose, Transform *outputLocal) {}

    void Pose(float *pose, Transform *outputWorld) {}

    void Pose(Transform *local, Transform *outputWorld) {}

    void PoseSkin(float *pose, Transform *output) {}

    void PoseSkin(Transform *skelWorldPose, Transform *output) {}

    void PoseLocalToSkin(Transform *skelLocalPose, Transform *output, BoneMask *mask) {}

    void BuildBoneMask(int boneIdx, bool subtree, BoneMask &bm, int rootBoneIdx) const;

    void BuildMirrorBoneMask(const BoneMask &bm, BoneMask &result) const;

    void BuildSymmetricBoneMask(const BoneMask &bm, BoneMask &result) const;

    void GetBoneLengthScale(float *originalScale) const;

    void RestoreBoneLengthScale(const float *originalScale);

    void ScaleBoneLength(int boneId, float scale, float *invScaleBuffer);

    void MirrorPose(float *pose, float *mirrorPose, bool local, const BoneMask *mask);

    void MirrorPose(int beginBoneIdx, int endBoneIdx, float *pose, float *mirrorPose, bool local);

    void PoseSQTToLocal(float *pose, Transform *output, BoneMask *mask);

    void PoseLocalToGlobal(Transform *local, Transform *output, BoneMask *mask);

    void PoseSQTToGlobal(float *pose, Transform *output, BoneMask *mask);

    void PoseSQTToSkin(float *pose, Transform *output, BoneMask *mask);

    void PoseGlobalToSkin(Transform *skelPose, Transform *output, BoneMask *mask);

    void GetStillPose(float *pose, const BoneMask *mask) const;

    void GetStillPose(int boneIdx, float *pose) const;

    void OrthoScaleBone(int boneId, const Transform &orthoScalingMatrix);

    void GetStillTrans(float *trans);

    void PoseTrans(float *trans, Transform *output);

    void PoseBoneSQTToGlobal(int boneIdx, float *pose, Transform &boneWorld);

    void PoseQTToGlobal(int beginBoneIdx, int endBoneIdx, float *pose, Transform *output);

    void PoseTToGlobal(int beginBoneIdx, int endBoneIdx, float *pose, Transform *output);

    void PoseQTToSkin(int beginBoneIdx, int endBoneIdx, float *pose, Transform *output);
};

}; // namespace EAGL4Anim

#endif
