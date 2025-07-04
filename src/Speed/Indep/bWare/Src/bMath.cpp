#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

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

float bSin(unsigned short angle);

float bSin(float angle) {
    return bSin(bRadToAng(angle));
}

float bCos(unsigned short angle) {
    return bSin(static_cast<unsigned short>(angle + bDegToAng(90.0f)));
}

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
    7856, 7877, 7899, 7920, 7942, 7963, 7984, 8005, 8026, 8047, 8068, 8089, 8110, 8131, 8151, 8172, 8192, 8192};

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

float bDistBetween(const bVector3 *v1, const bVector3 *v2) {
    float x = v1->x - v2->x;
    float y = v1->y - v2->y;
    float z = v1->z - v2->z;
    return bSqrt(x * x + y * y + z * z);
}
