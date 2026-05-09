#ifndef EAXSOUND_EAXFRONTEND_H
#define EAXSOUND_EAXFRONTEND_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/World/Car.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"

struct Class;

struct PlayCommonSampleStruct { // 0x10
    /* 0x0 */ int id;
    /* 0x4 */ int volume;
    /* 0x8 */ int pitch;
    /* 0xc */ int azimuth;
};

struct PlayFrontEndSampleStruct { // 0x10
    /* 0x0 */ int id;
    /* 0x4 */ int volume;
    /* 0x8 */ int pitch;
    /* 0xc */ int azimuth;
};

// total size: 0x10
struct PlayFrontEndSample_RSStruct {
    // Members
    int id;      // offset 0x0, size 0x4
    int volume;  // offset 0x4, size 0x4
    int pitch;   // offset 0x8, size 0x4
    int azimuth; // offset 0xC, size 0x4
};

// total size: 0x14
struct PlayFrontEndSample_RS {
    // Functions
    inline void SetId(int x) {}

    inline int GetId() {}

    inline void SetVolume(int x) {}

    inline int GetVolume() {}

    inline void SetPitch(int x) {}

    inline int GetPitch() {}

    inline void SetAzimuth(int x) {}

    inline int GetAzimuth() {}

    inline int GetRefCount() {}

    static inline void *operator new(size_t size) {}

    static inline void operator delete(void *ptr) {}

    inline PlayFrontEndSample_RS(int id, int volume, int pitch, int azimuth) {}

    inline ~PlayFrontEndSample_RS() {}

    inline void CommitMemberData() {}

    // Members
    struct Class *mpClass;                    // offset 0x0, size 0x4
    struct PlayFrontEndSample_RSStruct mData; // offset 0x4, size 0x10
};

struct PlayFrontEndSample_loopStruct { // 0x10
    /* 0x0 */ int id;
    /* 0x4 */ int volume;
    /* 0x8 */ int pitch;
    /* 0xc */ int azimuth;
};

struct FEDriveOnStruct { // 0x10
    /* 0x0 */ int id;
    /* 0x4 */ int volume;
    /* 0x8 */ int pitch;
    /* 0xc */ int azimuth;
};

struct PlayCommonSample { // 0x14
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ PlayCommonSampleStruct mData;

  public:
    void SetId();
    int GetId();
    void SetVolume();
    int GetVolume();
    void SetPitch();
    int GetPitch();
    void SetAzimuth();
    int GetAzimuth();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    PlayCommonSample();
    void CommitMemberData();
};

struct PlayFrontEndSample { // 0x14
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ PlayFrontEndSampleStruct mData;

  public:
    void SetId();
    int GetId();
    void SetVolume();
    int GetVolume();
    void SetPitch();
    int GetPitch();
    void SetAzimuth();
    int GetAzimuth();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);

    PlayFrontEndSample();
    void CommitMemberData();
};

struct PlayFrontEndSample_loop { // 0x14
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ PlayFrontEndSample_loopStruct mData;

  public:
    void SetId();
    int GetId();
    void SetVolume();
    int GetVolume();
    void SetPitch();
    int GetPitch();
    void SetAzimuth();
    int GetAzimuth();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    PlayFrontEndSample_loop();
    void CommitMemberData();
};

struct FEDriveOn { // 0x14
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FEDriveOnStruct mData;

  public:
    void SetId();
    int GetId();
    void SetVolume();
    int GetVolume();
    void SetPitch();
    int GetPitch();
    void SetAzimuth();
    int GetAzimuth();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FEDriveOn();
    void CommitMemberData();
};

typedef enum { FXSHIFTING01TYPETYPE_SHIFT = 0, FXSHIFTING01TYPETYPE_ATTACK_ = 1 } FXSHIFTING01TypeType;

struct FX_NITROUSStruct { // 0x24
    /* 0x00 */ int nIT_ID;
    /* 0x04 */ int nIT_volume;
    /* 0x08 */ int nIT_azimuth;
    /* 0x0c */ int nIT_STOP;
    /* 0x10 */ int pitch;
    /* 0x14 */ int filter_Effects_LoPass;
    /* 0x18 */ int filter_Effects_HiPass;
    /* 0x1c */ int filter_Effects_Dry_FX;
    /* 0x20 */ int filter_Effects_Wet_FX;
};

