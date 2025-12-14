#ifndef SUPPORT_UCRC_H
#define SUPPORT_UCRC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

class bHash32 {
    unsigned int mCRC;

  public:
    bHash32() {}
    bHash32(const char *name) {}
    bHash32(const bHash32 &from) {}
    bHash32(unsigned int crc) {}

    bHash32 &operator=(bHash32 &from) {
        this->mCRC = from.mCRC;
        return *this;
    }
    bHash32 &operator=(const char *from) {}

    unsigned int GetValue() {
        return mCRC;
    }
};

class UCrc32 {
  public:
    static const UCrc32 kNull;

    UCrc32() : mCRC(0) {}

    UCrc32(const char *name) : mCRC(stringhash(name)) {}

    // UCrc32(const void *data, unsigned int datalen) {}

    UCrc32(const UCrc32 &from) : mCRC(from.mCRC) {}

    UCrc32(unsigned int crc) : mCRC(crc) {}

    UCrc32(const Attrib::StringKey &key) : mCRC(key.GetHash32()) {}

    const UCrc32 &operator=(const UCrc32 &from) {
        mCRC = from.mCRC;
        return *this;
    }

    // const  UCrc32 &operator=(const char *from) {
    //     UCrc32 tmp;
    // }

    bool operator<(const UCrc32 &from) const {
        return mCRC < from.mCRC;
    }

    unsigned int GetValue() const {
        return mCRC;
    }

    void SetValue(unsigned int crc) {
        mCRC = crc;
    }

  private:
    // total size: 0x4
    unsigned int mCRC; // offset 0x0, size 0x4
};

inline bool operator==(const UCrc32 &a, const UCrc32 &b) {
    return a.GetValue() == b.GetValue();
}

// inline bool operator!=(const UCrc32 &a, const UCrc32 &b) {}

// inline bool operator==(const UCrc32 &a, const char *b) {}

// inline bool operator!=(const UCrc32 &a, const char *b) {}

// inline bool operator==(const char *a, const UCrc32 &b) {}

// inline bool operator!=(const char *a, const UCrc32 &b) {}

#endif
