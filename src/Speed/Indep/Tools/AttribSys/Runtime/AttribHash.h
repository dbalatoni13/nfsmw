#ifndef ATTRIBSYS_ATTRIB_HASH_H
#define ATTRIBSYS_ATTRIB_HASH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Attrib {

class StringKey {
  public:
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

} // namespace Attrib

#endif
