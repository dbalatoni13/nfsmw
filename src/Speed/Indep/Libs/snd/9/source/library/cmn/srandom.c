#include "./sndcmn.h"

unsigned int SNDIrandseed[6];

unsigned int SNDIrandseedorig[6] = {
    0xF22D0E56,
    0x883126E9,
    0xC624DD2F,
    0x0702C49C,
    0x9E353F7D,
    0x6FDF3B64,
};

void SNDI_randomseed(unsigned int seed) {
    int i;

    for (i = 0; i <= 5; i++) {
        SNDIrandseed[i] = SNDIrandseedorig[i] + seed;
    }
}

unsigned int iSNDrandom() {
    unsigned int carry;
    unsigned int sum = SNDIrandseed[5] + SNDIrandseed[4];

    carry = sum < SNDIrandseed[5] || sum < SNDIrandseed[4];
    SNDIrandseed[4] = sum;
    sum = sum + SNDIrandseed[3] + carry;
    carry = sum < SNDIrandseed[3];
    SNDIrandseed[3] = sum;
    sum = sum + SNDIrandseed[2] + carry;
    carry = sum < SNDIrandseed[2];
    SNDIrandseed[2] = sum;
    sum = sum + SNDIrandseed[1] + carry;
    carry = sum < SNDIrandseed[1];
    SNDIrandseed[1] = sum;
    sum = sum + SNDIrandseed[0] + carry;
    SNDIrandseed[0] = sum;

    if (++SNDIrandseed[5] != 0) {
        return sum;
    }
    if (++SNDIrandseed[4] != 0) {
        return sum;
    }
    if (++SNDIrandseed[3] != 0) {
        return sum;
    }
    if (++SNDIrandseed[2] != 0) {
        return sum;
    }
    if (++SNDIrandseed[1] != 0) {
        return sum;
    }

    unsigned int result = SNDIrandseed[0] + 1;

    SNDIrandseed[0] = result;
    return result;
}