struct FX_PURGEStruct { // 0x20
    /* 0x00 */ int pURGE_volume;
    /* 0x04 */ int pURGE_azimuth;
    /* 0x08 */ int pURGE_STOP;
    /* 0x0c */ int pURGE_pitch;
    /* 0x10 */ int filter_Effects_LoPass;
    /* 0x14 */ int filter_Effects_HiPass;
    /* 0x18 */ int filter_Effects_Dry_FX;
    /* 0x1c */ int filter_Effects_Wet_FX;
};

struct FX_SHIFTING_01Struct { // 0x18
    /* 0x00 */ int id;
    /* 0x04 */ int volume;
    /* 0x08 */ int pitch;
    /* 0x0c */ int azimuth;
    /* 0x10 */ FXSHIFTING01TypeType type;
    /* 0x14 */ int cAM;
};

struct FX_SPARKCHATTERStruct { // 0x20
    /* 0x00 */ int id;
    /* 0x04 */ int volume;
    /* 0x08 */ int pitch;
    /* 0x0c */ int azimuth;
    /* 0x10 */ int rotation;
    /* 0x14 */ int rPM;
    /* 0x18 */ int sPEED;
    /* 0x1c */ int cAM;
};

struct FX_SKIDStruct { // 0x58
    /* 0x00 */ int vOL_Fwd;
    /* 0x04 */ int vOL_Side;
    /* 0x08 */ int vOL_Back;
    /* 0x0c */ int azimuth;
    /* 0x10 */ int tYPE;
    /* 0x14 */ int pITCH_OFFSET;
    /* 0x18 */ int hOP;
    /* 0x1c */ int sPEED;
    /* 0x20 */ int oPPOS_Side;
    /* 0x24 */ int uNDERSTEER;
    /* 0x28 */ int oVERSTEER;
    /* 0x2c */ int surface;
    /* 0x30 */ int front_FB;
    /* 0x34 */ int front_LR;
    /* 0x38 */ int front_Load;
    /* 0x3c */ int back_FB;
    /* 0x40 */ int back_LR;
    /* 0x44 */ int back_Load;
    /* 0x48 */ int filter_Effects_LoPass;
    /* 0x4c */ int filter_Effects_HiPass;
    /* 0x50 */ int filter_Effects_Dry_FX;
    /* 0x54 */ int filter_Effects_Wet_FX;
};

struct FX_HydraulicStruct { // 0x18
    /* 0x00 */ int hYD_ID;
    /* 0x04 */ int hYD_PITCH_OFFSET;
    /* 0x08 */ int hYD_STOP;
    /* 0x0c */ int hYD_Up_Down;
    /* 0x10 */ int hYD_volume;
    /* 0x14 */ int hYD_azimuth;
};

struct FX_HelicopterStruct { // 0x30
    /* 0x00 */ int hELI_ID;
    /* 0x04 */ int hELI_Pitch_Offset;
    /* 0x08 */ int hELI_Stop;
    /* 0x0c */ int hELI_Volume;
    /* 0x10 */ int hELI_Azmuth;
    /* 0x14 */ int hELI_Speed;
    /* 0x18 */ int hELI_Distance;
    /* 0x1c */ int hELI_LowPass;
    /* 0x20 */ int hELI_HiPass;
    /* 0x24 */ int hELI_FX_Dry;
    /* 0x28 */ int hELI_FX_Wet;
    /* 0x2c */ int hELI_Rotation;
};

struct FX_Hydr_BounceStruct { // 0x10
    /* 0x0 */ int hYD_ID;
    /* 0x4 */ int hYD_PITCH_OFFSET;
    /* 0x8 */ int hYD_volume;
    /* 0xc */ int hYD_azimuth;
};

struct FX_WeatherStruct { // 0x28
    /* 0x00 */ int hood_Rain;
    /* 0x04 */ int hood_Rain_Vol_Substract;
    /* 0x08 */ int volume;
    /* 0x0c */ int width;
    /* 0x10 */ int pitch_Offset;
    /* 0x14 */ int rain_on_off;
    /* 0x18 */ int filter_Effects_LoPass;
    /* 0x1c */ int filter_Effects_HiPass;
    /* 0x20 */ int filter_Effects_Dry_FX;
    /* 0x24 */ int filter_Effects_Wet_FX;
};

struct FX_CameraStruct { // 0x20
    /* 0x00 */ int iD;
    /* 0x04 */ int volume;
    /* 0x08 */ int width;
    /* 0x0c */ int pitch_Offset;
    /* 0x10 */ int cam_beep;
    /* 0x14 */ int cam_beep_Volume;
    /* 0x18 */ int cam_whine;
    /* 0x1c */ int cam_whine_Volume;
};

