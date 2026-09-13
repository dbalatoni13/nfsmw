#ifndef FEMEMSTREAM_H
#define FEMEMSTREAM_H

#include <types.h>

// total size: 0xC
// Decl: 17
class FEMemStream {
  private:
    char *pBuf;   // offset 0x0, size 0x4, Decl: 19
    u32 Len, Pos; // offset 0x4, size 0x4, Decl: 20

  public:
    FEMemStream() {} // Decl: 23
    ~FEMemStream() {}

    u32 GetPos() { // Decl: 26
        return Pos;
    }

    void UseBuffer(void *pDest, u32 Size) {}

    void Write(const char *pSrc, u32 Bytes);
};

#endif
