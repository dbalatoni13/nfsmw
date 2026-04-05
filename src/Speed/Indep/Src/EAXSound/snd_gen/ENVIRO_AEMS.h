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
} // namespace Csis

enum FXROADNOISETypeType { FXROADNOISEType_Off = 0, FXROADNOISEType_On = 1 };

enum FXROADNOISETRANSTypeType {
    FXROADNOISETRANSType_Off = 0,
    FXROADNOISETRANSType_On = 1,
};

struct FX_ROADNOISEStruct {
    int id;                     // offset 0x0, size 0x4
    int volume;                 // offset 0x4, size 0x4
    int pitch;                  // offset 0x8, size 0x4
    int azimuth;                // offset 0xC, size 0x4
    int speed;                  // offset 0x10, size 0x4
    int secondaryNoise;         // offset 0x14, size 0x4
    int filter_Effects_Wet_FX;  // offset 0x18, size 0x4
    FXROADNOISETypeType type;   // offset 0x1C, size 0x4
};

struct FX_ROADNOISE : public Csis::Class::CImpl<FX_ROADNOISEStruct> {
    FX_ROADNOISE(FXROADNOISETypeType _Type = FXROADNOISEType_On, int _Id = 0)
        : Csis::Class::CImpl<FX_ROADNOISEStruct>(Csis::Class::CreateInstance(Csis::gFX_ROADNOISEHandle, Csis::FX_ROADNOISEId)) {
        if (!mpClass) {
            mpClass = static_cast<FX_ROADNOISEStruct *>(System::Memory::Malloc(sizeof(FX_ROADNOISEStruct)));
        }
        SetId(_Id);
        SetVolume(0x7fff);
        SetPitch(0x1000);
        SetAzimuth(0);
        SetSpeed(0x1000);
        SetSecondaryNoise(0);
        SetFilter_Effects_Wet_FX(0);
        SetType(_Type);
        CommitMemberData();
    }

    ~FX_ROADNOISE() {
        if (mpClass) {
            Csis::gFX_ROADNOISEHandle.ReleaseInstance(mpClass);
        }
    }

    static void *operator new(unsigned int size) {
        return System::Memory::Malloc(size);
    }

    static void operator delete(void *ptr) {
        System::Memory::Free(ptr);
    }

    void CommitMemberData() {
        mpClass->volume = SetMemberData(mpClass->volume);
        mpClass->pitch = SetMemberData(mpClass->pitch);
        mpClass->azimuth = SetMemberData(mpClass->azimuth);
        mpClass->speed = SetMemberData(mpClass->speed);
        mpClass->secondaryNoise = SetMemberData(mpClass->secondaryNoise);
        mpClass->filter_Effects_Wet_FX = SetMemberData(mpClass->filter_Effects_Wet_FX);
        mpClass->type = SetMemberData(mpClass->type);
    }

    int GetRefCount() {
        return Csis::gFX_ROADNOISEHandle.GetRefCount(mpClass);
    }

    void SetId(int _Id) {
        mpClass->id = _Id;
    }

    void SetVolume(int _Volume) {
        if (_Volume < 0) {
            _Volume = 0;
        } else if (_Volume > 0x7fff) {
            _Volume = 0x7fff;
        }
        mpClass->volume = _Volume;
    }

    void SetPitch(int _Pitch) {
        if (_Pitch < 0) {
            _Pitch = 0;
        } else if (_Pitch > 0x1fff) {
            _Pitch = 0x1fff;
        }
        mpClass->pitch = _Pitch;
    }

    void SetAzimuth(int _Azimuth) {
        if (_Azimuth < 0) {
            _Azimuth = 0;
        } else if (_Azimuth > 0xffff) {
            _Azimuth = 0xffff;
        }
        mpClass->azimuth = _Azimuth;
    }

    void SetSpeed(int _Speed) {
        mpClass->speed = _Speed;
    }

    void SetSecondaryNoise(int _SecondaryNoise) {
        mpClass->secondaryNoise = _SecondaryNoise;
    }

    void SetFilter_Effects_Wet_FX(int _Filter_Effects_Wet_FX) {
        mpClass->filter_Effects_Wet_FX = _Filter_Effects_Wet_FX;
    }

    void SetType(FXROADNOISETypeType _Type) {
        mpClass->type = _Type;
    }
};

struct FX_ROADNOISE_TRANSStruct {
    int id;                        // offset 0x0, size 0x4
    int volume;                    // offset 0x4, size 0x4
    int pitch;                     // offset 0x8, size 0x4
    int azimuth;                   // offset 0xC, size 0x4
    FXROADNOISETRANSTypeType type; // offset 0x10, size 0x4
};

struct FX_ROADNOISE_TRANS : public Csis::Class::CImpl<FX_ROADNOISE_TRANSStruct> {
    FX_ROADNOISE_TRANS(FXROADNOISETRANSTypeType _Type = FXROADNOISETRANSType_On, int _Id = 0)
        : Csis::Class::CImpl<FX_ROADNOISE_TRANSStruct>(Csis::Class::CreateInstance(Csis::gFX_ROADNOISE_TRANSHandle, Csis::FX_ROADNOISE_TRANSId)) {
        if (!mpClass) {
            mpClass = static_cast<FX_ROADNOISE_TRANSStruct *>(System::Memory::Malloc(sizeof(FX_ROADNOISE_TRANSStruct)));
        }
        SetId(_Id);
        SetVolume(0x7fff);
        SetPitch(0x1000);
        SetAzimuth(0);
        SetType(_Type);
        CommitMemberData();
    }

    ~FX_ROADNOISE_TRANS() {
        if (mpClass) {
            Csis::gFX_ROADNOISE_TRANSHandle.ReleaseInstance(mpClass);
        }
    }

    static void *operator new(unsigned int size) {
        return System::Memory::Malloc(size);
    }

    static void operator delete(void *ptr) {
        System::Memory::Free(ptr);
    }

    void CommitMemberData() {
        mpClass->volume = SetMemberData(mpClass->volume);
        mpClass->pitch = SetMemberData(mpClass->pitch);
        mpClass->azimuth = SetMemberData(mpClass->azimuth);
        mpClass->type = SetMemberData(mpClass->type);
    }

    int GetRefCount() {
        return Csis::gFX_ROADNOISE_TRANSHandle.GetRefCount(mpClass);
    }

    void SetId(int _Id) {
        mpClass->id = _Id;
    }

    void SetVolume(int _Volume) {
        if (_Volume < 0) {
            _Volume = 0;
        } else if (_Volume > 0x7fff) {
            _Volume = 0x7fff;
        }
        mpClass->volume = _Volume;
    }

    void SetPitch(int _Pitch) {
        if (_Pitch < 0) {
            _Pitch = 0;
        } else if (_Pitch > 0x1fff) {
            _Pitch = 0x1fff;
        }
        mpClass->pitch = _Pitch;
    }

    void SetAzimuth(int _Azimuth) {
        if (_Azimuth < 0) {
            _Azimuth = 0;
        } else if (_Azimuth > 0xffff) {
            _Azimuth = 0xffff;
        }
        mpClass->azimuth = _Azimuth;
    }

    void SetType(FXROADNOISETRANSTypeType _Type) {
        mpClass->type = _Type;
    }
};

#endif
