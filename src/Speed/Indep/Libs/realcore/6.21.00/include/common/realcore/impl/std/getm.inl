#ifndef REALCORE_IMPL_STD_GETM_INL
#define REALCORE_IMPL_STD_GETM_INL

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// Native-order and little-endian readers; recovered from the RCMP source owner.
static inline unsigned int getm(const void *src, int bytes) {
    switch (bytes) {
    case 1:
        return *static_cast<const unsigned char *>(src);
    case 2:
        return *static_cast<const unsigned short *>(src);
    case 3:
        return (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[0]) << 16) |
               (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[1]) << 8) |
               static_cast<unsigned int>(static_cast<const unsigned char *>(src)[2]);
    default:
        return *static_cast<const unsigned int *>(src);
    }
}

static inline unsigned int geti(const void *src, int bytes) {
    if (bytes == 1) {
        return *static_cast<const unsigned char *>(src);
    }
    if (bytes == 3) {
        return static_cast<unsigned int>(static_cast<const unsigned char *>(src)[0]) |
               (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[1]) << 8) |
               (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[2]) << 16);
    }
    if (bytes == 2) {
        return static_cast<unsigned int>(static_cast<const unsigned char *>(src)[0]) | (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[1]) << 8);
    }
    src = static_cast<const unsigned char *>(src) + 4;
    return (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[-1]) << 24) |
           (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[-2]) << 16) |
           (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[-3]) << 8) |
           static_cast<unsigned int>(static_cast<const unsigned char *>(src)[-4]);
}

#endif
