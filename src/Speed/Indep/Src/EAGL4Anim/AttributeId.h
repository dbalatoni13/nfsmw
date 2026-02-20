#ifndef EAGL4ANIM_ATTRIBUTEID_H
#define EAGL4ANIM_ATTRIBUTEID_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace EAGL4Anim {

// total size: 0x2
class AttributeId {
  public:
    enum ID {
        ID_INVALID = 0,
        ID_FPS = 1,
        ID_MAX = 2,
        ID_USERBASE = 128,
    };

    AttributeId() {}

    AttributeId(unsigned short id) {
        mId = id;
    }

    AttributeId(ID id) {}

    AttributeId(unsigned char prefix, unsigned char id) {}

    AttributeId(const AttributeId &other) {
        mId = other.mId;
    }

    ~AttributeId() {}

    ID GetId() const {
        return static_cast<ID>(mId);
    }

    AttributeId &operator=(const AttributeId &rhs) {
        mId = rhs.mId;

        return *this;
    }

    AttributeId &operator=(const ID &rhs) {
        mId = rhs;

        return *this;
    }

    bool operator>=(const AttributeId &rhs) const {
        return mId >= rhs.mId;
    }

    bool operator>(const AttributeId &rhs) const {
        return mId > rhs.mId;
    }

    bool operator<=(const AttributeId &rhs) const {
        return mId <= rhs.mId;
    }

    bool operator<(const AttributeId &rhs) const {
        return mId < rhs.mId;
    }

    bool operator==(const AttributeId &rhs) const {
        return mId == rhs.mId;
    }

    bool operator!=(const AttributeId &rhs) const {
        return mId != rhs.mId;
    }

    bool operator==(const ID &rhs) const {
        return mId == rhs;
    }

    bool operator!=(const ID &rhs) const {
        return mId != rhs;
    }

    bool operator==(unsigned int rhs) const {
        return mId == rhs;
    }

    bool operator!=(unsigned int rhs) const {
        return mId == rhs;
    }

    operator unsigned short const() {
        return mId;
    }

    operator short const() {
        return mId;
    }

    operator unsigned int const() {
        return mId;
    }

    operator int const() {
        return mId;
    }

  private:
    short unsigned int mId; // offset 0x0, size 0x2
};

}; // namespace EAGL4Anim

#endif
