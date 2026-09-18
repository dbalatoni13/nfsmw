extern int idctinput[64];

extern int madquant[64];
extern int madvlctbl1[512];
extern int madvlctbl2[256];
extern int madvlctbl3[256];

/* rcmp se compilo SIN -G0: estos tres globales viven en .sbss y se
   direccionan por @sda21. Los cflags de la biblioteca ya no llevan -G0. */

extern const unsigned short *maddataptr;
extern unsigned int madshiftreg;
extern int madbitcount;

static const int zigzag[64] = {
    0,  8,  1,  2,  9,  16, 24, 17,
    10, 3,  4,  11, 18, 25, 32, 40,
    33, 26, 19, 12, 5,  6,  13, 20,
    27, 34, 41, 48, 56, 49, 42, 35,
    28, 21, 14, 7,  15, 22, 29, 36,
    43, 50, 57, 58, 51, 44, 37, 30,
    23, 31, 38, 45, 52, 59, 60, 53,
    46, 39, 47, 54, 61, 62, 55, 63,
};

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

extern "C" int madvlcdecode()
{
    int *entry;
    int val;
    int bits;
    int index;
    int i;

    idctinput[0] = ((int)madshiftreg >> 24) * madquant[0];
    discardbits(8);

    entry = &idctinput[1];
    for (i = 0; i < 9; i++)
    {
        entry[0] = 0;
        entry[1] = 0;
        entry[2] = 0;
        entry[3] = 0;
        entry[4] = 0;
        entry[5] = 0;
        entry[6] = 0;
        entry += 7;
    }

    index = 1;
    for (;;)
    {
        val = madvlctbl1[madshiftreg >> 23];
        bits = val & 0xff;
        if (bits > 9)
        {
            if ((val & 0x20) == 0)
            {
                if ((val & 0x10) == 0)
                {
                    discardbits(9);
                    val = madvlctbl2[madshiftreg >> 24];
                    bits = val & 0xff;
                }
                else
                {
                    discardbits(6);
                    val = madvlctbl3[madshiftreg >> 24];
                    bits = val & 0xff;
                }
            }
            else if (val & 0x10)
            {
                discardbits(2);
                return index;
            }
            else
            {
                discardbits(6);
                val = madshiftreg;
                bits = 16;
            }
        }
        discardbits(bits);
        index += (val >> 16) & 0x3f;
        idctinput[zigzag[index]] = (val >> 22) * madquant[zigzag[index]];
        index++;
    }
}
