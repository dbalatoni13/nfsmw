#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

unsigned int bDefaultSeed = 0x12345678;

void bEndianSwap64(void *value) {
    int64_t temp = *reinterpret_cast<int64_t *>(value);
    *reinterpret_cast<uint8_t *>(value) = temp;
    *(reinterpret_cast<uint8_t *>(value) + 1) = temp >> 8;
    *(reinterpret_cast<uint8_t *>(value) + 2) = temp >> 16;
    *(reinterpret_cast<uint8_t *>(value) + 3) = temp >> 24;
    *(reinterpret_cast<uint8_t *>(value) + 4) = temp >> 32;
    *(reinterpret_cast<uint8_t *>(value) + 5) = temp >> 40;
    *(reinterpret_cast<uint8_t *>(value) + 6) = temp >> 48;
    *(reinterpret_cast<uint8_t *>(value) + 7) = temp >> 56;
}

void bEndianSwap32(void *value) {
    uint32_t temp = *reinterpret_cast<uint32_t *>(value);
    *reinterpret_cast<uint8_t *>(value) = temp;
    *(reinterpret_cast<uint8_t *>(value) + 1) = temp >> 8;
    *(reinterpret_cast<uint8_t *>(value) + 2) = temp >> 16;
    *(reinterpret_cast<uint8_t *>(value) + 3) = temp >> 24;
}

void bEndianSwap16(void *value) {
    uint16_t temp = *reinterpret_cast<uint16_t *>(value);
    *reinterpret_cast<uint8_t *>(value) = temp;
    *(reinterpret_cast<uint8_t *>(value) + 1) = temp >> 8;
}

void bPlatEndianSwap(bVector2 *value) {
    bPlatEndianSwap(&value->x);
    bPlatEndianSwap(&value->y);
}

void bPlatEndianSwap(bVector3 *value) {
    bPlatEndianSwap(&value->x);
    bPlatEndianSwap(&value->y);
    bPlatEndianSwap(&value->z);
}

void bPlatEndianSwap(bVector4 *value) {
    bPlatEndianSwap(&value->x);
    bPlatEndianSwap(&value->y);
    bPlatEndianSwap(&value->z);
    bPlatEndianSwap(&value->w);
}

void bPlatEndianSwap(bMatrix4 *value) {
    bPlatEndianSwap(&value->v0);
    bPlatEndianSwap(&value->v1);
    bPlatEndianSwap(&value->v2);
    bPlatEndianSwap(&value->v3);
}

// UNSOLVED
int bDiv(int a, int b) {
    if (b == 0) {
        if (a == 0) {
            return 0;
        }
        if (a > 0) {
            return 0x7fffffff;
        }
        return -0x80000000;
    } else {
        int half_inverse_b = 0x7fffffff / b;
        return ((((long long)a * half_inverse_b) >> 32) << 16 | (unsigned int)((long long)a * half_inverse_b) >> 16) << 1;
    }
}

void bSetRandomSeed(unsigned int value, unsigned int *seed) {
    *seed = value;
    bRandom(1, seed);
}

unsigned int bRandom(int range, unsigned int *seed) {
    if (range == 0) {
        return 0;
    }
    unsigned int result = *seed;
    unsigned int next = result ^ 0x1d872b41;
    unsigned int temp = next ^ (next >> 5);
    *seed = temp ^ (next ^ (temp << 0x1b));
    return result - (result / range) * range;
}

float bRandom(float range, unsigned int *seed) {
    return range * 4.656613e-10f * bRandom(0x7fffffff, seed);
}

unsigned int bRandom(int range) {
    return bRandom(range, &bDefaultSeed);
}

float bRandom(float range) {
    return bRandom(range, &bDefaultSeed);
}

// UNSOLVED
float bFMod(float a, float b) {
    float d = bAbs(b);
    float c = a / d;
    return (c - bFloor(c)) * d;
}

float bSin(unsigned short angle);

float bSin(float angle) {
    return bSin(bRadToAng(angle));
}

