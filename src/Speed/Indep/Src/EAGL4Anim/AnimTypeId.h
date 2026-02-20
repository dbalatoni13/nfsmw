#ifndef EAGL4ANIM_ANIMTYPEID_H
#define EAGL4ANIM_ANIMTYPEID_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace EAGL4Anim {

// total size: 0x2
class AnimTypeId {
  public:
    enum Type {
        ANIM_INVALID = -1,
        ANIM_RAWPOSE = 0,
        ANIM_RAWEVENT = 1,
        ANIM_RAWLINEAR = 2,
        ANIM_CYCLE = 3,
        ANIM_EVENTBLENDER = 4,
        ANIM_GRAFT = 5,
        ANIM_POSEBLENDER = 6,
        ANIM_POSEMIRROR = 7,
        ANIM_RUNBLENDER = 8,
        ANIM_TURNBLENDER = 9,
        ANIM_DELTALERP = 10,
        ANIM_DELTAQUAT = 11,
        ANIM_KEYLERP = 12,
        ANIM_KEYQUAT = 13,
        ANIM_PHASE = 14,
        ANIM_COMPOUND = 15,
        ANIM_RAWSTATE = 16,
        ANIM_DELTAQ = 17,
        ANIM_DELTAQFAST = 18,
        ANIM_DELTASINGLEQ = 19,
        ANIM_DELTAF3 = 20,
        ANIM_DELTAF1 = 21,
        ANIM_STATELESSQ = 22,
        ANIM_STATELESSF3 = 23,
        ANIM_CSISEVENT = 24,
        ANIM_POSEANIM = 25,
        ANIM_MAX = 26,
    };

    AnimTypeId() {}

    AnimTypeId(unsigned short id) : mId(id) {}

    AnimTypeId(Type id) {}

    AnimTypeId(unsigned char prefix, unsigned char id) {}

    AnimTypeId(const AnimTypeId &other) : mId(other.mId) {}

    ~AnimTypeId() {}

    Type GetType() const {
        return static_cast<Type>(mId);
    }

    AnimTypeId &operator=(const AnimTypeId &rhs) {
        mId = rhs.mId;

        return *this;
    }

    AnimTypeId &operator=(const Type &rhs) {
        mId = rhs;

        return *this;
    }

    bool operator==(const AnimTypeId &rhs) const {
        return mId == rhs.mId;
    }

    bool operator!=(const AnimTypeId &rhs) const {
        return mId != rhs.mId;
    }

    bool operator==(const Type &rhs) const {
        return static_cast<Type>(mId) == rhs;
    }

    bool operator!=(const Type &rhs) const {
        return static_cast<Type>(mId) != rhs;
    }

    bool operator==(unsigned int rhs) const {
        return mId == rhs;
    }

    bool operator!=(unsigned int rhs) const {
        return mId == rhs;
    }

  private:
    short unsigned int mId; // offset 0x0, size 0x2
};

}; // namespace EAGL4Anim

#endif
