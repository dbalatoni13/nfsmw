#ifndef EAGL4ANIM_EAGL4FNANIMBANK_H
#define EAGL4ANIM_EAGL4FNANIMBANK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "eagl4AnimBank.h"

namespace EAGL4Anim {

// total size: 0x4
class FnAnimBank {
  public:
    FnAnimBank() {}

    virtual void Init(AnimBank *bank);
    virtual int GetNumAnims() const;
    virtual AnimMemoryMap *GetAnim(int i);
    virtual const char *GetAnimName(int i) const;
    virtual AnimMemoryMap *GetAnim(const char *name);
    virtual int GetAnimIndex(const char *name);
    virtual const AttributeDictionary *GetAttributeDictionary() const;
};

}; // namespace EAGL4Anim

#endif