struct FX_UVESStruct { // 0x18
    /* 0x00 */ int iD;
    /* 0x04 */ int volume;
    /* 0x08 */ int width;
    /* 0x0c */ int pitch_Offset;
    /* 0x10 */ int intensity;
    /* 0x14 */ int stop;
};

struct FX_RadarStruct { // 0x14
    /* 0x00 */ int iD;
    /* 0x04 */ int volume;
    /* 0x08 */ int pitch_Offset;
    /* 0x0c */ int intensity;
    /* 0x10 */ int stop;
};

struct FX_ScrapeStruct { // 0x24
    /* 0x00 */ int volume;
    /* 0x04 */ int pitch;
    /* 0x08 */ int azimuth;
    /* 0x0c */ int terrain_type;
    /* 0x10 */ int impulse_magnitude;
    /* 0x14 */ int filter_Effects_LoPass;
    /* 0x18 */ int filter_Effects_HiPass;
    /* 0x1c */ int filter_Effects_Dry_FX;
    /* 0x20 */ int filter_Effects_Wet_FX;
};

struct FX_NITROUS { // 0x28
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_NITROUSStruct mData;

  public:
    void SetNIT_ID();
    int GetNIT_ID();
    void SetNIT_volume();
    int GetNIT_volume();
    void SetNIT_azimuth();
    int GetNIT_azimuth();
    void SetNIT_STOP();
    int GetNIT_STOP();
    void SetPitch();
    int GetPitch();
    void SetFilter_Effects_LoPass();
    int GetFilter_Effects_LoPass();
    void SetFilter_Effects_HiPass();
    int GetFilter_Effects_HiPass();
    void SetFilter_Effects_Dry_FX();
    int GetFilter_Effects_Dry_FX();
    void SetFilter_Effects_Wet_FX();
    int GetFilter_Effects_Wet_FX();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_NITROUS();

    void CommitMemberData();
};

struct FX_PURGE { // 0x24
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_PURGEStruct mData;

  public:
    void SetPURGE_volume();
    int GetPURGE_volume();
    void SetPURGE_azimuth();
    int GetPURGE_azimuth();
    void SetPURGE_STOP();
    int GetPURGE_STOP();
    void SetPURGE_pitch();
    int GetPURGE_pitch();
    void SetFilter_Effects_LoPass();
    int GetFilter_Effects_LoPass();
    void SetFilter_Effects_HiPass();
    int GetFilter_Effects_HiPass();
    void SetFilter_Effects_Dry_FX();
    int GetFilter_Effects_Dry_FX();
    void SetFilter_Effects_Wet_FX();
    int GetFilter_Effects_Wet_FX();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_PURGE();

    void CommitMemberData();
};

struct FX_SHIFTING_01 { // 0x1c
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_SHIFTING_01Struct mData;

  public:
    void SetId();
    int GetId();
    void SetVolume();
    int GetVolume();
    void SetPitch();
    int GetPitch();
    void SetAzimuth();
    int GetAzimuth();
    void SetType();
    FXSHIFTING01TypeType GetType();
    void SetCAM();
    int GetCAM();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_SHIFTING_01();

    void CommitMemberData();
};

struct FX_SPARKCHATTER { // 0x24
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_SPARKCHATTERStruct mData;

  public:
    void SetId();
    int GetId();
    void SetVolume();
    int GetVolume();
    void SetPitch();
    int GetPitch();
    void SetAzimuth();
    int GetAzimuth();
    void SetRotation();
    int GetRotation();
    void SetRPM();
    int GetRPM();
    void SetSPEED();
    int GetSPEED();
    void SetCAM();
    int GetCAM();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_SPARKCHATTER();

    void CommitMemberData();
};

struct FX_SKID { // 0x5c
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_SKIDStruct mData;

  public:
    void SetVOL_Fwd();
    int GetVOL_Fwd();
    void SetVOL_Side();
    int GetVOL_Side();
    void SetVOL_Back();
    int GetVOL_Back();
    void SetAzimuth();
    int GetAzimuth();
    void SetTYPE();
    int GetTYPE();
    void SetPITCH_OFFSET();
    int GetPITCH_OFFSET();
    void SetHOP();
    int GetHOP();
    void SetSPEED();
    int GetSPEED();
    void SetOPPOS_Side();
    int GetOPPOS_Side();
    void SetUNDERSTEER();
    int GetUNDERSTEER();
    void SetOVERSTEER();
    int GetOVERSTEER();
    void SetSurface();
    int GetSurface();
    void SetFront_FB();
    int GetFront_FB();
    void SetFront_LR();
    int GetFront_LR();
    void SetFront_Load();
    int GetFront_Load();
    void SetBack_FB();
    int GetBack_FB();
    void SetBack_LR();
    int GetBack_LR();
    void SetBack_Load();
    int GetBack_Load();
    void SetFilter_Effects_LoPass();
    int GetFilter_Effects_LoPass();
    void SetFilter_Effects_HiPass();
    int GetFilter_Effects_HiPass();
    void SetFilter_Effects_Dry_FX();
    int GetFilter_Effects_Dry_FX();
    void SetFilter_Effects_Wet_FX();
    int GetFilter_Effects_Wet_FX();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_SKID();

