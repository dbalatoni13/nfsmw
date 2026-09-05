#include "types.h"
#include "../../../../../../../../../include/dol2asm.h"

const unsigned short *maddataptr __attribute__((section(".sbss")));
unsigned int madshiftreg __attribute__((section(".sbss")));
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

static const int encodetbl1[0x17c] = {
    0x00000002, 0x0000FE00, 0x00000000, 0x00008000, 0x00000003, 0x00000001, 0x00000000, 0x0000C000,
    0x00000003, 0x000003FF, 0x00000000, 0x0000E000, 0x00000004, 0x00000401, 0x00000000, 0x00006000,
    0x00000004, 0x000007FF, 0x00000000, 0x00007000, 0x00000005, 0x00000002, 0x00000000, 0x00004000,
    0x00000005, 0x000003FE, 0x00000000, 0x00004800, 0x00000005, 0x00000801, 0x00000000, 0x00005000,
    0x00000005, 0x00000BFF, 0x00000000, 0x00005800, 0x00000006, 0x00000003, 0x00000000, 0x00002800,
    0x00000006, 0x000003FD, 0x00000000, 0x00002C00, 0x00000006, 0x00000C01, 0x00000000, 0x00003800,
    0x00000006, 0x00000FFF, 0x00000000, 0x00003C00, 0x00000006, 0x00001001, 0x00000000, 0x00003000,
    0x00000006, 0x000013FF, 0x00000000, 0x00003400, 0x00000007, 0x00000402, 0x00000000, 0x00001800,
    0x00000007, 0x000007FE, 0x00000000, 0x00001A00, 0x00000007, 0x00001401, 0x00000000, 0x00001C00,
    0x00000007, 0x000017FF, 0x00000000, 0x00001E00, 0x00000007, 0x00001801, 0x00000000, 0x00001400,
    0x00000007, 0x00001BFF, 0x00000000, 0x00001600, 0x00000007, 0x00001C01, 0x00000000, 0x00001000,
    0x00000007, 0x00001FFF, 0x00000000, 0x00001200, 0x00000008, 0x00000004, 0x00000000, 0x00000C00,
    0x00000008, 0x000003FC, 0x00000000, 0x00000D00, 0x00000008, 0x00000802, 0x00000000, 0x00000800,
    0x00000008, 0x00000BFE, 0x00000000, 0x00000900, 0x00000008, 0x00002001, 0x00000000, 0x00000E00,
    0x00000008, 0x000023FF, 0x00000000, 0x00000F00, 0x00000008, 0x00002401, 0x00000000, 0x00000A00,
    0x00000008, 0x000027FF, 0x00000000, 0x00000B00, 0x00000009, 0x00000005, 0x00000000, 0x00002600,
    0x00000009, 0x000003FB, 0x00000000, 0x00002680, 0x00000009, 0x00000006, 0x00000000, 0x00002100,
    0x00000009, 0x000003FA, 0x00000000, 0x00002180, 0x00000009, 0x00000403, 0x00000000, 0x00002500,
    0x00000009, 0x000007FD, 0x00000000, 0x00002580, 0x00000009, 0x00000C02, 0x00000000, 0x00002400,
    0x00000009, 0x00000FFE, 0x00000000, 0x00002480, 0x00000009, 0x00002801, 0x00000000, 0x00002700,
    0x00000009, 0x00002BFF, 0x00000000, 0x00002780, 0x00000009, 0x00002C01, 0x00000000, 0x00002300,
    0x00000009, 0x00002FFF, 0x00000000, 0x00002380, 0x00000009, 0x00003001, 0x00000000, 0x00002200,
    0x00000009, 0x000033FF, 0x00000000, 0x00002280, 0x00000009, 0x00003401, 0x00000000, 0x00002000,
    0x00000009, 0x000037FF, 0x00000000, 0x00002080, 0x0000000B, 0x00000007, 0x00000000, 0x00000280,
    0x0000000B, 0x000003F9, 0x00000000, 0x000002A0, 0x0000000B, 0x00000404, 0x00000000, 0x00000300,
    0x0000000B, 0x000007FC, 0x00000000, 0x00000320, 0x0000000B, 0x00000803, 0x00000000, 0x000002C0,
    0x0000000B, 0x00000BFD, 0x00000000, 0x000002E0, 0x0000000B, 0x00001002, 0x00000000, 0x000003C0,
    0x0000000B, 0x000013FE, 0x00000000, 0x000003E0, 0x0000000B, 0x00001402, 0x00000000, 0x00000240,
    0x0000000B, 0x000017FE, 0x00000000, 0x00000260, 0x0000000B, 0x00003801, 0x00000000, 0x00000380,
    0x0000000B, 0x00003BFF, 0x00000000, 0x000003A0, 0x0000000B, 0x00003C01, 0x00000000, 0x00000340,
    0x0000000B, 0x00003FFF, 0x00000000, 0x00000360, 0x0000000B, 0x00004001, 0x00000000, 0x00000200,
    0x0000000B, 0x000043FF, 0x00000000, 0x00000220, 0x0000000D, 0x00000008, 0x00000000, 0x000001D0,
    0x0000000D, 0x000003F8, 0x00000000, 0x000001D8, 0x0000000D, 0x00000009, 0x00000000, 0x00000180,
    0x0000000D, 0x000003F7, 0x00000000, 0x00000188, 0x0000000D, 0x0000000A, 0x00000000, 0x00000130,
    0x0000000D, 0x000003F6, 0x00000000, 0x00000138, 0x0000000D, 0x0000000B, 0x00000000, 0x00000100,
    0x0000000D, 0x000003F5, 0x00000000, 0x00000108, 0x0000000D, 0x00000405, 0x00000000, 0x000001B0,
    0x0000000D, 0x000007FB, 0x00000000, 0x000001B8, 0x0000000D, 0x00000804, 0x00000000, 0x00000140,
    0x0000000D, 0x00000BFC, 0x00000000, 0x00000148, 0x0000000D, 0x00000C03, 0x00000000, 0x000001C0,
    0x0000000D, 0x00000FFD, 0x00000000, 0x000001C8, 0x0000000D, 0x00001003, 0x00000000, 0x00000120,
    0x0000000D, 0x000013FD, 0x00000000, 0x00000128, 0x0000000D, 0x00001802, 0x00000000, 0x000001E0,
    0x0000000D, 0x00001BFE, 0x00000000, 0x000001E8, 0x0000000D, 0x00001C02, 0x00000000, 0x00000150,
    0x0000000D, 0x00001FFE, 0x00000000, 0x00000158, 0x0000000D, 0x00002002, 0x00000000, 0x00000110,
    0x0000000D, 0x000023FE, 0x00000000, 0x00000118, 0x0000000D, 0x00004401, 0x00000000, 0x000001F0,
    0x0000000D, 0x000047FF, 0x00000000, 0x000001F8, 0x0000000D, 0x00004801, 0x00000000, 0x000001A0,
    0x0000000D, 0x00004BFF, 0x00000000, 0x000001A8, 0x0000000D, 0x00004C01, 0x00000000, 0x00000190,
    0x0000000D, 0x00004FFF, 0x00000000, 0x00000198, 0x0000000D, 0x00005001, 0x00000000, 0x00000170,
    0x0000000D, 0x000053FF, 0x00000000, 0x00000178, 0x0000000D, 0x00005401, 0x00000000, 0x00000160,
    0x0000000D, 0x000057FF, 0x00000000, 0x00000168,
};

