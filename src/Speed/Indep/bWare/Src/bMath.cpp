#include "Speed/Indep/bWare/Inc/bMath.hpp"

#if defined(EA_PLATFORM_WIN32)
extern "C" float TWOPI;
#endif
#include "Speed/Indep/bWare/Inc/bWare.hpp"

unsigned int bDefaultSeed = 0x12345678;

void bEndianSwap64(void *value) {
    int64 temp = *reinterpret_cast<int64 *>(value);
    *(reinterpret_cast<uint8 *>(value) + 7) = temp;
    *(reinterpret_cast<uint8 *>(value) + 6) = temp >> 8;
    *(reinterpret_cast<uint8 *>(value) + 5) = temp >> 16;
    *(reinterpret_cast<uint8 *>(value) + 4) = temp >> 24;
    *(reinterpret_cast<uint8 *>(value) + 3) = temp >> 32;
    *(reinterpret_cast<uint8 *>(value) + 2) = temp >> 40;
    *(reinterpret_cast<uint8 *>(value) + 1) = temp >> 48;
    *reinterpret_cast<uint8 *>(value) = temp >> 56;
}

void bEndianSwap32(void *value) {
    uint32 temp = *reinterpret_cast<uint32 *>(value);
    *(reinterpret_cast<uint8 *>(value) + 2) = temp >> 8;
    *(reinterpret_cast<uint8 *>(value) + 3) = temp;
    *(reinterpret_cast<uint8 *>(value) + 1) = temp >> 16;
    *reinterpret_cast<uint8 *>(value) = temp >> 24;
}

