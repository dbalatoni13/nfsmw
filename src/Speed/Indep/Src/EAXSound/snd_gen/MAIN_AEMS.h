//
//
//
#ifndef MAIN_AEMS_H
#define MAIN_AEMS_H

#include "csis/csis.h"

namespace Csis {

// Decl: 16
enum FXSHIFTING01TypeType {
    FXSHIFTING01TYPETYPE_SHIFT = 0,
    FXSHIFTING01TYPETYPE_ATTACK_ = 1,
};

extern InterfaceId FX_NITROUSId;      // size: 0x8, Decl: 26
extern ClassHandle gFX_NITROUSHandle; // size: 0x8, Decl: 27

// total size: 0x24
// Decl: 29
struct FX_NITROUSStruct {
    int nIT_ID;                // offset 0x0, size 0x4
    int nIT_volume;            // offset 0x4, size 0x4
    int nIT_azimuth;           // offset 0x8, size 0x4
    int nIT_STOP;              // offset 0xC, size 0x4
    int pitch;                 // offset 0x10, size 0x4
    int filter_Effects_LoPass; // offset 0x14, size 0x4
    int filter_Effects_HiPass; // offset 0x18, size 0x4
    int filter_Effects_Dry_FX; // offset 0x1C, size 0x4
    int filter_Effects_Wet_FX; // offset 0x20, size 0x4
};

extern InterfaceId FX_PURGEId;      // size: 0x8, Decl: 59
extern ClassHandle gFX_PURGEHandle; // size: 0x8, Decl: 60

// total size: 0x20
// Decl: 62
struct FX_PURGEStruct {
    int pURGE_volume; // offset 0x0, size 0x4, Decl: 65

    int pURGE_azimuth; // offset 0x4, size 0x4, Decl: 68

    int pURGE_STOP; // offset 0x8, size 0x4, Decl: 71

    int pURGE_pitch; // offset 0xC, size 0x4, Decl: 74

    int filter_Effects_LoPass; // offset 0x10, size 0x4, Decl: 77

    int filter_Effects_HiPass; // offset 0x14, size 0x4, Decl: 80

    int filter_Effects_Dry_FX; // offset 0x18, size 0x4, Decl: 83

    int filter_Effects_Wet_FX; // offset 0x1C, size 0x4, Decl: 86
};

extern InterfaceId FX_SHIFTING_01Id;      // size: 0x8, Decl: 89
extern ClassHandle gFX_SHIFTING_01Handle; // size: 0x8, Decl: 90

// total size: 0x18
// Decl: 92
struct FX_SHIFTING_01Struct {
    int id;                    // offset 0x0, size 0x4
    int volume;                // offset 0x4, size 0x4
    int pitch;                 // offset 0x8, size 0x4
    int azimuth;               // offset 0xC, size 0x4
    FXSHIFTING01TypeType type; // offset 0x10, size 0x4
    int cAM;                   // offset 0x14, size 0x4
};

extern InterfaceId FX_HydraulicId;      // size: 0x8, Decl: 129
extern ClassHandle gFX_HydraulicHandle; // size: 0x8, Decl: 130

// total size: 0x18
// Decl: 132
struct FX_HydraulicStruct {
    int hYD_ID;           // offset 0x0, size 0x4
    int hYD_PITCH_OFFSET; // offset 0x4, size 0x4
    int hYD_STOP;         // offset 0x8, size 0x4
    int hYD_Up_Down;      // offset 0xC, size 0x4
    int hYD_volume;       // offset 0x10, size 0x4
    int hYD_azimuth;      // offset 0x14, size 0x4
};

extern InterfaceId FX_HelicopterId;      // size: 0x8, Decl: 153
extern ClassHandle gFX_HelicopterHandle; // size: 0x8, Decl: 154

// total size: 0x30
// Decl: 156
struct FX_HelicopterStruct {
    int hELI_ID; // offset 0x0, size 0x4, Decl: 159

    int hELI_Pitch_Offset; // offset 0x4, size 0x4, Decl: 162

