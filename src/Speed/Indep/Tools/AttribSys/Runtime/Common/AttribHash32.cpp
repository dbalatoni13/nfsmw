#include <cstring>

namespace Attrib {

static unsigned int hash32(const unsigned char *k, unsigned int length, unsigned int initval) {
    unsigned int len = length;
    unsigned int b = 0x9e3779b9;
    unsigned int a = 0x9e3779b9;
    unsigned int c = initval;

    while (len > 11) {
        len -= 12;
        a += k[0] + k[1] * 0x100 + k[2] * 0x10000 + k[3] * 0x1000000;
        b += k[4] + k[5] * 0x100 + k[6] * 0x10000 + k[7] * 0x1000000;
        c += k[8] + k[9] * 0x100 + k[10] * 0x10000 + k[11] * 0x1000000;

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
            c += k[10] * 0x1000000;
            // all of these fall through
        case 10:
            c += k[9] * 0x10000;
        case 9:
            c += k[8] * 0x100;
        case 8:
            b += k[7] * 0x1000000;
        case 7:
            b += k[6] * 0x10000;
        case 6:
            b += k[5] * 0x100;
        case 5:
            b += k[4];
        case 4:
            a += k[3] * 0x1000000;
        case 3:
            a += k[2] * 0x10000;
        case 2:
            a += k[1] * 0x100;
        case 1:
            a += k[0];
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
        return hash32((const unsigned char *)k, std::strlen(k), 0xabcdef00);
    } else {
        return 0;
    }
}

static unsigned long long hash64(const unsigned char *k, unsigned int length, unsigned long long initval) {};

unsigned long long StringHash64(const char *k) {
    if (k && *k != '\0') {
        return hash64((const unsigned char *)k, std::strlen(k), 0xabcdef0011223344ULL);
    } else {
        return 0;
    }
}

} // namespace Attrib