static const int encodetbl2[0x200] = {
    0x00000006, 0x0000000C, 0x00000000, 0x0000D000, 0x00000006, 0x000003F4, 0x00000000, 0x0000D400,
    0x00000006, 0x0000000D, 0x00000000, 0x0000C800, 0x00000006, 0x000003F3, 0x00000000, 0x0000CC00,
    0x00000006, 0x0000000E, 0x00000000, 0x0000C000, 0x00000006, 0x000003F2, 0x00000000, 0x0000C400,
    0x00000006, 0x0000000F, 0x00000000, 0x0000B800, 0x00000006, 0x000003F1, 0x00000000, 0x0000BC00,
    0x00000006, 0x00000406, 0x00000000, 0x0000B000, 0x00000006, 0x000007FA, 0x00000000, 0x0000B400,
    0x00000006, 0x00000407, 0x00000000, 0x0000A800, 0x00000006, 0x000007F9, 0x00000000, 0x0000AC00,
    0x00000006, 0x00000805, 0x00000000, 0x0000A000, 0x00000006, 0x00000BFB, 0x00000000, 0x0000A400,
    0x00000006, 0x00000C04, 0x00000000, 0x00009800, 0x00000006, 0x00000FFC, 0x00000000, 0x00009C00,
    0x00000006, 0x00001403, 0x00000000, 0x00009000, 0x00000006, 0x000017FD, 0x00000000, 0x00009400,
    0x00000006, 0x00002402, 0x00000000, 0x00008800, 0x00000006, 0x000027FE, 0x00000000, 0x00008C00,
    0x00000006, 0x00002802, 0x00000000, 0x00008000, 0x00000006, 0x00002BFE, 0x00000000, 0x00008400,
    0x00000006, 0x00005801, 0x00000000, 0x0000F800, 0x00000006, 0x00005BFF, 0x00000000, 0x0000FC00,
    0x00000006, 0x00005C01, 0x00000000, 0x0000F000, 0x00000006, 0x00005FFF, 0x00000000, 0x0000F400,
    0x00000006, 0x00006001, 0x00000000, 0x0000E800, 0x00000006, 0x000063FF, 0x00000000, 0x0000EC00,
    0x00000006, 0x00006401, 0x00000000, 0x0000E000, 0x00000006, 0x000067FF, 0x00000000, 0x0000E400,
    0x00000006, 0x00006801, 0x00000000, 0x0000D800, 0x00000006, 0x00006BFF, 0x00000000, 0x0000DC00,
    0x00000007, 0x00000010, 0x00000000, 0x00007C00, 0x00000007, 0x000003F0, 0x00000000, 0x00007E00,
    0x00000007, 0x00000011, 0x00000000, 0x00007800, 0x00000007, 0x000003EF, 0x00000000, 0x00007A00,
    0x00000007, 0x00000012, 0x00000000, 0x00007400, 0x00000007, 0x000003EE, 0x00000000, 0x00007600,
    0x00000007, 0x00000013, 0x00000000, 0x00007000, 0x00000007, 0x000003ED, 0x00000000, 0x00007200,
    0x00000007, 0x00000014, 0x00000000, 0x00006C00, 0x00000007, 0x000003EC, 0x00000000, 0x00006E00,
    0x00000007, 0x00000015, 0x00000000, 0x00006800, 0x00000007, 0x000003EB, 0x00000000, 0x00006A00,
    0x00000007, 0x00000016, 0x00000000, 0x00006400, 0x00000007, 0x000003EA, 0x00000000, 0x00006600,
    0x00000007, 0x00000017, 0x00000000, 0x00006000, 0x00000007, 0x000003E9, 0x00000000, 0x00006200,
    0x00000007, 0x00000018, 0x00000000, 0x00005C00, 0x00000007, 0x000003E8, 0x00000000, 0x00005E00,
    0x00000007, 0x00000019, 0x00000000, 0x00005800, 0x00000007, 0x000003E7, 0x00000000, 0x00005A00,
    0x00000007, 0x0000001A, 0x00000000, 0x00005400, 0x00000007, 0x000003E6, 0x00000000, 0x00005600,
    0x00000007, 0x0000001B, 0x00000000, 0x00005000, 0x00000007, 0x000003E5, 0x00000000, 0x00005200,
    0x00000007, 0x0000001C, 0x00000000, 0x00004C00, 0x00000007, 0x000003E4, 0x00000000, 0x00004E00,
    0x00000007, 0x0000001D, 0x00000000, 0x00004800, 0x00000007, 0x000003E3, 0x00000000, 0x00004A00,
    0x00000007, 0x0000001E, 0x00000000, 0x00004400, 0x00000007, 0x000003E2, 0x00000000, 0x00004600,
    0x00000007, 0x0000001F, 0x00000000, 0x00004000, 0x00000007, 0x000003E1, 0x00000000, 0x00004200,
    0x00000008, 0x00000020, 0x00000000, 0x00003000, 0x00000008, 0x000003E0, 0x00000000, 0x00003100,
    0x00000008, 0x00000021, 0x00000000, 0x00002E00, 0x00000008, 0x000003DF, 0x00000000, 0x00002F00,
    0x00000008, 0x00000022, 0x00000000, 0x00002C00, 0x00000008, 0x000003DE, 0x00000000, 0x00002D00,
    0x00000008, 0x00000023, 0x00000000, 0x00002A00, 0x00000008, 0x000003DD, 0x00000000, 0x00002B00,
    0x00000008, 0x00000024, 0x00000000, 0x00002800, 0x00000008, 0x000003DC, 0x00000000, 0x00002900,
    0x00000008, 0x00000025, 0x00000000, 0x00002600, 0x00000008, 0x000003DB, 0x00000000, 0x00002700,
    0x00000008, 0x00000026, 0x00000000, 0x00002400, 0x00000008, 0x000003DA, 0x00000000, 0x00002500,
    0x00000008, 0x00000027, 0x00000000, 0x00002200, 0x00000008, 0x000003D9, 0x00000000, 0x00002300,
    0x00000008, 0x00000028, 0x00000000, 0x00002000, 0x00000008, 0x000003D8, 0x00000000, 0x00002100,
    0x00000008, 0x00000408, 0x00000000, 0x00003E00, 0x00000008, 0x000007F8, 0x00000000, 0x00003F00,
    0x00000008, 0x00000409, 0x00000000, 0x00003C00, 0x00000008, 0x000007F7, 0x00000000, 0x00003D00,
    0x00000008, 0x0000040A, 0x00000000, 0x00003A00, 0x00000008, 0x000007F6, 0x00000000, 0x00003B00,
    0x00000008, 0x0000040B, 0x00000000, 0x00003800, 0x00000008, 0x000007F5, 0x00000000, 0x00003900,
    0x00000008, 0x0000040C, 0x00000000, 0x00003600, 0x00000008, 0x000007F4, 0x00000000, 0x00003700,
    0x00000008, 0x0000040D, 0x00000000, 0x00003400, 0x00000008, 0x000007F3, 0x00000000, 0x00003500,
    0x00000008, 0x0000040E, 0x00000000, 0x00003200, 0x00000008, 0x000007F2, 0x00000000, 0x00003300,
    0x00000009, 0x0000040F, 0x00000000, 0x00001300, 0x00000009, 0x000007F1, 0x00000000, 0x00001380,
    0x00000009, 0x00000410, 0x00000000, 0x00001200, 0x00000009, 0x000007F0, 0x00000000, 0x00001280,
    0x00000009, 0x00000411, 0x00000000, 0x00001100, 0x00000009, 0x000007EF, 0x00000000, 0x00001180,
    0x00000009, 0x00000412, 0x00000000, 0x00001000, 0x00000009, 0x000007EE, 0x00000000, 0x00001080,
    0x00000009, 0x00001803, 0x00000000, 0x00001400, 0x00000009, 0x00001BFD, 0x00000000, 0x00001480,
    0x00000009, 0x00002C02, 0x00000000, 0x00001A00, 0x00000009, 0x00002FFE, 0x00000000, 0x00001A80,
    0x00000009, 0x00003002, 0x00000000, 0x00001900, 0x00000009, 0x000033FE, 0x00000000, 0x00001980,
    0x00000009, 0x00003402, 0x00000000, 0x00001800, 0x00000009, 0x000037FE, 0x00000000, 0x00001880,
    0x00000009, 0x00003802, 0x00000000, 0x00001700, 0x00000009, 0x00003BFE, 0x00000000, 0x00001780,
    0x00000009, 0x00003C02, 0x00000000, 0x00001600, 0x00000009, 0x00003FFE, 0x00000000, 0x00001680,
    0x00000009, 0x00004002, 0x00000000, 0x00001500, 0x00000009, 0x000043FE, 0x00000000, 0x00001580,
    0x00000009, 0x00006C01, 0x00000000, 0x00001F00, 0x00000009, 0x00006FFF, 0x00000000, 0x00001F80,
    0x00000009, 0x00007001, 0x00000000, 0x00001E00, 0x00000009, 0x000073FF, 0x00000000, 0x00001E80,
    0x00000009, 0x00007401, 0x00000000, 0x00001D00, 0x00000009, 0x000077FF, 0x00000000, 0x00001D80,
    0x00000009, 0x00007801, 0x00000000, 0x00001C00, 0x00000009, 0x00007BFF, 0x00000000, 0x00001C80,
    0x00000009, 0x00007C01, 0x00000000, 0x00001B00, 0x00000009, 0x00007FFF, 0x00000000, 0x00001B80,
};

