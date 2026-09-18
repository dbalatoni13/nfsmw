#ifndef EAXSOUND_SND_GEN_FE_AEMS_H
#define EAXSOUND_SND_GEN_FE_AEMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

namespace Csis {
extern ClassHandle gPlayCommonSampleHandle;
extern InterfaceId PlayCommonSampleId;
extern ClassHandle gPlayFrontEndSampleHandle;
extern InterfaceId PlayFrontEndSampleId;
extern ClassHandle gPlayFrontEndSample_RSHandle;
extern InterfaceId PlayFrontEndSample_RSId;

// enums que la version del arbol declaraba y la de la rama perdia
enum ePlayCommonSample {
    SCMN_MENUSTART = 0,
    SCMN_MENULEFTRIGHT = 1,
    SCMN_MENUUPDOWN = 2,
    SCMN_MENUSELECT = 3,
    SCMN_MENUCANCEL = 4,
    SCMN_MENUWRONG = 5,
    SCMN_MENUADJUSTVOLUME = 6,
};

enum ePlayFrontEndSample {
    SFE_TRACKLR = 0,
    SFE_CARRIGHT = 1,
    SFE_CARLEFT = 2,
    SFE_CARCOLOR = 3,
    SFE_CARDISPON = 4,
    SFE_CARDISPACTIVE = 5,
    SFE_CARDISPOFF = 6,
    SFE_CARSELECT = 7,
    SFE_CARORBITBMP = 8,
    SFE_TREEUPDOWN = 9,
    SFE_TREELR = 10,
    SFE_TREEZOOM = 11,
    SFE_TREESELECT = 12,
    SFE_TREELOCKED = 13,
    SFE_REWARDVIEW = 14,
    SFE_NAMEKB = 15,
    SFE_NAMESELECT = 16,
    SFE_NAMECASECHANGE = 17,
    SFE_SELRACE = 18,
    SFE_SELPAINT = 19,
    SFE_SELDECAL = 20,
    SFE_SELPARTS = 21,
};

extern InterfaceId PlayCommonSampleId;      // size: 0x8, address: 0x80418880
extern ClassHandle gPlayCommonSampleHandle; // size: 0x8, Decl: 261

// total size: 0x10
// Decl: 19
typedef struct {
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
} PlayCommonSampleStruct;

extern InterfaceId PlayFrontEndSampleId;
extern ClassHandle gPlayFrontEndSampleHandle;

// total size: 0x10
// Decl: 48
typedef struct {
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
} PlayFrontEndSampleStruct;

extern InterfaceId PlayFrontEndSample_RSId;
extern ClassHandle gPlayFrontEndSample_RSHandle;

// total size: 0x10
// Decl: 62
typedef struct {
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
} PlayFrontEndSample_RSStruct;

extern InterfaceId FEDriveOnId;
extern ClassHandle gFEDriveOnHandle;

// total size: 0x10
typedef struct {
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
} FEDriveOnStruct;

// Unused classes TODO

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

    /* MEDIDO DOS VECES (r19 y r20, por separado): pasar esta forma a la de la
     * copia --`int volume = x;` como la de PlayFrontEndSample_RS-- para sacar el
     * `mr. r11,r28` que el objetivo tiene en EAXFrontEnd::Play NO cambia NADA:
     * la propagacion de copias la deshace. Probadas las dos clases por separado,
     * las dos identicas al bit. */
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

        Csis::Result result =
            static_cast<Csis::Result>(Csis::Class::CreateInstance(&Csis::gPlayFrontEndSampleHandle, &mData, &mpClass));
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
        int volume = x;

        if (x < 0) {
            volume = 0;
        } else if (x > 0x7FFF) {
            volume = 0x7FFF;
        }
        mData.volume = volume;
    }

    int GetVolume() {
        return mData.volume;
    }

    void SetPitch(int x) {
        int pitch = x;

        if (x < 0) {
            pitch = 0;
        } else if (x > 0x1FFF) {
            pitch = 0x1FFF;
        }
        mData.pitch = pitch;
    }

    int GetPitch() {
        return mData.pitch;
    }

    void SetAzimuth(int x) {
        int azimuth = x;

        if (x < 0) {
            azimuth = 0;
        } else if (x > 0xFFFF) {
            azimuth = 0xFFFF;
        }
        mData.azimuth = azimuth;
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

        Csis::Result result =
            static_cast<Csis::Result>(Csis::Class::CreateInstance(&Csis::gPlayFrontEndSample_RSHandle, &mData, &mpClass));
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

} // namespace Csis

#endif
