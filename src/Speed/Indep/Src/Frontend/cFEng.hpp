#ifndef _CFENG
#define _CFENG

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class FEObject;
class FEPackage;

// total size: 0x8
class cFEng {
public:
    static cFEng *Get();

    void QueueGameMessage(unsigned int pMessage, const char *pPackageName,
                          unsigned int controlMask);

    static cFEng *mInstance;
};

extern cFEng *_5cFEng_mInstance;

#endif