static const int quanttbl[0x40] = {
    8, 0x10, 0x13, 0x16, 0x1a, 0x1b, 0x1d, 0x22,
    0x10, 0x10, 0x16, 0x18, 0x1b, 0x1d, 0x22, 0x25,
    0x13, 0x16, 0x1a, 0x1b, 0x1d, 0x22, 0x22, 0x26,
    0x16, 0x16, 0x1a, 0x1b, 0x1d, 0x22, 0x25, 0x28,
    0x16, 0x1a, 0x1b, 0x1d, 0x20, 0x23, 0x28, 0x30,
    0x1a, 0x1b, 0x1d, 0x20, 0x23, 0x28, 0x30, 0x3a,
    0x1a, 0x1b, 0x1d, 0x22, 0x26, 0x2e, 0x38, 0x45,
    0x1b, 0x1d, 0x23, 0x26, 0x2e, 0x38, 0x45, 0x53,
};


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

#define fixedmul(a, b) \
    static_cast<int>((static_cast<long long>(a) * (b) + 0x8000) >> 16)

static void madinit() {
    int bits;
    int val;
    int vlc;
    int prefix;
    int count;
    int i;
    int j;
    int *entry;

    count = 0x1ff;
    i = -0x100;
    do {
        val = i;
        if (val < -0x80) {
            val = -0x80;
        } else if (val > 0x7f) {
            val = 0x7f;
        }
        clipbiastbl[i & 0x1ff] = static_cast<unsigned char>(val - 0x80);
        i++;
    } while (--count != 0);
    madvlctbl1[0] = 0xf;
    count = 7;
    i = 1;
    do {
        madvlctbl1[i++] = 0x1f;
    } while (--count != 0);
    count = 8;
    i = 0xf;
    do {
        madvlctbl1[i--] = 0x2f;
    } while (--count != 0);
    count = 0x80;
    i = 0x17f;
    do {
        madvlctbl1[i--] = 0x3f;
    } while (--count != 0);

    entry = const_cast<int *>(encodetbl1) + 4;
    i = 0x5e;
    do {
        bits = entry[3];
        vlc = entry[0];
        if (bits & 0xfc00) {
            val = (entry[2] << 0x16) | ((entry[2] << 6) & 0x3f0000) | entry[1];
            count = 1 << (9 - entry[0]);
            prefix = bits >> 7;
            j = count;
            do {
                madvlctbl1[prefix++] = val;
            } while (--j != 0);
        } else {
            val = (entry[2] << 0x16) | ((entry[2] << 6) & 0x3f0000) | (entry[1] - 6);
            count = 1 << (0xe - entry[0]);
            prefix = bits >> 2;
            j = count;
            do {
                madvlctbl3[prefix++] = val;
            } while (--j != 0);
        }
        entry += 4;
    } while (--i != 0);

    entry = const_cast<int *>(encodetbl2);
    i = 0x80;
    do {
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
        entry += 4;
    } while (--i != 0);
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
    int val;

    val = madvlctbl4[(madshiftreg >> 0x1a) & 0x3f];
    discardbits(val & 0xff);
    return val >> 0x16;
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
    madshiftreg = (geti(src, 2) << 0x10) |
                  geti(reinterpret_cast<const unsigned char *>(src) + 2, 2);
    maddataptr = reinterpret_cast<const unsigned short *>(reinterpret_cast<const unsigned char *>(src) + 4);
    madbitcount = 0x20;
    motionframe = motion;
    madquant[0] = fixedmul(quanttbl[0] << 16, idctprescale[0]);
    for (i = 1; i < 0x40; i++) {
        madquant[i] = fixedmul((quality * quanttbl[i]) << 0xd, idctprescale[i]);
    }
}

