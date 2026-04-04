#ifndef EAXSOUND_SND_GEN_MAIN_AEMS_H
#define EAXSOUND_SND_GEN_MAIN_AEMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
extern ClassHandle gFX_RadarHandle;
extern InterfaceId FX_RadarId;
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


#endif
