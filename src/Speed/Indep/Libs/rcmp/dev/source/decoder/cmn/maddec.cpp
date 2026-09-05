#include "types.h"
#include "../../../../../../../../../include/dol2asm.h"

unsigned int madshiftreg __attribute__((section(".sbss")));
const unsigned short *maddataptr __attribute__((section(".sbss")));
int madbitcount __attribute__((section(".sbss")));
static int motionframe __attribute__((section(".sbss")));
int madquant[0x40];
int madvlctbl1[0x200];
int madvlctbl2[0x100];
int madvlctbl3[0x100];
int madvlctbl4[0x40];
extern int idctinput[0x40];
extern int idctprescale[0x40];
extern "C" void idctcompute(int *, int);
extern int madvlcdecode();

static unsigned char clipbiastbl[0x200];
static int luma[0x100];
static int chroma[0x80];
static int initflag __attribute__((section(".sbss")));

static const int quanttbl[0x40] = {
    8, 0x10, 0x13, 0x16, 0x1a, 0x1b, 0x1d, 0x22,
    0x10, 0x10, 0x16, 0x18, 0x1b, 0x1d, 0x22, 0x25,
    0x13, 0x16, 0x1a, 0x1b, 0x1d, 0x22, 0x22, 0x26,
    0x16, 0x16, 0x1a, 0x1b, 0x1d, 0x22, 0x25, 0x28,
    0x16, 0x1a, 0x1b, 0x20, 0x23, 0x28, 0x30, 0x1a,
    0x1b, 0x1d, 0x20, 0x23, 0x28, 0x30, 0x3a, 0x1a,
    0x1b, 0x1d, 0x22, 0x26, 0x2e, 0x38, 0x45, 0x1b,
    0x1d, 0x23, 0x26, 0x2e, 0x38, 0x45, 0x53,
};

extern const int encodetbl1[];
extern const int encodetbl2[];

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

static void discardbits(int bits) {
    madshiftreg = madshiftreg << bits;
    madbitcount = madbitcount - bits;
    if (madbitcount <= 0xf) {
        madshiftreg = madshiftreg | (geti(maddataptr, 2) << (0x10 - madbitcount));
        madbitcount = madbitcount + 0x10;
        maddataptr += 1;
    }
}

static int fixedmul(int a, int b) {
    long long product;

    product = static_cast<long long>(a) * b + 0x8000;
    return static_cast<int>(product >> 16);
}

static void madinit() {
    int i;
    int j;
    int *entry;

    for (i = -0x100; i < 0x100; i++) {
        j = i;
        if (j < -0x80) {
            j = -0x80;
        } else if (j > 0x7f) {
            j = 0x7f;
        }
        clipbiastbl[i & 0x1ff] = static_cast<unsigned char>(j - 0x80);
    }
    madvlctbl1[0] = 0xf;
    for (i = 1; i < 8; i++) {
        madvlctbl1[i] = 0x1f;
    }
    for (i = 0xf; i > 7; i--) {
        madvlctbl1[i] = 0x2f;
    }
    for (i = 0x17f; i > 0xff; i--) {
        madvlctbl1[i] = 0x3f;
    }

    entry = const_cast<int *>(encodetbl1) + 4;
    for (i = 0x5e; i > 0; i--, entry += 4) {
        int value;
        int count;

        if (entry[3] & 0xfc00) {
            value = (entry[2] << 0x16) | ((entry[2] << 6) & 0x3f0000) | entry[1];
            count = 1 << (9 - entry[0]);
            for (j = 0; j < count; j++) {
                madvlctbl1[(entry[3] >> 7) + j] = value;
            }
        } else {
            value = (entry[2] << 0x16) | ((entry[2] << 6) & 0x3f0000) | (entry[1] - 6);
            count = 1 << (0xe - entry[0]);
            for (j = 0; j < count; j++) {
                madvlctbl3[(entry[3] >> 2) + j] = value;
            }
        }
    }

    entry = const_cast<int *>(encodetbl2);
    for (i = 0x80; i > 0; i--, entry += 4) {
        int value;
        int count;

        if (!(entry[3] & 0x8000)) {
            value = (entry[2] << 0x16) | ((entry[2] << 6) & 0x3f0000) | (entry[0] - 1);
            count = 1 << (0x11 - (entry[0] + 8));
            for (j = 0; j < count; j++) {
                madvlctbl2[(entry[3] >> 7) + j] = value;
            }
        } else {
            value = (entry[2] << 0x16) | ((entry[2] << 6) & 0x3f0000) | (entry[0] + 2);
            count = 1 << (0xe - (entry[0] + 8));
            for (j = 0; j < count; j++) {
                madvlctbl3[(entry[3] >> 0xa) + j] = value;
            }
        }
    }
    for (i = 0; i < 0x20; i++) {
        madvlctbl4[i] = 1;
    }
    for (i = 0; i < 0x10; i++) {
        madvlctbl4[0x20 + i] = 0x400006 + i * 0x400000;
        madvlctbl4[0x30 + i] = static_cast<int>(0xfc000006 + i * 0x400000);
    }
    initflag = 1;
}

