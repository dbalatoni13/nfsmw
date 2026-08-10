#ifndef ENVIRO_AEMS_H
#define ENVIRO_AEMS_H

#include "csis/csis.h"

namespace Csis {

// Decl: 16
enum FXROADNOISETypeType {
    FXROADNOISETYPETYPE_LOOP = 0,
    FXROADNOISETYPETYPE_TRANSITION_ = 1,
};

// Decl: 22
enum FXROADNOISETRANSTypeType {
    FXROADNOISETRANSTYPETYPE_LOOP = 0,
    FXROADNOISETRANSTYPETYPE_TRANSITION_ = 1,
};

// Decl: 28
enum ENVSTATICTYPEType {
    ENVSTATICTYPETYPE_ENV_COMMON = 0,
    ENVSTATICTYPETYPE_ENV_LVLSPECIFIC_ = 1,
};

extern InterfaceId FX_ROADNOISEId;      // size: 0x8, address: 0x80418848, Decl: 38
extern ClassHandle gFX_ROADNOISEHandle; // size: 0x8, Decl: 39

// Decl: 41
typedef struct {
    int id;                    // offset 0x0, size 0x4
    int volume;                // offset 0x4, size 0x4
    int pitch;                 // offset 0x8, size 0x4
    int azimuth;               // offset 0xC, size 0x4
    FXROADNOISETypeType type;  // offset 0x10, size 0x4
    int secondaryNoise;        // offset 0x14, size 0x4
    int speed;                 // offset 0x18, size 0x4
    int filter_Effects_LoPass; // offset 0x1C, size 0x4
    int filter_Effects_HiPass; // offset 0x20, size 0x4
    int filter_Effects_Dry_FX; // offset 0x24, size 0x4
    int filter_Effects_Wet_FX; // offset 0x28, size 0x4
} FX_ROADNOISEStruct;

extern InterfaceId FX_ROADNOISE_TRANSId;      // size: 0x8, address: 0x80418850, Decl: 76
extern ClassHandle gFX_ROADNOISE_TRANSHandle; // size: 0x8, Decl: 77

// Decl: 79
typedef struct {
    int id;                        // offset 0x0, size 0x4
    int volume;                    // offset 0x4, size 0x4
    int pitch;                     // offset 0x8, size 0x4
    int azimuth;                   // offset 0xC, size 0x4
    FXROADNOISETRANSTypeType type; // offset 0x10, size 0x4
    int secondaryNoise;            // offset 0x14, size 0x4
    int speed;                     // offset 0x18, size 0x4
    int filter_Effects_LoPass;     // offset 0x1C, size 0x4
    int filter_Effects_HiPass;     // offset 0x20, size 0x4
    int filter_Effects_Dry_FX;     // offset 0x24, size 0x4
    int filter_Effects_Wet_FX;     // offset 0x28, size 0x4
} FX_ROADNOISE_TRANSStruct;

extern InterfaceId ENV_STATICId;      // size: 0x8, address: 0x80418858, Decl: 114
extern ClassHandle gENV_STATICHandle; // size: 0x8, Decl: 115

// Decl: 117
typedef struct {
    int iD;                 // offset 0x0, size 0x4
    int vOLUME;             // offset 0x4, size 0x4
    int pITCH;              // offset 0x8, size 0x4
    int aZIMUTH;            // offset 0xC, size 0x4
    ENVSTATICTYPEType tYPE; // offset 0x10, size 0x4
    int loPass;             // offset 0x14, size 0x4
    int hiPass;             // offset 0x18, size 0x4
    int fX_Dry;             // offset 0x1C, size 0x4
    int fX_Wet;             // offset 0x20, size 0x4
} ENV_STATICStruct;

extern InterfaceId FX_MAIN_MEMId;      // size: 0x8, address: 0x80418860, Decl: 142
extern ClassHandle gFX_MAIN_MEMHandle; // size: 0x8, Decl: 143

extern InterfaceId FX_WINDId;      // size: 0x8, address: 0x80418868, Decl: 163
extern ClassHandle gFX_WINDHandle; // size: 0x8, Decl: 164

// total size: 0x38
// Decl: 166
typedef struct {
    int pitch;            // offset 0x0, size 0x4, Decl: 169
    int volume_left;      // offset 0x4, size 0x4, Decl: 172
    int volume_right;     // offset 0x8, size 0x4, Decl: 175
    int azimuth_left;     // offset 0xC, size 0x4, Decl: 178
    int azimuth_right;    // offset 0x10, size 0x4, Decl: 181
    int intensity;        // offset 0x14, size 0x4, Decl: 184
    int camera;           // offset 0x18, size 0x4, Decl: 187
    int override_Trigger; // offset 0x1C, size 0x4, Decl: 190
    int rumble_Volume;    // offset 0x20, size 0x4, Decl: 193
    int lowPass;          // offset 0x24, size 0x4, Decl: 196
    int hiPass;           // offset 0x28, size 0x4, Decl: 199
    int fX_Dry;           // offset 0x2C, size 0x4, Decl: 202
    int fX_wet;           // offset 0x30, size 0x4, Decl: 205
    int panning_Depth;    // offset 0x34, size 0x4, Decl: 208
} FX_WINDStruct;

extern InterfaceId FX_WIND_WeatherId;      // size: 0x8, address: 0x80418870, Decl: 211
extern ClassHandle gFX_WIND_WeatherHandle; // size: 0x8, Decl: 212

// total size: 0x10
// Decl: 214
typedef struct {
    int volume;       // offset 0x0, size 0x4
    int pitch_offset; // offset 0x4, size 0x4
    int width;        // offset 0x8, size 0x4
    int intensity;    // offset 0xC, size 0x4
} FX_WIND_WeatherStruct;

extern InterfaceId FX_TRAFFICId;      // size: 0x8, address: 0x80418878, Decl: 227
extern ClassHandle gFX_TRAFFICHandle; // size: 0x8, Decl: 228

// total size: 0x24
// Decl: 230
typedef struct {
    int id;                    // offset 0x0, size 0x4
    int volume;                // offset 0x4, size 0x4
    int pitch_OFFSET;          // offset 0x8, size 0x4
    int azimuth;               // offset 0xC, size 0x4
    int range;                 // offset 0x10, size 0x4
    int filter_Effects_LoPass; // offset 0x14, size 0x4
    int filter_Effects_HiPass; // offset 0x18, size 0x4
    int filter_Effects_Dry_FX; // offset 0x1C, size 0x4
    int filter_Effects_Wet_FX; // offset 0x20, size 0x4
} FX_TRAFFICStruct;

extern InterfaceId FX_TRUCK_FXId;      // size: 0x8, address: 0x80418880, Decl: 260
extern ClassHandle gFX_TRUCK_FXHandle; // size: 0x8, Decl: 261

// total size: 0x20
// Decl: 263
typedef struct {
    int id;           // offset 0x0, size 0x4
    int volume;       // offset 0x4, size 0x4
    int pitch_OFFSET; // offset 0x8, size 0x4
    int azimuth;      // offset 0xC, size 0x4
    int loPass;       // offset 0x10, size 0x4
    int hiPass;       // offset 0x14, size 0x4
    int fX_Dry;       // offset 0x18, size 0x4
    int fX_Wet;       // offset 0x1C, size 0x4
} FX_TRUCK_FXStruct;

// total size: 0x30
// Decl: 311
class FX_ROADNOISE {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xF) {
            x = 0xF;
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

