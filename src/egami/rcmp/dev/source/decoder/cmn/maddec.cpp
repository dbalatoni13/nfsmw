extern "C" {
void idctcompute(int *dest, int stride);
int madvlcdecode();
}

extern int idctinput[64];
extern int idctprescale[64];

/* rcmp se compilo SIN -G0: los globales pequenos viven en .sbss y se
   direccionan por @sda21. Los cflags de la biblioteca ya no llevan -G0. */

static inline int MULT(int a, int b)
{
    return (int)((((long long)(a) * (b)) + 32768) >> 16);
}

static const int encodetbl1[380] = {
    2, 0x0000FE00, 0, 0x00008000,
    3, 1, 0, 0x0000C000,
    3, 0x000003FF, 0, 0x0000E000,
    4, 0x00000401, 0, 0x00006000,
    4, 0x000007FF, 0, 0x00007000,
    5, 2, 0, 0x00004000,
    5, 0x000003FE, 0, 0x00004800,
    5, 0x00000801, 0, 0x00005000,
    5, 0x00000BFF, 0, 0x00005800,
    6, 3, 0, 0x00002800,
    6, 0x000003FD, 0, 0x00002C00,
    6, 0x00000C01, 0, 0x00003800,
    6, 0x00000FFF, 0, 0x00003C00,
    6, 0x00001001, 0, 0x00003000,
    6, 0x000013FF, 0, 0x00003400,
    7, 0x00000402, 0, 0x00001800,
    7, 0x000007FE, 0, 0x00001A00,
    7, 0x00001401, 0, 0x00001C00,
    7, 0x000017FF, 0, 0x00001E00,
    7, 0x00001801, 0, 0x00001400,
    7, 0x00001BFF, 0, 0x00001600,
    7, 0x00001C01, 0, 0x00001000,
    7, 0x00001FFF, 0, 0x00001200,
    8, 4, 0, 0x00000C00,
    8, 0x000003FC, 0, 0x00000D00,
    8, 0x00000802, 0, 0x00000800,
    8, 0x00000BFE, 0, 0x00000900,
    8, 0x00002001, 0, 0x00000E00,
    8, 0x000023FF, 0, 0x00000F00,
    8, 0x00002401, 0, 0x00000A00,
    8, 0x000027FF, 0, 0x00000B00,
    9, 5, 0, 0x00002600,
    9, 0x000003FB, 0, 0x00002680,
    9, 6, 0, 0x00002100,
    9, 0x000003FA, 0, 0x00002180,
    9, 0x00000403, 0, 0x00002500,
    9, 0x000007FD, 0, 0x00002580,
    9, 0x00000C02, 0, 0x00002400,
    9, 0x00000FFE, 0, 0x00002480,
    9, 0x00002801, 0, 0x00002700,
    9, 0x00002BFF, 0, 0x00002780,
    9, 0x00002C01, 0, 0x00002300,
    9, 0x00002FFF, 0, 0x00002380,
    9, 0x00003001, 0, 0x00002200,
    9, 0x000033FF, 0, 0x00002280,
    9, 0x00003401, 0, 0x00002000,
    9, 0x000037FF, 0, 0x00002080,
    0x0000000B, 7, 0, 0x00000280,
    0x0000000B, 0x000003F9, 0, 0x000002A0,
    0x0000000B, 0x00000404, 0, 0x00000300,
    0x0000000B, 0x000007FC, 0, 0x00000320,
    0x0000000B, 0x00000803, 0, 0x000002C0,
    0x0000000B, 0x00000BFD, 0, 0x000002E0,
    0x0000000B, 0x00001002, 0, 0x000003C0,
    0x0000000B, 0x000013FE, 0, 0x000003E0,
    0x0000000B, 0x00001402, 0, 0x00000240,
    0x0000000B, 0x000017FE, 0, 0x00000260,
    0x0000000B, 0x00003801, 0, 0x00000380,
    0x0000000B, 0x00003BFF, 0, 0x000003A0,
    0x0000000B, 0x00003C01, 0, 0x00000340,
    0x0000000B, 0x00003FFF, 0, 0x00000360,
    0x0000000B, 0x00004001, 0, 0x00000200,
    0x0000000B, 0x000043FF, 0, 0x00000220,
    0x0000000D, 8, 0, 0x000001D0,
    0x0000000D, 0x000003F8, 0, 0x000001D8,
    0x0000000D, 9, 0, 0x00000180,
    0x0000000D, 0x000003F7, 0, 0x00000188,
    0x0000000D, 0x0000000A, 0, 0x00000130,
    0x0000000D, 0x000003F6, 0, 0x00000138,
    0x0000000D, 0x0000000B, 0, 0x00000100,
    0x0000000D, 0x000003F5, 0, 0x00000108,
    0x0000000D, 0x00000405, 0, 0x000001B0,
    0x0000000D, 0x000007FB, 0, 0x000001B8,
    0x0000000D, 0x00000804, 0, 0x00000140,
    0x0000000D, 0x00000BFC, 0, 0x00000148,
    0x0000000D, 0x00000C03, 0, 0x000001C0,
    0x0000000D, 0x00000FFD, 0, 0x000001C8,
    0x0000000D, 0x00001003, 0, 0x00000120,
    0x0000000D, 0x000013FD, 0, 0x00000128,
    0x0000000D, 0x00001802, 0, 0x000001E0,
    0x0000000D, 0x00001BFE, 0, 0x000001E8,
    0x0000000D, 0x00001C02, 0, 0x00000150,
    0x0000000D, 0x00001FFE, 0, 0x00000158,
    0x0000000D, 0x00002002, 0, 0x00000110,
    0x0000000D, 0x000023FE, 0, 0x00000118,
    0x0000000D, 0x00004401, 0, 0x000001F0,
    0x0000000D, 0x000047FF, 0, 0x000001F8,
    0x0000000D, 0x00004801, 0, 0x000001A0,
    0x0000000D, 0x00004BFF, 0, 0x000001A8,
    0x0000000D, 0x00004C01, 0, 0x00000190,
    0x0000000D, 0x00004FFF, 0, 0x00000198,
    0x0000000D, 0x00005001, 0, 0x00000170,
    0x0000000D, 0x000053FF, 0, 0x00000178,
    0x0000000D, 0x00005401, 0, 0x00000160,
    0x0000000D, 0x000057FF, 0, 0x00000168
};

