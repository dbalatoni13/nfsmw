#ifndef EAXSOUND_SND_GEN_TURBO_H
#define EAXSOUND_SND_GEN_TURBO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
extern ClassHandle gFX_TURBO_01Handle;
extern InterfaceId FX_TURBO_01Id;
} // namespace Csis

// total size: 0x18
struct FX_TURBO_01Struct {
    int id;       // offset 0x0, size 0x4
    int volume;   // offset 0x4, size 0x4
    int pSI;      // offset 0x8, size 0x4
    int azimuth;  // offset 0xC, size 0x4
    int rotation; // offset 0x10, size 0x4
    int rPM;      // offset 0x14, size 0x4
};

// total size: 0x1C
struct FX_TURBO_01 {
    Csis::Class *mpClass;      // offset 0x0, size 0x4
    FX_TURBO_01Struct mData;   // offset 0x4, size 0x18

    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 5) {
            x = 5;
        }
        mData.id = x;
    }

    void SetVolume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.volume = x;
    }

    void SetPSI(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        mData.pSI = x;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.azimuth = x;
    }

    void SetRotation(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        mData.rotation = x;
    }

    void SetRPM(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 10000) {
            x = 10000;
        }
        mData.rPM = x;
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

    FX_TURBO_01(int id, int volume, int pSI, int azimuth, int rotation, int rPM) {
        SetId(id);
        SetVolume(volume);
        SetPSI(pSI);
        SetAzimuth(azimuth);
        SetRotation(rotation);
        SetRPM(rPM);

        Csis::System::Result result = static_cast<Csis::System::Result>(Csis::Class::CreateInstance(&Csis::gFX_TURBO_01Handle, &mData, &mpClass));
        if (result < 0) {
            Csis::gFX_TURBO_01Handle.Set(&Csis::FX_TURBO_01Id);
            Csis::Class::CreateInstance(&Csis::gFX_TURBO_01Handle, &mData, &mpClass);
        }
    }

    ~FX_TURBO_01() {
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
