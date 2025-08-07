#ifndef ATTRIBSYS_ATTRIB_HASH_H
#define ATTRIBSYS_ATTRIB_HASH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Attrib {

struct StringKey {
  private:
    // total size: 0x10
    unsigned long long mHash64; // offset 0x0, size 0x8
    unsigned int mHash32;       // offset 0x8, size 0x4
    const char *mString;        // offset 0xC, size 0x4

  public:
    operator long long() const {
        return this->mHash64;
    }

    operator unsigned int() const {
        return this->mHash32;
    }
};

} // namespace Attrib

#endif