static const int encodetbl2[512] = {
    6, 0x0000000C, 0, 0x0000D000,
    6, 0x000003F4, 0, 0x0000D400,
    6, 0x0000000D, 0, 0x0000C800,
    6, 0x000003F3, 0, 0x0000CC00,
    6, 0x0000000E, 0, 0x0000C000,
    6, 0x000003F2, 0, 0x0000C400,
    6, 0x0000000F, 0, 0x0000B800,
    6, 0x000003F1, 0, 0x0000BC00,
    6, 0x00000406, 0, 0x0000B000,
    6, 0x000007FA, 0, 0x0000B400,
    6, 0x00000407, 0, 0x0000A800,
    6, 0x000007F9, 0, 0x0000AC00,
    6, 0x00000805, 0, 0x0000A000,
    6, 0x00000BFB, 0, 0x0000A400,
    6, 0x00000C04, 0, 0x00009800,
    6, 0x00000FFC, 0, 0x00009C00,
    6, 0x00001403, 0, 0x00009000,
    6, 0x000017FD, 0, 0x00009400,
    6, 0x00002402, 0, 0x00008800,
    6, 0x000027FE, 0, 0x00008C00,
    6, 0x00002802, 0, 0x00008000,
    6, 0x00002BFE, 0, 0x00008400,
    6, 0x00005801, 0, 0x0000F800,
    6, 0x00005BFF, 0, 0x0000FC00,
    6, 0x00005C01, 0, 0x0000F000,
    6, 0x00005FFF, 0, 0x0000F400,
    6, 0x00006001, 0, 0x0000E800,
    6, 0x000063FF, 0, 0x0000EC00,
    6, 0x00006401, 0, 0x0000E000,
    6, 0x000067FF, 0, 0x0000E400,
    6, 0x00006801, 0, 0x0000D800,
    6, 0x00006BFF, 0, 0x0000DC00,
    7, 0x00000010, 0, 0x00007C00,
    7, 0x000003F0, 0, 0x00007E00,
    7, 0x00000011, 0, 0x00007800,
    7, 0x000003EF, 0, 0x00007A00,
    7, 0x00000012, 0, 0x00007400,
    7, 0x000003EE, 0, 0x00007600,
    7, 0x00000013, 0, 0x00007000,
    7, 0x000003ED, 0, 0x00007200,
    7, 0x00000014, 0, 0x00006C00,
    7, 0x000003EC, 0, 0x00006E00,
    7, 0x00000015, 0, 0x00006800,
    7, 0x000003EB, 0, 0x00006A00,
    7, 0x00000016, 0, 0x00006400,
    7, 0x000003EA, 0, 0x00006600,
    7, 0x00000017, 0, 0x00006000,
    7, 0x000003E9, 0, 0x00006200,
    7, 0x00000018, 0, 0x00005C00,
    7, 0x000003E8, 0, 0x00005E00,
    7, 0x00000019, 0, 0x00005800,
    7, 0x000003E7, 0, 0x00005A00,
    7, 0x0000001A, 0, 0x00005400,
    7, 0x000003E6, 0, 0x00005600,
    7, 0x0000001B, 0, 0x00005000,
    7, 0x000003E5, 0, 0x00005200,
    7, 0x0000001C, 0, 0x00004C00,
    7, 0x000003E4, 0, 0x00004E00,
    7, 0x0000001D, 0, 0x00004800,
    7, 0x000003E3, 0, 0x00004A00,
    7, 0x0000001E, 0, 0x00004400,
    7, 0x000003E2, 0, 0x00004600,
    7, 0x0000001F, 0, 0x00004000,
    7, 0x000003E1, 0, 0x00004200,
    8, 0x00000020, 0, 0x00003000,
    8, 0x000003E0, 0, 0x00003100,
    8, 0x00000021, 0, 0x00002E00,
    8, 0x000003DF, 0, 0x00002F00,
    8, 0x00000022, 0, 0x00002C00,
    8, 0x000003DE, 0, 0x00002D00,
    8, 0x00000023, 0, 0x00002A00,
    8, 0x000003DD, 0, 0x00002B00,
    8, 0x00000024, 0, 0x00002800,
    8, 0x000003DC, 0, 0x00002900,
    8, 0x00000025, 0, 0x00002600,
    8, 0x000003DB, 0, 0x00002700,
    8, 0x00000026, 0, 0x00002400,
    8, 0x000003DA, 0, 0x00002500,
    8, 0x00000027, 0, 0x00002200,
    8, 0x000003D9, 0, 0x00002300,
    8, 0x00000028, 0, 0x00002000,
    8, 0x000003D8, 0, 0x00002100,
    8, 0x00000408, 0, 0x00003E00,
    8, 0x000007F8, 0, 0x00003F00,
    8, 0x00000409, 0, 0x00003C00,
    8, 0x000007F7, 0, 0x00003D00,
    8, 0x0000040A, 0, 0x00003A00,
    8, 0x000007F6, 0, 0x00003B00,
    8, 0x0000040B, 0, 0x00003800,
    8, 0x000007F5, 0, 0x00003900,
    8, 0x0000040C, 0, 0x00003600,
    8, 0x000007F4, 0, 0x00003700,
    8, 0x0000040D, 0, 0x00003400,
    8, 0x000007F3, 0, 0x00003500,
    8, 0x0000040E, 0, 0x00003200,
    8, 0x000007F2, 0, 0x00003300,
    9, 0x0000040F, 0, 0x00001300,
    9, 0x000007F1, 0, 0x00001380,
    9, 0x00000410, 0, 0x00001200,
    9, 0x000007F0, 0, 0x00001280,
    9, 0x00000411, 0, 0x00001100,
    9, 0x000007EF, 0, 0x00001180,
    9, 0x00000412, 0, 0x00001000,
    9, 0x000007EE, 0, 0x00001080,
    9, 0x00001803, 0, 0x00001400,
    9, 0x00001BFD, 0, 0x00001480,
    9, 0x00002C02, 0, 0x00001A00,
    9, 0x00002FFE, 0, 0x00001A80,
    9, 0x00003002, 0, 0x00001900,
    9, 0x000033FE, 0, 0x00001980,
    9, 0x00003402, 0, 0x00001800,
    9, 0x000037FE, 0, 0x00001880,
    9, 0x00003802, 0, 0x00001700,
    9, 0x00003BFE, 0, 0x00001780,
    9, 0x00003C02, 0, 0x00001600,
    9, 0x00003FFE, 0, 0x00001680,
    9, 0x00004002, 0, 0x00001500,
    9, 0x000043FE, 0, 0x00001580,
    9, 0x00006C01, 0, 0x00001F00,
    9, 0x00006FFF, 0, 0x00001F80,
    9, 0x00007001, 0, 0x00001E00,
    9, 0x000073FF, 0, 0x00001E80,
    9, 0x00007401, 0, 0x00001D00,
    9, 0x000077FF, 0, 0x00001D80,
    9, 0x00007801, 0, 0x00001C00,
    9, 0x00007BFF, 0, 0x00001C80,
    9, 0x00007C01, 0, 0x00001B00,
    9, 0x00007FFF, 0, 0x00001B80
};

