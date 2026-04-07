#ifndef EAXSOUND_SND_GEN_ENVIRO_AEMS_H
#define EAXSOUND_SND_GEN_ENVIRO_AEMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
extern ClassHandle gFX_ROADNOISEHandle;
extern InterfaceId FX_ROADNOISEId;
extern ClassHandle gFX_ROADNOISE_TRANSHandle;
extern InterfaceId FX_ROADNOISE_TRANSId;
extern ClassHandle gFX_TRAFFICHandle;
extern InterfaceId FX_TRAFFICId;
extern ClassHandle gENV_STATICHandle;
extern InterfaceId ENV_STATICId;
} // namespace Csis

enum FXROADNOISETypeType {
    FXROADNOISETYPETYPE_LOOP = 0,
    FXROADNOISETYPETYPE_TRANSITION_ = 1,
};

enum FXROADNOISETRANSTypeType {
    FXROADNOISETRANSTYPETYPE_LOOP = 0,
    FXROADNOISETRANSTYPETYPE_TRANSITION_ = 1,
};

struct FX_ROADNOISEStruct {
    int id;                     // offset 0x0, size 0x4
    int volume;                 // offset 0x4, size 0x4
    int pitch;                  // offset 0x8, size 0x4
    int azimuth;                // offset 0xC, size 0x4
    FXROADNOISETypeType type;   // offset 0x10, size 0x4
    int secondaryNoise;         // offset 0x14, size 0x4
    int speed;                  // offset 0x18, size 0x4
    int filter_Effects_LoPass;  // offset 0x1C, size 0x4
    int filter_Effects_HiPass;  // offset 0x20, size 0x4
    int filter_Effects_Dry_FX;  // offset 0x24, size 0x4
    int filter_Effects_Wet_FX;  // offset 0x28, size 0x4
};