float bCos(unsigned short angle) {
    return bSin(static_cast<unsigned short>(angle + bDegToAng(90.0f)));
}

struct ASinTableEntry {
    // total size: 0x8
    unsigned short Angle; // offset 0x0, size 0x2
    float Slope;          // offset 0x4, size 0x4
};

struct ASinTableEntry bASinTable[209] = {
    {0, 0.1592f},       {0x146, 0.1593f},   {0x28C, 0.1596f},  {0x3D3, 0.1601f},   {0x51B, 0.1608f},   {0x664, 0.1616f},   {0x7AF, 0.1625f},
    {0x8FC, 0.1637f},   {0xA4B, 0.1651f},   {0xB9D, 0.1667f},  {0xCF3, 0.1685f},   {0xE4C, 0.1705f},   {0xFA9, 0.1729f},   {0x110B, 0.1756f},
    {0x1273, 0.1785f},  {0x13E0, 0.1819f},  {0x1555, 0.1848f}, {0x1612, 0.1868f},  {0x16D1, 0.189f},   {0x1793, 0.1913f},  {0x1857, 0.1938f},
    {0x191D, 0.1964f},  {0x19E6, 0.1992f},  {0x1AB2, 0.2023f}, {0x1B81, 0.2056f},  {0x1C54, 0.2091f},  {0x1D2A, 0.2129f},  {0x1E04, 0.217f},
    {0x1EE2, 0.2215f},  {0x1FC5, 0.2263f},  {0x20AD, 0.2316f}, {0x219A, 0.2375f},  {0x228D, 0.2422f},  {0x2309, 0.2457f},  {0x2387, 0.2492f},
    {0x2407, 0.253f},   {0x2488, 0.257f},   {0x250C, 0.2612f}, {0x2591, 0.2657f},  {0x2619, 0.2705f},  {0x26A4, 0.2756f},  {0x2731, 0.2811f},
    {0x27C1, 0.2869f},  {0x2854, 0.2932f},  {0x28EA, 0.3f},    {0x2984, 0.3074f},  {0x2A21, 0.3154f},  {0x2AC2, 0.3241f},  {0x2B68, 0.3312f},
    {0x2BBD, 0.3362f},  {0x2C13, 0.3415f},  {0x2C6B, 0.3471f}, {0x2CC4, 0.353f},   {0x2D1E, 0.3592f},  {0x2D7A, 0.3658f},  {0x2DD8, 0.3728f},
    {0x2E37, 0.3803f},  {0x2E98, 0.3882f},  {0x2EFC, 0.3968f}, {0x2F61, 0.4059f},  {0x2FC9, 0.4158f},  {0x3034, 0.4264f},  {0x30A1, 0.438f},
    {0x3111, 0.4507f},  {0x3184, 0.4609f},  {0x31BF, 0.4681f}, {0x31FB, 0.4758f},  {0x3238, 0.4838f},  {0x3276, 0.4923f},  {0x32B5, 0.5012f},
    {0x32F5, 0.5107f},  {0x3337, 0.5208f},  {0x3379, 0.5314f}, {0x33BD, 0.5429f},  {0x3403, 0.5551f},  {0x344A, 0.5682f},  {0x3493, 0.5823f},
    {0x34DD, 0.5975f},  {0x352A, 0.614f},   {0x3578, 0.6321f}, {0x35C9, 0.6467f},  {0x35F2, 0.657f},   {0x361D, 0.6679f},  {0x3647, 0.6793f},
    {0x3673, 0.6914f},  {0x369F, 0.7042f},  {0x36CC, 0.7177f}, {0x36FA, 0.732f},   {0x3729, 0.7472f},  {0x3759, 0.7635f},  {0x378A, 0.7808f},
    {0x37BB, 0.7995f},  {0x37EF, 0.8195f},  {0x3823, 0.8412f}, {0x3859, 0.8647f},  {0x3890, 0.8903f},  {0x38C9, 0.911f},   {0x38E6, 0.9257f},
    {0x3904, 0.9411f},  {0x3922, 0.9574f},  {0x3941, 0.9745f}, {0x3960, 0.9926f},  {0x3980, 1.0117f},  {0x39A0, 1.0321f},  {0x39C1, 1.0537f},
    {0x39E3, 1.0767f},  {0x3A05, 1.1013f},  {0x3A29, 1.1277f}, {0x3A4D, 1.1562f},  {0x3A72, 1.1869f},  {0x3A98, 1.2202f},  {0x3ABF, 1.2565f},
    {0x3AE7, 1.2858f},  {0x3AFB, 1.3066f},  {0x3B10, 1.3285f}, {0x3B26, 1.3515f},  {0x3B3B, 1.3758f},  {0x3B51, 1.4015f},  {0x3B68, 1.4286f},
    {0x3B7F, 1.4574f},  {0x3B96, 1.488f},   {0x3BAE, 1.5206f}, {0x3BC6, 1.5555f},  {0x3BDF, 1.5929f},  {0x3BF8, 1.6331f},  {0x3C12, 1.6766f},
    {0x3C2D, 1.7237f},  {0x3C49, 1.7751f},  {0x3C65, 1.8167f}, {0x3C74, 1.8461f},  {0x3C83, 1.8771f},  {0x3C92, 1.9097f},  {0x3CA1, 1.9441f},
    {0x3CB0, 1.9804f},  {0x3CC0, 2.0188f},  {0x3CD0, 2.0595f}, {0x3CE1, 2.1028f},  {0x3CF2, 2.149f},   {0x3D03, 2.1983f},  {0x3D14, 2.2513f},
    {0x3D27, 2.3082f},  {0x3D39, 2.3697f},  {0x3D4C, 2.4364f}, {0x3D5F, 2.5091f},  {0x3D73, 2.5679f},  {0x3D7E, 2.6096f},  {0x3D88, 2.6535f},
    {0x3D93, 2.6996f},  {0x3D9E, 2.7482f},  {0x3DA9, 2.7995f}, {0x3DB4, 2.8539f},  {0x3DBF, 2.9115f},  {0x3DCB, 2.9728f},  {0x3DD7, 3.0381f},
    {0x3DE3, 3.1079f},  {0x3DEF, 3.1828f},  {0x3DFC, 3.2633f}, {0x3E09, 3.3504f},  {0x3E17, 3.4447f},  {0x3E24, 3.5476f},  {0x3E33, 3.6307f},
    {0x3E3A, 3.6897f},  {0x3E41, 3.7517f},  {0x3E49, 3.817f},  {0x3E50, 3.8857f},  {0x3E58, 3.9583f},  {0x3E60, 4.0352f},  {0x3E68, 4.1167f},
    {0x3E70, 4.2034f},  {0x3E79, 4.2958f},  {0x3E81, 4.3945f}, {0x3E8A, 4.5004f},  {0x3E93, 4.6144f},  {0x3E9C, 4.7374f},  {0x3EA6, 4.8709f},
    {0x3EB0, 5.0164f},  {0x3EBA, 5.134f},   {0x3EBF, 5.2175f}, {0x3EC4, 5.3052f},  {0x3EC9, 5.3974f},  {0x3ECF, 5.4947f},  {0x3ED4, 5.5974f},
    {0x3EDA, 5.7061f},  {0x3EDF, 5.8214f},  {0x3EE5, 5.9439f}, {0x3EEB, 6.0746f},  {0x3EF1, 6.2143f},  {0x3EF7, 6.3641f},  {0x3EFE, 6.5252f},
    {0x3F04, 6.6993f},  {0x3F0B, 6.8881f},  {0x3F12, 7.0938f}, {0x3F19, 7.3192f},  {0x3F20, 7.5674f},  {0x3F28, 7.8429f},  {0x3F30, 8.1507f},
    {0x3F38, 8.498f},   {0x3F40, 8.8939f},  {0x3F49, 9.3508f}, {0x3F53, 9.8864f},  {0x3F5D, 10.5261f}, {0x3F67, 11.3089f}, {0x3F72, 12.2977f},
    {0x3F7F, 13.6025f}, {0x3F8C, 15.4395f}, {0x3F9C, 18.314f}, {0x3FAE, 23.8672f}, {0x3FC6, 57.6203f}, {0x3FFF, -0.0003f},
};

