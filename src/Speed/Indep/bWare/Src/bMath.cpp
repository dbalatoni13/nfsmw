#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

unsigned int bDefaultSeed;

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
