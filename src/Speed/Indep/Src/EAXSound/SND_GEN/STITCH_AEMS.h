//
//
//
#ifndef STITCH_AEMS_H
#define STITCH_AEMS_H

#include "csis/csis.h"

namespace Csis {

extern InterfaceId AEMS_StichCollisionId;      // size: 0x8, Decl: 16
extern ClassHandle gAEMS_StichCollisionHandle; // size: 0x8, Decl: 17

// total size: 0x28
// Decl: 19
typedef struct {
    int type;          // offset 0x0, size 0x4
    int iD;            // offset 0x4, size 0x4
    int vol;           // offset 0x8, size 0x4
    int pitch;         // offset 0xC, size 0x4
    int az;            // offset 0x10, size 0x4
    int offset;        // offset 0x14, size 0x4
    int filter_DryFX;  // offset 0x18, size 0x4
    int filter_WetFX;  // offset 0x1C, size 0x4
    int filter_LoPass; // offset 0x20, size 0x4
    int filter_HiPass; // offset 0x24, size 0x4
} AEMS_StichCollisionStruct;

extern InterfaceId AEMS_StichWooshId; // size: 0x8, Decl: 44
extern ClassHandle gAEMS_StichWooshHandle;

// total size: 0x28
// Decl: 47
typedef struct {
    int type;          // offset 0x0, size 0x4
    int iD;            // offset 0x4, size 0x4
    int vol;           // offset 0x8, size 0x4
    int pitch;         // offset 0xC, size 0x4
    int az;            // offset 0x10, size 0x4
    int offset;        // offset 0x14, size 0x4
    int filter_DryFX;  // offset 0x18, size 0x4
    int filter_WetFX;  // offset 0x1C, size 0x4
    int filter_LoPass; // offset 0x20, size 0x4
    int filter_HiPass; // offset 0x24, size 0x4
} AEMS_StichWooshStruct;

extern InterfaceId AEMS_StichStaticId;      // size: 0x8
extern ClassHandle gAEMS_StichStaticHandle; // size: 0x8

// total size: 0x28
// Decl: 75
typedef struct {
    int type;          // offset 0x0, size 0x4
    int iD;            // offset 0x4, size 0x4
    int vol;           // offset 0x8, size 0x4
    int pitch;         // offset 0xC, size 0x4
    int az;            // offset 0x10, size 0x4
    int offset;        // offset 0x14, size 0x4
    int filter_DryFX;  // offset 0x18, size 0x4
    int filter_WetFX;  // offset 0x1C, size 0x4
    int filter_LoPass; // offset 0x20, size 0x4
    int filter_HiPass; // offset 0x24, size 0x4
} AEMS_StichStaticStruct;

// total size: 0x2C
// Decl: 116
class AEMS_StichCollision {
  public:
    void SetType(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10) {
            x = 0x10;
        }
        this->mData.type = x;
    }

    int GetType() {
        return this->mData.type;
    }

    void SetID(int x) {
        if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.iD = x;
    }

    int GetID() {
        return this->mData.iD;
    }

    void SetVol(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vol = x;
    }

