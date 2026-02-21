#ifndef EAGL4ANIM_BONEMASK_H
#define EAGL4ANIM_BONEMASK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

namespace EAGL4Anim {

// total size: 0x20
class BoneMask {
  private:
    BoneMask(bool allOn);

    BoneMask(const BoneMask &bm);

    void SetAll(bool on);

    // bool GetBone(int boneIdx) const {}

    void SetBone(int boneIdx, bool on);

    const BoneMask &operator&=(const BoneMask &m);

    const BoneMask &operator|=(const BoneMask &m);

    const BoneMask &operator^=(const BoneMask &m);

    BoneMask operator&(const BoneMask &m) const;

    BoneMask operator|(const BoneMask &m) const;

    BoneMask operator^(const BoneMask &m) const;

    BoneMask operator~() const;

    bool operator==(const BoneMask &m) const;

    // Members
    unsigned int mMask[8]; // offset 0x0, size 0x20
};

}; // namespace EAGL4Anim

#endif
