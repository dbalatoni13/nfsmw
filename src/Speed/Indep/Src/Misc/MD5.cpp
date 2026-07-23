#include "Speed/Indep/Src/Misc/MD5.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

#define FF(x, y, z) (z ^ ((y ^ z) & x)) // Decl: 76
#define GG(x, y, z) (y ^ ((x ^ y) & z)) // Decl: 77
#define HH(x, y, z) (x ^ y ^ z)         // Decl: 78
#define II(x, y, z) (y ^ (x | ~z))      // Decl: 79

#define ACC(r, t, s, x)                                                                                                                              \
    {                                                                                                                                                \
        r += (t);                                                                                                                                    \
        r = (r << s) | (r >> (32 - s));                                                                                                              \
        r += x;                                                                                                                                      \
    } // Decl: 82

// Decl: 95
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

// Decl: 179
void MD5::_Transform() {
    unsigned int uData[16];
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned char *pInput;

    pInput = this->strData;
    for (b = 0, a = 0; a <= 15; a++, b += 4) {
        uData[a] = static_cast<unsigned int>(pInput[b]) | (static_cast<unsigned int>(pInput[b + 1]) << 8) |
                   (static_cast<unsigned int>(pInput[b + 2]) << 16) | (static_cast<unsigned int>(pInput[b + 3]) << 24);
    }

    a = this->uRegs[0];
    b = this->uRegs[1];
    c = this->uRegs[2];
    d = this->uRegs[3];

    ACC(a, FF(b, c, d) + uData[0] + 0xd76aa478, 7, b);
    ACC(d, FF(a, b, c) + uData[1] + 0xe8c7b756, 12, a);
    ACC(c, FF(d, a, b) + uData[2] + 0x242070db, 17, d);
    ACC(b, FF(c, d, a) + uData[3] + 0xc1bdceee, 22, c);
    ACC(a, FF(b, c, d) + uData[4] + 0xf57c0faf, 7, b);
    ACC(d, FF(a, b, c) + uData[5] + 0x4787c62a, 12, a);
    ACC(c, FF(d, a, b) + uData[6] + 0xa8304613, 17, d);
    ACC(b, FF(c, d, a) + uData[7] + 0xfd469501, 22, c);
    ACC(a, FF(b, c, d) + uData[8] + 0x698098d8, 7, b);
    ACC(d, FF(a, b, c) + uData[9] + 0x8b44f7af, 12, a);
    ACC(c, FF(d, a, b) + uData[10] + 0xffff5bb1, 17, d);
    ACC(b, FF(c, d, a) + uData[11] + 0x895cd7be, 22, c);
    ACC(a, FF(b, c, d) + uData[12] + 0x6b901122, 7, b);
    ACC(d, FF(a, b, c) + uData[13] + 0xfd987193, 12, a);
    ACC(c, FF(d, a, b) + uData[14] + 0xa679438e, 17, d);
    ACC(b, FF(c, d, a) + uData[15] + 0x49b40821, 22, c);

    ACC(a, GG(b, c, d) + uData[1] + 0xf61e2562, 5, b);
    ACC(d, GG(a, b, c) + uData[6] + 0xc040b340, 9, a);
    ACC(c, GG(d, a, b) + uData[11] + 0x265e5a51, 14, d);
    ACC(b, GG(c, d, a) + uData[0] + 0xe9b6c7aa, 20, c);
    ACC(a, GG(b, c, d) + uData[5] + 0xd62f105d, 5, b);
    ACC(d, GG(a, b, c) + uData[10] + 0x02441453, 9, a);
    ACC(c, GG(d, a, b) + uData[15] + 0xd8a1e681, 14, d);
    ACC(b, GG(c, d, a) + uData[4] + 0xe7d3fbc8, 20, c);
    ACC(a, GG(b, c, d) + uData[9] + 0x21e1cde6, 5, b);
    ACC(d, GG(a, b, c) + uData[14] + 0xc33707d6, 9, a);
    ACC(c, GG(d, a, b) + uData[3] + 0xf4d50d87, 14, d);
    ACC(b, GG(c, d, a) + uData[8] + 0x455a14ed, 20, c);
    ACC(a, GG(b, c, d) + uData[13] + 0xa9e3e905, 5, b);
    ACC(d, GG(a, b, c) + uData[2] + 0xfcefa3f8, 9, a);
    ACC(c, GG(d, a, b) + uData[7] + 0x676f02d9, 14, d);
    ACC(b, GG(c, d, a) + uData[12] + 0x8d2a4c8a, 20, c);

    ACC(a, HH(b, c, d) + uData[5] + 0xfffa3942, 4, b);
    ACC(d, HH(a, b, c) + uData[8] + 0x8771f681, 11, a);
    ACC(c, HH(d, a, b) + uData[11] + 0x6d9d6122, 16, d);
    ACC(b, HH(c, d, a) + uData[14] + 0xfde5380c, 23, c);
    ACC(a, HH(b, c, d) + uData[1] + 0xa4beea44, 4, b);
    ACC(d, HH(a, b, c) + uData[4] + 0x4bdecfa9, 11, a);
    ACC(c, HH(d, a, b) + uData[7] + 0xf6bb4b60, 16, d);
    ACC(b, HH(c, d, a) + uData[10] + 0xbebfbc70, 23, c);
    ACC(a, HH(b, c, d) + uData[13] + 0x289b7ec6, 4, b);
    ACC(d, HH(a, b, c) + uData[0] + 0xeaa127fa, 11, a);
    ACC(c, HH(d, a, b) + uData[3] + 0xd4ef3085, 16, d);
    ACC(b, HH(c, d, a) + uData[6] + 0x04881d05, 23, c);
    ACC(a, HH(b, c, d) + uData[9] + 0xd9d4d039, 4, b);
    ACC(d, HH(a, b, c) + uData[12] + 0xe6db99e5, 11, a);
    ACC(c, HH(d, a, b) + uData[15] + 0x1fa27cf8, 16, d);
    ACC(b, HH(c, d, a) + uData[2] + 0xc4ac5665, 23, c);

    ACC(a, II(b, c, d) + uData[0] + 0xf4292244, 6, b);
    ACC(d, II(a, b, c) + uData[7] + 0x432aff97, 10, a);
    ACC(c, II(d, a, b) + uData[14] + 0xab9423a7, 15, d);
    ACC(b, II(c, d, a) + uData[5] + 0xfc93a039, 21, c);
    ACC(a, II(b, c, d) + uData[12] + 0x655b59c3, 6, b);
    ACC(d, II(a, b, c) + uData[3] + 0x8f0ccc92, 10, a);
    ACC(c, II(d, a, b) + uData[10] + 0xffeff47d, 15, d);
    ACC(b, II(c, d, a) + uData[1] + 0x85845dd1, 21, c);
    ACC(a, II(b, c, d) + uData[8] + 0x6fa87e4f, 6, b);
    ACC(d, II(a, b, c) + uData[15] + 0xfe2ce6e0, 10, a);
    ACC(c, II(d, a, b) + uData[6] + 0xa3014314, 15, d);
    ACC(b, II(c, d, a) + uData[13] + 0x4e0811a1, 21, c);
    ACC(a, II(b, c, d) + uData[4] + 0xf7537e82, 6, b);
    ACC(d, II(a, b, c) + uData[11] + 0xbd3af235, 10, a);
    ACC(c, II(d, a, b) + uData[2] + 0x2ad7d2bb, 15, d);
    ACC(b, II(c, d, a) + uData[9] + 0xeb86d391, 21, c);

    this->uRegs[0] += a;
    this->uRegs[1] += b;
    this->uRegs[2] += c;
    this->uRegs[3] += d;
}

