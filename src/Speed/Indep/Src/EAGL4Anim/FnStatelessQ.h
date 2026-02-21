#ifndef EAGL4ANIM_FNSTATELESSQ_H
#define EAGL4ANIM_FNSTATELESSQ_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "BoneMask.h"
#include "FnAnimMemoryMap.h"

#include <types.h>

namespace EAGL4Anim {

// total size: 0x18
class FnStatelessQ : public FnAnimMemoryMap {
  public:
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    void PatchVtbl(FnStatelessQ *statelessQ) {}

    FnStatelessQ();

    // Overrides: FnAnimSuper
    ~FnStatelessQ() override;

    // Overrides: FnAnimMemoryMap
    void SetAnimMemoryMap(AnimMemoryMap *anim) override;

    // Overrides: FnAnim
    bool GetLength(float &timeLength) const override;

    // Overrides: FnAnim
    void Eval(float, float currTime, float *sqt) override;

    bool EvalSQTMask(float, float *sqt, const BoneMask *boneMask, bool slerpReqd, int floorKey, float scale);

    // Overrides: FnAnim
    void UseFPS(bool u) override;

    unsigned char GetFPS() const;

    // Overrides: FnAnim
    unsigned short GetTargetCheckSum() const override {}

    // Overrides: FnAnim
    const AttributeBlock *GetAttributes() const override {}

  protected:
    // Overrides: FnAnim
    bool EvalSQT(float currTime, float *sqt, const BoneMask *boneMask) override;

    unsigned short mPrevKey;   // offset 0x10, size 0x2
    unsigned char mUseFPS;     // offset 0x12, size 0x1
    unsigned char mFPS;        // offset 0x13, size 0x1
    const BoneMask *mBoneMask; // offset 0x14, size 0x4
};

}; // namespace EAGL4Anim

#endif