static unsigned short bFastATanTable[] = {
    0,    41,   81,   122,  163,  204,  244,  285,  326,  367,  407,  448,  489,  529,  570,  610,  651,  692,  732,  773,  813,  854,  894,  935,
    975,  1015, 1056, 1096, 1136, 1177, 1217, 1257, 1297, 1337, 1377, 1417, 1457, 1497, 1537, 1577, 1617, 1656, 1696, 1736, 1775, 1815, 1854, 1894,
    1933, 1973, 2012, 2051, 2090, 2129, 2168, 2207, 2246, 2285, 2324, 2363, 2401, 2440, 2478, 2517, 2555, 2594, 2632, 2670, 2708, 2746, 2784, 2822,
    2860, 2897, 2935, 2973, 3010, 3047, 3085, 3122, 3159, 3196, 3233, 3270, 3307, 3344, 3380, 3417, 3453, 3490, 3526, 3562, 3599, 3635, 3670, 3706,
    3742, 3778, 3813, 3849, 3884, 3920, 3955, 3990, 4025, 4060, 4095, 4129, 4164, 4199, 4233, 4267, 4302, 4336, 4370, 4404, 4438, 4471, 4505, 4539,
    4572, 4605, 4639, 4672, 4705, 4738, 4771, 4803, 4836, 4869, 4901, 4933, 4966, 4998, 5030, 5062, 5094, 5125, 5157, 5188, 5220, 5251, 5282, 5313,
    5344, 5375, 5406, 5437, 5467, 5498, 5528, 5559, 5589, 5619, 5649, 5679, 5708, 5738, 5768, 5797, 5826, 5856, 5885, 5914, 5943, 5972, 6000, 6029,
    6058, 6086, 6114, 6142, 6171, 6199, 6227, 6254, 6282, 6310, 6337, 6365, 6392, 6419, 6446, 6473, 6500, 6527, 6554, 6580, 6607, 6633, 6660, 6686,
    6712, 6738, 6764, 6790, 6815, 6841, 6867, 6892, 6917, 6943, 6968, 6993, 7018, 7043, 7068, 7092, 7117, 7141, 7166, 7190, 7214, 7238, 7262, 7286,
    7310, 7334, 7358, 7381, 7405, 7428, 7451, 7475, 7498, 7521, 7544, 7566, 7589, 7612, 7635, 7657, 7679, 7702, 7724, 7746, 7768, 7790, 7812, 7834,
    7856, 7877, 7899, 7920, 7942, 7963, 7984, 8005, 8026, 8047, 8068, 8089, 8110, 8131, 8151, 8172, 8192, 8192,
};