static int getdelta() {
    int value;

    value = madvlctbl4[(madshiftreg >> 0x18) & 0x3f];
    discardbits(value & 0xff);
    return value >> 0x16;
}

static void dcblock(int *dest, int stride) {
    int i;

    i = 8;
    do {
        dest[0] = idctinput[0];
        dest[1] = idctinput[0];
        dest[2] = idctinput[0];
        dest[3] = idctinput[0];
        dest[4] = idctinput[0];
        dest[5] = idctinput[0];
        dest[6] = idctinput[0];
        dest[7] = idctinput[0];
        dest += stride;
    } while (--i != 0);
}

static void getluma(const unsigned char *src, int stride, int *dest, int correction) {
    int i;

    i = 8;
    do {
        dest[0] = (src[0] + correction) << 16;
        dest[1] = (src[1] + correction) << 16;
        dest[2] = (src[2] + correction) << 16;
        dest[3] = (src[3] + correction) << 16;
        dest[4] = (src[4] + correction) << 16;
        dest[5] = (src[5] + correction) << 16;
        dest[6] = (src[6] + correction) << 16;
        dest[7] = (src[7] + correction) << 16;
        src += stride;
        dest += 16;
    } while (--i != 0);
}

static void getchroma(const unsigned char *src, int stride, int *dest, int correction) {
    int i;

    i = 8;
    do {
        dest[0] = (src[0] + correction) << 16;
        dest[1] = (src[1] + correction) << 16;
        dest[2] = (src[2] + correction) << 16;
        dest[3] = (src[3] + correction) << 16;
        dest[4] = (src[4] + correction) << 16;
        dest[5] = (src[5] + correction) << 16;
        dest[6] = (src[6] + correction) << 16;
        dest[7] = (src[7] + correction) << 16;
        src += stride;
        dest += 8;
    } while (--i != 0);
}

static void setluma(const int *src, unsigned char *dest, int stride) {
    int i;

    i = 0x10;
    do {
        dest[0] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[0] & 0x1ff];
        dest[1] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[2] & 0x1ff];
        dest[2] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[4] & 0x1ff];
        dest[3] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[6] & 0x1ff];
        dest[4] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[8] & 0x1ff];
        dest[5] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[10] & 0x1ff];
        dest[6] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[12] & 0x1ff];
        dest[7] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[14] & 0x1ff];
        dest[8] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[16] & 0x1ff];
        dest[9] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[18] & 0x1ff];
        dest[10] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[20] & 0x1ff];
        dest[11] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[22] & 0x1ff];
        dest[12] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[24] & 0x1ff];
        dest[13] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[26] & 0x1ff];
        dest[14] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[28] & 0x1ff];
        dest[15] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[30] & 0x1ff];
        src += 16;
        dest += stride;
    } while (--i != 0);
}

