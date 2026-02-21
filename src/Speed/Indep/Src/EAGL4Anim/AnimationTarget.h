#ifndef EAGL4ANIM_ANIMATIONTARGET_H
#define EAGL4ANIM_ANIMATIONTARGET_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace EAGL4Anim {

typedef unsigned short CheckSum;

// total size: 0x4
struct AnimationTarget {
  public:
    AnimationTarget() {}

    CheckSum GetCheckSum() const {
        return mCheckSum;
    }

    void SetCheckSum(CheckSum cs) {
        mCheckSum = cs;
    }

    unsigned short GetNumDofs() const {
        return mNumDofs;
    }

    void SetNumDofs(unsigned short n) {
        mNumDofs = n;
    }

  protected:
    CheckSum mCheckSum;          // offset 0x0, size 0x2
    short unsigned int mNumDofs; // offset 0x2, size 0x2
};

}; // namespace EAGL4Anim

#endif
