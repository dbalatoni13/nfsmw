#ifndef EAXSOUND_SND_GEN_MAIN_AEMS_H
#define EAXSOUND_SND_GEN_MAIN_AEMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
extern ClassHandle gFX_RadarHandle;
extern InterfaceId FX_RadarId;
extern ClassHandle gFX_SKIDHandle;
extern InterfaceId FX_SKIDId;
} // namespace Csis

struct FX_RadarStruct {
    int iD;           // offset 0x0, size 0x4
    int volume;       // offset 0x4, size 0x4
    int pitch_Offset; // offset 0x8, size 0x4
    int intensity;    // offset 0xC, size 0x4
    int stop;         // offset 0x10, size 0x4
};

struct FX_Hydraulic {
    Csis::Class *mpClass;

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    ~FX_Hydraulic() {
        if (mpClass) {
            mpClass->Release();
        }
    }
};

struct FX_Radar {
    Csis::Class *mpClass;
    FX_RadarStruct mData;

    void SetID(int x) {
        mData.iD = x;
    }

    int GetID() {
        return mData.iD;
    }

    void SetVolume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.volume = x;
    }

    int GetVolume() {
        return mData.volume;
    }

    void SetPitch_Offset(int x) {
        mData.pitch_Offset = x;
    }

    int GetPitch_Offset() {
        return mData.pitch_Offset;
    }

    void SetIntensity(int x) {
        mData.intensity = x;
    }

    int GetIntensity() {
        return mData.intensity;
    }

    void SetStop(int x) {
        mData.stop = x;
    }

    int GetStop() {
        return mData.stop;
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

    FX_Radar(int iD, int volume, int pitch_Offset, int intensity, int stop) {
        SetID(iD);
        SetVolume(volume);
        SetStop(stop);
        SetPitch_Offset(pitch_Offset);
        SetIntensity(intensity);

        int result = Csis::Class::CreateInstance(&Csis::gFX_RadarHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_RadarHandle.Set(&Csis::FX_RadarId);
            Csis::Class::CreateInstance(&Csis::gFX_RadarHandle, &mData, &mpClass);
        }
    }

    ~FX_Radar() {
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

struct FX_SKIDStruct {
    int vOL_Fwd;                  // offset 0x0, size 0x4
    int vOL_Side;                 // offset 0x4, size 0x4
    int vOL_Back;                 // offset 0x8, size 0x4
    int azimuth;                  // offset 0xC, size 0x4
    int tYPE;                     // offset 0x10, size 0x4
    int pITCH_OFFSET;             // offset 0x14, size 0x4
    int hOP;                      // offset 0x18, size 0x4
    int sPEED;                    // offset 0x1C, size 0x4
    int oPPOS_Side;               // offset 0x20, size 0x4
    int uNDERSTEER;               // offset 0x24, size 0x4
    int oVERSTEER;                // offset 0x28, size 0x4
    int surface;                  // offset 0x2C, size 0x4
    int front_FB;                 // offset 0x30, size 0x4
    int front_LR;                 // offset 0x34, size 0x4
    int front_Load;               // offset 0x38, size 0x4
    int back_FB;                  // offset 0x3C, size 0x4
    int back_LR;                  // offset 0x40, size 0x4
    int back_Load;                // offset 0x44, size 0x4
    int filter_Effects_LoPass;    // offset 0x48, size 0x4
    int filter_Effects_HiPass;    // offset 0x4C, size 0x4
    int filter_Effects_Dry_FX;    // offset 0x50, size 0x4
    int filter_Effects_Wet_FX;    // offset 0x54, size 0x4
};

struct FX_SKID {
    Csis::Class *mpClass;         // offset 0x0, size 0x4
    FX_SKIDStruct mData;          // offset 0x4, size 0x58

    void SetVOL_Fwd(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.vOL_Fwd = x;
    }

    void SetVOL_Side(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.vOL_Side = x;
    }

    void SetVOL_Back(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.vOL_Back = x;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.azimuth = x;
    }

    void SetTYPE(int x) {
        mData.tYPE = x;
    }

    void SetPITCH_OFFSET(int x) {
        if (x < -0x4000) {
            x = -0x4000;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        mData.pITCH_OFFSET = x;
    }

    void SetHOP(int x) {
        mData.hOP = x;
    }

    void SetSPEED(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1000) {
            x = 1000;
        }
        mData.sPEED = x;
    }

    void SetOPPOS_Side(int x) {
        mData.oPPOS_Side = x;
    }

    void SetUNDERSTEER(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.uNDERSTEER = x;
    }

    void SetOVERSTEER(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.oVERSTEER = x;
    }

    void SetSurface(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 5) {
            x = 5;
        }
        mData.surface = x;
    }

    void SetFront_FB(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.front_FB = x;
    }

    void SetFront_LR(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.front_LR = x;
    }

    void SetFront_Load(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.front_Load = x;
    }

    void SetBack_FB(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.back_FB = x;
    }

    void SetBack_LR(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.back_LR = x;
    }

    void SetBack_Load(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.back_Load = x;
    }

    void SetFilter_Effects_LoPass(int x) {
        mData.filter_Effects_LoPass = x;
    }

    void SetFilter_Effects_HiPass(int x) {
        mData.filter_Effects_HiPass = x;
    }

    void SetFilter_Effects_Dry_FX(int x) {
        mData.filter_Effects_Dry_FX = x;
    }

    void SetFilter_Effects_Wet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.filter_Effects_Wet_FX = x;
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

    FX_SKID(int vOL_Fwd, int vOL_Side, int vOL_Back, int azimuth, int tYPE, int pITCH_OFFSET, int hOP, int sPEED, int oPPOS_Side, int uNDERSTEER, int oVERSTEER, int surface, int front_FB, int front_LR, int front_Load, int back_FB, int back_LR, int back_Load, int filter_Effects_LoPass, int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        SetVOL_Fwd(vOL_Fwd);
        SetVOL_Side(vOL_Side);
        SetVOL_Back(vOL_Back);
        SetAzimuth(azimuth);
        SetTYPE(tYPE);
        SetPITCH_OFFSET(pITCH_OFFSET);
        SetHOP(hOP);
        SetSPEED(sPEED);
        SetOPPOS_Side(oPPOS_Side);
        SetUNDERSTEER(uNDERSTEER);
        SetOVERSTEER(oVERSTEER);
        SetSurface(surface);
        SetFront_FB(front_FB);
        SetFront_LR(front_LR);
        SetFront_Load(front_Load);
        SetBack_FB(back_FB);
        SetBack_LR(back_LR);
        SetBack_Load(back_Load);
        SetFilter_Effects_LoPass(filter_Effects_LoPass);
        SetFilter_Effects_HiPass(filter_Effects_HiPass);
        SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gFX_SKIDHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_SKIDHandle.Set(&Csis::FX_SKIDId);
            Csis::Class::CreateInstance(&Csis::gFX_SKIDHandle, &mData, &mpClass);
        }
    }

    ~FX_SKID() {
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