static void setchroma(const int *src, unsigned char *dest, int stride) {
    int i;

    i = 8;
    do {
        dest[0] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[0] & 0x1ff];
        dest[1] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[2] & 0x1ff];
        dest[2] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[4] & 0x1ff];
        dest[3] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[6] & 0x1ff];
        dest[4] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[8] & 0x1ff];
        dest[5] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[10] & 0x1ff];
        dest[6] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[12] & 0x1ff];
        dest[7] = clipbiastbl[reinterpret_cast<volatile const short *>(src)[14] & 0x1ff];
        src += 8;
        dest += stride;
    } while (--i != 0);
}

void MAD_initdecode(const unsigned short *src, int motion, int quality) {
    int i;

    if (initflag == 0) {
        madinit();
    }
    madbitcount = 0x20;
    madshiftreg = (src[0] << 0x18) | (src[1] << 0x10) | (src[2] << 8) | src[3];
    maddataptr = reinterpret_cast<const unsigned short *>(reinterpret_cast<const unsigned char *>(src) + 4);
    motionframe = motion;
    madquant[0] = fixedmul(quanttbl[0] << 16, idctprescale[0]);
    for (i = 1; i < 0x40; i++) {
        madquant[i] = fixedmul((quality * quanttbl[i]) << 0xd, idctprescale[i]);
    }
}

void MAD_decodemacroblock(const unsigned char *src_y, const unsigned char *src_cb,
                          const unsigned char *src_cr, unsigned char *dest_y,
                          unsigned char *dest_cb, unsigned char *dest_cr, int width) {
    unsigned int mode;
    int chroma_width;
    int dx;
    int dy;

    chroma_width = width >> 1;
    if (motionframe == 0) {
        mode = 0;
    } else if (!(madshiftreg & 0xc0000000)) {
        mode = 0;
        discardbits(2);
    } else {
        if (static_cast<int>(madshiftreg) < 0) {
            mode = 0x3ff;
            discardbits(1);
        } else {
            mode = madshiftreg >> 0x18;
            discardbits(8);
        }
        dx = getdelta();
        dy = getdelta();
        src_y += dy * width + dx;
        src_cb += (dy >> 1) * chroma_width + (dx >> 1);
        src_cr += (dy >> 1) * chroma_width + (dx >> 1);
    }
    if (!(mode & 1)) {
        if (madvlcdecode() == 1) {
            dcblock(luma, 0x10);
        } else {
            idctcompute(luma, 0x10);
        }
    } else {
        getluma(src_y, width, luma, getdelta() * 2 - 0x80);
    }
    if (!(mode & 2)) {
        if (madvlcdecode() == 1) {
            dcblock(luma + 0x20, 0x10);
        } else {
            idctcompute(luma + 0x20, 0x10);
        }
    } else {
        getluma(src_y + 8, width, luma + 0x20, getdelta() * 2 - 0x80);
    }
    if (!(mode & 4)) {
        if (madvlcdecode() == 1) {
            dcblock(luma + 0x200, 0x10);
        } else {
            idctcompute(luma + 0x200, 0x10);
        }
    } else {
        getluma(src_y + width * 8, width, luma + 0x200, getdelta() * 2 - 0x80);
    }
    if (!(mode & 8)) {
        if (madvlcdecode() == 1) {
            dcblock(luma + 0x220, 0x10);
        } else {
            idctcompute(luma + 0x220, 0x10);
        }
    } else {
        getluma(src_y + width * 8 + 8, width, luma + 0x220, getdelta() * 2 - 0x80);
    }
    if (!(mode & 0x10)) {
        if (madvlcdecode() == 1) {
            dcblock(chroma, 8);
        } else {
            idctcompute(chroma, 8);
        }
    } else {
        getchroma(src_cb, chroma_width, chroma, getdelta() * 2 - 0x80);
    }
    if (!(mode & 0x20)) {
        if (madvlcdecode() == 1) {
            dcblock(chroma + 0x100, 8);
        } else {
            idctcompute(chroma + 0x100, 8);
        }
    } else {
        getchroma(src_cr, chroma_width, chroma + 0x100, getdelta() * 2 - 0x80);
    }
    setluma(luma, dest_y, width);
    setchroma(chroma, dest_cb, chroma_width);
    setchroma(chroma + 0x100, dest_cr, chroma_width);
}