void MAD_decodemacroblock(const unsigned char *src_y, const unsigned char *src_cb,
                          const unsigned char *src_cr, unsigned char *dest_y,
                          unsigned char *dest_cb, unsigned char *dest_cr, int width) {
    int flags;
    int dx;
    int dy;
    int index;
    int correction;
    int chromawidth;

    chromawidth = width >> 1;
    if (motionframe == 0) {
        flags = 0;
    } else if (!(madshiftreg & 0xc0000000)) {
        flags = 0;
        discardbits(2);
    } else {
        if (static_cast<int>(madshiftreg) < 0) {
            flags = 0x3ff;
            discardbits(1);
        } else {
            flags = madshiftreg >> 0x18;
            discardbits(8);
        }
        dx = getdelta();
        dy = getdelta();
        src_y += dy * width + dx;
        src_cb += (dy >> 1) * chromawidth + (dx >> 1);
        src_cr += (dy >> 1) * chromawidth + (dx >> 1);
    }
    if (!(flags & 1)) {
        if (madvlcdecode() == 1) {
            dcblock(luma, 0x10);
        } else {
            idctcompute(luma, 0x10);
        }
    } else {
        correction = getdelta() * 2 - 0x80;
        getluma(src_y, width, luma, correction);
    }
    if (!(flags & 2)) {
        if (madvlcdecode() == 1) {
            dcblock(luma + 0x20, 0x10);
        } else {
            idctcompute(luma + 0x20, 0x10);
        }
    } else {
        correction = getdelta() * 2 - 0x80;
        getluma(src_y + 8, width, luma + 0x20, correction);
    }
    if (!(flags & 4)) {
        if (madvlcdecode() == 1) {
            dcblock(luma + 0x200, 0x10);
        } else {
            idctcompute(luma + 0x200, 0x10);
        }
    } else {
        correction = getdelta() * 2 - 0x80;
        getluma(src_y + width * 8, width, luma + 0x200, correction);
    }
    if (!(flags & 8)) {
        if (madvlcdecode() == 1) {
            dcblock(luma + 0x220, 0x10);
        } else {
            idctcompute(luma + 0x220, 0x10);
        }
    } else {
        correction = getdelta() * 2 - 0x80;
        getluma(src_y + width * 8 + 8, width, luma + 0x220, correction);
    }
    if (!(flags & 0x10)) {
        if (madvlcdecode() == 1) {
            dcblock(chroma, 8);
        } else {
            idctcompute(chroma, 8);
        }
    } else {
        correction = getdelta() * 2 - 0x80;
        getchroma(src_cb, chromawidth, chroma, correction);
    }
    if (!(flags & 0x20)) {
        if (madvlcdecode() == 1) {
            dcblock(chroma + 0x100, 8);
        } else {
            idctcompute(chroma + 0x100, 8);
        }
    } else {
        correction = getdelta() * 2 - 0x80;
        getchroma(src_cr, chromawidth, chroma + 0x100, correction);
    }
    setluma(luma, dest_y, width);
    setchroma(chroma, dest_cb, chromawidth);
    setchroma(chroma + 0x100, dest_cr, chromawidth);
}
