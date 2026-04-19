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
  public:
    void GetOrthoScale(int boneId, EAGL4::Transform &orthoScalingMatrix) const {}

    void RestoreOrthoScale(int boneId, const EAGL4::Transform &orthoScalingMatrix) {}

    static int ComputePoseBufferLength(int numBones) {
        return numBones * 0xC; // TODO: should 0xC be a sizeof?
    }

    int GetPoseBufferLength() const {
        return ComputePoseBufferLength(GetNumBones());
    }

    void PoseLocal(float *pose, EAGL4::Transform *outputLocal) {}

    void Pose(float *pose, EAGL4::Transform *outputWorld) {}

    void Pose(EAGL4::Transform *local, EAGL4::Transform *outputWorld) {}

    void PoseSkin(float *pose, EAGL4::Transform *output) {}

    void PoseSkin(EAGL4::Transform *skelWorldPose, EAGL4::Transform *output) {}

    void PoseLocalToSkin(EAGL4::Transform *skelLocalPose, EAGL4::Transform *output, BoneMask *mask) {}

    void BuildBoneMask(int boneIdx, bool subtree, BoneMask &bm, int rootBoneIdx) const;

    void BuildMirrorBoneMask(const BoneMask &bm, BoneMask &result) const;

    void BuildSymmetricBoneMask(const BoneMask &bm, BoneMask &result) const;

    void GetBoneLengthScale(float *originalScale) const;

    void RestoreBoneLengthScale(const float *originalScale);

    void ScaleBoneLength(int boneId, float scale, float *invScaleBuffer);

    void MirrorPose(float *pose, float *mirrorPose, bool local, const BoneMask *mask);

    void MirrorPose(int beginBoneIdx, int endBoneIdx, float *pose, float *mirrorPose, bool local);

    void PoseSQTToLocal(float *pose, EAGL4::Transform *output, BoneMask *mask);

    void PoseLocalToGlobal(EAGL4::Transform *local, EAGL4::Transform *output, BoneMask *mask);

    void PoseSQTToGlobal(float *pose, EAGL4::Transform *output, BoneMask *mask);

    void PoseSQTToSkin(float *pose, EAGL4::Transform *output, BoneMask *mask);

    void PoseGlobalToSkin(EAGL4::Transform *skelPose, EAGL4::Transform *output, BoneMask *mask);

    void GetStillPose(float *pose, const BoneMask *mask) const;

    void GetStillPose(int boneIdx, float *pose) const;

    void OrthoScaleBone(int boneId, const EAGL4::Transform &orthoScalingMatrix);

    void GetStillTrans(float *trans);

    void PoseTrans(float *trans, EAGL4::Transform *output);

    void PoseBoneSQTToGlobal(int boneIdx, float *pose, EAGL4::Transform &boneWorld);

    void PoseQTToGlobal(int beginBoneIdx, int endBoneIdx, float *pose, EAGL4::Transform *output);

    void PoseTToGlobal(int beginBoneIdx, int endBoneIdx, float *pose, EAGL4::Transform *output);

    void PoseQTToSkin(int beginBoneIdx, int endBoneIdx, float *pose, EAGL4::Transform *output);
};

}; // namespace EAGL4Anim

#endif
