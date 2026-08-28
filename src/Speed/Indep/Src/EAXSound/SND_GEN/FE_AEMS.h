//
//
//
#ifndef FE_AEMS_H
#define FE_AEMS_H

#include "csis/csis.h"

namespace Csis {

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

// Decl: 116
inline Result CacheHandlesFE_AEMS() {
    int result;
    result = gPlayCommonSampleHandle.Set(&PlayCommonSampleId);
    result = gPlayFrontEndSampleHandle.Set(&PlayFrontEndSampleId);
    result = gPlayFrontEndSample_RSHandle.Set(&PlayFrontEndSample_RSId);
    result = gFEDriveOnHandle.Set(&FEDriveOnId);
    return static_cast<Result>(result);
}

// Decl: 135

class PlayCommonSample {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 2000) {
            x = 2000;
        }
        this->mData.id = x;
    }

    int GetId() {
        return this->mData.id;
    }

    void SetVolume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.volume = x;
    }

    int GetVolume() {
        return this->mData.volume;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x1FFF) {
            x = 0x1FFF;
        }
        this->mData.pitch = x;
    }

    int GetPitch() {
        return this->mData.pitch;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.azimuth = x;
    }

    int GetAzimuth() {
        return this->mData.azimuth;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    void *operator new(size_t size) {
        return System::Alloc(size);
    }

    void operator delete(void *ptr) {
        System::Free(ptr);
    }

    PlayCommonSample(int id, int volume, int pitch, int azimuth) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch(pitch);
        this->SetAzimuth(azimuth);

        Result result = Class::CreateInstance(&gPlayCommonSampleHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gPlayCommonSampleHandle.Set(&PlayCommonSampleId);
            Class::CreateInstance(&gPlayCommonSampleHandle, &this->mData, &this->mpClass);
        }
    }

    ~PlayCommonSample() {
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
    Class *mpClass;
    PlayCommonSampleStruct mData;
};

// Decl: 393
class PlayFrontEndSample {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 2000) {
            x = 2000;
        }
        this->mData.id = x;
    }

    int GetId() {
        return this->mData.id;
    }

    void SetVolume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.volume = x;
    }

    int GetVolume() {
        return this->mData.volume;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x1FFF) {
            x = 0x1FFF;
        }
        this->mData.pitch = x;
    }

    int GetPitch() {
        return this->mData.pitch;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.azimuth = x;
    }

    int GetAzimuth() {
        return this->mData.azimuth;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    void *operator new(size_t size) {
        return System::Alloc(size);
    }

    void operator delete(void *ptr) {
        System::Free(ptr);
    }

    PlayFrontEndSample(int id, int volume, int pitch, int azimuth) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch(pitch);
        this->SetAzimuth(azimuth);

        Result result = Class::CreateInstance(&gPlayFrontEndSampleHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gPlayFrontEndSampleHandle.Set(&PlayFrontEndSampleId);
            Class::CreateInstance(&gPlayFrontEndSampleHandle, &this->mData, &this->mpClass);
        }
    }

    ~PlayFrontEndSample() {
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
    Class *mpClass;
    PlayFrontEndSampleStruct mData;
};

// Decl: 522
class PlayFrontEndSample_RS {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 500) {
            x = 500;
        }
        this->mData.id = x;
    }

    int GetId() {
        return this->mData.id;
    }

    void SetVolume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.volume = x;
    }

    int GetVolume() {
        return this->mData.volume;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x1FFF) {
            x = 0x1FFF;
        }
        this->mData.pitch = x;
    }

    int GetPitch() {
        return this->mData.pitch;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.azimuth = x;
    }

    int GetAzimuth() {
        return this->mData.azimuth;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    void *operator new(size_t size) {
        return System::Alloc(size);
    }

    void operator delete(void *ptr) {
        System::Free(ptr);
    }

    PlayFrontEndSample_RS(int id, int volume, int pitch, int azimuth) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch(pitch);
        this->SetAzimuth(azimuth);

        Result result = Class::CreateInstance(&gPlayFrontEndSample_RSHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gPlayFrontEndSample_RSHandle.Set(&PlayFrontEndSample_RSId);
            Class::CreateInstance(&gPlayFrontEndSample_RSHandle, &this->mData, &this->mpClass);
        }
    }

    ~PlayFrontEndSample_RS() {
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
    Class *mpClass;
    PlayFrontEndSample_RSStruct mData;
};

// Decl: 651
class FEDriveOn {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.id = x;
    }

    int GetId() {
        return this->mData.id;
    }

    void SetVolume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.volume = x;
    }

    int GetVolume() {
        return this->mData.volume;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x1FFF) {
            x = 0x1FFF;
        }
        this->mData.pitch = x;
    }

    int GetPitch() {
        return this->mData.pitch;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.azimuth = x;
    }

    int GetAzimuth() {
        return this->mData.azimuth;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    void *operator new(size_t size) {
        return System::Alloc(size);
    }

    void operator delete(void *ptr) {
        System::Free(ptr);
    }

    FEDriveOn(int id, int volume, int pitch, int azimuth) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch(pitch);
        this->SetAzimuth(azimuth);

        Result result = Class::CreateInstance(&gFEDriveOnHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFEDriveOnHandle.Set(&FEDriveOnId);
            Class::CreateInstance(&gFEDriveOnHandle, &this->mData, &this->mpClass);
        }
    }

    ~FEDriveOn() {
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
    Class *mpClass;
    FEDriveOnStruct mData;
};

}; // namespace Csis

#endif