unsigned short bFixATanTableLow[129] = {
    0,      0x145,  0x28B,  0x3CE,  0x511,  0x650,  0x78D,  0x8C6,  0x9FB,  0xB2B,  0xC57,  0xD7D,  0xE9E,  0xFB8,  0x10CD, 0x11DC, 0x12E4,
    0x13E5, 0x14E0, 0x15D4, 0x16C2, 0x17A9, 0x188A, 0x1964, 0x1A37, 0x1B05, 0x1BCC, 0x1C8E, 0x1D49, 0x1DFF, 0x1EAF, 0x1F5A, 0x1FFF, 0x20A0,
    0x213B, 0x21D2, 0x2264, 0x22F2, 0x237B, 0x2401, 0x2482, 0x24FF, 0x2579, 0x25EE, 0x2661, 0x26D0, 0x273C, 0x27A5, 0x280A, 0x286D, 0x28CD,
    0x292B, 0x2986, 0x29DE, 0x2A34, 0x2A87, 0x2AD9, 0x2B28, 0x2B75, 0x2BC0, 0x2C09, 0x2C50, 0x2C96, 0x2CD9, 0x2D1B, 0x2D5C, 0x2D9B, 0x2DD8,
    0x2E14, 0x2E4E, 0x2E87, 0x2EBF, 0x2EF5, 0x2F2A, 0x2F5E, 0x2F91, 0x2FC3, 0x2FF3, 0x3023, 0x3051, 0x307F, 0x30AB, 0x30D7, 0x3101, 0x312B,
    0x3154, 0x317C, 0x31A3, 0x31CA, 0x31EF, 0x3214, 0x3239, 0x325C, 0x327F, 0x32A1, 0x32C3, 0x32E4, 0x3304, 0x3324, 0x3343, 0x3361, 0x337F,
    0x339D, 0x33BA, 0x33D6, 0x33F2, 0x340D, 0x3428, 0x3443, 0x345D, 0x3477, 0x3490, 0x34A9, 0x34C1, 0x34D9, 0x34F1, 0x3508, 0x351F, 0x3535,
    0x354C, 0x3561, 0x3577, 0x358C, 0x35A1, 0x35B5, 0x35C9, 0x35DD, 0x35F1, 0x3604,
};

