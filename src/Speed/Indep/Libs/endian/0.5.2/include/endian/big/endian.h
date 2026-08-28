#ifndef BIG_ENDIAN_H
#define BIG_ENDIAN_H

#include "types.h"

namespace EA {

namespace Endian {

// Decl: 174
// uint32_t make_id32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {}

// Decl: 253
inline void little_put(void *dst, unsigned int data, int n) {
    switch (n) {
        case 2:
            static_cast<unsigned char *>(dst)[0] = static_cast<unsigned char>(data);
            static_cast<unsigned char *>(dst)[1] = static_cast<unsigned char>(data >> 8);
            return;

        case 3:
            static_cast<unsigned char *>(dst)[0] = static_cast<unsigned char>(data);
            static_cast<unsigned char *>(dst)[1] = static_cast<unsigned char>(data >> 8);
            static_cast<unsigned char *>(dst)[2] = static_cast<unsigned char>(data >> 16);
            return;

        case 4:
            static_cast<unsigned char *>(dst)[0] = static_cast<unsigned char>(data);
            static_cast<unsigned char *>(dst)[1] = static_cast<unsigned char>(data >> 8);
            static_cast<unsigned char *>(dst)[2] = static_cast<unsigned char>(data >> 16);
            static_cast<unsigned char *>(dst)[3] = static_cast<unsigned char>(data >> 24);
            return;
    }
}

// Decl: 308
inline uint32 little_get(const void *src, int32_t n) {
    switch (n) {
        case 2:
            return (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[1]) << 8) | static_cast<const unsigned char *>(src)[0];

        case 3:
            return (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[2]) << 16) |
                   (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[1]) << 8) | static_cast<const unsigned char *>(src)[0];

        case 4:
            return (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[3]) << 24) |
                   (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[2]) << 16) |
                   (static_cast<unsigned int>(static_cast<const unsigned char *>(src)[1]) << 8) | static_cast<const unsigned char *>(src)[0];

        default:
            return 0;
    }
}

}; // namespace Endian

} // namespace EA

#endif
