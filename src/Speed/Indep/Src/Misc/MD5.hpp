#ifndef MISC_MD5_H
#define MISC_MD5_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class MD5 {
private:
    unsigned int uCount;              // offset 0x0
    unsigned int uRegs[4];            // offset 0x4
    unsigned char strData[64];        // offset 0x14
    bool computed;                    // offset 0x54
    unsigned char rawMD5[16];         // offset 0x58
    unsigned char strMD5[33];         // offset 0x68

public:
    MD5() {
        Reset();
    }

    virtual ~MD5() {}

    void Reset() {
        uRegs[0] = 0x67452301;
        uRegs[1] = 0xEFCDAB89;
        uRegs[2] = 0x98BADCFE;
        uRegs[3] = 0x10325476;
        computed = false;
        uCount = 0;
    }

    int GetRawLength() { return 16; }

    int GetStringLength() { return 32; }

    void Update(const void *buffer, int length);
    void *GetRaw();

    const char *GetString() {
        if (uCount == 0) {
            return 0;
        }
        if (!computed) {
            _Final();
        }
        return reinterpret_cast<const char *>(strMD5);
    }

private:
    void _Transform();
    void _Final();

    const struct __vtbl_ptr_type *__vptr; // offset 0x8C
};

#endif
