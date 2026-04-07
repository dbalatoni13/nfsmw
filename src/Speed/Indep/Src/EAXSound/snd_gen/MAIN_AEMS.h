#ifndef EAXSOUND_SND_GEN_MAIN_AEMS_H
#define EAXSOUND_SND_GEN_MAIN_AEMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
extern ClassHandle gFX_RadarHandle;
extern InterfaceId FX_RadarId;
extern ClassHandle gFX_HelicopterHandle;
extern InterfaceId FX_HelicopterId;
extern ClassHandle gFX_SKIDHandle;
extern InterfaceId FX_SKIDId;
extern ClassHandle gFX_NITROUSHandle;
extern InterfaceId FX_NITROUSId;
extern ClassHandle gFX_PURGEHandle;
extern InterfaceId FX_PURGEId;
extern ClassHandle gFX_ScrapeHandle;
extern InterfaceId FX_ScrapeId;
extern ClassHandle gFX_CameraHandle;
extern InterfaceId FX_CameraId;
extern ClassHandle gFX_UVESHandle;
extern InterfaceId FX_UVESId;
extern ClassHandle gFX_SHIFTING_01Handle;
extern InterfaceId FX_SHIFTING_01Id;
extern ClassHandle gFX_WeatherHandle;
extern InterfaceId FX_WeatherId;
} // namespace Csis

enum FXSHIFTING01TypeType {
    FXSHIFTING01TYPETYPE_SHIFT = 0,
    FXSHIFTING01TYPETYPE_ATTACK_ = 1,
};

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

struct FX_NITROUSStruct {
    int nIT_ID;                 // offset 0x0, size 0x4
    int nIT_volume;             // offset 0x4, size 0x4
    int nIT_azimuth;            // offset 0x8, size 0x4
    int nIT_STOP;               // offset 0xC, size 0x4
    int pitch;                  // offset 0x10, size 0x4
    int filter_Effects_LoPass;  // offset 0x14, size 0x4
    int filter_Effects_HiPass;  // offset 0x18, size 0x4
    int filter_Effects_Dry_FX;  // offset 0x1C, size 0x4
    int filter_Effects_Wet_FX;  // offset 0x20, size 0x4
};

struct FX_NITROUS {
    Csis::Class *mpClass;         // offset 0x0, size 0x4
    FX_NITROUSStruct mData;       // offset 0x4, size 0x24