    void CommitMemberData();
};

struct FX_Hydraulic { // 0x1c
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_HydraulicStruct mData;

  public:
    void SetHYD_ID();
    int GetHYD_ID();
    void SetHYD_PITCH_OFFSET();
    int GetHYD_PITCH_OFFSET();
    void SetHYD_STOP();
    int GetHYD_STOP();
    void SetHYD_Up_Down();
    int GetHYD_Up_Down();
    void SetHYD_volume();
    int GetHYD_volume();
    void SetHYD_azimuth();
    int GetHYD_azimuth();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_Hydraulic();

    void CommitMemberData();
};

struct FX_Helicopter { // 0x34
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_HelicopterStruct mData;

  public:
    void SetHELI_ID();
    int GetHELI_ID();
    void SetHELI_Pitch_Offset();
    int GetHELI_Pitch_Offset();
    void SetHELI_Stop();
    int GetHELI_Stop();
    void SetHELI_Volume();
    int GetHELI_Volume();
    void SetHELI_Azmuth();
    int GetHELI_Azmuth();
    void SetHELI_Speed();
    int GetHELI_Speed();
    void SetHELI_Distance();
    int GetHELI_Distance();
    void SetHELI_LowPass();
    int GetHELI_LowPass();
    void SetHELI_HiPass();
    int GetHELI_HiPass();
    void SetHELI_FX_Dry();
    int GetHELI_FX_Dry();
    void SetHELI_FX_Wet();
    int GetHELI_FX_Wet();
    void SetHELI_Rotation();
    int GetHELI_Rotation();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);

    FX_Helicopter();
    void CommitMemberData();
};

struct FX_Hydr_Bounce { // 0x14
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_Hydr_BounceStruct mData;

  public:
    void SetHYD_ID();
    int GetHYD_ID();
    void SetHYD_PITCH_OFFSET();
    int GetHYD_PITCH_OFFSET();
    void SetHYD_volume();
    int GetHYD_volume();
    void SetHYD_azimuth();
    int GetHYD_azimuth();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_Hydr_Bounce();

    void CommitMemberData();
};

struct FX_Weather { // 0x2c
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_WeatherStruct mData;

  public:
    void SetHood_Rain();
    int GetHood_Rain();
    void SetHood_Rain_Vol_Substract();
    int GetHood_Rain_Vol_Substract();
    void SetVolume();
    int GetVolume();
    void SetWidth();
    int GetWidth();
    void SetPitch_Offset();
    int GetPitch_Offset();
    void SetRain_on_off();
    int GetRain_on_off();
    void SetFilter_Effects_LoPass();
    int GetFilter_Effects_LoPass();
    void SetFilter_Effects_HiPass();
    int GetFilter_Effects_HiPass();
    void SetFilter_Effects_Dry_FX();
    int GetFilter_Effects_Dry_FX();
    void SetFilter_Effects_Wet_FX();
    int GetFilter_Effects_Wet_FX();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_Weather();

    void CommitMemberData();
};

struct FX_Camera { // 0x24
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_CameraStruct mData;

  public:
    void SetID();
    int GetID();
    void SetVolume();
    int GetVolume();
    void SetWidth();
    int GetWidth();
    void SetPitch_Offset();
    int GetPitch_Offset();
    void SetCam_beep();
    int GetCam_beep();
    void SetCam_beep_Volume();
    int GetCam_beep_Volume();
    void SetCam_whine();
    int GetCam_whine();
    void SetCam_whine_Volume();
    int GetCam_whine_Volume();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_Camera();

    void CommitMemberData();
};

struct FX_UVES { // 0x1c
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_UVESStruct mData;

  public:
    void SetID();
    int GetID();
    void SetVolume();
    int GetVolume();
    void SetWidth();
    int GetWidth();
    void SetPitch_Offset();
    int GetPitch_Offset();
    void SetIntensity();
    int GetIntensity();
    void SetStop();
    int GetStop();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_UVES();