void bEndianSwap16(void *value) {
    uint16 temp = *reinterpret_cast<uint16 *>(value);
    *(reinterpret_cast<uint8 *>(value) + 1) = temp;
    *reinterpret_cast<uint8 *>(value) = temp >> 8;
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

bFix bInverse(bFix a) {
    if (a == 0) {
        return 0x7fffffff;
    }

    // The original fixed-point helper computes the reciprocal in Q16.15
    // form, then doubles it to compensate for the half-range constant used
    // by bDiv.  Keep the intermediate wide so the signed edge cases retain
    // the target's two's-complement result when narrowed back to bFix.
    int64 quotient = static_cast<int64>(0x7fffffff) / a;
    return static_cast<bFix>(static_cast<int32>(quotient * 2));
}

bFix bDiv(bFix a, bFix b) {
    if (b == 0) {
        if (a == 0) {
            return 0;
        }
        if (a > 0) {
            return 0x7fffffff;
        }
        return -0x80000000;
    } else {
        bFix half_inverse_b = 0x7fffffff / b;
        return bMult(a, half_inverse_b) * 2;
    }
}

uint32 bSqrt32(uint32 a) {
    uint32 result = 0;
    uint32 remainder = 0;
    int count = 16;

    do {
        remainder = (remainder << 2) + (a >> 30);
        result += result;
        if (result < remainder) {
            remainder -= result + 1;
            result += 2;
        }
        a <<= 2;
    } while (--count != 0);

    return result >> 1;
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
    unsigned int random = result % range;
    result ^= 0x1d872b41;
    unsigned int temp = result ^ (result >> 5);
    *seed = temp ^ (result ^ (temp << 0x1b));
    return random;
}

float bRandom(float range, unsigned int *seed) {
    return bRandom(0x7fffffff, seed) * range * 4.656613e-10f;
}

unsigned int bRandom(int range) {
    return bRandom(range, &bDefaultSeed);
}

float bRandom(float range) {
    return bRandom(0x7fffffff, &bDefaultSeed) * range * 4.656613e-10f;
}

float bFMod(float a, float b) {
    float d = bAbs(b);
    float c = a / d;
    return (c - bFloor(c)) * d;
}

float bSin(bAngle angle) {
    float a = bAngToRad(angle);
    float flip_sign = 1.0f;
    const float pi = 3.1415927f;

    if (a >= 4.712389f) {
        a -= 6.2831855f;
    } else if (a >= 1.5707964f) {
        a -= pi;
        flip_sign = -1.0f;
    }

    float result_sin = a;
    float a2 = a * a;

    float a3 = a * a2;
    result_sin -= a3 * 0.16666667f;

    float a5 = a3 * a2;
    result_sin += a5 * 0.008333334f;

    float a7 = a5 * a2;
    result_sin -= a7 * 0.0001984127f;

    float a9 = a7 * a2;
    result_sin += a9 * 0.0000027557319f;

    return result_sin * flip_sign;
}

float bSin(float angle) {
    return bSin(bRadToAng(angle));
}

float bCos(bAngle angle) {
    return bSin(static_cast<bAngle>(angle + bDegToAng(90.0f)));
}

void bSinCos(float *presult_sin, float *presult_cos, bAngle angle) {
    float a = bAngToRad(angle);
    float flip_sign = 1.0f;
    const float pi = 3.1415927f;

    if (a >= 4.712389f) {
#if defined(EA_PLATFORM_WIN32)
        a -= TWOPI;
#else
        a -= 2.0f * pi;
#endif
    } else if (a >= 1.5707964f) {
        flip_sign = -flip_sign;
        a -= pi;
    }

    float result_sin = a;
    float result_cos = 1.0f;
    float a2 = a * a;
    float a3 = a * a2;
    result_cos -= a2 * 0.5f;
    result_sin -= a3 * 0.16666667f;
    float a4 = a2 * a2;
    float a5 = a3 * a2;
    result_cos += a4 * 0.041666668f;
    result_sin += a5 * 0.008333334f;
    float a6 = a4 * a2;
    float a7 = a5 * a2;
    result_cos -= a6 * 0.0013888889f;
    result_sin -= a7 * 0.0001984127f;
    float a8 = a6 * a2;
    float a9 = a7 * a2;
    result_cos += a8 * 0.000024801588f;
    result_sin += a9 * 0.0000027557319f;
    float a10 = a8 * a2;
    result_cos -= a10 * 0.00000027557319f;

    *presult_sin = result_sin * flip_sign;
    *presult_cos = result_cos * flip_sign;
}

struct ASinTableEntry {
    // total size: 0x8
    bAngle Angle; // offset 0x0, size 0x2
    float Slope;  // offset 0x4, size 0x4
};

ASinTableEntry bASinTable[209] = {
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

// Retail SPEED.EXE stores this 0x402-byte Q15 sine table immediately after
// bASinTable. bFixSin doubles the samples to return the engine's Q16 value.
static const int16 bSinTable[513] = {
    0, 402, 804, 1206, 1607, 2009, 2410, 2811, 3211, 3611, 4011, 4409, 4807, 5205, 5601, 5997,
    6392, 6786, 7179, 7571, 7961, 8351, 8739, 9126, 9511, 9895, 10278, 10659, 11038, 11416, 11792, 12166,
    12539, 12909, 13278, 13645, 14009, 14372, 14732, 15090, 15446, 15799, 16150, 16499, 16845, 17189, 17530, 17868,
    18204, 18537, 18867, 19194, 19519, 19840, 20159, 20474, 20787, 21096, 21402, 21705, 22005, 22301, 22594, 22883,
    23169, 23452, 23731, 24007, 24278, 24547, 24811, 25072, 25329, 25582, 25831, 26077, 26318, 26556, 26790, 27019,
    27245, 27466, 27683, 27896, 28105, 28310, 28510, 28706, 28898, 29085, 29268, 29447, 29621, 29791, 29956, 30117,
    30273, 30424, 30571, 30714, 30852, 30985, 31113, 31237, 31356, 31471, 31580, 31685, 31785, 31880, 31971, 32057,
    32138, 32214, 32285, 32351, 32413, 32469, 32521, 32568, 32610, 32647, 32679, 32706, 32728, 32745, 32758, 32765,
    32767, 32765, 32758, 32745, 32728, 32706, 32679, 32647, 32610, 32568, 32521, 32470, 32413, 32352, 32285, 32214,
    32138, 32057, 31972, 31881, 31786, 31686, 31581, 31471, 31357, 31238, 31114, 30986, 30853, 30715, 30572, 30425,
    30274, 30118, 29957, 29792, 29622, 29448, 29270, 29087, 28899, 28708, 28512, 28311, 28107, 27898, 27685, 27468,
    27246, 27021, 26791, 26558, 26320, 26079, 25833, 25584, 25331, 25074, 24813, 24549, 24280, 24009, 23733, 23454,
    23172, 22886, 22596, 22303, 22007, 21707, 21404, 21098, 20789, 20477, 20161, 19843, 19521, 19197, 18870, 18539,
    18206, 17871, 17532, 17191, 16848, 16502, 16153, 15802, 15448, 15093, 14735, 14374, 14012, 13647, 13281, 12912,
    12542, 12169, 11795, 11419, 11041, 10662, 10281, 9898, 9514, 9129, 8742, 8354, 7964, 7574, 7182, 6789,
    6395, 6000, 5604, 5208, 4810, 4412, 4014, 3614, 3214, 2814, 2413, 2012, 1610, 1209, 807, 405,
    3, -399, -801, -1203, -1605, -2006, -2407, -2808, -3209, -3609, -4008, -4407, -4805, -5202, -5599, -5995,
    -6390, -6783, -7176, -7568, -7959, -8348, -8736, -9123, -9509, -9893, -10276, -10657, -11036, -11414, -11790, -12164,
    -12537, -12907, -13276, -13642, -14007, -14369, -14730, -15088, -15444, -15797, -16148, -16497, -16843, -17187, -17528, -17866,
    -18202, -18535, -18865, -19192, -19517, -19838, -20157, -20473, -20785, -21094, -21400, -21703, -22003, -22299, -22592, -22882,
    -23168, -23450, -23729, -24005, -24277, -24545, -24810, -25071, -25328, -25581, -25830, -26076, -26317, -26555, -26788, -27018,
    -27243, -27465, -27682, -27895, -28104, -28309, -28509, -28705, -28897, -29084, -29267, -29446, -29620, -29790, -29955, -30116,
    -30272, -30424, -30571, -30713, -30851, -30984, -31113, -31237, -31356, -31470, -31580, -31685, -31785, -31880, -31971, -32057,
    -32138, -32214, -32285, -32351, -32413, -32469, -32521, -32568, -32610, -32647, -32679, -32706, -32728, -32746, -32758, -32765,
    -32768, -32766, -32758, -32746, -32729, -32707, -32680, -32648, -32611, -32569, -32522, -32471, -32414, -32353, -32286, -32215,
    -32139, -32058, -31973, -31882, -31787, -31687, -31582, -31473, -31358, -31239, -31116, -30987, -30854, -30716, -30574, -30427,
    -30276, -30119, -29959, -29794, -29624, -29450, -29271, -29088, -28901, -28709, -28514, -28313, -28109, -27900, -27687, -27470,
    -27248, -27023, -26794, -26560, -26323, -26081, -25836, -25586, -25333, -25076, -24816, -24551, -24283, -24011, -23736, -23457,
    -23174, -22888, -22599, -22306, -22010, -21710, -21407, -21101, -20792, -20480, -20164, -19846, -19524, -19200, -18873, -18542,
    -18210, -17874, -17535, -17194, -16851, -16505, -16156, -15805, -15452, -15096, -14738, -14378, -14015, -13651, -13284, -12916,
    -12545, -12173, -11798, -11422, -11045, -10665, -10284, -9902, -9518, -9132, -8745, -8357, -7968, -7577, -7185, -6792,
    -6398, -6004, -5608, -5211, -4814, -4416, -4017, -3618, -3218, -2817, -2417, -2015, -1614, -1212, -810, -408,
    0,
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

// UNSOLVED, matches in ProStreet
bAngle bASin(float x) {
    int negative = 0;
    if (x < 0.0f) {
        x = -x;
        negative = 1;
    }
    if (x >= 1.0f) {
        if (negative) {
            return 0xC000;
        } else {
            return 0x4000;
        }
    }

    bFix fix_x = static_cast<int>(x * 65536.0f);
    int table_number = 0;     // r7
    bFix table_size = 0x8000; // r8
    bFix table_top = 0x8000;  // r0

    while (fix_x >= table_top && table_number < 11) {
        table_size >>= 1;
        table_number++;
        table_top += table_size;
    }

    bFix table_bottom = table_top - table_size;                                  // r0
    int table_index = (fix_x - table_bottom) >> (11 - table_number);             // r10
    bFix table_spacing = table_number * 16 + table_index;                        // r8
    float table_x = (table_bottom + table_index * (table_size >> 4)) / 65536.0f; // f0
    float remainder_x = x - table_x;                                             // f0
    table_spacing <<= 3;
    bAngle table_a = *reinterpret_cast<bAngle *>(reinterpret_cast<char *>(bASinTable) + table_spacing);
    float slope = *reinterpret_cast<float *>(reinterpret_cast<char *>(bASinTable) + table_spacing + 4); // f11
    bAngle a = table_a + static_cast<int>(remainder_x * slope * 65536.0f);

    if (negative) {
        return 0x10000 - a;
    } else {
        return a;
    }
}

// The legacy atan path uses the same 257-entry table as bATan, but selects a
// single 1/256 interval instead of interpolating between adjacent entries.
bAngle bOldATan(float x, float y) {
    int quad = 0;
    if (x < 0.0f) {
        quad = 1;
        x = -x;
    }
    if (y < 0.0f) {
        quad ^= 3;
        y = -y;
    }

    bAngle a;
    if (x > y) {
        const int index = static_cast<int>((y / x) * 256.0f);
        a = bFastATanTable[index];
    } else if (y > x) {
        const int index = static_cast<int>((x / y) * 256.0f);
        a = bDegToAng(90.0f) - bFastATanTable[index];
    } else if (y == 0.0f) {
        a = 0;
    } else {
        a = bDegToAng(45.0f);
    }

    if (quad == 0) {
        return a;
    } else if (quad == 3) {
        return -a;
    } else if (quad == 1) {
        return bDegToAng(180.0f) - a;
    }
    return bDegToAng(180.0f) + a;
}

// The retail PC build's public atan entry point is the interpolating fast
// path. Keep the legacy name as a source-level alias for callers that still
// reference it in platform-neutral code.
bAngle bFastATan(float x, float y) {
    return bATan(x, y);
}

// Credit: Brawltendo
bAngle bATan(float x, float y) {
    int quad = 0;
    if (x < 0.0f) {
        quad = 1;
        x = -x;
    }

    if (y < 0.0f) {
        quad ^= 3;
        y = -y;
    }

    bAngle a;
    if (x > y) {
        float r = y;
        int i = static_cast<int>((r / x) * 65536.0f);
        const bAngle *table = &bFastATanTable[i >> 8];
        a = (table[0] + (((table[1] - table[0]) * (i & 0xFF)) >> 8));
    } else {
        if (y > x) {
            float r = y;
            int i = static_cast<int>((x / r) * 65536.0f);
            const bAngle *table = &bFastATanTable[i >> 8];
            a = bDegToAng(90.0f) - (table[0] + (((table[1] - table[0]) * (i & 0xFF)) >> 8));
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

bFix bFixSin(bAngle angle) {
    const unsigned int index = angle >> 7;
    const int32 lower = static_cast<int32>(bSinTable[index]) << 1;
    const int32 upper = static_cast<int32>(bSinTable[index + 1]) << 1;
    const int32 fraction = static_cast<int32>(angle & 0x7f) << 9;
    return lower + static_cast<int32>((static_cast<int64>(upper - lower) * fraction) >> 16);
}

void bFixSinCos(bFix *result_sin, bFix *result_cos, bAngle angle) {
    *result_sin = bFixSin(angle);
    *result_cos = bFixSin(static_cast<bAngle>(angle + 0x4000));
}

bAngle bFixATan(bFix x) {
    int quad = 0;
    if (x < 0) {
        quad = 1;
        x = -x;
    }

    bAngle a;
    bAngle b;
    bFix interpolation_ratio;

    if (x < 0x200000) {
        bAngle *table_entry;

        if (x < 0x40000) {
            table_entry = bFixATanTableLow + (x >> 0xb);
            interpolation_ratio = (x << 5) & 0xffff;
        } else {
            table_entry = bFixATanTableHigh + (x >> 0xe);
            interpolation_ratio = (x << 2) & 0xffff;
        }

        a = table_entry[0];
        b = table_entry[1];
    } else if (x < 0x1000000) {
        interpolation_ratio = x >> 8;
        a = bDegToAng(88.21f);
        b = bDegToAng(89.78f);
    } else {
        a = 0x3fff;
        b = 0x3fff;
        interpolation_ratio = 0;
    }

    bFix accurate_answer = a + (((b - a) * interpolation_ratio) >> 16);
    if (quad) {
        return static_cast<bAngle>(-accurate_answer);
    } else {
        return static_cast<bAngle>(accurate_answer);
    }
}

bAngle bFixATan(bFix x, bFix y) {
    int quad = 0;
    if (x < 0) {
        quad = 1;
        x = -x;
    }

    if (y < 0) {
        quad ^= 3;
        y = -y;
    }

    bFix a;
    if (x <= (y >> 14)) {
        a = 0x4000;
    } else {
        bFix r = bFixATan(bDiv(y, x));
        a = r;
    }

    switch (quad) {
        case 1:
            a = -0x8000 - a;
            break;
        case 0:
            break;
        case 2:
            a += -0x8000;
            break;
        default:
            a = -a;
            break;
    }

    return static_cast<bAngle>(a);
}

bPolar *bToPolar(bPolar *dest, bVector2 *cartesian) {
    float x = cartesian->x;
    float y = cartesian->y;
    dest->a = bATan(cartesian->x, cartesian->y);
    dest->r = bSqrt(y * y + x * x);
    return dest;
}

void bConvertToBond(bMatrix4 &dest, const bMatrix4 &m) {
    float v1x = m.v1.y;
    float v1y = m.v1.z;
    float v1z = m.v1.x;
    float v1w = m.v1.w;

    bConvertToBond(dest.v1, m.v2);
    bConvertToBond(dest.v2, m.v0);

    dest.v0.x = v1x;
    dest.v0.y = -v1y;
    dest.v0.z = -v1z;
    dest.v0.w = v1w;

    bConvertToBond(dest.v3, m.v3);
}

void bConvertFromBond(bMatrix4 &dest, const bMatrix4 &m) {
    float v0x = m.v0.z;
    float v0y = m.v0.x;
    float v0z = m.v0.y;
    float v0w = m.v0.w;

    bConvertFromBond(dest.v0, m.v2);
    bConvertFromBond(dest.v2, m.v1);

    dest.v1.x = -v0x;
    dest.v1.y = v0y;
    dest.v1.z = -v0z;
    dest.v1.w = v0w;

    bConvertFromBond(dest.v3, m.v3);
}

void bMathTimingTest() {}