static const int quanttbl[64] = {
    8, 16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83
};

const unsigned short *maddataptr;
unsigned int madshiftreg;
int madbitcount;

int madvlctbl1[512];
int madvlctbl2[256];
int madvlctbl3[256];
volatile int madvlctbl4[64];
int madquant[64];

static int initflag;
static int motionframe;

static int luma[256];
static int chroma[2][64];
static unsigned char clipbiastbl[512];

static void madinit()
{
    int bits, val, vlc, prefix, count, i, j;

    for (i = -256; i < 255; i++)
    {
        val = i;
        if (val < -128)
            val = -128;
        else if (val > 127)
            val = 127;
        clipbiastbl[i & 511] = (unsigned char)(val - 128);
    }

    madvlctbl1[0] = 15;
    for (i = 1; i < 8; i++)
        madvlctbl1[i] = 31;
    for (i = 15; i >= 8; i--)
        madvlctbl1[i] = 47;
    for (i = 383; i >= 256; i--)
        madvlctbl1[i] = 63;

    for (i = 1; i < 95; i++)
    {
        bits = encodetbl1[i * 4];
        val = encodetbl1[i * 4 + 1];
        vlc = encodetbl1[i * 4 + 3];
        if (vlc & 0xfc00)
        {
            count = 1 << (9 - bits);
            val = (val << 22) | ((val << 6) & 0x3f0000) | bits;
            prefix = vlc >> 7;
            for (j = 0; j < count; j++)
                madvlctbl1[prefix + j] = val;
        }
        else
        {
            count = 1 << (14 - bits);
            val = (val << 22) | ((val << 6) & 0x3f0000) | (bits - 6);
            prefix = vlc >> 2;
            for (j = 0; j < count; j++)
                madvlctbl3[prefix + j] = val;
        }
    }

    for (i = 0; i < 128; i++)
    {
        bits = encodetbl2[i * 4] + 8;
        val = encodetbl2[i * 4 + 1];
        vlc = encodetbl2[i * 4 + 3];
        if ((vlc & 0x8000) == 0)
        {
            count = 1 << (17 - bits);
            val = (val << 22) | ((val << 6) & 0x3f0000) | (bits - 9);
            prefix = vlc >> 7;
            for (j = 0; j < count; j++)
                madvlctbl2[prefix + j] = val;
        }
        else
        {
            count = 1 << (14 - bits);
            val = (val << 22) | ((val << 6) & 0x3f0000) | (bits - 6);
            prefix = vlc >> 10;
            for (j = 0; j < count; j++)
                madvlctbl3[prefix + j] = val;
        }
    }

    for (i = 0; i < 32; i++)
        madvlctbl4[i] = 1;

    for (i = 0; i < 16; i++)
    {
        madvlctbl4[i + 32] = ((i + 1) << 22) | 6;
        madvlctbl4[i + 48] = ((i - 16) << 22) | 6;
    }

    initflag = 1;
}

