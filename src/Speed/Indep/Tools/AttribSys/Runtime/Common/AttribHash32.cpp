#include <cstring>

#include "../AttribHash.h"
#include <types.h>

namespace Attrib {

// TODO move?
const StringKey StringKeyPtr::gDefault = StringKey();

static unsigned int hash32(const uint8_t *k, unsigned int length, unsigned int initval) {
    unsigned int len = length;
    unsigned int b = 0x9e3779b9;
    unsigned int a = 0x9e3779b9;
    unsigned int c = initval;

    while (len > 11) {
        len -= 12;
        a += k[0] + ((unsigned int)k[1] << 8) + ((unsigned int)k[2] << 16) + ((unsigned int)k[3] << 24);
        b += k[4] + ((unsigned int)k[5] << 8) + ((unsigned int)k[6] << 16) + ((unsigned int)k[7] << 24);
        c += k[8] + ((unsigned int)k[9] << 8) + ((unsigned int)k[10] << 16) + ((unsigned int)k[11] << 24);

        a -= b;
        a -= c;
        a ^= c >> 13;

        b -= c;
        b -= a;
        b ^= a << 8;

        c -= a;
        c -= b;
        c ^= b >> 13;

        a -= b;
        a -= c;
        a ^= c >> 12;

        b -= c;
        b -= a;
        b ^= a << 16;

        c -= a;
        c -= b;
        c ^= b >> 5;

        a -= b;
        a -= c;
        a ^= c >> 3;

        b -= c;
        b -= a;
        b ^= a << 10;

        c -= a;
        c -= b;
        c ^= b >> 15;

        k += 12;
    }
    c += length;
    switch (len) {
        case 11:
            c += (unsigned int)k[10] << 24;
            // all of these fall through
        case 10:
            c += (unsigned int)k[9] << 16;
        case 9:
            c += (unsigned int)k[8] << 8;
        case 8:
            b += (unsigned int)k[7] << 24;
        case 7:
            b += (unsigned int)k[6] << 16;
        case 6:
            b += (unsigned int)k[5] << 8;
        case 5:
            b += (unsigned int)k[4];
        case 4:
            a += (unsigned int)k[3] << 24;
        case 3:
            a += (unsigned int)k[2] << 16;
        case 2:
            a += (unsigned int)k[1] << 8;
        case 1:
            a += (unsigned int)k[0];
    }
    a -= b;
    a -= c;
    a ^= c >> 13;

    b -= c;
    b -= a;
    b ^= a << 8;

    c -= a;
    c -= b;
    c ^= b >> 13;

    a -= b;
    a -= c;
    a ^= c >> 12;

    b -= c;
    b -= a;
    b ^= a << 16;

    c -= a;
    c -= b;
    c ^= b >> 5;

    a -= b;
    a -= c;
    a ^= c >> 3;

    b -= c;
    b -= a;
    b ^= a << 10;

    c -= a;
    c -= b;
    c ^= b >> 15;
    return c;
};

unsigned int StringHash32(const char *k) {
    if (k && *k != '\0') {
        return hash32((const unsigned char *)k, strlen(k), 0xabcdef00);
    } else {
        return 0;
    }
}

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

        a -= b;
        a -= c;
        a ^= c >> 43;

        b -= c;
        b -= a;
        b ^= a << 9;

        c -= a;
        c -= b;
        c ^= b >> 8;

        a -= b;
        a -= c;
        a ^= c >> 38;

        b -= c;
        b -= a;
        b ^= a << 23;

        c -= a;
        c -= b;
        c ^= b >> 5;

        a -= b;
        a -= c;
        a ^= c >> 35;

        b -= c;
        b -= a;
        b ^= a << 49;

        c -= a;
        c -= b;
        c ^= b >> 11;

        a -= b;
        a -= c;
        a ^= c >> 12;

        b -= c;
        b -= a;
        b ^= a << 18;

        c -= a;
        c -= b;
        c ^= b >> 22;
        k += 24;
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

    a -= b;
    a -= c;
    a ^= c >> 43;

    b -= c;
    b -= a;
    b ^= a << 9;

    c -= a;
    c -= b;
    c ^= b >> 8;

    a -= b;
    a -= c;
    a ^= c >> 38;

    b -= c;
    b -= a;
    b ^= a << 23;

    c -= a;
    c -= b;
    c ^= b >> 5;

    a -= b;
    a -= c;
    a ^= c >> 35;

    b -= c;
    b -= a;
    b ^= a << 49;

    c -= a;
    c -= b;
    c ^= b >> 11;

    a -= b;
    a -= c;
    a ^= c >> 12;

    b -= c;
    b -= a;
    b ^= a << 18;

    c -= a;
    c -= b;
    c ^= b >> 22;
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
