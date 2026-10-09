#ifndef __MD5_HPP
#define __MD5_HPP // Decl: 69

#include "types.h"

// Decl: 78
class MD5 {
  public:
    MD5() {
        this->Reset();
    }

    virtual ~MD5() {}

    void Update(const void *buffer, int length);
    void *GetRaw();
    const char *GetString();

    const unsigned char operator[](int index) {
        return this->rawMD5[index];
    }
    // operator char const *() {}
    // operator unsigned char const *() {}

    int GetRawLength() {
        return 16;
    }

    int GetStringLength() {
        return 32;
    }

    void Reset() {
        this->uRegs[0] = 0x67452301;
        this->uRegs[1] = 0xEFCDAB89;
        this->uRegs[2] = 0x98BADCFE;
        this->uRegs[3] = 0x10325476;
        this->uCount = 0;
        this->computed = false;
    }

  private:
    void _Transform();
    void _Final();

    uint32 uCount;             // offset 0x0, Decl: 116
    uint32 uRegs[4];           // offset 0x4, Decl: 117
    unsigned char strData[64]; // offset 0x14, Decl: 118

    bool computed;            // offset 0x54, Decl: 120
    unsigned char rawMD5[16]; // offset 0x58, Decl: 122
    unsigned char strMD5[33]; // offset 0x68, Decl: 122
};

#endif