    int hELI_Stop; // offset 0x8, size 0x4, Decl: 164

    int hELI_Volume; // offset 0xC, size 0x4, Decl: 167

    int hELI_Azmuth; // offset 0x10, size 0x4, Decl: 170

    int hELI_Speed; // offset 0x14, size 0x4, Decl: 173

    int hELI_Distance; // offset 0x18, size 0x4, Decl: 176

    int hELI_LowPass; // offset 0x1C, size 0x4, Decl: 179

    int hELI_HiPass; // offset 0x20, size 0x4, Decl: 182

    int hELI_FX_Dry; // offset 0x24, size 0x4, Decl: 185

    int hELI_FX_Wet; // offset 0x28, size 0x4, Decl: 187

    int hELI_Rotation; // offset 0x2C, size 0x4, Decl: 190
};

extern InterfaceId FX_SKIDId;      // size: 0x8
extern ClassHandle gFX_SKIDHandle; // size: 0x8

// total size: 0x58
struct FX_SKIDStruct {
    int vOL_Fwd;               // offset 0x0, size 0x4
    int vOL_Side;              // offset 0x4, size 0x4
    int vOL_Back;              // offset 0x8, size 0x4
    int azimuth;               // offset 0xC, size 0x4
    int tYPE;                  // offset 0x10, size 0x4
    int pITCH_OFFSET;          // offset 0x14, size 0x4
    int hOP;                   // offset 0x18, size 0x4
    int sPEED;                 // offset 0x1C, size 0x4
    int oPPOS_Side;            // offset 0x20, size 0x4
    int uNDERSTEER;            // offset 0x24, size 0x4
    int oVERSTEER;             // offset 0x28, size 0x4
    int surface;               // offset 0x2C, size 0x4
    int front_FB;              // offset 0x30, size 0x4
    int front_LR;              // offset 0x34, size 0x4
    int front_Load;            // offset 0x38, size 0x4
    int back_FB;               // offset 0x3C, size 0x4
    int back_LR;               // offset 0x40, size 0x4
    int back_Load;             // offset 0x44, size 0x4
    int filter_Effects_LoPass; // offset 0x48, size 0x4
    int filter_Effects_HiPass; // offset 0x4C, size 0x4
    int filter_Effects_Dry_FX; // offset 0x50, size 0x4
    int filter_Effects_Wet_FX; // offset 0x54, size 0x4
};

extern InterfaceId FX_WeatherId;      // size: 0x8, Decl: 211
extern ClassHandle gFX_WeatherHandle; // size: 0x8, Decl: 212

// total size: 0x28
// Decl: 214
struct FX_WeatherStruct {
    int hood_Rain;               // offset 0x0, size 0x4, Decl: 217
    int hood_Rain_Vol_Substract; // offset 0x4, size 0x4, Decl: 219
    int volume;                  // offset 0x8, size 0x4, Decl: 221
    int width;                   // offset 0xC, size 0x4, Decl: 224
    int pitch_Offset;            // offset 0x10, size 0x4, Decl: 227
    int rain_on_off;             // offset 0x14, size 0x4, Decl: 230
    int filter_Effects_LoPass;   // offset 0x18, size 0x4, Decl: 233
    int filter_Effects_HiPass;   // offset 0x1C, size 0x4, Decl: 236
    int filter_Effects_Dry_FX;   // offset 0x20, size 0x4, Decl: 239
    int filter_Effects_Wet_FX;   // offset 0x24, size 0x4, Decl: 242
};

extern InterfaceId FX_CameraId;      // size: 0x8, Decl: 245
extern ClassHandle gFX_CameraHandle; // size: 0x8, Decl: 246

// total size: 0x20
// Decl: 248
struct FX_CameraStruct {
    int iD;               // offset 0x0, size 0x4
    int volume;           // offset 0x4, size 0x4
    int width;            // offset 0x8, size 0x4
    int pitch_Offset;     // offset 0xC, size 0x4
    int cam_beep;         // offset 0x10, size 0x4
    int cam_beep_Volume;  // offset 0x14, size 0x4
    int cam_whine;        // offset 0x18, size 0x4
    int cam_whine_Volume; // offset 0x1C, size 0x4
};

extern InterfaceId FX_UVESId;      // size: 0x8, Decl: 271
extern ClassHandle gFX_UVESHandle; // size: 0x10, Decl: 272

// total size: 0x18
// Decl: 274
struct FX_UVESStruct {
    int iD;           // offset 0x0, size 0x4
    int volume;       // offset 0x4, size 0x4
    int width;        // offset 0x8, size 0x4
    int pitch_Offset; // offset 0xC, size 0x4
    int intensity;    // offset 0x10, size 0x4
    int stop;         // offset 0x14, size 0x4
};

extern InterfaceId FX_RadarId;      // size: 0x8, Decl: 291
extern ClassHandle gFX_RadarHandle; // size: 0x8, Decl: 292

// total size: 0x14
// Decl: 294
struct FX_RadarStruct {
    int iD;           // offset 0x0, size 0x4
    int volume;       // offset 0x4, size 0x4
    int pitch_Offset; // offset 0x8, size 0x4
    int intensity;    // offset 0xC, size 0x4
    int stop;         // offset 0x10, size 0x4
};

extern InterfaceId FX_ScrapeId;      // size: 0x8, Decl: 311
extern ClassHandle gFX_ScrapeHandle; // size: 0x8, Decl: 312

// total size: 0x24
// Decl: 314
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

// total size: 0x28
// Decl: 387
class FX_NITROUS {
  public:
    void SetNIT_ID(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 2) {
            x = 2;
        }
        this->mData.nIT_ID = x;
    }

