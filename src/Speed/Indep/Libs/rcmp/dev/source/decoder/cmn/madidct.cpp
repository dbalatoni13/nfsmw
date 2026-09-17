#include "types.h"

int idctprescale[0x40] = {
    0x2000, 0x1712, 0x187e, 0x1b37, 0x2000, 0x28ba, 0x3b21, 0x73fc,
    0x1712, 0x10a2, 0x11a8, 0x139f, 0x1712, 0x1d5d, 0x2aa1, 0x539f,
    0x187e, 0x11a8, 0x12bf, 0x14d4, 0x187e, 0x1f2c, 0x2d41, 0x58c5,
    0x1b37, 0x139f, 0x14d4, 0x1725, 0x1b37, 0x22a3, 0x3249, 0x62a3,
    0x2000, 0x1712, 0x187e, 0x1b37, 0x2000, 0x28ba, 0x3b21, 0x73fc,
    0x28ba, 0x1d5d, 0x1f2c, 0x22a3, 0x28ba, 0x33d6, 0x4b42, 0x939f,
    0x3b21, 0x2aa1, 0x2d41, 0x3249, 0x3b21, 0x4b42, 0x6d41, 0xd650,
    0x73fc, 0x539f, 0x58c5, 0x62a3, 0x73fc, 0x939f, 0xd650, 0x1a463,
};

int idctinput[0x40];
static int work[0x40];

extern "C" {
#define idctmul(a, b) static_cast<int>((static_cast<long long>(a) * static_cast<long long>(b) + 0x8000) >> 16)

static void IdctColumn(int *src, int *dest) {
    int t1;
    int t2;
    int t3;
    int t4;
    int t5;
    int t6;
    int t7;
    int t8;
    int t9;

    if ((src[1] | src[2] | src[3] | src[4] | src[5] | src[6] | src[7]) == 0) {
        dest[0] = src[0];
        dest[8] = src[0];
        dest[16] = src[0];
        dest[24] = src[0];
        dest[32] = src[0];
        dest[40] = src[0];
        dest[48] = src[0];
        dest[56] = src[0];
        return;
    }

    t1 = src[5] - src[3];
    t2 = src[1] + src[7];
    t3 = src[1] - src[7];
    t4 = src[5] + src[3];
    t5 = t2 - t4;
    t4 += t2;
    t2 = idctmul(t1 + t3, 0x61f8);
    t1 = t2 + idctmul(t1, 0x8a8c);
    t3 = idctmul(t3, 0x14e7b) - t2;
    t5 = idctmul(t5, 0xb505);
    t6 = src[0] + src[4];
    t7 = src[0] - src[4];
    t8 = idctmul(src[2] - src[6], 0xb505);
    t9 = src[2] + src[6] + t8;
    t2 = t6 - t9;
    t6 += t9;
    t9 = t7 - t8;
    t7 += t8;
    t4 += t3;
    t3 += t5;
    t5 += t1;

    dest[0] = t6 + t4;
    dest[8] = t7 + t3;
    dest[16] = t9 + t5;
    dest[24] = t2 + t1;
    dest[32] = t2 - t1;
    dest[40] = t9 - t5;
    dest[48] = t7 - t3;
    dest[56] = t6 - t4;
}

static void IdctRow(int *src, int *dest) {
    int t1;
    int t2;
    int t3;
    int t4;
    int t5;
    int t6;
    int t7;
    int t8;
    int t9;

    t1 = src[5] - src[3];
    t2 = src[1] + src[7];
    t3 = src[1] - src[7];
    t4 = src[5] + src[3];
    t5 = t2 - t4;
    t4 += t2;
    t2 = idctmul(t1 + t3, 0x61f8);
    t1 = t2 + idctmul(t1, 0x8a8c);
    t3 = idctmul(t3, 0x14e7b) - t2;
    t5 = idctmul(t5, 0xb505);
    t6 = src[0] + src[4];
    t7 = src[0] - src[4];
    t8 = idctmul(src[2] - src[6], 0xb505);
    t9 = src[2] + src[6] + t8;
    t2 = t6 - t9;
    t6 += t9;
    t9 = t7 - t8;
    t7 += t8;
    t4 += t3;
    t3 += t5;
    t5 += t1;

    dest[0] = t6 + t4;
    dest[1] = t7 + t3;
    dest[2] = t9 + t5;
    dest[3] = t2 + t1;
    dest[4] = t2 - t1;
    dest[5] = t9 - t5;
    dest[6] = t7 - t3;
    dest[7] = t6 - t4;
}

}

extern "C" void idctcompute(int *dest, int stride) {
    IdctColumn(idctinput, work);
    IdctColumn(idctinput + 8, work + 1);
    IdctColumn(idctinput + 16, work + 2);
    IdctColumn(idctinput + 24, work + 3);
    IdctColumn(idctinput + 32, work + 4);
    IdctColumn(idctinput + 40, work + 5);
    IdctColumn(idctinput + 48, work + 6);
    IdctColumn(idctinput + 56, work + 7);
    IdctRow(work, dest);
    IdctRow(work + 8, dest + stride);
    IdctRow(work + 16, dest + stride * 2);
    IdctRow(work + 24, dest + stride * 3);
    IdctRow(work + 32, dest + stride * 4);
    IdctRow(work + 40, dest + stride * 5);
    IdctRow(work + 48, dest + stride * 6);
    IdctRow(work + 56, dest + stride * 7);
}
