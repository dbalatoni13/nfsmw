#ifndef EAGL4ANIM_BONEDATA_H
#define EAGL4ANIM_BONEDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

namespace EAGL4Anim {

// total size: 0x70
struct BoneData {
    UMath::Vector3 mS;             // offset 0x0, size 0xC
    int mParentIdx;                // offset 0xC, size 0x4
    UMath::Vector4 mQ;             // offset 0x10, size 0x10
    UMath::Vector3 mT;             // offset 0x20, size 0xC
    int mLeftRightIdx;             // offset 0x2C, size 0x4
    UMath::Matrix4 mInvBaseMatrix; // offset 0x30, size 0x40
};

}; // namespace EAGL4Anim

#endif