static void discardbits(int bits)
{
    const unsigned char *p;

    madshiftreg <<= bits;
    madbitcount -= bits;
    if (madbitcount <= 15)
    {
        p = (const unsigned char *)maddataptr;
        madshiftreg |= (unsigned int)((p[1] << 8) | p[0]) << (16 - madbitcount);
        madbitcount += 16;
        maddataptr++;
    }
}

static int getdelta()
{
    int val;

    val = madvlctbl4[madshiftreg >> 26];
    discardbits(val & 0xff);
    return val >> 22;
}

static void dcblock(int *dest, int stride)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        dest[0] = idctinput[0];
        dest[1] = idctinput[0];
        dest[2] = idctinput[0];
        dest[3] = idctinput[0];
        dest[4] = idctinput[0];
        dest[5] = idctinput[0];
        dest[6] = idctinput[0];
        dest[7] = idctinput[0];
        dest += stride;
    }
}

static void getluma(const unsigned char *src, int stride, int *dest, int correction)
{
    int i;

    for (i = 0; i < 8; i++)
    {
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
    }
}

static void getchroma(const unsigned char *src, int stride, int *dest, int correction)
{
    int i;

    for (i = 0; i < 8; i++)
    {
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
    }
}

static void setluma(const int *src, unsigned char *dest, int stride)
{
    int i;

    for (i = 0; i < 16; i++)
    {
        dest[0] = clipbiastbl[(short)(src[0] >> 16) & 511];
        dest[1] = clipbiastbl[(short)(src[1] >> 16) & 511];
        dest[2] = clipbiastbl[(short)(src[2] >> 16) & 511];
        dest[3] = clipbiastbl[(short)(src[3] >> 16) & 511];
        dest[4] = clipbiastbl[(short)(src[4] >> 16) & 511];
        dest[5] = clipbiastbl[(short)(src[5] >> 16) & 511];
        dest[6] = clipbiastbl[(short)(src[6] >> 16) & 511];
        dest[7] = clipbiastbl[(short)(src[7] >> 16) & 511];
        dest[8] = clipbiastbl[(short)(src[8] >> 16) & 511];
        dest[9] = clipbiastbl[(short)(src[9] >> 16) & 511];
        dest[10] = clipbiastbl[(short)(src[10] >> 16) & 511];
        dest[11] = clipbiastbl[(short)(src[11] >> 16) & 511];
        dest[12] = clipbiastbl[(short)(src[12] >> 16) & 511];
        dest[13] = clipbiastbl[(short)(src[13] >> 16) & 511];
        dest[14] = clipbiastbl[(short)(src[14] >> 16) & 511];
        dest[15] = clipbiastbl[(short)(src[15] >> 16) & 511];
        src += 16;
        dest += stride;
    }
}