    void CommitMemberData();
};

struct FX_Radar { // 0x18
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_RadarStruct mData;

  public:
    void SetID();
    int GetID();
    void SetVolume();
    int GetVolume();
    void SetPitch_Offset();
    int GetPitch_Offset();
    void SetIntensity();
    int GetIntensity();
    void SetStop();
    int GetStop();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_Radar();

    void CommitMemberData();
};

struct FX_Scrape { // 0x28
  private:
    /* 0x00 */ Class *mpClass;
    /* 0x04 */ FX_ScrapeStruct mData;

  public:
    void SetVolume();
    int GetVolume();
    void SetPitch();
    int GetPitch();
    void SetAzimuth();
    int GetAzimuth();
    void SetTerrain_type();
    int GetTerrain_type();
    void SetImpulse_magnitude();
    int GetImpulse_magnitude();
    void SetFilter_Effects_LoPass();
    int GetFilter_Effects_LoPass();
    void SetFilter_Effects_HiPass();
    int GetFilter_Effects_HiPass();
    void SetFilter_Effects_Dry_FX();
    int GetFilter_Effects_Dry_FX();
    void SetFilter_Effects_Wet_FX();
    int GetFilter_Effects_Wet_FX();
    int GetRefCount();
    // static void *__nw(/* parameters unknown */);
    // static void __dl(/* parameters unknown */);
    FX_Scrape();

    void CommitMemberData();
};

typedef enum {
    SFXOBJ_MUSIC = 0,
    SFXOBJ_SPEECH = 1,
    SFXOBJ_AMBIENCE = 2,
    SFXOBJ_MOVIES = 3,
    SFXOBJ_COMMON = 4,
    SFXOBJ_NISPROJ_STRMS = 5,
    SFXOBJ_MOMENT_STRMS = 6,
    SFXOBJ_FEHUD = 7
} eSFXOBJ_MAIN_TYPES;

typedef eSFXOBJ_MAIN_TYPES reflection_typedef_eSFXOBJ_MAIN_TYPES;

typedef enum { DRIVE_ON_NONE = -1, DRIVE_ON_IDLEING = 0, DRIVE_ON_ENTERING = 1, DRIVE_ON_LEAVING = 2, NUM_DRIVE_ON_STATES = 3 } DRIVE_ON_STATE;

typedef enum { CAR_1 = 0, CAR_2 = 1, NUM_CAR_INDEXS = 2 } DRIVE_ON_CAR_INDEX;

// total size: 0x110
class EAXFrontEnd {
  public:
    // Functions
    EAXFrontEnd();

    virtual ~EAXFrontEnd();

    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);

    virtual void Initialize();

    virtual void Stop(eMenuSoundTriggers etrigger);

    virtual int Play(eMenuSoundTriggers etrigger);

    virtual int Play(void *peventst);

    virtual void Update(void *peventst);

    void PlayHydraulics(HydraulicState eType, int iWheel);

    void StopHydraulics(int iWheel);

    void PlayBounce(int iWheel);

    virtual void *GetEventPointer(int neventindex);

    void UpdateDriveOn();

    void SetFEDrivingCarState(bVector3 *car_position, bVector3 *car_velocity, Camera *camera, int view_id);

    void PlayDriveOnSnd(enum eMenuSoundTriggers etrigger, int view_id, CarType eCarType);

    void EndDriveOnSnd(enum eMenuSoundTriggers etrigger, int view_id);

    void DestroyDriveOn(DRIVE_ON_STATE SndIndex, DRIVE_ON_CAR_INDEX CarIndex);

    void DestroyAllDriveOnSnds();

    // Members
    FX_Hydraulic *m_hydraulicsControls[4]; // offset 0x0, size 0x10
    FX_Hydr_Bounce *m_hydraulicsBounce[4]; // offset 0x10, size 0x10
  private:
    PlayFrontEndSample *m_pPlayFrontEndSampleHandle; // offset 0x20, size 0x4
    PlayFrontEndSample_RS *m_pPlayRapSheet;          // offset 0x24, size 0x4
    FEDriveOn *m_pDriveOnOffSampleHandle[3][2];      // offset 0x28, size 0x18
    cInterpLine DriveOnFadeOut[3][2];                // offset 0x40, size 0xA8
    bool IsEnding[3][2];                             // offset 0xE8, size 0x6
    DRIVE_ON_STATE DriveONCarState[2];               // offset 0x100, size 0x8
    SFX_Base *m_pSFXOBJ_FEHUD;                       // offset 0x108, size 0x4
};

#endif