    int GetNIT_ID() {
        return this->mData.nIT_ID;
    }

    void SetNIT_volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.nIT_volume = x;
    }

    int GetNIT_volume() {
        return this->mData.nIT_volume;
    }

    void SetNIT_azimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.nIT_azimuth = x;
    }

    int GetNIT_azimuth() {
        return this->mData.nIT_azimuth;
    }

    void SetNIT_STOP(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1) {
            x = 1;
        }
        this->mData.nIT_STOP = x;
    }

    int GetNIT_STOP() {
        return this->mData.nIT_STOP;
    }

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

    static void operator delete(void *ptr) {
        System::Free(ptr);
    }

    FX_NITROUS(int nIT_ID, int nIT_volume, int nIT_azimuth, int nIT_STOP, int pitch, int filter_Effects_LoPass, int filter_Effects_HiPass,
               int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        this->SetNIT_ID(nIT_ID);
        this->SetNIT_volume(nIT_volume);
        this->SetNIT_azimuth(nIT_azimuth);
        this->SetNIT_STOP(nIT_STOP);
        this->SetPitch(pitch);
        this->SetFilter_Effects_LoPass(filter_Effects_LoPass);
        this->SetFilter_Effects_HiPass(filter_Effects_HiPass);
        this->SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        this->SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        Result result = Class::CreateInstance(&gFX_NITROUSHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_NITROUSHandle.Set(&FX_NITROUSId);
            Class::CreateInstance(&gFX_NITROUSHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_NITROUS() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (this->mpClass != nullptr) {
            this->mpClass->SetMemberData(&mData);
        }
    }

  private:
    Class *mpClass;         // offset 0x0, size 0x4
    FX_NITROUSStruct mData; // offset 0x4, size 0x24
};

// total size: 0x24
// Decl: 601
class FX_PURGE {
  public:
    void SetPURGE_volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.pURGE_volume = x;
    }

    int GetPURGE_volume() {
        return this->mData.pURGE_volume;
    }

    void SetPURGE_azimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.pURGE_azimuth = x;
    }

    int GetPURGE_azimuth() {
        return this->mData.pURGE_azimuth;
    }

    void SetPURGE_STOP(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1) {
            x = 1;
        }
        this->mData.pURGE_STOP = x;
    }

    int GetPURGE_STOP() {
        return this->mData.pURGE_STOP;
    }

    void SetPURGE_pitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        this->mData.pURGE_pitch = x;
    }

    int GetPURGE_pitch() {
        return this->mData.pURGE_pitch;
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

    FX_PURGE(int pURGE_volume, int pURGE_azimuth, int pURGE_STOP, int pURGE_pitch, int filter_Effects_LoPass, int filter_Effects_HiPass,
             int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        this->SetPURGE_volume(pURGE_volume);
        this->SetPURGE_azimuth(pURGE_azimuth);
        this->SetPURGE_STOP(pURGE_STOP);
        this->SetPURGE_pitch(pURGE_pitch);
        this->SetFilter_Effects_LoPass(filter_Effects_LoPass);
        this->SetFilter_Effects_HiPass(filter_Effects_HiPass);
        this->SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        this->SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        Result result = Class::CreateInstance(&gFX_PURGEHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_PURGEHandle.Set(&FX_PURGEId);
            Class::CreateInstance(&gFX_PURGEHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_PURGE() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (this->mpClass != nullptr) {
            this->mpClass->SetMemberData(&mData);
        }
    }

  private:
    Class *mpClass;       // offset 0x0, size 0x4
    FX_PURGEStruct mData; // offset 0x4, size 0x20
};

// Decl: 798
class FX_SHIFTING_01 {
  public:
    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 2) {
            x = 2;
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

    void SetType(FXSHIFTING01TypeType x) {
        this->mData.type = x;
    }

    FXSHIFTING01TypeType GetType() {
        return this->mData.type;
    }

    void SetCAM(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1) {
            x = 1;
        }
        this->mData.cAM = x;
    }

    int GetCAM() {
        return this->mData.cAM;
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

    FX_SHIFTING_01(int id, int volume, int pitch, int azimuth, FXSHIFTING01TypeType type, int cAM) {
        this->SetId(id);
        this->SetVolume(volume);
        this->SetPitch(pitch);
        this->SetAzimuth(azimuth);
        this->SetType(type);
        this->SetCAM(cAM);

        Result result = Class::CreateInstance(&gFX_SHIFTING_01Handle, &this->mData, &mpClass);
        if (result < RESULT_OK) {
            gFX_SHIFTING_01Handle.Set(&FX_SHIFTING_01Id);
            Class::CreateInstance(&gFX_SHIFTING_01Handle, &this->mData, &mpClass);
        }
    }

    ~FX_SHIFTING_01() {
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
    Class *mpClass;             // offset 0x0, size 0x4
    FX_SHIFTING_01Struct mData; // offset 0x4, size 0x18
};

// total size: 0x1C
// Decl: 1158
class FX_Hydraulic {
  public:
    void operator delete(void *ptr) {
        System::Free(ptr);
    }

    ~FX_Hydraulic() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

  private:
    Class *mpClass;           // offset 0x0, size 0x4
    FX_HydraulicStruct mData; // offset 0x4, size 0x18
};

// Decl: 1321
class FX_Helicopter {
  public:
    void SetHELI_ID(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.hELI_ID = x;
    }

    int GetHELI_ID() {
        return this->mData.hELI_ID;
    }

    void SetHELI_Pitch_Offset(int x) {
        if (x < -0x2000) {
            x = -0x2000;
        } else if (x > 0x2000) {
            x = 0x2000;
        }
        this->mData.hELI_Pitch_Offset = x;
    }

    int GetHELI_Pitch_Offset() {
        return this->mData.hELI_Pitch_Offset;
    }

    void SetHELI_Stop(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1) {
            x = 1;
        }
        this->mData.hELI_Stop = x;
    }

    int GetHELI_Stop() {
        return this->mData.hELI_Stop;
    }

    void SetHELI_Volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.hELI_Volume = x;
    }

    int GetHELI_Volume() {
        return this->mData.hELI_Volume;
    }

    void SetHELI_Azmuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.hELI_Azmuth = x;
    }

    int GetHELI_Azmuth() {
        return this->mData.hELI_Azmuth;
    }

    void SetHELI_Speed(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        this->mData.hELI_Speed = x;
    }

    int GetHELI_Speed() {
        return this->mData.hELI_Speed;
    }

    void SetHELI_Distance(int x) {
        if (x < -100) {
            x = -100;
        } else if (x > 100) {
            x = 100;
        }
        this->mData.hELI_Distance = x;
    }

    int GetHELI_Distance() {
        return this->mData.hELI_Distance;
    }

    void SetHELI_LowPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.hELI_LowPass = x;
    }

    int GetHELI_LowPass() {
        return this->mData.hELI_LowPass;
    }

    void SetHELI_HiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        this->mData.hELI_HiPass = x;
    }

    int GetHELI_HiPass() {
        return this->mData.hELI_HiPass;
    }

    void SetHELI_FX_Dry(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.hELI_FX_Dry = x;
    }

    int GetHELI_FX_Dry() {
        return this->mData.hELI_FX_Dry;
    }

    void SetHELI_FX_Wet(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.hELI_FX_Wet = x;
    }

    int GetHELI_FX_Wet() {
        return this->mData.hELI_FX_Wet;
    }

    void SetHELI_Rotation(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.hELI_Rotation = x;
    }

    int GetHELI_Rotation() {
        return this->mData.hELI_Rotation;
    }

    int GetRefCount() {
        return this->mData.hELI_ID;
    }

    void *operator new(size_t size) {
        return System::Alloc(size);
    }

    void operator delete(void *ptr) {
        System::Free(ptr);
    }

    FX_Helicopter(int hELI_ID, int hELI_Pitch_Offset, int hELI_Stop, int hELI_Volume, int hELI_Azmuth, int hELI_Speed, int hELI_Distance,
                  int hELI_LowPass, int hELI_HiPass, int hELI_FX_Dry, int hELI_FX_Wet, int hELI_Rotation)
        : mpClass(nullptr) {

        this->SetHELI_ID(hELI_ID);
        this->SetHELI_Pitch_Offset(hELI_Pitch_Offset);
        this->SetHELI_Stop(hELI_Stop);
        this->SetHELI_Volume(hELI_Volume);
        this->SetHELI_Azmuth(hELI_Azmuth);
        this->SetHELI_Speed(hELI_Speed);
        this->SetHELI_Distance(hELI_Distance);
        this->SetHELI_LowPass(hELI_LowPass);
        this->SetHELI_HiPass(hELI_HiPass);
        this->SetHELI_FX_Dry(hELI_FX_Dry);
        this->SetHELI_FX_Wet(hELI_FX_Wet);
        this->SetHELI_Rotation(hELI_Rotation);

        Result result = Class::CreateInstance(&gFX_HelicopterHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_HelicopterHandle.Set(&FX_HelicopterId);
            Class::CreateInstance(&gFX_HelicopterHandle, &mData, &mpClass);
        }
    }

    ~FX_Helicopter() {
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
    Class *mpClass;            // offset 0x0, size 0x4
    FX_HelicopterStruct mData; // offset 0x4, size 0x30
};