struct FX_ROADNOISE {
    Csis::Class *mpClass;
    FX_ROADNOISEStruct mData;

    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xF) {
            x = 0xF;
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

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x1FFF) {
            x = 0x1FFF;
        }
        mData.pitch = x;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.azimuth = x;
    }

    void SetType(FXROADNOISETypeType x) {
        mData.type = x;
    }

    void SetSecondaryNoise(int x) {
        mData.secondaryNoise = x;
    }

    void SetSpeed(int x) {
        mData.speed = x;
    }

    void SetFilter_Effects_LoPass(int x) {
        mData.filter_Effects_LoPass = x;
    }

    void SetFilter_Effects_HiPass(int x) {
        mData.filter_Effects_HiPass = x;
    }

    void SetFilter_Effects_Dry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
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

    FX_ROADNOISE(int id, int volume, int pitch, int azimuth, FXROADNOISETypeType type, int secondaryNoise, int speed,
                 int filter_Effects_LoPass, int filter_Effects_HiPass, int filter_Effects_Dry_FX,
                 int filter_Effects_Wet_FX) {
        SetId(id);
        SetVolume(volume);
        SetPitch(pitch);
        SetAzimuth(azimuth);
        SetType(type);
        SetSecondaryNoise(secondaryNoise);
        SetSpeed(speed);
        SetFilter_Effects_LoPass(filter_Effects_LoPass);
        SetFilter_Effects_HiPass(filter_Effects_HiPass);
        SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gFX_ROADNOISEHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_ROADNOISEHandle.Set(&Csis::FX_ROADNOISEId);
            Csis::Class::CreateInstance(&Csis::gFX_ROADNOISEHandle, &mData, &mpClass);
        }
    }

    ~FX_ROADNOISE() {
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

struct FX_ROADNOISE_TRANSStruct {
    int id;                           // offset 0x0, size 0x4
    int volume;                       // offset 0x4, size 0x4
    int pitch;                        // offset 0x8, size 0x4
    int azimuth;                      // offset 0xC, size 0x4
    FXROADNOISETRANSTypeType type;    // offset 0x10, size 0x4
    int secondaryNoise;               // offset 0x14, size 0x4
    int speed;                        // offset 0x18, size 0x4
    int filter_Effects_LoPass;        // offset 0x1C, size 0x4
    int filter_Effects_HiPass;        // offset 0x20, size 0x4
    int filter_Effects_Dry_FX;        // offset 0x24, size 0x4
    int filter_Effects_Wet_FX;        // offset 0x28, size 0x4
};

struct FX_ROADNOISE_TRANS {
    Csis::Class *mpClass;
    FX_ROADNOISE_TRANSStruct mData;

    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xF) {
            x = 0xF;
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

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x1FFF) {
            x = 0x1FFF;
        }
        mData.pitch = x;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.azimuth = x;
    }

    void SetType(FXROADNOISETRANSTypeType x) {
        mData.type = x;
    }

    void SetSecondaryNoise(int x) {
        mData.secondaryNoise = x;
    }

    void SetSpeed(int x) {
        mData.speed = x;
    }

    void SetFilter_Effects_LoPass(int x) {
        mData.filter_Effects_LoPass = x;
    }

    void SetFilter_Effects_HiPass(int x) {
        mData.filter_Effects_HiPass = x;
    }

    void SetFilter_Effects_Dry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
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

    FX_ROADNOISE_TRANS(int id, int volume, int pitch, int azimuth, FXROADNOISETRANSTypeType type, int secondaryNoise,
                       int speed, int filter_Effects_LoPass, int filter_Effects_HiPass, int filter_Effects_Dry_FX,
                       int filter_Effects_Wet_FX) {
        SetId(id);
        SetVolume(volume);
        SetPitch(pitch);
        SetAzimuth(azimuth);
        SetType(type);
        SetSecondaryNoise(secondaryNoise);
        SetSpeed(speed);
        SetFilter_Effects_LoPass(filter_Effects_LoPass);
        SetFilter_Effects_HiPass(filter_Effects_HiPass);
        SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gFX_ROADNOISE_TRANSHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_ROADNOISE_TRANSHandle.Set(&Csis::FX_ROADNOISE_TRANSId);
            Csis::Class::CreateInstance(&Csis::gFX_ROADNOISE_TRANSHandle, &mData, &mpClass);
        }
    }

    ~FX_ROADNOISE_TRANS() {
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

struct FX_TRAFFICStruct {
    int id;                       // offset 0x0, size 0x4
    int volume;                   // offset 0x4, size 0x4
    int pitch_OFFSET;             // offset 0x8, size 0x4
    int azimuth;                  // offset 0xC, size 0x4
    int range;                    // offset 0x10, size 0x4
    int filter_Effects_LoPass;    // offset 0x14, size 0x4
    int filter_Effects_HiPass;    // offset 0x18, size 0x4
    int filter_Effects_Dry_FX;    // offset 0x1C, size 0x4
    int filter_Effects_Wet_FX;    // offset 0x20, size 0x4
};

struct FX_TRAFFIC {
    Csis::Class *mpClass;
    FX_TRAFFICStruct mData;

    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xF) {
            x = 0xF;
        }
        mData.id = x;
    }

    int GetId() {
        return mData.id;
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

    void SetPitch_OFFSET(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        mData.pitch_OFFSET = x;
    }

    int GetPitch_OFFSET() {
        return mData.pitch_OFFSET;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.azimuth = x;
    }

    int GetAzimuth() {
        return mData.azimuth;
    }

    void SetRange(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.range = x;
    }

    int GetRange() {
        return mData.range;
    }

    void SetFilter_Effects_LoPass(int x) {
        mData.filter_Effects_LoPass = x;
    }

    int GetFilter_Effects_LoPass() {
        return mData.filter_Effects_LoPass;
    }

    void SetFilter_Effects_HiPass(int x) {
        mData.filter_Effects_HiPass = x;
    }

    int GetFilter_Effects_HiPass() {
        return mData.filter_Effects_HiPass;
    }

    void SetFilter_Effects_Dry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.filter_Effects_Dry_FX = x;
    }

    int GetFilter_Effects_Dry_FX() {
        return mData.filter_Effects_Dry_FX;
    }

    void SetFilter_Effects_Wet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.filter_Effects_Wet_FX = x;
    }

    int GetFilter_Effects_Wet_FX() {
        return mData.filter_Effects_Wet_FX;
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

    FX_TRAFFIC(int id, int volume, int pitch_OFFSET, int azimuth, int range, int filter_Effects_LoPass, int filter_Effects_HiPass,
               int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        SetId(id);
        SetVolume(volume);
        SetPitch_OFFSET(pitch_OFFSET);
        SetAzimuth(azimuth);
        SetRange(range);
        SetFilter_Effects_LoPass(filter_Effects_LoPass);
        SetFilter_Effects_HiPass(filter_Effects_HiPass);
        SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gFX_TRAFFICHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_TRAFFICHandle.Set(&Csis::FX_TRAFFICId);
            Csis::Class::CreateInstance(&Csis::gFX_TRAFFICHandle, &mData, &mpClass);
        }
    }

    ~FX_TRAFFIC() {
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

enum ENVSTATICTYPEType {
    ENVSTATICTYPETYPE_ENV_COMMON = 0,
    ENVSTATICTYPETYPE_ENV_LVLSPECIFIC_ = 1,
};

struct ENV_STATICStruct {
    int iD;                 // offset 0x0, size 0x4
    int vOLUME;             // offset 0x4, size 0x4
    int pITCH;              // offset 0x8, size 0x4
    int aZIMUTH;            // offset 0xC, size 0x4
    ENVSTATICTYPEType tYPE; // offset 0x10, size 0x4
    int loPass;             // offset 0x14, size 0x4
    int hiPass;             // offset 0x18, size 0x4
    int fX_Dry;             // offset 0x1C, size 0x4
    int fX_Wet;             // offset 0x20, size 0x4
};

struct ENV_STATIC {
    Csis::Class *mpClass;      // offset 0x0, size 0x4
    ENV_STATICStruct mData;    // offset 0x4, size 0x24

    void SetID(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x32) {
            x = 0x32;
        }
        mData.iD = x;
    }

    int GetID() {
        return mData.iD;
    }

    void SetVOLUME(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.vOLUME = x;
    }

    int GetVOLUME() {
        return mData.vOLUME;
    }

    void SetPITCH(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        mData.pITCH = x;
    }

    int GetPITCH() {
        return mData.pITCH;
    }

    void SetAZIMUTH(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.aZIMUTH = x;
    }

    int GetAZIMUTH() {
        return mData.aZIMUTH;
    }

    void SetTYPE(ENVSTATICTYPEType x) {
        mData.tYPE = x;
    }

    ENVSTATICTYPEType GetTYPE() {
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
        mData.hiPass = x;
    }

    int GetHiPass() {
        return mData.hiPass;
    }

    void SetFX_Dry(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.fX_Dry = x;
    }

    int GetFX_Dry() {
        return mData.fX_Dry;
    }

    void SetFX_Wet(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.fX_Wet = x;
    }

    int GetFX_Wet() {
        return mData.fX_Wet;
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

    ENV_STATIC(int iD, int vOLUME, int pITCH, int aZIMUTH, ENVSTATICTYPEType tYPE, int loPass, int hiPass, int fX_Dry, int fX_Wet) {
        SetID(iD);
        SetVOLUME(vOLUME);
        SetPITCH(pITCH);
        SetAZIMUTH(aZIMUTH);
        SetTYPE(tYPE);
        SetLoPass(loPass);
        SetHiPass(hiPass);
        SetFX_Dry(fX_Dry);
        SetFX_Wet(fX_Wet);

        int result = Csis::Class::CreateInstance(&Csis::gENV_STATICHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gENV_STATICHandle.Set(&Csis::ENV_STATICId);
            Csis::Class::CreateInstance(&Csis::gENV_STATICHandle, &mData, &mpClass);
        }
    }

    ~ENV_STATIC() {
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
