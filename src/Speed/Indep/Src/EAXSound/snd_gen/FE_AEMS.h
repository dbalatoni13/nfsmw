#ifndef EAXSOUND_SND_GEN_FE_AEMS_H
#define EAXSOUND_SND_GEN_FE_AEMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

namespace Csis {
extern ClassHandle gPlayCommonSampleHandle;
extern InterfaceId PlayCommonSampleId;
extern ClassHandle gPlayFrontEndSampleHandle;
extern InterfaceId PlayFrontEndSampleId;
extern ClassHandle gPlayFrontEndSample_RSHandle;
extern InterfaceId PlayFrontEndSample_RSId;
} // namespace Csis

struct PlayFrontEndSampleStruct {
    int id;
    int volume;
    int pitch;
    int azimuth;
};

struct PlayFrontEndSampleSt {
    int nHack;
    int id;
    int volume;
    int pitch;
    int azimuth;
};

struct PlayFrontEndSample_RSStruct {
    int id;
    int volume;
    int pitch;
    int azimuth;
};

struct PlayCommonSampleStruct {
    int id;
    int volume;
    int pitch;
    int azimuth;
};

struct PlayCommonSampleSt {
    int nHack;
    int id;
    int volume;
    int pitch;
    int azimuth;
};

struct PlayCommonSample {
    Csis::Class *mpClass;
    PlayCommonSampleStruct mData;

    void SetId(int x) {
        if (x < UISND_COMMON_UP) {
            x = UISND_COMMON_UP;
        } else if (x > 2000) {
            x = 2000;
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
        if (x < 0) {
            x = 0;
        } else if (x > 0x1FFF) {
            x = 0x1FFF;
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

    PlayCommonSample(int id, int volume, int pitch, int azimuth) {
        SetId(id);
        SetVolume(volume);
        SetPitch(pitch);
        SetAzimuth(azimuth);

        int result = Csis::Class::CreateInstance(&Csis::gPlayCommonSampleHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gPlayCommonSampleHandle.Set(&Csis::PlayCommonSampleId);
            Csis::Class::CreateInstance(&Csis::gPlayCommonSampleHandle, &mData, &mpClass);
        }
    }

    ~PlayCommonSample() {
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

struct PlayFrontEndSample {
    Csis::Class *mpClass;
    PlayFrontEndSampleStruct mData;

    void SetId(int x) {
        if (x < UISND_COMMON_UP) {
            x = UISND_COMMON_UP;
        } else if (x > 2000) {
            x = 2000;
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
        if (x < 0) {
            x = 0;
        } else if (x > 0x1FFF) {
            x = 0x1FFF;
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

    PlayFrontEndSample(int id, int volume, int pitch, int azimuth) {
        SetId(id);
        SetVolume(volume);
        SetPitch(pitch);
        SetAzimuth(azimuth);

        int result = Csis::Class::CreateInstance(&Csis::gPlayFrontEndSampleHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gPlayFrontEndSampleHandle.Set(&Csis::PlayFrontEndSampleId);
            Csis::Class::CreateInstance(&Csis::gPlayFrontEndSampleHandle, &mData, &mpClass);
        }
    }

    ~PlayFrontEndSample() {
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

struct PlayFrontEndSample_RS {
    Csis::Class *mpClass;
    PlayFrontEndSample_RSStruct mData;

    void SetId(int x) {
        if (x < UISND_COMMON_UP) {
            x = UISND_COMMON_UP;
        } else if (x > 500) {
            x = 500;
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
        mData.azimuth = x;
    }

    int GetAzimuth() {
        return mData.azimuth;
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

    PlayFrontEndSample_RS(int id, int volume, int pitch, int azimuth) {
        SetId(id);
        SetVolume(volume);
        SetPitch(pitch);
        SetAzimuth(azimuth);

        int result = Csis::Class::CreateInstance(&Csis::gPlayFrontEndSample_RSHandle, &mData, &mpClass);
        if (result < 0) {
            Csis::gPlayFrontEndSample_RSHandle.Set(&Csis::PlayFrontEndSample_RSId);
            Csis::Class::CreateInstance(&Csis::gPlayFrontEndSample_RSHandle, &mData, &mpClass);
        }
    }

    ~PlayFrontEndSample_RS() {
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
