#ifndef ATTRIBSYS_ATTRIB_HASH_H
#define ATTRIBSYS_ATTRIB_HASH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

namespace Attrib {

unsigned long long StringHash64(const char *k);
unsigned int StringHash32(const char *k);

class StringKey {
  public:
    StringKey() {
        mHash64 = 0;
        mHash32 = 0;
        mString = "";
    }

    StringKey(const char *str) {
        mHash64 = StringHash64(str);
        mHash32 = StringHash32(str);
        mString = str;
    }

    bool operator==(const StringKey &rhs) const {
        return mHash64 == rhs.mHash64;
    }

    operator long long() const {
        return this->mHash64;
    }

    operator unsigned int() const {
        return mHash32;
    }

    unsigned int GetHash32() const {
        return mHash32;
    }

  private:
    // total size: 0x10
    unsigned long long mHash64; // offset 0x0, size 0x8
    unsigned int mHash32;       // offset 0x8, size 0x4
    const char *mString;        // offset 0xC, size 0x4
};

// total size: 0x4
class StringKeyPtr {
    static const StringKey gDefault;

    const StringKey *mKey; // offset 0x0, size 0x4
};

} // namespace Attrib

#endif
