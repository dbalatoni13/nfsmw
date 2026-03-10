#ifndef _CFENG
#define _CFENG

#include "FEPackage.h"

struct FEngine;

// total size: 0x8
struct cFEng {
    static cFEng* mInstance;

    FEngine* mFEng; // offset 0x0, size 0x4
    bool bWasPaused; // offset 0x4, size 0x1

    static inline cFEng* Get() { return mInstance; }

    FEPackage* FindPackage(const char* pPackageName);

    void QueueGameMessagePkg(unsigned int pMessage, FEPackage* topkg);
};

#endif
