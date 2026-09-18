#ifndef FEMEMSTREAM_H
#define FEMEMSTREAM_H

#include <types.h>

// File: speed/indep/src/feng/FEMemStream.h
// total size: 0xC
// Decl: speed/indep/src/feng/FEMemStream.h:17
class FEMemStream {
  private:
    char *pBuf;   // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEMemStream.h:19
    u32 Len, Pos; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEMemStream.h:20

  public:
    FEMemStream() {} // Decl: speed/indep/src/feng/FEMemStream.h:23
    ~FEMemStream() {}

    u32 GetPos() { // Decl: speed/indep/src/feng/FEMemStream.h:26
        return Pos;
    }

    void UseBuffer(void *pDest, u32 Size) {}

    void Write(const char *pSrc, u32 Bytes);
};

#endif
