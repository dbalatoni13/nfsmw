#include "StringHash.h"

// Bob Jenkins lookup3-style hash (EA variant), Decl: stringhash.cpp: 69
static unsigned int hash32(const unsigned char *k, unsigned int length, unsigned int initval) {
    unsigned int a, b, c;
    unsigned int remaining = length;

    a = b = 0x9e3779b9;
    c = initval;

    while (remaining > 11) {
        a += k[0] + (k[1] << 8) + (k[2] << 16) + (k[3] << 24);
        b += k[4] + (k[5] << 8) + (k[6] << 16) + (k[7] << 24);
        c += k[8] + (k[9] << 8) + (k[10] << 16) + (k[11] << 24);

        a -= b; a -= c; a ^= (c >> 13);
        b -= c; b -= a; b ^= (a << 8);
        c -= a; c -= b; c ^= (b >> 13);
        a -= b; a -= c; a ^= (c >> 12);
        b -= c; b -= a; b ^= (a << 16);
        c -= a; c -= b; c ^= (b >> 5);
        a -= b; a -= c; a ^= (c >> 3);
        b -= c; b -= a; b ^= (a << 10);
        c -= a; c -= b; c ^= (b >> 15);

        remaining -= 12;
        k += 12;
    }

    c += length;

    switch (remaining) {
    case 11:
        c += k[10] << 24;
    case 10:
        c += k[9] << 16;
    case 9:
        c += k[8] << 8;
    case 8:
        b += k[7] << 24;
    case 7:
        b += k[6] << 16;
    case 6:
        b += k[5] << 8;
    case 5:
        b += k[4];
    case 4:
        a += k[3] << 24;
    case 3:
        a += k[2] << 16;
    case 2:
        a += k[1] << 8;
    case 1:
        a += k[0];
    }

    a -= b; a -= c; a ^= (c >> 13);
    b -= c; b -= a; b ^= (a << 8);
    c -= a; c -= b; c ^= (b >> 13);
    a -= b; a -= c; a ^= (c >> 12);
    b -= c; b -= a; b ^= (a << 16);
    c -= a; c -= b; c ^= (b >> 5);
    a -= b; a -= c; a ^= (c >> 3);
    b -= c; b -= a; b ^= (a << 10);
    c -= a; c -= b; c ^= (b >> 15);

    return c;
}

// Decl: stringhash.cpp: 126
unsigned int stringhash32(const char *string) {
    if (string != 0 && string[0] != 0) {
        return hash32(reinterpret_cast<const unsigned char *>(string), strlen(string), 0xABCDEF00);
    }
    return 0;
}