    void SetNIT_ID(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 2) {
            x = 2;
        }
        mData.nIT_ID = x;
    }

    int GetNIT_ID() {
        return mData.nIT_ID;
    }

    void SetNIT_volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.nIT_volume = x;
    }

    int GetNIT_volume() {
        return mData.nIT_volume;
    }

    void SetNIT_azimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.nIT_azimuth = x;
    }

    int GetNIT_azimuth() {
        return mData.nIT_azimuth;
    }

    void SetNIT_STOP(int x) {
        mData.nIT_STOP = x;
    }

    int GetNIT_STOP() {
        return mData.nIT_STOP;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        mData.pitch = x;
    }

    int GetPitch() {
        return mData.pitch;
    }

    void SetFilter_Effects_LoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
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

    FX_NITROUS(int nIT_ID, int nIT_volume, int nIT_azimuth, int nIT_STOP, int pitch, int filter_Effects_LoPass,
               int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        SetNIT_ID(nIT_ID);
        SetNIT_volume(nIT_volume);
        SetNIT_azimuth(nIT_azimuth);
        SetNIT_STOP(nIT_STOP);
        SetPitch(pitch);
        SetFilter_Effects_LoPass(filter_Effects_LoPass);
        SetFilter_Effects_HiPass(filter_Effects_HiPass);
        SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gFX_NITROUSHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_NITROUSHandle.Set(&Csis::FX_NITROUSId);
            Csis::Class::CreateInstance(&Csis::gFX_NITROUSHandle, &mData, &mpClass);
        }
    }

    ~FX_NITROUS() {
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

struct FX_PURGEStruct {
    int pURGE_volume;            // offset 0x0, size 0x4
    int pURGE_azimuth;           // offset 0x4, size 0x4
    int pURGE_STOP;              // offset 0x8, size 0x4
    int pURGE_pitch;             // offset 0xC, size 0x4
    int filter_Effects_LoPass;   // offset 0x10, size 0x4
    int filter_Effects_HiPass;   // offset 0x14, size 0x4
    int filter_Effects_Dry_FX;   // offset 0x18, size 0x4
    int filter_Effects_Wet_FX;   // offset 0x1C, size 0x4
};

struct FX_PURGE {
    Csis::Class *mpClass;       // offset 0x0, size 0x4
    FX_PURGEStruct mData;       // offset 0x4, size 0x20

    void SetPURGE_volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.pURGE_volume = x;
    }

    int GetPURGE_volume() {
        return mData.pURGE_volume;
    }

    void SetPURGE_azimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.pURGE_azimuth = x;
    }

    int GetPURGE_azimuth() {
        return mData.pURGE_azimuth;
    }

    void SetPURGE_STOP(int x) {
        mData.pURGE_STOP = x;
    }

    int GetPURGE_STOP() {
        return mData.pURGE_STOP;
    }

    void SetPURGE_pitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        mData.pURGE_pitch = x;
    }

    int GetPURGE_pitch() {
        return mData.pURGE_pitch;
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
        mData.filter_Effects_Dry_FX = x;
    }

    int GetFilter_Effects_Dry_FX() {
        return mData.filter_Effects_Dry_FX;
    }

    void SetFilter_Effects_Wet_FX(int x) {
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

    FX_PURGE(int pURGE_volume, int pURGE_azimuth, int pURGE_STOP, int pURGE_pitch, int filter_Effects_LoPass,
             int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        SetPURGE_volume(pURGE_volume);
        SetPURGE_azimuth(pURGE_azimuth);
        SetPURGE_STOP(pURGE_STOP);
        SetPURGE_pitch(pURGE_pitch);
        SetFilter_Effects_LoPass(filter_Effects_LoPass);
        SetFilter_Effects_HiPass(filter_Effects_HiPass);
        SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gFX_PURGEHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_PURGEHandle.Set(&Csis::FX_PURGEId);
            Csis::Class::CreateInstance(&Csis::gFX_PURGEHandle, &mData, &mpClass);
        }
    }

    ~FX_PURGE() {
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

struct FX_HelicopterStruct {
    int hELI_ID;             // offset 0x0, size 0x4
    int hELI_Pitch_Offset;   // offset 0x4, size 0x4
    int hELI_Stop;           // offset 0x8, size 0x4
    int hELI_Volume;         // offset 0xC, size 0x4
    int hELI_Azmuth;         // offset 0x10, size 0x4
    int hELI_Speed;          // offset 0x14, size 0x4
    int hELI_Distance;       // offset 0x18, size 0x4
    int hELI_LowPass;        // offset 0x1C, size 0x4
    int hELI_HiPass;         // offset 0x20, size 0x4
    int hELI_FX_Dry;         // offset 0x24, size 0x4
    int hELI_FX_Wet;         // offset 0x28, size 0x4
    int hELI_Rotation;       // offset 0x2C, size 0x4
};

struct FX_Helicopter {
    Csis::Class *mpClass;         // offset 0x0, size 0x4
    FX_HelicopterStruct mData;    // offset 0x4, size 0x30

    void SetHELI_ID(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.hELI_ID = x;
    }

    int GetHELI_ID() {
        return mData.hELI_ID;
    }

    void SetHELI_Pitch_Offset(int x) {
        if (x < -0x2000) {
            x = -0x2000;
        } else if (x > 0x2000) {
            x = 0x2000;
        }
        mData.hELI_Pitch_Offset = x;
    }

    int GetHELI_Pitch_Offset() {
        return mData.hELI_Pitch_Offset;
    }

    void SetHELI_Stop(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1) {
            x = 1;
        }
        mData.hELI_Stop = x;
    }

    int GetHELI_Stop() {
        return mData.hELI_Stop;
    }

    void SetHELI_Volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.hELI_Volume = x;
    }

    int GetHELI_Volume() {
        return mData.hELI_Volume;
    }

    void SetHELI_Azmuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.hELI_Azmuth = x;
    }

    int GetHELI_Azmuth() {
        return mData.hELI_Azmuth;
    }

    void SetHELI_Speed(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        mData.hELI_Speed = x;
    }

    int GetHELI_Speed() {
        return mData.hELI_Speed;
    }

    void SetHELI_Distance(int x) {
        if (x < -100) {
            x = -100;
        } else if (x > 100) {
            x = 100;
        }
        mData.hELI_Distance = x;
    }

    int GetHELI_Distance() {
        return mData.hELI_Distance;
    }

    void SetHELI_LowPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        mData.hELI_LowPass = x;
    }

    int GetHELI_LowPass() {
        return mData.hELI_LowPass;
    }

    void SetHELI_HiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        mData.hELI_HiPass = x;
    }

    int GetHELI_HiPass() {
        return mData.hELI_HiPass;
    }

    void SetHELI_FX_Dry(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.hELI_FX_Dry = x;
    }

    int GetHELI_FX_Dry() {
        return mData.hELI_FX_Dry;
    }

    void SetHELI_FX_Wet(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.hELI_FX_Wet = x;
    }

    int GetHELI_FX_Wet() {
        return mData.hELI_FX_Wet;
    }

    void SetHELI_Rotation(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.hELI_Rotation = x;
    }

    int GetHELI_Rotation() {
        return mData.hELI_Rotation;
    }

    int GetRefCount() {
        return mData.hELI_ID;
    }

    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    FX_Helicopter(int hELI_ID, int hELI_Pitch_Offset, int hELI_Stop, int hELI_Volume, int hELI_Azmuth, int hELI_Speed, int hELI_Distance,
                  int hELI_LowPass, int hELI_HiPass, int hELI_FX_Dry, int hELI_FX_Wet, int hELI_Rotation)
        : mpClass(nullptr) {
        Csis::System::Result result;

        SetHELI_ID(hELI_ID);
        SetHELI_Pitch_Offset(hELI_Pitch_Offset);
        SetHELI_Stop(hELI_Stop);
        SetHELI_Volume(hELI_Volume);
        SetHELI_Azmuth(hELI_Azmuth);
        SetHELI_Speed(hELI_Speed);
        SetHELI_Distance(hELI_Distance);
        SetHELI_LowPass(hELI_LowPass);
        SetHELI_HiPass(hELI_HiPass);
        SetHELI_FX_Dry(hELI_FX_Dry);
        SetHELI_FX_Wet(hELI_FX_Wet);
        SetHELI_Rotation(hELI_Rotation);

        result = static_cast<Csis::System::Result>(Csis::Class::CreateInstance(&Csis::gFX_HelicopterHandle, &mData, &mpClass));
        if (result < Csis::System::kResult_Ok) {
            Csis::gFX_HelicopterHandle.Set(&Csis::FX_HelicopterId);
            Csis::Class::CreateInstance(&Csis::gFX_HelicopterHandle, &mData, &mpClass);
        }
    }

    ~FX_Helicopter() {
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

struct FX_ScrapeStruct {
    int volume;                // offset 0x0, size 0x4
    int pitch;                 // offset 0x4, size 0x4
    int azimuth;               // offset 0x8, size 0x4
    int terrain_type;          // offset 0xC, size 0x4
    int impulse_magnitude;     // offset 0x10, size 0x4
    int filter_Effects_LoPass; // offset 0x14, size 0x4
    int filter_Effects_HiPass; // offset 0x18, size 0x4
    int filter_Effects_Dry_FX; // offset 0x1C, size 0x4
    int filter_Effects_Wet_FX; // offset 0x20, size 0x4
};

struct FX_Scrape {
    Csis::Class *mpClass; // offset 0x0, size 0x4
    FX_ScrapeStruct mData; // offset 0x4, size 0x24

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

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 9000) {
            x = 9000;
        }
        mData.pitch = x;
    }

    int GetPitch() {
        return mData.pitch;
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

    void SetTerrain_type(int x) {
        mData.terrain_type = x;
    }

    int GetTerrain_type() {
        return mData.terrain_type;
    }

    void SetImpulse_magnitude(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7F) {
            x = 0x7F;
        }
        mData.impulse_magnitude = x;
    }

    int GetImpulse_magnitude() {
        return mData.impulse_magnitude;
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

    FX_Scrape(int volume, int pitch, int azimuth, int terrain_type, int impulse_magnitude, int filter_Effects_LoPass,
              int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        SetVolume(volume);
        SetPitch(pitch);
        SetAzimuth(azimuth);
        SetTerrain_type(terrain_type);
        SetImpulse_magnitude(impulse_magnitude);
        SetFilter_Effects_LoPass(filter_Effects_LoPass);
        SetFilter_Effects_HiPass(filter_Effects_HiPass);
        SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gFX_ScrapeHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_ScrapeHandle.Set(&Csis::FX_ScrapeId);
            Csis::Class::CreateInstance(&Csis::gFX_ScrapeHandle, &mData, &mpClass);
        }
    }

    ~FX_Scrape() {
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

struct FX_UVESStruct {
    int iD;           // offset 0x0, size 0x4
    int volume;       // offset 0x4, size 0x4
    int width;        // offset 0x8, size 0x4
    int pitch_Offset; // offset 0xC, size 0x4
    int intensity;    // offset 0x10, size 0x4
    int stop;         // offset 0x14, size 0x4
};

class FX_UVES {
  public:
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

    void SetWidth(int x) {
        mData.width = x;
    }

    int GetWidth() {
        return mData.width;
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

    FX_UVES(int iD, int volume, int width, int pitch_Offset, int intensity, int stop) {
        SetID(iD);
        SetVolume(volume);
        SetWidth(width);
        SetPitch_Offset(pitch_Offset);
        SetIntensity(intensity);
        SetStop(stop);

        int result = Csis::Class::CreateInstance(&Csis::gFX_UVESHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_UVESHandle.Set(&Csis::FX_UVESId);
            Csis::Class::CreateInstance(&Csis::gFX_UVESHandle, &mData, &mpClass);
        }
    }

    ~FX_UVES() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }

  private:
    Csis::Class *mpClass; // offset 0x0, size 0x4
    FX_UVESStruct mData;  // offset 0x4, size 0x18
};

struct FX_CameraStruct {
    int iD;                // offset 0x0, size 0x4
    int volume;            // offset 0x4, size 0x4
    int width;             // offset 0x8, size 0x4
    int pitch_Offset;      // offset 0xC, size 0x4
    int cam_beep;          // offset 0x10, size 0x4
    int cam_beep_Volume;   // offset 0x14, size 0x4
    int cam_whine;         // offset 0x18, size 0x4
    int cam_whine_Volume;  // offset 0x1C, size 0x4
};

class FX_Camera {
  public:
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

    void SetWidth(int x) {
        mData.width = x;
    }

    int GetWidth() {
        return mData.width;
    }

    void SetPitch_Offset(int x) {
        mData.pitch_Offset = x;
    }

    int GetPitch_Offset() {
        return mData.pitch_Offset;
    }

    void SetCam_beep(int x) {
        mData.cam_beep = x;
    }

    int GetCam_beep() {
        return mData.cam_beep;
    }

    void SetCam_beep_Volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.cam_beep_Volume = x;
    }

    int GetCam_beep_Volume() {
        return mData.cam_beep_Volume;
    }

    void SetCam_whine(int x) {
        mData.cam_whine = x;
    }

    int GetCam_whine() {
        return mData.cam_whine;
    }

    void SetCam_whine_Volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.cam_whine_Volume = x;
    }

    int GetCam_whine_Volume() {
        return mData.cam_whine_Volume;
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

    FX_Camera(int iD, int volume, int width, int pitch_Offset, int cam_beep, int cam_beep_Volume, int cam_whine,
              int cam_whine_Volume) {
        SetID(iD);
        SetVolume(volume);
        SetWidth(width);
        SetPitch_Offset(pitch_Offset);
        SetCam_beep(cam_beep);
        SetCam_beep_Volume(cam_beep_Volume);
        SetCam_whine(cam_whine);
        SetCam_whine_Volume(cam_whine_Volume);

        int result = Csis::Class::CreateInstance(&Csis::gFX_CameraHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_CameraHandle.Set(&Csis::FX_CameraId);
            Csis::Class::CreateInstance(&Csis::gFX_CameraHandle, &mData, &mpClass);
        }
    }

    ~FX_Camera() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }

  private:
    Csis::Class *mpClass;   // offset 0x0, size 0x4
    FX_CameraStruct mData;  // offset 0x4, size 0x20
};

