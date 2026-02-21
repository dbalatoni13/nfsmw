#ifndef EAGL4ANIM_FNPOSEANIM_H
#define EAGL4ANIM_FNPOSEANIM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnimMemoryMap.h"
#include "PosePalette.h"

namespace EAGL4Anim {

// total size: 0x14
class FnPoseAnim : public FnAnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    // Overrides: FnAnimSuper
    ~FnPoseAnim() override {}

    static void PatchVtbl(FnPoseAnim *poseAnim) {}

    FnPoseAnim();

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override;

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override;

    // Overrides: FnAnim
    bool EvalPose(float currTime, const PosePaletteBank *paletteBank, float *sqt) override;

    // Overrides: FnAnim
    unsigned short GetTargetCheckSum() const override {}

  protected:
    unsigned short mPrevKey; // offset 0x10, size 0x2
};

}; // namespace EAGL4Anim

#endif