class FX_SKID {
  public:
    void SetVOL_Fwd(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vOL_Fwd = x;
    }

    void SetVOL_Side(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vOL_Side = x;
    }

    void SetVOL_Back(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.vOL_Back = x;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        this->mData.azimuth = x;
    }

    void SetTYPE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 3) {
            x = 3;
        }

        this->mData.tYPE = x;
    }

    void SetPITCH_OFFSET(int x) {
        if (x < -0x4000) {
            x = -0x4000;
        } else if (x > 0x4000) {
            x = 0x4000;
        }
        this->mData.pITCH_OFFSET = x;
    }

    void SetHOP(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }

        this->mData.hOP = x;
    }

    void SetSPEED(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1000) {
            x = 1000;
        }
        this->mData.sPEED = x;
    }

    void SetOPPOS_Side(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }

        this->mData.oPPOS_Side = x;
    }

    void SetUNDERSTEER(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.uNDERSTEER = x;
    }

    void SetOVERSTEER(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.oVERSTEER = x;
    }

    void SetSurface(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 5) {
            x = 5;
        }
        this->mData.surface = x;
    }

    void SetFront_FB(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.front_FB = x;
    }

    int GetFront_FB() {
        return this->mData.front_FB;
    }

    void SetFront_LR(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.front_LR = x;
    }

    int GetFront_LR() {
        return this->mData.front_LR;
    }

    void SetFront_Load(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.front_Load = x;
    }

    void SetBack_FB(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.back_FB = x;
    }

    int GetBack_FB() {
        return this->mData.back_FB;
    }

    void SetBack_LR(int x) {
        if (x < -0x3FF) {
            x = -0x3FF;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.back_LR = x;
    }

    int GetBack_LR() {
        return this->mData.back_LR;
    }

    void SetBack_Load(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x3FF) {
            x = 0x3FF;
        }
        this->mData.back_Load = x;
    }

    void SetFilter_Effects_LoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }

        this->mData.filter_Effects_LoPass = x;
    }
    void SetFilter_Effects_HiPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }

        this->mData.filter_Effects_HiPass = x;
    }

    void SetFilter_Effects_Dry_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_Effects_Dry_FX = x;
    }

    void SetFilter_Effects_Wet_FX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.filter_Effects_Wet_FX = x;
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

    FX_SKID(int vOL_Fwd, int vOL_Side, int vOL_Back, int azimuth, int tYPE, int pITCH_OFFSET, int hOP, int sPEED, int oPPOS_Side, int uNDERSTEER,
            int oVERSTEER, int surface, int front_FB, int front_LR, int front_Load, int back_FB, int back_LR, int back_Load,
            int filter_Effects_LoPass, int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        this->SetVOL_Fwd(vOL_Fwd);
        this->SetVOL_Side(vOL_Side);
        this->SetVOL_Back(vOL_Back);
        this->SetAzimuth(azimuth);
        this->SetTYPE(tYPE);
        this->SetPITCH_OFFSET(pITCH_OFFSET);
        this->SetHOP(hOP);
        this->SetSPEED(sPEED);
        this->SetOPPOS_Side(oPPOS_Side);
        this->SetUNDERSTEER(uNDERSTEER);
        this->SetOVERSTEER(oVERSTEER);
        this->SetSurface(surface);
        this->SetFront_FB(front_FB);
        this->SetFront_LR(front_LR);
        this->SetFront_Load(front_Load);
        this->SetBack_FB(back_FB);
        this->SetBack_LR(back_LR);
        this->SetBack_Load(back_Load);
        this->SetFilter_Effects_LoPass(filter_Effects_LoPass);
        this->SetFilter_Effects_HiPass(filter_Effects_HiPass);
        this->SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        this->SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        Result result = Class::CreateInstance(&gFX_SKIDHandle, &this->mData, &this->mpClass);
        if (result < 0) {
            gFX_SKIDHandle.Set(&FX_SKIDId);
            Class::CreateInstance(&gFX_SKIDHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_SKID() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (this->mpClass != nullptr) {
            this->mpClass->SetMemberData(&mData);
        }
    }

  private:
    Class *mpClass;      // offset 0x0, size 0x4
    FX_SKIDStruct mData; // offset 0x4, size 0x58
};

// Decl: 1715
class FX_Weather {
  public:
    void SetHood_Rain(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 3) {
            x = 3;
        }
        this->mData.hood_Rain = x;
    }

    int GetHood_Rain() {
        return this->mData.hood_Rain;
    }

    void SetHood_Rain_Vol_Substract(int x) {
        this->mData.hood_Rain_Vol_Substract = x;
    }

    int GetHood_Rain_Vol_Substract() {
        return this->mData.hood_Rain_Vol_Substract;
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

    void SetWidth(int x) {
        this->mData.width = x;
    }

    int GetWidth() {
        return this->mData.width;
    }

    void SetPitch_Offset(int x) {
        this->mData.pitch_Offset = x;
    }

    int GetPitch_Offset() {
        return this->mData.pitch_Offset;
    }

    void SetRain_on_off(int x) {
        this->mData.rain_on_off = x;
    }

    int GetRain_on_off() {
        return this->mData.rain_on_off;
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
            mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    void *operator new(size_t size) {
        return System::Alloc(size);
    }

    void operator delete(void *ptr) {
        System::Free(ptr);
    }

    FX_Weather(int hood_Rain, int hood_Rain_Vol_Substract, int volume, int width, int pitch_Offset, int rain_on_off, int filter_Effects_LoPass,
               int filter_Effects_HiPass, int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        this->SetHood_Rain(hood_Rain);
        this->SetHood_Rain_Vol_Substract(hood_Rain_Vol_Substract);
        this->SetVolume(volume);
        this->SetWidth(width);
        this->SetPitch_Offset(pitch_Offset);
        this->SetRain_on_off(rain_on_off);
        this->SetFilter_Effects_LoPass(filter_Effects_LoPass);
        this->SetFilter_Effects_HiPass(filter_Effects_HiPass);
        this->SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        this->SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        Result result = Class::CreateInstance(&gFX_WeatherHandle, &this->mData, &this->mpClass);
        if (result < 0) {
            gFX_WeatherHandle.Set(&FX_WeatherId);
            Class::CreateInstance(&gFX_WeatherHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_Weather() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (this->mpClass != nullptr) {
            this->mpClass->SetMemberData(&mData);
        }
    }

  private:
    Class *mpClass;         // offset 0x0, size 0x4
    FX_WeatherStruct mData; // offset 0x4, size 0x28
};

// Decl: 1946
class FX_Camera {
  public:
    void SetID(int x) {
        this->mData.iD = x;
    }

    int GetID() {
        return this->mData.iD;
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

    void SetWidth(int x) {
        this->mData.width = x;
    }

    int GetWidth() {
        return this->mData.width;
    }

    void SetPitch_Offset(int x) {
        this->mData.pitch_Offset = x;
    }

    int GetPitch_Offset() {
        return this->mData.pitch_Offset;
    }

    void SetCam_beep(int x) {
        this->mData.cam_beep = x;
    }

    int GetCam_beep() {
        return this->mData.cam_beep;
    }

    void SetCam_beep_Volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.cam_beep_Volume = x;
    }

    int GetCam_beep_Volume() {
        return this->mData.cam_beep_Volume;
    }

    void SetCam_whine(int x) {
        this->mData.cam_whine = x;
    }

    int GetCam_whine() {
        return this->mData.cam_whine;
    }

    void SetCam_whine_Volume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        this->mData.cam_whine_Volume = x;
    }

    int GetCam_whine_Volume() {
        return this->mData.cam_whine_Volume;
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

    FX_Camera(int iD, int volume, int width, int pitch_Offset, int cam_beep, int cam_beep_Volume, int cam_whine, int cam_whine_Volume) {
        this->SetID(iD);
        this->SetVolume(volume);
        this->SetWidth(width);
        this->SetPitch_Offset(pitch_Offset);
        this->SetCam_beep(cam_beep);
        this->SetCam_beep_Volume(cam_beep_Volume);
        this->SetCam_whine(cam_whine);
        this->SetCam_whine_Volume(cam_whine_Volume);

        Result result = Class::CreateInstance(&gFX_CameraHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_CameraHandle.Set(&FX_CameraId);
            Class::CreateInstance(&gFX_CameraHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_Camera() {
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
    Class *mpClass;        // offset 0x0, size 0x4
    FX_CameraStruct mData; // offset 0x4, size 0x20
};

// Decl: 2143
class FX_UVES {
  public:
    void SetID(int x) {
        this->mData.iD = x;
    }

    int GetID() {
        return this->mData.iD;
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

    void SetWidth(int x) {
        this->mData.width = x;
    }

    int GetWidth() {
        return this->mData.width;
    }

    void SetPitch_Offset(int x) {
        this->mData.pitch_Offset = x;
    }

    int GetPitch_Offset() {
        return this->mData.pitch_Offset;
    }

    void SetIntensity(int x) {
        this->mData.intensity = x;
    }

    int GetIntensity() {
        return this->mData.intensity;
    }

    void SetStop(int x) {
        this->mData.stop = x;
    }

    int GetStop() {
        return this->mData.stop;
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

    FX_UVES(int iD, int volume, int width, int pitch_Offset, int intensity, int stop) {
        this->SetID(iD);
        this->SetVolume(volume);
        this->SetWidth(width);
        this->SetPitch_Offset(pitch_Offset);
        this->SetIntensity(intensity);
        this->SetStop(stop);

        Result result = Class::CreateInstance(&gFX_UVESHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_UVESHandle.Set(&FX_UVESId);
            Class::CreateInstance(&gFX_UVESHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_UVES() {
        if (this->mpClass != nullptr) {
            this->mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (this->mpClass != nullptr) {
            this->mpClass->SetMemberData(&mData);
        }
    }

  private:
    Class *mpClass;      // offset 0x0, size 0x4
    FX_UVESStruct mData; // offset 0x4, size 0x18
};

// Decl: 2306
class FX_Radar {
  public:
    void SetID(int x) {
        this->mData.iD = x;
    }

    int GetID() {
        return this->mData.iD;
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

    void SetPitch_Offset(int x) {
        this->mData.pitch_Offset = x;
    }

    int GetPitch_Offset() {
        return this->mData.pitch_Offset;
    }

    void SetIntensity(int x) {
        this->mData.intensity = x;
    }

    int GetIntensity() {
        return this->mData.intensity;
    }

    void SetStop(int x) {
        this->mData.stop = x;
    }

    int GetStop() {
        return this->mData.stop;
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

    FX_Radar(int iD, int volume, int pitch_Offset, int intensity, int stop) {
        this->SetID(iD);
        this->SetVolume(volume);
        this->SetStop(stop);
        this->SetPitch_Offset(pitch_Offset);
        this->SetIntensity(intensity);

        Result result = Class::CreateInstance(&gFX_RadarHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_RadarHandle.Set(&FX_RadarId);
            Class::CreateInstance(&gFX_RadarHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_Radar() {
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
    FX_RadarStruct mData;
};

// Decl: 2452
class FX_Scrape {
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

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 9000) {
            x = 9000;
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

    void SetTerrain_type(int x) {
        this->mData.terrain_type = x;
    }

    int GetTerrain_type() {
        return this->mData.terrain_type;
    }

    void SetImpulse_magnitude(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7F) {
            x = 0x7F;
        }
        this->mData.impulse_magnitude = x;
    }

    int GetImpulse_magnitude() {
        return this->mData.impulse_magnitude;
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

    FX_Scrape(int volume, int pitch, int azimuth, int terrain_type, int impulse_magnitude, int filter_Effects_LoPass, int filter_Effects_HiPass,
              int filter_Effects_Dry_FX, int filter_Effects_Wet_FX) {
        this->SetVolume(volume);
        this->SetPitch(pitch);
        this->SetAzimuth(azimuth);
        this->SetTerrain_type(terrain_type);
        this->SetImpulse_magnitude(impulse_magnitude);
        this->SetFilter_Effects_LoPass(filter_Effects_LoPass);
        this->SetFilter_Effects_HiPass(filter_Effects_HiPass);
        this->SetFilter_Effects_Dry_FX(filter_Effects_Dry_FX);
        this->SetFilter_Effects_Wet_FX(filter_Effects_Wet_FX);

        Result result = Class::CreateInstance(&gFX_ScrapeHandle, &this->mData, &this->mpClass);
        if (result < RESULT_OK) {
            gFX_ScrapeHandle.Set(&FX_ScrapeId);
            Class::CreateInstance(&gFX_ScrapeHandle, &this->mData, &this->mpClass);
        }
    }

    ~FX_Scrape() {
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
    Class *mpClass;        // offset 0x0, size 0x4
    FX_ScrapeStruct mData; // offset 0x4, size 0x24
};

}; // namespace Csis

#endif