void MD5::_Final() {
    static char hexChars[17] = "0123456789abcdef";
    int uIndex;
    unsigned int uData = 0;
    unsigned char uPad;
    unsigned char *raw;
    unsigned char *str;

    uIndex = this->uCount & 0x3F;
    uPad = 0x80;
    do {
        this->strData[uIndex] = uPad;
        uIndex++;
        uPad = 0;
        if (uIndex == 64) {
            this->_Transform();
            uIndex = 0;
        }
    } while (uIndex != 56);

    this->strData[56] = static_cast<unsigned char>(this->uCount << 3);
    this->strData[57] = static_cast<unsigned char>(this->uCount >> 5);
    this->strData[58] = static_cast<unsigned char>(this->uCount >> 13);
    this->strData[59] = static_cast<unsigned char>(this->uCount >> 21);
    this->strData[60] = static_cast<unsigned char>(this->uCount >> 29);
    this->strData[61] = 0;
    this->strData[62] = 0;
    this->strData[63] = 0;

    this->_Transform();

    raw = this->rawMD5;
    str = this->strMD5;
    for (uIndex = 0; uIndex <= 15; uIndex++) {
        if ((uIndex & 3) == 0) {
            uData = this->uRegs[uIndex >> 2];
        }

        *raw++ = static_cast<unsigned char>(uData);
        *str++ = hexChars[(uData >> 4) & 0xF];
        *str++ = hexChars[uData & 0xF];
        uData >>= 8;
    }

    *str = '\0';
    this->computed = true;
}