static void setchroma(const int *src, unsigned char *dest, int stride)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        dest[0] = clipbiastbl[(short)(src[0] >> 16) & 511];
        dest[1] = clipbiastbl[(short)(src[1] >> 16) & 511];
        dest[2] = clipbiastbl[(short)(src[2] >> 16) & 511];
        dest[3] = clipbiastbl[(short)(src[3] >> 16) & 511];
        dest[4] = clipbiastbl[(short)(src[4] >> 16) & 511];
        dest[5] = clipbiastbl[(short)(src[5] >> 16) & 511];
        dest[6] = clipbiastbl[(short)(src[6] >> 16) & 511];
        dest[7] = clipbiastbl[(short)(src[7] >> 16) & 511];
        src += 8;
        dest += stride;
    }
}

void MAD_initdecode(const unsigned short *src, int motion, int quality)
{
    int i;

    if (!initflag)
        madinit();

    madshiftreg = ((((const unsigned char *)src)[1] << 8 | ((const unsigned char *)src)[0]) << 16)
                  | (((const unsigned char *)src)[3] << 8 | ((const unsigned char *)src)[2]);
    maddataptr = src + 2;
    madbitcount = 32;
    motionframe = motion;
    madquant[0] = MULT(quanttbl[0] << 16, idctprescale[0]);

    for (i = 1; i < 64; i++)
        madquant[i] = MULT((quality * quanttbl[i]) << 13, idctprescale[i]);
}