struct FX_WeatherStruct {
    int hood_Rain;               // offset 0x0, size 0x4
    int hood_Rain_Vol_Substract; // offset 0x4, size 0x4
    int volume;                  // offset 0x8, size 0x4
    int width;                   // offset 0xC, size 0x4
    int pitch_Offset;            // offset 0x10, size 0x4
    int rain_on_off;             // offset 0x14, size 0x4
    int filter_Effects_LoPass;   // offset 0x18, size 0x4
    int filter_Effects_HiPass;   // offset 0x1C, size 0x4
    int filter_Effects_Dry_FX;   // offset 0x20, size 0x4
    int filter_Effects_Wet_FX;   // offset 0x24, size 0x4
};

struct FX_Weather {
  public:
    void SetHood_Rain(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 3) {
            x = 3;
        }
        mData.hood_Rain = x;
    }

    int GetHood_Rain() {
        return mData.hood_Rain;
    }

    void SetHood_Rain_Vol_Substract(int x) {
        mData.hood_Rain_Vol_Substract = x;
    }

    int GetHood_Rain_Vol_Substract() {
        return mData.hood_Rain_Vol_Substract;
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

    void SetWidth(int x) {
        mData.width = x;
    }

    int GetWidth() {
        return mData.width;
    }

    void SetPitch_Offset(int x) {
        mData.pitch_Offset = x;
    }

    int GetPitch_Offset() {
        return mData.pitch_Offset;
    }

    void SetRain_on_off(int x) {
        mData.rain_on_off = x;
    }

    int GetRain_on_off() {
        return mData.rain_on_off;
    }

    void SetFilter_Effects_LoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
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

    FX_Weather(int hood_Rain, int hood_Rain_Vol_Substract, int volume, int width, int pitch_Offset, int rain_on_off,
               int filter_Effects_LoPass, int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        SetHood_Rain(hood_Rain);
        SetHood_Rain_Vol_Substract(hood_Rain_Vol_Substract);
        SetVolume(volume);
        SetWidth(width);
        SetPitch_Offset(pitch_Offset);
        SetRain_on_off(rain_on_off);
        SetFilter_Effects_LoPass(filter_Effects_LoPass);
        SetFilter_Effects_HiPass(filter_Effects_HiPass);
        SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        int result = Csis::Class::CreateInstance(&Csis::gFX_WeatherHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_WeatherHandle.Set(&Csis::FX_WeatherId);
            Csis::Class::CreateInstance(&Csis::gFX_WeatherHandle, &mData, &mpClass);
        }
    }

    ~FX_Weather() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }

  private:
    Csis::Class *mpClass;    // offset 0x0, size 0x4
    FX_WeatherStruct mData;  // offset 0x4, size 0x28
};

