//
//
//
#ifndef TURBO_H
#define TURBO_H // Decl: 5

#include "csis/csis.h"
namespace Csis {

extern ClassHandle gFX_TURBO_01Handle;
extern InterfaceId FX_TURBO_01Id;

// total size: 0x18
// Decl: 19
typedef struct {
    int id;       // offset 0x0, size 0x4
    int volume;   // offset 0x4, size 0x4
    int pSI;      // offset 0x8, size 0x4
    int azimuth;  // offset 0xC, size 0x4
    int rotation; // offset 0x10, size 0x4
    int rPM;      // offset 0x14, size 0x4
} FX_TURBO_01Struct;

// total size: 0x1C
// Decl: 48
class FX_TURBO_01 {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 5) {
            x = 5;
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

    void SetPSI(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        this->mData.pSI = x;
    }

    int GetPSI() {
        return this->mData.pSI;
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

    void SetRotation(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        this->mData.rotation = x;
    }

    int GetRotation() {
        return this->mData.rotation;
    }

    void SetRPM(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 10000) {
            x = 10000;
        }
        this->mData.rPM = x;
    }

    int GetRPM() {
        return this->mData.rPM;
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

    FX_TURBO_01(int id, int volume, int pSI, int azimuth, int rotation, int rPM) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPSI(pSI);
        this->SetAzimuth(azimuth);
        this->SetRotation(rotation);
        this->SetRPM(rPM);

        Result result = Class::CreateInstance(&gFX_TURBO_01Handle, &this->mData, &this->mpClass);

        if (result < RESULT_OK) {
            gFX_TURBO_01Handle.Set(&FX_TURBO_01Id);
            Class::CreateInstance(&gFX_TURBO_01Handle, &this->mData, &this->mpClass);
        }
    }

    ~FX_TURBO_01() {
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
    FX_TURBO_01Struct mData;
};

}; // namespace Csis

#endif
