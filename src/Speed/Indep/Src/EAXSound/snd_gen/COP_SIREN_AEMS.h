#ifndef EAXSOUND_SND_GEN_COP_SIREN_AEMS_H
#define EAXSOUND_SND_GEN_COP_SIREN_AEMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
extern ClassHandle gSIRENHandle;
extern InterfaceId SIRENId;
extern ClassHandle gSIREN_BEDHandle;
extern InterfaceId SIREN_BEDId;
} // namespace Csis

// total size: 0x24
struct SIRENStruct {
    int vOL;        // offset 0x0, size 0x4
    int pITCH_OFFS; // offset 0x4, size 0x4
    int pAN;        // offset 0x8, size 0x4
    int dISTANCE;   // offset 0xC, size 0x4
    int tYPE;       // offset 0x10, size 0x4
    int loPass;     // offset 0x14, size 0x4
    int hiPass;     // offset 0x18, size 0x4
    int dry_FX;     // offset 0x1C, size 0x4
    int wet_FX;     // offset 0x20, size 0x4
};

// total size: 0x28
struct SIREN {
    Csis::Class *mpClass; // offset 0x0, size 0x4
    SIRENStruct mData;    // offset 0x4, size 0x24

    void SetVOL(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.vOL = x;
    }

    int GetVOL() {
        return mData.vOL;
    }

    void SetPITCH_OFFS(int x) {
        if (x < -0x4000) {
            x = -0x4000;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        mData.pITCH_OFFS = x;
    }

    int GetPITCH_OFFS() {
        return mData.pITCH_OFFS;
    }

    void SetPAN(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.pAN = x;
    }

    int GetPAN() {
        return mData.pAN;
    }

    void SetDISTANCE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        mData.dISTANCE = x;
    }

    int GetDISTANCE() {
        return mData.dISTANCE;
    }

    void SetTYPE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 7) {
            x = 7;
        }
        mData.tYPE = x;
    }

    int GetTYPE() {
        return mData.tYPE;
    }

    void SetLoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        mData.loPass = x;
    }

    int GetLoPass() {
        return mData.loPass;
    }

    void SetHiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        mData.hiPass = x;
    }

    int GetHiPass() {
        return mData.hiPass;
    }

    void SetDry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.dry_FX = x;
    }

    int GetDry_FX() {
        return mData.dry_FX;
    }

    void SetWet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.wet_FX = x;
    }

    int GetWet_FX() {
        return mData.wet_FX;
    }

    int GetRefCount() {
        int refCount = 0;

        if (mpClass) {
            mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    SIREN(int vOL, int pITCH_OFFS, int pAN, int dISTANCE, int tYPE, int loPass, int hiPass, int dry_FX, int wet_FX) {
        SetVOL(vOL);
        SetPITCH_OFFS(pITCH_OFFS);
        SetPAN(pAN);
        SetDISTANCE(dISTANCE);
        SetTYPE(tYPE);
        SetLoPass(loPass);
        SetHiPass(hiPass);
        SetDry_FX(dry_FX);
        SetWet_FX(wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gSIRENHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gSIRENHandle.Set(&Csis::SIRENId);
            Csis::Class::CreateInstance(&Csis::gSIRENHandle, &mData, &mpClass);
        }
    }

    ~SIREN() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }
};

#endif