struct FX_SHIFTING_01Struct {
    int id;                  // offset 0x0, size 0x4
    int volume;              // offset 0x4, size 0x4
    int pitch;               // offset 0x8, size 0x4
    int azimuth;             // offset 0xC, size 0x4
    FXSHIFTING01TypeType type; // offset 0x10, size 0x4
    int cAM;                 // offset 0x14, size 0x4
};

struct FX_SHIFTING_01 {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 2) {
            x = 2;
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

    void SetPitch(int x) {
        mData.pitch = x;
    }

    int GetPitch() {
        return mData.pitch;
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

    void SetType(FXSHIFTING01TypeType x) {
        mData.type = x;
    }

    FXSHIFTING01TypeType GetType() {
        return mData.type;
    }

    void SetCAM(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1) {
            x = 1;
        }
        mData.cAM = x;
    }

    int GetCAM() {
        return mData.cAM;
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

    FX_SHIFTING_01(int id, int volume, int pitch, int azimuth, FXSHIFTING01TypeType type, int cAM) {
        SetId(id);
        SetVolume(volume);
        SetPitch(pitch);
        SetAzimuth(azimuth);
        SetType(type);
        SetCAM(cAM);

        int result = Csis::Class::CreateInstance(&Csis::gFX_SHIFTING_01Handle, &mData, &mpClass);
        if (result < 0) {
            Csis::gFX_SHIFTING_01Handle.Set(&Csis::FX_SHIFTING_01Id);
            Csis::Class::CreateInstance(&Csis::gFX_SHIFTING_01Handle, &mData, &mpClass);
        }
    }

    ~FX_SHIFTING_01() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }

  private:
    Csis::Class *mpClass;         // offset 0x0, size 0x4
    FX_SHIFTING_01Struct mData;   // offset 0x4, size 0x18
};


#endif