    void SetType(FXROADNOISETypeType x) {
        if (x < FXROADNOISETYPETYPE_LOOP) {
            x = FXROADNOISETYPETYPE_LOOP;
        } else if (x > FXROADNOISETYPETYPE_TRANSITION_) {
            x = FXROADNOISETYPETYPE_TRANSITION_;
        }
        this->mData.type = x;
    }

    FXROADNOISETypeType GetType() {
        return this->mData.type;
    }

    void SetSecondaryNoise(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.secondaryNoise = x;
    }

    int GetSecondaryNoise() {
        return this->mData.secondaryNoise;
    }

    void SetSpeed(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x410) {
            x = 0x410;
        }
        this->mData.speed = x;
    }

    int GetSpeed() {
        return this->mData.speed;
    }

    void SetFilter_Effects_LoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.filter_Effects_LoPass = x;
    }

    int GetFilter_Effects_LoPass() {
        return this->mData.filter_Effects_LoPass;
    }

    void SetFilter_Effects_HiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.filter_Effects_HiPass = x;
    }

    int GetFilter_Effects_HiPass() {
        return this->mData.filter_Effects_HiPass;
    }

    void SetFilter_Effects_Dry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_Effects_Dry_FX = x;
    }

    int GetFilter_Effects_Dry_FX() {
        return this->mData.filter_Effects_Dry_FX;
    }

    void SetFilter_Effects_Wet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_Effects_Wet_FX = x;
    }

    int GetFilter_Effects_Wet_FX() {
        return this->mData.filter_Effects_Wet_FX;
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

    FX_ROADNOISE(int id, int volume, int pitch, int azimuth, FXROADNOISETypeType type, int secondaryNoise, int speed, int filter_Effects_LoPass,
                 int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch(pitch);
        this->SetAzimuth(azimuth);
        this->SetType(type);
        this->SetSecondaryNoise(secondaryNoise);
        this->SetSpeed(speed);
        this->SetFilter_Effects_LoPass(filter_Effects_LoPass);
        this->SetFilter_Effects_HiPass(filter_Effects_HiPass);
        this->SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        this->SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        Result result = Class::CreateInstance(&gFX_ROADNOISEHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_ROADNOISEHandle.Set(&FX_ROADNOISEId);
            Class::CreateInstance(&gFX_ROADNOISEHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_ROADNOISE() {
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
    FX_ROADNOISEStruct mData;
};

// total size: 0x30
// Decl: 559
class FX_ROADNOISE_TRANS {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xF) {
            x = 0xF;
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

    void SetType(FXROADNOISETRANSTypeType x) {
        if (x < FXROADNOISETRANSTYPETYPE_LOOP) {
            x = FXROADNOISETRANSTYPETYPE_LOOP;
        } else if (x > FXROADNOISETRANSTYPETYPE_TRANSITION_) {
            x = FXROADNOISETRANSTYPETYPE_TRANSITION_;
        }
        this->mData.type = x;
    }

    FXROADNOISETRANSTypeType GetType() {
        return this->mData.type;
    }

    void SetSecondaryNoise(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.secondaryNoise = x;
    }

    int GetSecondaryNoise() {
        return this->mData.secondaryNoise;
    }

    void SetSpeed(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x410) {
            x = 0x410;
        }
        this->mData.speed = x;
    }

    int GetSpeed() {
        return this->mData.speed;
    }

    void SetFilter_Effects_LoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.filter_Effects_LoPass = x;
    }

    int GetFilter_Effects_LoPass() {
        return this->mData.filter_Effects_LoPass;
    }

    void SetFilter_Effects_HiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.filter_Effects_HiPass = x;
    }

    int GetFilter_Effects_HiPass() {
        return this->mData.filter_Effects_HiPass;
    }

    void SetFilter_Effects_Dry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_Effects_Dry_FX = x;
    }

    int GetFilter_Effects_Dry_FX() {
        return this->mData.filter_Effects_Dry_FX;
    }

    void SetFilter_Effects_Wet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_Effects_Wet_FX = x;
    }

    int GetFilter_Effects_Wet_FX() {
        return this->mData.filter_Effects_Wet_FX;
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

    FX_ROADNOISE_TRANS(int id, int volume, int pitch, int azimuth, FXROADNOISETRANSTypeType type, int secondaryNoise, int speed,
                       int filter_Effects_LoPass, int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch(pitch);
        this->SetAzimuth(azimuth);
        this->SetType(type);
        this->SetSecondaryNoise(secondaryNoise);
        this->SetSpeed(speed);
        this->SetFilter_Effects_LoPass(filter_Effects_LoPass);
        this->SetFilter_Effects_HiPass(filter_Effects_HiPass);
        this->SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        this->SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        Result result = Class::CreateInstance(&gFX_ROADNOISE_TRANSHandle, &this->mData, &this->mpClass);

        if (result < RESULT_OK) {
            gFX_ROADNOISE_TRANSHandle.Set(&FX_ROADNOISE_TRANSId);
            Class::CreateInstance(&gFX_ROADNOISE_TRANSHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_ROADNOISE_TRANS() {
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
    FX_ROADNOISE_TRANSStruct mData;
};

// total size: 0x28
// Decl: 807
class ENV_STATIC {
  public:
    void SetID(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x32) {
            x = 0x32;
        }
        this->mData.iD = x;
    }

    int GetID() {
        return this->mData.iD;
    }

    void SetVOLUME(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vOLUME = x;
    }

    int GetVOLUME() {
        return this->mData.vOLUME;
    }

    void SetPITCH(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        this->mData.pITCH = x;
    }

    int GetPITCH() {
        return this->mData.pITCH;
    }

    void SetAZIMUTH(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.aZIMUTH = x;
    }

    int GetAZIMUTH() {
        return this->mData.aZIMUTH;
    }

    void SetTYPE(ENVSTATICTYPEType x) {
        if (x < ENVSTATICTYPETYPE_ENV_COMMON) {
            x = ENVSTATICTYPETYPE_ENV_COMMON;
        } else if (x > ENVSTATICTYPETYPE_ENV_LVLSPECIFIC_) {
            x = ENVSTATICTYPETYPE_ENV_LVLSPECIFIC_;
        }
        this->mData.tYPE = x;
    }

    ENVSTATICTYPEType GetTYPE() {
        return this->mData.tYPE;
    }

    void SetLoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.loPass = x;
    }

    int GetLoPass() {
        return this->mData.loPass;
    }

    void SetHiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.hiPass = x;
    }

    int GetHiPass() {
        return this->mData.hiPass;
    }

    void SetFX_Dry(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.fX_Dry = x;
    }

    int GetFX_Dry() {
        return this->mData.fX_Dry;
    }

    void SetFX_Wet(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.fX_Wet = x;
    }

    int GetFX_Wet() {
        return this->mData.fX_Wet;
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

    ENV_STATIC(int iD, int vOLUME, int pITCH, int aZIMUTH, ENVSTATICTYPEType tYPE, int loPass, int hiPass, int fX_Dry, int fX_Wet) {
        this->SetID(iD);
        this->SetVOLUME(vOLUME);
        this->SetPITCH(pITCH);
        this->SetAZIMUTH(aZIMUTH);
        this->SetTYPE(tYPE);
        this->SetLoPass(loPass);
        this->SetHiPass(hiPass);
        this->SetFX_Dry(fX_Dry);
        this->SetFX_Wet(fX_Wet);

        Result result = Class::CreateInstance(&gENV_STATICHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gENV_STATICHandle.Set(&ENV_STATICId);
            Class::CreateInstance(&gENV_STATICHandle, &this->mData, &this->mpClass);
        }
    }

    ~ENV_STATIC() {
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
    Class *mpClass;         // offset 0x0, size 0x4
    ENV_STATICStruct mData; // offset 0x4, size 0x24
};

// total size: 0x3C
// Decl: 1167
class FX_WIND {
  public:
    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        this->mData.pitch = x;
    }

    int GetPitch() {
        return this->mData.pitch;
    }

    void SetVolume_left(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.volume_left = x;
    }

    int GetVolume_left() {
        return this->mData.volume_left;
    }

    void SetVolume_right(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.volume_right = x;
    }

    int GetVolume_right() {
        return this->mData.volume_right;
    }

    void SetAzimuth_left(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.azimuth_left = x;
    }

    int GetAzimuth_left() {
        return this->mData.azimuth_left;
    }

    void SetAzimuth_right(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.azimuth_right = x;
    }

    int GetAzimuth_right() {
        return this->mData.azimuth_right;
    }

    void SetIntensity(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x410) {
            x = 0x410;
        }
        this->mData.intensity = x;
    }

    int GetIntensity() {
        return this->mData.intensity;
    }

    void SetCamera(int x) {
        this->mData.camera = x;
    }

    int GetCamera() {
        return this->mData.camera;
    }

    void SetOverride_Trigger(int x) {
        this->mData.override_Trigger = x;
    }

    int GetOverride_Trigger() {
        return this->mData.override_Trigger;
    }

    void SetRumble_Volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.rumble_Volume = x;
    }

    int GetRumble_Volume() {
        return this->mData.rumble_Volume;
    }

    void SetLowPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.lowPass = x;
    }

    int GetLowPass() {
        return this->mData.lowPass;
    }

    void SetHiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.hiPass = x;
    }

    int GetHiPass() {
        return this->mData.hiPass;
    }

    void SetFX_Dry(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.fX_Dry = x;
    }

    int GetFX_Dry() {
        return this->mData.fX_Dry;
    }

    void SetFX_wet(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.fX_wet = x;
    }

    int GetFX_wet() {
        return this->mData.fX_wet;
    }

    void SetPanning_Depth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.panning_Depth = x;
    }

    int GetPanning_Depth() {
        return this->mData.panning_Depth;
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

    FX_WIND(int pitch, int volume_left, int volume_right, int azimuth_left, int azimuth_right, int intensity, int camera, int override_Trigger,
            int rumble_Volume, int lowPass, int hiPass, int fX_Dry, int fX_wet, int panning_Depth) {
        this->SetPitch(pitch);
        this->SetVolume_left(volume_left);
        this->SetVolume_right(volume_right);
        this->SetAzimuth_left(azimuth_left);
        this->SetAzimuth_right(azimuth_right);
        this->SetIntensity(intensity);
        this->SetCamera(camera);
        this->SetOverride_Trigger(override_Trigger);
        this->SetRumble_Volume(rumble_Volume);
        this->SetLowPass(lowPass);
        this->SetHiPass(hiPass);
        this->SetFX_Dry(fX_Dry);
        this->SetFX_wet(fX_wet);
        this->SetPanning_Depth(panning_Depth);

        Result result = Class::CreateInstance(&gFX_WINDHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_WINDHandle.Set(&FX_WINDId);
            Class::CreateInstance(&gFX_WINDHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_WIND() {
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
    FX_WINDStruct mData;
};

// toal size: 0x14
// Decl: 1466
class FX_WIND_Weather {
  public:
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

    void SetPitch_offset(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        this->mData.pitch_offset = x;
    }

    int GetPitch_offset() {
        return this->mData.pitch_offset;
    }

    void SetWidth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.width = x;
    }

    int GetWidth() {
        return this->mData.width;
    }

    void SetIntensity(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x410) {
            x = 0x410;
        }
        this->mData.intensity = x;
    }

    int GetIntensity() {
        return this->mData.intensity;
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

    FX_WIND_Weather(int volume, int pitch_offset, int width, int intensity) {
        this->SetVolume(volume);
        this->SetPitch_offset(pitch_offset);
        this->SetWidth(width);
        this->SetIntensity(intensity);

        Result result = Class::CreateInstance(&gFX_WIND_WeatherHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_WIND_WeatherHandle.Set(&FX_WIND_WeatherId);
            Class::CreateInstance(&gFX_WIND_WeatherHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_WIND_Weather() {
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
    FX_WIND_WeatherStruct mData;
};

// total size: 0x28
// Decl: 1595
class FX_TRAFFIC {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xF) {
            x = 0xF;
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

    void SetPitch_OFFSET(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        this->mData.pitch_OFFSET = x;
    }

    int GetPitch_OFFSET() {
        return this->mData.pitch_OFFSET;
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

    void SetRange(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.range = x;
    }

    int GetRange() {
        return this->mData.range;
    }

    void SetFilter_Effects_LoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.filter_Effects_LoPass = x;
    }

    int GetFilter_Effects_LoPass() {
        return this->mData.filter_Effects_LoPass;
    }

    void SetFilter_Effects_HiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.filter_Effects_HiPass = x;
    }

    int GetFilter_Effects_HiPass() {
        return this->mData.filter_Effects_HiPass;
    }

    void SetFilter_Effects_Dry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_Effects_Dry_FX = x;
    }

    int GetFilter_Effects_Dry_FX() {
        return this->mData.filter_Effects_Dry_FX;
    }

    void SetFilter_Effects_Wet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_Effects_Wet_FX = x;
    }

    int GetFilter_Effects_Wet_FX() {
        return this->mData.filter_Effects_Wet_FX;
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

    FX_TRAFFIC(int id, int volume, int pitch_OFFSET, int azimuth, int range, int filter_Effects_LoPass, int filter_Effects_HiPass,
               int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch_OFFSET(pitch_OFFSET);
        this->SetAzimuth(azimuth);
        this->SetRange(range);
        this->SetFilter_Effects_LoPass(filter_Effects_LoPass);
        this->SetFilter_Effects_HiPass(filter_Effects_HiPass);
        this->SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        this->SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        Result result = Class::CreateInstance(&gFX_TRAFFICHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_TRAFFICHandle.Set(&FX_TRAFFICId);
            Class::CreateInstance(&gFX_TRAFFICHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_TRAFFIC() {
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
    FX_TRAFFICStruct mData;
};

// total size: 0x24
// Decl: 1809
class FX_TRUCK_FX {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xF) {
            x = 0xF;
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

    void SetPitch_OFFSET(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        this->mData.pitch_OFFSET = x;
    }

    int GetPitch_OFFSET() {
        return this->mData.pitch_OFFSET;
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

    void SetLoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.loPass = x;
    }

    int GetLoPass() {
        return this->mData.loPass;
    }

    void SetHiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.hiPass = x;
    }

    int GetHiPass() {
        return this->mData.hiPass;
    }

    void SetFX_Dry(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.fX_Dry = x;
    }

    int GetFX_Dry() {
        return this->mData.fX_Dry;
    }

    void SetFX_Wet(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.fX_Wet = x;
    }

    int GetFX_Wet() {
        return this->mData.fX_Wet;
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

    FX_TRUCK_FX(int id, int volume, int pitch_OFFSET, int azimuth, int loPass, int hiPass, int fX_Dry, int fX_Wet) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch_OFFSET(pitch_OFFSET);
        this->SetAzimuth(azimuth);
        this->SetLoPass(loPass);
        this->SetHiPass(hiPass);
        this->SetFX_Dry(fX_Dry);
        this->SetFX_Wet(fX_Wet);

        Result result = Class::CreateInstance(&gFX_TRUCK_FXHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_TRUCK_FXHandle.Set(&FX_TRUCK_FXId);
            Class::CreateInstance(&gFX_TRUCK_FXHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_TRUCK_FX() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass != nullptr) {
            this->mpClass->SetMemberData(&this->mData);
        }
    }

  private:
    Csis::Class *mpClass;    // offset 0x0, size 0x4
    FX_TRUCK_FXStruct mData; // offset 0x4, size 0x20
};

}; // namespace Csis

#endif
