#include "Speed/Indep/Src/Misc/MD5.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

void MD5::Update(const void *buffer, int length) {
    uint32 uIndex;
    const unsigned char *_buffer = static_cast<const unsigned char *>(buffer);

    if (computed == true) {
        Reset();
    }

    if (length < 0) {
        length = bStrLen(reinterpret_cast<const char *>(_buffer));
    }

    uIndex = uCount & 0x3F;

    for (; length > 0; length--) {
        strData[uIndex] = *_buffer;
        _buffer++;
        uIndex++;
        uCount++;
        if (uIndex == 64) {
            _Transform();
            uIndex = 0;
        }
    }
}

void *MD5::GetRaw() {
    if (uCount == 0) {
        return nullptr;
    }
    if (!computed) {
        _Final();
    }
    return rawMD5;
}

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) ((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | ~(z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define FF(a, b, c, d, x, s, ac)                                                                                                                     \
    (a) += F((b), (c), (d)) + (x) + (unsigned int)(ac);                                                                                              \
    (a) = ROTATE_LEFT((a), (s)) + (b);

#define GG(a, b, c, d, x, s, ac)                                                                                                                     \
    (a) += G((b), (c), (d)) + (x) + (unsigned int)(ac);                                                                                              \
    (a) = ROTATE_LEFT((a), (s)) + (b);

#define HH(a, b, c, d, x, s, ac)                                                                                                                     \
    (a) += H((b), (c), (d)) + (x) + (unsigned int)(ac);                                                                                              \
    (a) = ROTATE_LEFT((a), (s)) + (b);

#define II(a, b, c, d, x, s, ac)                                                                                                                     \
    (a) += I((b), (c), (d)) + (x) + (unsigned int)(ac);                                                                                              \
    (a) = ROTATE_LEFT((a), (s)) + (b);

// UNSOLVED, mostly wrong
void MD5::_Transform() {
    unsigned int uData[16];
    unsigned char *pInput = strData;

    unsigned int i = 0;
    do {
        uData[i] = static_cast<unsigned int>(pInput[0]) | (static_cast<unsigned int>(pInput[1]) << 8) | (static_cast<unsigned int>(pInput[2]) << 16) |
                   (static_cast<unsigned int>(pInput[3]) << 24);
        pInput += 4;
        i++;
    } while (i <= 15);

    unsigned int c = uRegs[2];
    unsigned int d = uRegs[3];
    unsigned int b = uRegs[1];
    unsigned int a = uRegs[0];

    // Round 1
    FF(a, b, c, d, uData[0], 7, 0xd76aa478);
    FF(d, a, b, c, uData[1], 12, 0xe8c7b756);
    FF(c, d, a, b, uData[2], 17, 0x242070db);
    FF(b, c, d, a, uData[3], 22, 0xc1bdceee);
    FF(a, b, c, d, uData[4], 7, 0xf57c0faf);
    FF(d, a, b, c, uData[5], 12, 0x4787c62a);
    FF(c, d, a, b, uData[6], 17, 0xa8304613);
    FF(b, c, d, a, uData[7], 22, 0xfd469501);
    FF(a, b, c, d, uData[8], 7, 0x698098d8);
    FF(d, a, b, c, uData[9], 12, 0x8b44f7af);
    FF(c, d, a, b, uData[10], 17, 0xffff5bb1);
    FF(b, c, d, a, uData[11], 22, 0x895cd7be);
    FF(a, b, c, d, uData[12], 7, 0x6b901122);
    FF(d, a, b, c, uData[13], 12, 0xfd987193);
    FF(c, d, a, b, uData[14], 17, 0xa679438e);
    FF(b, c, d, a, uData[15], 22, 0x49b40821);

    // Round 2
    GG(a, b, c, d, uData[1], 5, 0xf61e2562);
    GG(d, a, b, c, uData[6], 9, 0xc040b340);
    GG(c, d, a, b, uData[11], 14, 0x265e5a51);
    GG(b, c, d, a, uData[0], 20, 0xe9b6c7aa);
    GG(a, b, c, d, uData[5], 5, 0xd62f105d);
    GG(d, a, b, c, uData[10], 9, 0x02441453);
    GG(c, d, a, b, uData[15], 14, 0xd8a1e681);
    GG(b, c, d, a, uData[4], 20, 0xe7d3fbc8);
    GG(a, b, c, d, uData[9], 5, 0x21e1cde6);
    GG(d, a, b, c, uData[14], 9, 0xc33707d6);
    GG(c, d, a, b, uData[3], 14, 0xf4d50d87);
    GG(b, c, d, a, uData[8], 20, 0x455a14ed);
    GG(a, b, c, d, uData[13], 5, 0xa9e3e905);
    GG(d, a, b, c, uData[2], 9, 0xfcefa3f8);
    GG(c, d, a, b, uData[7], 14, 0x676f02d9);
    GG(b, c, d, a, uData[12], 20, 0x8d2a4c8a);

    // Round 3
    HH(a, b, c, d, uData[5], 4, 0xfffa3942);
    HH(d, a, b, c, uData[8], 11, 0x8771f681);
    HH(c, d, a, b, uData[11], 16, 0x6d9d6122);
    HH(b, c, d, a, uData[14], 23, 0xfde5380c);
    HH(a, b, c, d, uData[1], 4, 0xa4beea44);
    HH(d, a, b, c, uData[4], 11, 0x4bdecfa9);
    HH(c, d, a, b, uData[7], 16, 0xf6bb4b60);
    HH(b, c, d, a, uData[14 - 6], 23, 0xbebfbc70);
    HH(a, b, c, d, uData[13], 4, 0x289b7ec6);
    HH(d, a, b, c, uData[0], 11, 0xeaa127fa);
    HH(c, d, a, b, uData[3], 16, 0xd4ef3085);
    HH(b, c, d, a, uData[6], 23, 0x04881d05);
    HH(a, b, c, d, uData[9], 4, 0xd9d4d039);
    HH(d, a, b, c, uData[12], 11, 0xe6db99e5);
    HH(c, d, a, b, uData[15], 16, 0x1fa27cf8);
    HH(b, c, d, a, uData[2], 23, 0xc4ac5665);

    // Round 4
    II(a, b, c, d, uData[0], 6, 0xf4292244);
    II(d, a, b, c, uData[7], 10, 0x432aff97);
    II(c, d, a, b, uData[14], 15, 0xab9423a7);
    II(b, c, d, a, uData[5], 21, 0xfc93a039);
    II(a, b, c, d, uData[12], 6, 0x655b59c3);
    II(d, a, b, c, uData[3], 10, 0x8f0ccc92);
    II(c, d, a, b, uData[10], 15, 0xffef9851);
    II(b, c, d, a, uData[1], 21, 0x85845dd1);
    II(a, b, c, d, uData[8], 6, 0x6fa87e4f);
    II(d, a, b, c, uData[15], 10, 0xfe2ce6e0);
    II(c, d, a, b, uData[6], 15, 0xa3014314);
    II(b, c, d, a, uData[13], 21, 0x4e0811a1);
    II(a, b, c, d, uData[4], 6, 0xf7537e82);
    II(d, a, b, c, uData[11], 10, 0xbd3af235);
    II(c, d, a, b, uData[2], 15, 0x2ad7d2bb);
    II(b, c, d, a, uData[9], 21, 0xeb86d391);

    uRegs[0] += a;
    uRegs[3] += d;
    uRegs[2] += c;
    uRegs[1] += b;
}

// UNSOLVED, mostly wrong
void MD5::_Final() {
    static const char hexChars[] = "0123456789abcdef";
    unsigned int uIndex = uCount & 0x3F;
    unsigned char uPad = 0x80;

    do {
        strData[uIndex] = uPad;
        uIndex++;
        uPad = 0;
        if (uIndex == 64) {
            _Transform();
            uIndex = 0;
        }
    } while (uIndex != 56);

    unsigned int count = uCount;
    strData[59] = static_cast<unsigned char>(count >> 21);
    strData[57] = static_cast<unsigned char>(count >> 5);
    strData[60] = static_cast<unsigned char>(count >> 29);
    strData[58] = static_cast<unsigned char>(count >> 13);
    strData[56] = static_cast<unsigned char>(count << 3);
    strData[62] = 0;
    strData[61] = 0;
    strData[63] = 0;

    _Transform();

    unsigned char *raw = rawMD5;
    unsigned char *str = strMD5;
    unsigned int uData = 0;

    int uIndex2 = 0;
    do {
        if ((uIndex2 & 3) == 0) {
            uData = uRegs[uIndex2 >> 2];
        }
        *raw = static_cast<unsigned char>(uData);
        str[0] = hexChars[(uData >> 4) & 0xF];
        str[1] = hexChars[uData & 0xF];
        uIndex2++;
        raw++;
        uData >>= 8;
        str += 2;
    } while (uIndex2 <= 15);

    *str = '\0';
    computed = true;
}

#undef F
#undef G
#undef H
#undef I
#undef ROTATE_LEFT

#undef FF
#undef GG
#undef HH
#undef II