    int GetVol() {
        return this->mData.vol;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x2000) {
            x = 0x2000;
        }
        this->mData.pitch = x;
    }

    int GetPitch() {
        return this->mData.pitch;
    }

    void SetAz(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        this->mData.az = x;
    }

    int GetAz() {
        return this->mData.az;
    }

    void SetOffset(int x) {
        if (x > 4000) {
            x = 4000;
        }
        this->mData.offset = x;
    }

    int GetOffset() {
        return this->mData.offset;
    }

    void SetFilter_DryFX(int x) {
        this->mData.filter_DryFX = x;
    }

    int GetFilter_DryFX() {
        return this->mData.filter_DryFX;
    }

    void SetFilter_WetFX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_WetFX = x;
    }

    int GetFilter_WetFX() {
        return this->mData.filter_WetFX;
    }

    void SetFilter_LoPass(int x) {
        this->mData.filter_LoPass = x;
    }

    int GetFilter_LoPass() {
        return this->mData.filter_LoPass;
    }

    void SetFilter_HiPass(int x) {
        this->mData.filter_HiPass = x;
    }

    int GetFilter_HiPass() {
        return this->mData.filter_HiPass;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(size_t size) {
        return System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        System::Free(ptr);
    }

    AEMS_StichCollision(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX, int filter_LoPass,
                        int filter_HiPass) {
        this->SetType(type);
        this->SetID(iD);
        this->SetVol(vol);
        this->SetPitch(pitch);
        this->SetAz(az);
        this->SetOffset(offset);
        this->SetFilter_DryFX(filter_DryFX);
        this->SetFilter_WetFX(filter_WetFX);
        this->SetFilter_LoPass(filter_LoPass);
        this->SetFilter_HiPass(filter_HiPass);

        Result result = Class::CreateInstance(&gAEMS_StichCollisionHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gAEMS_StichCollisionHandle.Set(&AEMS_StichCollisionId);
            Class::CreateInstance(&gAEMS_StichCollisionHandle, &this->mData, &this->mpClass);
        }
    }

    ~AEMS_StichCollision() {
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
    AEMS_StichCollision();
    AEMS_StichCollision &operator=(const AEMS_StichCollision &);

    Class *mpClass;                  // offset 0x0, size 0x4, Decl: 338
    AEMS_StichCollisionStruct mData; // offset 0x4, size 0x28, Decl: 339
};

// total size: 0x2C
// Decl: 347
class AEMS_StichWoosh {
  public:
    void SetType(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10) {
            x = 0x10;
        }
        this->mData.type = x;
    }

    int GetType() {
        return this->mData.type;
    }

    void SetID(int x) {
        if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.iD = x;
    }

    int GetID() {
        return this->mData.iD;
    }

    void SetVol(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vol = x;
    }

    int GetVol() {
        return this->mData.vol;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x2000) {
            x = 0x2000;
        }
        this->mData.pitch = x;
    }

    int GetPitch() {
        return this->mData.pitch;
    }

    void SetAz(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        this->mData.az = x;
    }

    int GetAz() {
        return this->mData.az;
    }

    void SetOffset(int x) {
        if (x > 4000) {
            x = 4000;
        }
        this->mData.offset = x;
    }

    int GetOffset() {
        return this->mData.offset;
    }

    void SetFilter_DryFX(int x) {
        this->mData.filter_DryFX = x;
    }

    int GetFilter_DryFX() {
        return this->mData.filter_DryFX;
    }

    void SetFilter_WetFX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_WetFX = x;
    }

    int GetFilter_WetFX() {
        return this->mData.filter_WetFX;
    }

    void SetFilter_LoPass(int x) {
        this->mData.filter_LoPass = x;
    }

    int GetFilter_LoPass() {
        return this->mData.filter_LoPass;
    }

    void SetFilter_HiPass(int x) {
        this->mData.filter_HiPass = x;
    }

    int GetFilter_HiPass() {
        return this->mData.filter_HiPass;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(size_t size) {
        return System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        System::Free(ptr);
    }

    AEMS_StichWoosh(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX, int filter_LoPass,
                    int filter_HiPass) {
        this->SetType(type);
        this->SetID(iD);
        this->SetVol(vol);
        this->SetPitch(pitch);
        this->SetAz(az);
        this->SetOffset(offset);
        this->SetFilter_DryFX(filter_DryFX);
        this->SetFilter_WetFX(filter_WetFX);
        this->SetFilter_LoPass(filter_LoPass);
        this->SetFilter_HiPass(filter_HiPass);

        Result result = Class::CreateInstance(&gAEMS_StichWooshHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gAEMS_StichWooshHandle.Set(&AEMS_StichWooshId);
            Class::CreateInstance(&gAEMS_StichWooshHandle, &this->mData, &this->mpClass);
        }
    }

    ~AEMS_StichWoosh() {
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
    AEMS_StichWoosh();
    AEMS_StichWoosh &operator=(const AEMS_StichWoosh &);

    Class *mpClass;              // offset 0x0, size 0x4
    AEMS_StichWooshStruct mData; // offset 0x4, size 0x28
};

// total size: 0x2C
// Decl: 578
class AEMS_StichStatic {
  public:
    void SetType(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10) {
            x = 0x10;
        }
        this->mData.type = x;
    }

    int GetType() {
        return this->mData.type;
    }

    void SetID(int x) {
        if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.iD = x;
    }

    int GetID() {
        return this->mData.iD;
    }

    void SetVol(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vol = x;
    }

    int GetVol() {
        return this->mData.vol;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x2000) {
            x = 0x2000;
        }
        this->mData.pitch = x;
    }

    int GetPitch() {
        return this->mData.pitch;
    }

    void SetAz(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        this->mData.az = x;
    }

    int GetAz() {
        return this->mData.az;
    }

    void SetOffset(int x) {
        if (x > 4000) {
            x = 4000;
        }
        this->mData.offset = x;
    }

    int GetOffset() {
        return this->mData.offset;
    }

    void SetFilter_DryFX(int x) {
        this->mData.filter_DryFX = x;
    }

    int GetFilter_DryFX() {
        return this->mData.filter_DryFX;
    }

    void SetFilter_WetFX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_WetFX = x;
    }

    int GetFilter_WetFX() {
        return this->mData.filter_WetFX;
    }

    void SetFilter_LoPass(int x) {
        this->mData.filter_LoPass = x;
    }

    int GetFilter_LoPass() {
        return this->mData.filter_LoPass;
    }

    void SetFilter_HiPass(int x) {
        this->mData.filter_HiPass = x;
    }

    int GetFilter_HiPass() {
        return this->mData.filter_HiPass;
    }

    int GetRefCount() {
        int refCount = 0;

        if (this->mpClass != nullptr) {
            this->mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(size_t size) {
        return System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        System::Free(ptr);
    }

    AEMS_StichStatic(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX, int filter_LoPass,
                     int filter_HiPass) {
        this->SetType(type);
        this->SetID(iD);
        this->SetVol(vol);
        this->SetPitch(pitch);
        this->SetAz(az);
        this->SetOffset(offset);
        this->SetFilter_DryFX(filter_DryFX);
        this->SetFilter_WetFX(filter_WetFX);
        this->SetFilter_LoPass(filter_LoPass);
        this->SetFilter_HiPass(filter_HiPass);

        Result result = Class::CreateInstance(&gAEMS_StichStaticHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gAEMS_StichStaticHandle.Set(&AEMS_StichStaticId);
            Class::CreateInstance(&gAEMS_StichStaticHandle, &this->mData, &this->mpClass);
        }
    }

    ~AEMS_StichStatic() {
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
    AEMS_StichStatic();
    AEMS_StichStatic &operator=(const AEMS_StichStatic &);

    Class *mpClass;               // offset 0x0, size 0x4
    AEMS_StichStaticStruct mData; // offset 0x4, size 0x28
};

}; // namespace Csis

#endif
