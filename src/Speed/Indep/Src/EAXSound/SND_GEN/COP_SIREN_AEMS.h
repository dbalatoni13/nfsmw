//
//
//
#ifndef COP_SIREN_AEMS_H
#define COP_SIREN_AEMS_H

#include "csis/csis.h"

namespace Csis {

extern InterfaceId SIRENId;
extern ClassHandle gSIRENHandle;

// total size: 0x24
typedef struct {
    int vOL;        // offset 0x0, size 0x4
    int pITCH_OFFS; // offset 0x4, size 0x4
    int pAN;        // offset 0x8, size 0x4
    int dISTANCE;   // offset 0xC, size 0x4
    int tYPE;       // offset 0x10, size 0x4
    int loPass;     // offset 0x14, size 0x4
    int hiPass;     // offset 0x18, size 0x4
    int dry_FX;     // offset 0x1C, size 0x4
    int wet_FX;     // offset 0x20, size 0x4
} SIRENStruct;

extern InterfaceId SIREN_BEDId;
extern ClassHandle gSIREN_BEDHandle;

// total size: 0x20
typedef struct {
    int vOL;        // offset 0x0, size 0x4
    int pITCH_OFFS; // offset 0x4, size 0x4
    int dISTANCE;   // offset 0x8, size 0x4
    int tYPE;       // offset 0xC, size 0x4
    int loPass;     // offset 0x10, size 0x4
    int hiPass;     // offset 0x14, size 0x4
    int dry_FX;     // offset 0x18, size 0x4
    int wet_FX;     // offset 0x1C, size 0x4
} SIREN_BEDStruct;

// total size: 0x28
class SIREN {
  public:
    void SetVOL(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vOL = x;
    }

    int GetVOL() {
        return this->mData.vOL;
    }

    void SetPITCH_OFFS(int x) {
        if (x < -0x4000) {
            x = -0x4000;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        this->mData.pITCH_OFFS = x;
    }

    int GetPITCH_OFFS() {
        return this->mData.pITCH_OFFS;
    }

    void SetPAN(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.pAN = x;
    }

    int GetPAN() {
        return this->mData.pAN;
    }

    void SetDISTANCE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        this->mData.dISTANCE = x;
    }

    int GetDISTANCE() {
        return this->mData.dISTANCE;
    }

    void SetTYPE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 7) {
            x = 7;
        }
        this->mData.tYPE = x;
    }

    int GetTYPE() {
        return this->mData.tYPE;
    }

    void SetLoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.loPass = x;
    }

    int GetLoPass() {
        return this->mData.loPass;
    }

    void SetHiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.hiPass = x;
    }

    int GetHiPass() {
        return this->mData.hiPass;
    }

    void SetDry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.dry_FX = x;
    }

    int GetDry_FX() {
        return this->mData.dry_FX;
    }

    void SetWet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.wet_FX = x;
    }

    int GetWet_FX() {
        return this->mData.wet_FX;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(size_t size) {
        return System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        System::Free(ptr);
    }

    SIREN(int vOL, int pITCH_OFFS, int pAN, int dISTANCE, int tYPE, int loPass, int hiPass, int dry_FX, int wet_FX) {
        this->SetVOL(vOL);
        this->SetPITCH_OFFS(pITCH_OFFS);
        this->SetPAN(pAN);
        this->SetDISTANCE(dISTANCE);
        this->SetTYPE(tYPE);
        this->SetLoPass(loPass);
        this->SetHiPass(hiPass);
        this->SetDry_FX(dry_FX);
        this->SetWet_FX(wet_FX);

        Result result = Class::CreateInstance(&gSIRENHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gSIRENHandle.Set(&SIRENId);
            Class::CreateInstance(&gSIRENHandle, &this->mData, &this->mpClass);
        }
    }

    ~SIREN() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (this->mpClass != nullptr) {
            this->mpClass->SetMemberData(&this->mData);
        }
    }

  private:
    Class *mpClass;    // offset 0x0, size 0x4
    SIRENStruct mData; // offset 0x4, size 0x24
};

// total size: 0x24
class SIREN_BED {
  public:
    void SetVOL(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vOL = x;
    }

    int GetVOL() {
        return this->mData.vOL;
    }

    void SetPITCH_OFFS(int x) {
        if (x < -0x4000) {
            x = -0x4000;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        this->mData.pITCH_OFFS = x;
    }

    int GetPITCH_OFFS() {
        return this->mData.pITCH_OFFS;
    }

    void SetDISTANCE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        this->mData.dISTANCE = x;
    }

    int GetDISTANCE() {
        return this->mData.dISTANCE;
    }

    void SetTYPE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 7) {
            x = 7;
        }
        this->mData.tYPE = x;
    }

    int GetTYPE() {
        return this->mData.tYPE;
    }

    void SetLoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.loPass = x;
    }

    int GetLoPass() {
        return this->mData.loPass;
    }

    void SetHiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.hiPass = x;
    }

    int GetHiPass() {
        return this->mData.hiPass;
    }

    void SetDry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.dry_FX = x;
    }

    int GetDry_FX() {
        return this->mData.dry_FX;
    }

    void SetWet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.wet_FX = x;
    }

    int GetWet_FX() {
        return this->mData.wet_FX;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(size_t size) {
        return System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        System::Free(ptr);
    }

    SIREN_BED(int vOL, int pITCH_OFFS, int dISTANCE, int tYPE, int loPass, int hiPass, int dry_FX, int wet_FX) {
        this->SetVOL(vOL);
        this->SetPITCH_OFFS(pITCH_OFFS);
        this->SetDISTANCE(dISTANCE);
        this->SetTYPE(tYPE);
        this->SetLoPass(loPass);
        this->SetHiPass(hiPass);
        this->SetDry_FX(dry_FX);
        this->SetWet_FX(wet_FX);

        Result result = Class::CreateInstance(&gSIREN_BEDHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gSIREN_BEDHandle.Set(&SIREN_BEDId);
            Class::CreateInstance(&gSIREN_BEDHandle, &this->mData, &this->mpClass);
        }
    }

    ~SIREN_BED() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (this->mpClass != nullptr) {
            this->mpClass->SetMemberData(&this->mData);
        }
    }

  private:
    Class *mpClass;        // offset 0x0, size 0x4
    SIREN_BEDStruct mData; // offset 0x4, size 0x20
};

}; // namespace Csis

#endif
