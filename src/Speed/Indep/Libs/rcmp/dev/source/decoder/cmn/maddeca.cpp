#include "types.h"
#include "../../../../../../../../../include/dol2asm.h"

extern unsigned int madshiftreg __attribute__((section(".sbss")));
extern const unsigned short *maddataptr __attribute__((section(".sbss")));
extern int madbitcount __attribute__((section(".sbss")));
extern int madvlctbl1[0x200];
extern int madvlctbl2[0x100];
extern int madvlctbl3[0x100];
extern int madquant[0x40];
extern int idctinput[0x40];

static inline unsigned int geti(const void *src, int bytes) {
    if (bytes == 2) {
        return (static_cast<const unsigned char *>(src)[1] << 8) |
               static_cast<const unsigned char *>(src)[0];
    }
    return (static_cast<const unsigned char *>(src)[3] << 24) |
           (static_cast<const unsigned char *>(src)[2] << 16) |
           (static_cast<const unsigned char *>(src)[1] << 8) |
           static_cast<const unsigned char *>(src)[0];
}

static const int zigzag[0x40] = {
    0, 8, 1, 2, 9, 0x10, 0x18, 0x11, 0xa, 3, 4, 0xb, 0x12, 0x19, 0x20, 0x28,
    0x21, 0x1a, 0x13, 0xc, 5, 6, 0xd, 0x14, 0x1b, 0x22, 0x29, 0x30, 0x38, 0x31,
    0x2a, 0x23, 0x1c, 0x15, 0xe, 7, 0xf, 0x16, 0x1d, 0x24, 0x2b, 0x32, 0x39,
    0x3a, 0x33, 0x2c, 0x25, 0x1e, 0x17, 0x1f, 0x26, 0x2d, 0x34, 0x3b, 0x3c,
    0x35, 0x2e, 0x27, 0x2f, 0x36, 0x3d, 0x3e, 0x37, 0x3f,
};

static void discardbits(int bits) {
    madshiftreg = madshiftreg << bits;
    madbitcount = madbitcount - bits;
    if (madbitcount <= 0xf) {
        madshiftreg = madshiftreg | (geti(maddataptr, 2) << (0x10 - madbitcount));
        madbitcount = madbitcount + 0x10;
        maddataptr += 1;
    }
}

extern "C" int madvlcdecode() {
    int *entry;
    int val;
    int bits;
    int index;
    int i;

    idctinput[0] = (static_cast<int>(madshiftreg) >> 0x18) * madquant[0];
    discardbits(8);
    entry = idctinput + 1;
    i = 9;
    do {
        entry[0] = 0;
        entry[1] = 0;
        entry[2] = 0;
        entry[3] = 0;
        entry[4] = 0;
        entry[5] = 0;
        entry[6] = 0;
        entry += 7;
    } while (--i != 0);

    index = 1;
    for (;;) {
        entry = &madvlctbl1[(madshiftreg >> 0x15) & 0x1ff];
        val = *entry;
        bits = val & 0xff;
        if (bits > 9) {
            if (!(val & 0x20)) {
                if (!(val & 0x10)) {
                    discardbits(9);
                    val = madvlctbl2[(static_cast<unsigned char>(madshiftreg))];
                } else {
                    discardbits(6);
                    val = madvlctbl3[(static_cast<unsigned char>(madshiftreg))];
                }
                bits = val & 0xff;
            } else if (!(val & 0x10)) {
                discardbits(6);
                val = madshiftreg;
                bits = 0x10;
            } else {
                discardbits(2);
                return index;
            }
        }

        discardbits(bits);
        index += (val >> 0x10) & 0x3f;
        idctinput[zigzag[index]] = (val >> 0x16) * madquant[zigzag[index]];
        index++;
    }
}
