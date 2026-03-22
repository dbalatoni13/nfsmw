#ifndef MISC_MD5_H
#define MISC_MD5_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x90
class MD5 {
  public:
    MD5() {}
    virtual ~MD5() {}

    int GetRawLength() {
        return computed ? 16 : 0;
    }

    void Reset();
    void Update(const void *buffer, int length);
    void *GetRaw();
    const char *GetString();

  private:
    unsigned int uCount;       // offset 0x0, size 0x4
    unsigned int uRegs[4];     // offset 0x4, size 0x10
    unsigned char strData[64]; // offset 0x14, size 0x40
    bool computed;             // offset 0x54, size 0x1
    unsigned char rawMD5[16];  // offset 0x58, size 0x10
    unsigned char strMD5[33];  // offset 0x68, size 0x21
};

#endif
