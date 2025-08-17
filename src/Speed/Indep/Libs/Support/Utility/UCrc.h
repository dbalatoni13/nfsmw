#ifndef SUPPORT_UCRC_H
#define SUPPORT_UCRC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class UCrc32 {
    // total size: 0x4
    unsigned int mCRC; // offset 0x0, size 0x4

  public:
    static const UCrc32 kNull;

    // UCrc32() {}

    // UCrc32(const char *name) {}

    // UCrc32(const void *data, unsigned int datalen) {}

    UCrc32(const UCrc32 &from) {
        this->mCRC = from.mCRC;
    }

    // UCrc32(unsigned int crc) {}

    // const struct UCrc32 &operator=(const struct UCrc32 &from) {}

    // const struct UCrc32 &operator=(const char *from) {
    //     UCrc32 tmp;
    // }

    // bool operator<(const struct UCrc32 &from) const {}

    // unsigned int GetValue() const {}

    // void SetValue(unsigned int crc) {}
};

// inline bool operator==(const UCrc32 &a, const UCrc32 &b) {}

// inline bool operator!=(const UCrc32 &a, const UCrc32 &b) {}

// inline bool operator==(const UCrc32 &a, const char *b) {}

// inline bool operator!=(const UCrc32 &a, const char *b) {}

// inline bool operator==(const char *a, const UCrc32 &b) {}

// inline bool operator!=(const char *a, const UCrc32 &b) {}

#endif