void MAD_decodemacroblock(const unsigned char *src_y, const unsigned char *src_cb,
                          const unsigned char *src_cr, unsigned char *dest_y,
                          unsigned char *dest_cb, unsigned char *dest_cr, int width)
{
    int flags;
    int dx;
    int dy;
    int index;
    int correction;
    int chromawidth;

    chromawidth = width >> 1;

    if (motionframe == 0)
    {
        flags = 0;
    }
    else if ((madshiftreg & 0xc0000000) == 0)
    {
        flags = 0;
        discardbits(2);
    }
    else
    {
        if ((int)madshiftreg < 0)
        {
            flags = 0x3ff;
            discardbits(1);
        }
        else
        {
            flags = madshiftreg >> 24;
            discardbits(8);
        }
        dx = getdelta();
        dy = getdelta();
        src_y += dy * width + dx;
        dy >>= 1;
        dx >>= 1;
        index = dy * chromawidth + dx;
        src_cb += index;
        src_cr += index;
    }

    if ((flags & 1) == 0)
    {
        index = madvlcdecode();
        if (index == 1)
            dcblock(&luma[0], 16);
        else
            idctcompute(&luma[0], 16);
    }
    else
    {
        correction = getdelta() * 2 - 128;
        getluma(src_y, width, &luma[0], correction);
    }

    if ((flags & 2) == 0)
    {
        index = madvlcdecode();
        if (index == 1)
            dcblock(&luma[8], 16);
        else
            idctcompute(&luma[8], 16);
    }
    else
    {
        correction = getdelta() * 2 - 128;
        getluma(src_y + 8, width, &luma[8], correction);
    }

    if ((flags & 4) == 0)
    {
        index = madvlcdecode();
        if (index == 1)
            dcblock(&luma[128], 16);
        else
            idctcompute(&luma[128], 16);
    }
    else
    {
        correction = getdelta() * 2 - 128;
        getluma(src_y + width * 8, width, &luma[128], correction);
    }

    if ((flags & 8) == 0)
    {
        index = madvlcdecode();
        if (index == 1)
            dcblock(&luma[136], 16);
        else
            idctcompute(&luma[136], 16);
    }
    else
    {
        correction = getdelta() * 2 - 128;
        getluma(src_y + width * 8 + 8, width, &luma[136], correction);
    }

    if ((flags & 16) == 0)
    {
        index = madvlcdecode();
        if (index == 1)
            dcblock(&chroma[0][0], 8);
        else
            idctcompute(&chroma[0][0], 8);
    }
    else
    {
        correction = getdelta() * 2 - 128;
        getchroma(src_cb, chromawidth, &chroma[0][0], correction);
    }

    if ((flags & 32) == 0)
    {
        index = madvlcdecode();
        if (index == 1)
            dcblock(&chroma[1][0], 8);
        else
            idctcompute(&chroma[1][0], 8);
    }
    else
    {
        correction = getdelta() * 2 - 128;
        getchroma(src_cr, chromawidth, &chroma[1][0], correction);
    }

    setluma(&luma[0], dest_y, width);
    setchroma(&chroma[0][0], dest_cb, chromawidth);
    setchroma(&chroma[1][0], dest_cr, chromawidth);
}