unsigned short bFixATanTableHigh[129] = {
    0,      0x9FB,  0x12E4, 0x1A37, 0x1FFF, 0x2482, 0x280A, 0x2AD9, 0x2D1B, 0x2EF5, 0x307F, 0x31CA, 0x32E4, 0x33D6, 0x34A9, 0x3561, 0x3604,
    0x3695, 0x3717, 0x378B, 0x37F5, 0x3854, 0x38AC, 0x38FB, 0x3945, 0x3989, 0x39C7, 0x3A01, 0x3A37, 0x3A6A, 0x3A99, 0x3AC5, 0x3AEE, 0x3B15,
    0x3B3A, 0x3B5D, 0x3B7D, 0x3B9C, 0x3BBA, 0x3BD5, 0x3BF0, 0x3C09, 0x3C21, 0x3C38, 0x3C4E, 0x3C63, 0x3C77, 0x3C8A, 0x3C9C, 0x3CAE, 0x3CBF,
    0x3CCF, 0x3CDF, 0x3CEE, 0x3CFC, 0x3D0A, 0x3D18, 0x3D25, 0x3D31, 0x3D3D, 0x3D49, 0x3D55, 0x3D60, 0x3D6A, 0x3D74, 0x3D7E, 0x3D88, 0x3D92,
    0x3D9B, 0x3DA4, 0x3DAC, 0x3DB4, 0x3DBD, 0x3DC5, 0x3DCC, 0x3DD4, 0x3DDB, 0x3DE2, 0x3DE9, 0x3DF0, 0x3DF6, 0x3DFD, 0x3E03, 0x3E09, 0x3E0F,
    0x3E15, 0x3E1B, 0x3E20, 0x3E26, 0x3E2B, 0x3E30, 0x3E35, 0x3E3A, 0x3E3F, 0x3E44, 0x3E49, 0x3E4D, 0x3E52, 0x3E56, 0x3E5A, 0x3E5F, 0x3E63,
    0x3E67, 0x3E6B, 0x3E6F, 0x3E72, 0x3E76, 0x3E7A, 0x3E7D, 0x3E81, 0x3E84, 0x3E88, 0x3E8B, 0x3E8E, 0x3E92, 0x3E95, 0x3E98, 0x3E9B, 0x3E9E,
    0x3EA1, 0x3EA4, 0x3EA7, 0x3EAA, 0x3EAC, 0x3EAF, 0x3EB2, 0x3EB4, 0x3EB7, 0x3EBA,
};

