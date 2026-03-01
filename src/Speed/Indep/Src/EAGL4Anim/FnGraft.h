#ifndef EAGL4ANIM_FNGRAFT_H
#define EAGL4ANIM_FNGRAFT_H

#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdef.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FnAnim.h"

namespace EAGL4Anim {

// total size: 0x14
class FnGraft : public FnAnim {
  public:
    FnGraft() {
        mType = AnimTypeId::ANIM_GRAFT;
    }

    // Overrides: FnAnimSuper
    ~FnGraft() override {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    void operator delete(void *ptr, size_t size) {
        EAGL4Internal::EAGL4Free(ptr, size);
    }

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    void *operator new(size_t, void *ptr) {
        return ptr;
    }

    void Set(FnAnim **anims, int numAnims) {
        mppAnim = anims;
        mNumAnims = numAnims;
    }

    FnAnim **GetAnims() {
        return mppAnim;
    }

    // Overrides: FnAnim
    void Eval(float previousTime, float currentTime, float *dofs) override {
        for (int i = 0; i < mNumAnims; i++) {
            mppAnim[i]->Eval(previousTime, currentTime, dofs);
        }
    }

  private:
    FnAnim **mppAnim; // offset 0xC, size 0x4
    int mNumAnims;    // offset 0x10, size 0x4
};

}; // namespace EAGL4Anim

#endif
