#include <cstring>

#include "../AttribHash.h"
#include <types.h>

#define mix64(a, b, c)                                                                                                                               \
    {                                                                                                                                                \
        a = a - b;                                                                                                                                   \
        a = a - c;                                                                                                                                   \
        a = a ^ (c >> 43);                                                                                                                           \
        b = b - c;                                                                                                                                   \
        b = b - a;                                                                                                                                   \
        b = b ^ (a << 9);                                                                                                                            \
        c = c - a;                                                                                                                                   \
        c = c - b;                                                                                                                                   \
        c = c ^ (b >> 8);                                                                                                                            \
        a = a - b;                                                                                                                                   \
        a = a - c;                                                                                                                                   \
        a = a ^ (c >> 38);                                                                                                                           \
        b = b - c;                                                                                                                                   \
        b = b - a;                                                                                                                                   \
        b = b ^ (a << 23);                                                                                                                           \
        c = c - a;                                                                                                                                   \
        c = c - b;                                                                                                                                   \
        c = c ^ (b >> 5);                                                                                                                            \
        a = a - b;                                                                                                                                   \
        a = a - c;                                                                                                                                   \
        a = a ^ (c >> 35);                                                                                                                           \
        b = b - c;                                                                                                                                   \
        b = b - a;                                                                                                                                   \
        b = b ^ (a << 49);                                                                                                                           \
        c = c - a;                                                                                                                                   \
        c = c - b;                                                                                                                                   \
        c = c ^ (b >> 11);                                                                                                                           \
        a = a - b;                                                                                                                                   \
        a = a - c;                                                                                                                                   \
        a = a ^ (c >> 12);                                                                                                                           \
        b = b - c;                                                                                                                                   \
        b = b - a;                                                                                                                                   \
        b = b ^ (a << 18);                                                                                                                           \
        c = c - a;                                                                                                                                   \
        c = c - b;                                                                                                                                   \
        c = c ^ (b >> 22);                                                                                                                           \
    }

namespace Attrib {

static uint64_t hash64(const uint8_t *k, unsigned int length, uint64_t initval) {
    unsigned int len = length;
    uint64_t a = initval;
    uint64_t b = initval;
    uint64_t c = 0x9e3779b97f4a7c13ULL;
    while (len > 23) {
        len -= 24;
        a += (uint64_t)k[0] + ((uint64_t)k[1] << 8) + ((uint64_t)k[2] << 16) + ((uint64_t)k[3] << 24) + ((uint64_t)k[4] << 32) +
             ((uint64_t)k[5] << 40) + ((uint64_t)k[6] << 48) + ((uint64_t)k[7] << 56);

        b += (uint64_t)k[8] + ((uint64_t)k[9] << 8) + ((uint64_t)k[10] << 16) + ((uint64_t)k[11] << 24) + ((uint64_t)k[12] << 32) +
             ((uint64_t)k[13] << 40) + ((uint64_t)k[14] << 48) + ((uint64_t)k[15] << 56);

        c += (uint64_t)k[16] + ((uint64_t)k[17] << 8) + ((uint64_t)k[18] << 16) + ((uint64_t)k[19] << 24) + ((uint64_t)k[20] << 32) +
             ((uint64_t)k[21] << 40) + ((uint64_t)k[22] << 48) + ((uint64_t)k[23] << 56);

        mix64(a, b, c);
    }
    c += length;
    switch (len) {
        case 23:
            c += (uint64_t)k[22] << 56;
        case 22:
            c += (uint64_t)k[21] << 48;
        case 21:
            c += (uint64_t)k[20] << 40;
        case 20:
            c += (uint64_t)k[19] << 32;
        case 19:
            c += (uint64_t)k[18] << 24;
        case 18:
            c += (uint64_t)k[17] << 16;
        case 17:
            c += (uint64_t)k[16] << 8;
        case 16:
            b += (uint64_t)k[15] << 56;
        case 15:
            b += (uint64_t)k[14] << 48;
        case 14:
            b += (uint64_t)k[13] << 40;
        case 13:
            b += (uint64_t)k[12] << 32;
        case 12:
            b += (uint64_t)k[11] << 24;
        case 11:
            b += (uint64_t)k[10] << 16;
        case 10:
            b += (uint64_t)k[9] << 8;
        case 9:
            b += (uint64_t)k[8];
        case 8:
            a += (uint64_t)k[7] << 56;
        case 7:
            a += (uint64_t)k[6] << 48;
        case 6:
            a += (uint64_t)k[5] << 40;
        case 5:
            a += (uint64_t)k[4] << 32;
        case 4:
            a += (uint64_t)k[3] << 24;
        case 3:
            a += (uint64_t)k[2] << 16;
        case 2:
            a += (uint64_t)k[1] << 8;
        case 1:
            a += (uint64_t)k[0];
    }

    mix64(a, b, c);

    return c;
};

unsigned long long StringHash64(const char *k) {
    if (k && *k != '\0') {
        return hash64((const unsigned char *)k, std::strlen(k), 0xabcdef0011223344ULL);
    } else {
        return 0;
    }
}

} // namespace Attrib
