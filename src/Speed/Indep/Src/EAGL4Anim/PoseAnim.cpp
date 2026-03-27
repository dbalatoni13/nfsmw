#include "PoseAnim.h"

#include "FnPoseAnim.h"

namespace EAGL4Anim {

void PoseAnim::InitAnimMemoryMap(AnimMemoryMap *anim) {
    PoseAnim *poseAnim = reinterpret_cast<PoseAnim *>(anim);
    FnPoseAnim fnPoseAnim;
    FnPoseAnim *fnPoseAnimPtr = reinterpret_cast<FnPoseAnim *>(poseAnim->GetFnLocation());

    *reinterpret_cast<void **>(fnPoseAnimPtr) = *reinterpret_cast<void **>(&fnPoseAnim);
}

}; // namespace EAGL4Anim