// Credit: Brawltendo
unsigned short bATan(float x, float y) {
    int quad = 0;
    if (x < 0.0f) {
        quad = 1;
        x = -x;
    }

    if (y < 0.0f) {
        quad ^= 3;
        y = -y;
    }

    unsigned short a;
    if (x > y) {
        float r = y;
        int i = static_cast<int>((r / x) * 65536.0f);
        const unsigned short *table = &bFastATanTable[i >> 8];
        a = (table[0] + (((table[1] - table[0]) * (i & 0xFF)) >> 8));
    } else {
        if (y > x) {
            float r = y;
            int i = static_cast<int>((x / r) * 65536.0f);
            const unsigned short *table = &bFastATanTable[i >> 8];
            a = bDegToAng(90.0f) - (((table[1] - table[0]) * (i & 0xFF)) >> 8) - table[0];
        } else if (y == 0.0f) {
            a = 0;
        } else {
            a = bDegToAng(45.0f);
        }
    }

    if (quad == 0)
        return a;
    else if (quad == 3)
        return -a;
    else if (quad == 1)
        return bDegToAng(180.0f) - a;
    else
        return bDegToAng(180.0f) + a;
}

void bMathTimingTest() {}

void bInvertMatrix(bMatrix4 *dest, const bMatrix4 *src) {
    eInvertMatrix(dest, const_cast<bMatrix4 *>(src));
}

// UNSOLVED
float fDeterminant(bMatrix4 *m) {
    float value =
        m->v0.x * m->v1.y * m->v2.z * m->v3.w +
        (((m->v0.z * m->v1.x * m->v2.y * m->v3.w + m->v0.y * m->v1.z * m->v2.x * m->v3.w +
           (((m->v0.y * m->v1.x * m->v2.w * m->v3.z + m->v0.x * m->v1.w * m->v2.y * m->v3.z +
              (((m->v0.w * m->v1.y * m->v2.x * m->v3.z + m->v0.x * m->v1.z * m->v2.w * m->v3.y +
                 (((m->v0.w * m->v1.x * m->v2.z * m->v3.y + m->v0.z * m->v1.w * m->v2.x * m->v3.y +
                    (((m->v0.z * m->v1.y * m->v2.w * m->v3.x + m->v0.y * m->v1.w * m->v2.z * m->v3.x +
                       ((m->v0.w * m->v1.z * m->v2.y * m->v3.x - m->v0.z * m->v1.w * m->v2.y * m->v3.x) - m->v0.w * m->v1.y * m->v2.z * m->v3.x)) -
                      m->v0.y * m->v1.z * m->v2.w * m->v3.x) -
                     m->v0.w * m->v1.z * m->v2.x * m->v3.y)) -
                   m->v0.x * m->v1.w * m->v2.z * m->v3.y) -
                  m->v0.z * m->v1.x * m->v2.w * m->v3.y)) -
                m->v0.y * m->v1.w * m->v2.x * m->v3.z) -
               m->v0.w * m->v1.x * m->v2.y * m->v3.z)) -
             m->v0.x * m->v1.y * m->v2.w * m->v3.z) -
            m->v0.z * m->v1.y * m->v2.x * m->v3.w)) -
          m->v0.x * m->v1.z * m->v2.y * m->v3.w) -
         m->v0.y * m->v1.x * m->v2.z * m->v3.w);

    return value;
}

void hermite_parameter(bVector4 *dest, const bMatrix4 *b, float t) {
    bVector4 u;

    u.x = t * t * t;
    u.y = t * t;
    u.z = t;
    u.w = 1.0f;
    eMulVector(dest, b, &u);
}

void bMulMatrix(bMatrix4 *dest, const bMatrix4 *a, const bMatrix4 *b) {
    eMulMatrix(dest, const_cast<bMatrix4 *>(b), const_cast<bMatrix4 *>(a));
}

void bMulMatrix(bVector4 *dest, const bMatrix4 *m, const bVector4 *v) {
    eMulVector(dest, m, v);
}

void bMulMatrix(bVector3 *dest, const bMatrix4 *m, const bVector3 *v) {
    eMulVector(dest, m, v);
}

bMatrix4 *bTransposeMatrix(bMatrix4 *dest, const bMatrix4 *m) {
    MTX44Transpose(*reinterpret_cast<const Mtx44 *>(m), *reinterpret_cast<Mtx44 *>(dest));
    return dest;
}
