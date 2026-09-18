#include "PoseAnim.h"
#include "Speed/Indep/Src/EAGL4Anim/FnPoseAnim.h"

namespace EAGL4Anim {

void PoseAnim::InitAnimMemoryMap(AnimMemoryMap *anim) {
    PoseAnim *poseAnim = reinterpret_cast<PoseAnim *>(anim);

    FnPoseAnim::PatchVtbl(reinterpret_cast<FnPoseAnim *>(poseAnim->GetFnLocation()));
}

}; // namespace EAGL4Anim
