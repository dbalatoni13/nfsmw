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
    bool IsPackagePushed(const char *name);
    bool IsPackageInControl(const char *name);
    void PopNoControlPackage(const char *name);
    void QueuePackagePop(int all);
    void ServiceFengOnly();
    void QueuePackageSwitch(const char *name, int param2, unsigned int param3, bool param4);

    static cFEng *mInstance;
};

extern cFEng *_5cFEng_mInstance;

#endif
