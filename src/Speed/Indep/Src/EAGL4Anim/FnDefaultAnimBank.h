#ifndef EAGL4ANIM_FNDEFAULTANIMBANK_H
#define EAGL4ANIM_FNDEFAULTANIMBANK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "eagl4AnimBank.h"
#include "eagl4FnAnimBank.h"

#include <types.h>

namespace EAGL4Anim {

// total size: 0x8
class FnDefaultAnimBank : public FnAnimBank {
  public:
    FnDefaultAnimBank() {}

    virtual ~FnDefaultAnimBank() {}

    // Overrides: FnAnimBank
    void Init(AnimBank *bank) override {}

    // Overrides: FnAnimBank
    int GetNumAnims() const override {}

    // Overrides: FnAnimBank
    AnimMemoryMap *GetAnim(int i) override {}

    // Overrides: FnAnimBank
    const char *GetAnimName(int i) const override {}

    // Overrides: FnAnimBank
    AnimMemoryMap *GetAnim(const char *name) override {}

    // Overrides: FnAnimBank
    int GetAnimIndex(const char *name) override {}

    // Overrides: FnAnimBank
    const AttributeDictionary *GetAttributeDictionary() const override {}

  private:
    AnimBank *mpAnimBank; // offset 0x4, size 0x4
};

}; // namespace EAGL4Anim

#endif
