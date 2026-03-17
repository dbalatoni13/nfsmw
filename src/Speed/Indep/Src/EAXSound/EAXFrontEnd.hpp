#ifndef _EAXFRONTEND
#define _EAXFRONTEND

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct PlayFrontEndSample;
struct PlayFrontEndSample_RS;
struct FEDriveOn;
struct FX_Hydraulic;
struct FX_Hydr_Bounce;
struct bVector3;
struct Camera;

enum DRIVE_ON_STATE {
    DRIVE_ON_NONE = -1,
    DRIVE_ON_IDLEING = 0,
    DRIVE_ON_ENTERING = 1,
    DRIVE_ON_LEAVING = 2,
    NUM_DRIVE_ON_STATES = 3,
};

enum DRIVE_ON_CAR_INDEX {
    CAR_1 = 0,
    CAR_2 = 1,
    NUM_CAR_INDEXS = 2,
};

enum HydraulicState {
    HYDRAULICS_OFF = 0,
    HYDRAULICS_EXTENDED = 1,
    HYDRAULICS_EXTENDING = 2,
    HYDRAULICS_RELAXING = 3,
};

// total size: 0x110
struct EAXFrontEnd {
    FX_Hydraulic *m_hydraulicsControls[4];                     // offset 0x0, size 0x10
    FX_Hydr_Bounce *m_hydraulicsBounce[4];                     // offset 0x10, size 0x10
    PlayFrontEndSample *m_pPlayFrontEndSampleHandle;           // offset 0x20, size 0x4
    PlayFrontEndSample_RS *m_pPlayRapSheet;                    // offset 0x24, size 0x4
    FEDriveOn *m_pDriveOnOffSampleHandle[2][3];                // offset 0x28, size 0x18
    cInterpLine DriveOnFadeOut[2][3];                          // offset 0x40, size 0xA8
    bool IsEnding[2][3];                                       // offset 0xE8, size 0x6
    DRIVE_ON_STATE DriveONCarState[2];                         // offset 0x100, size 0x8
    SFX_Base *m_pSFXOBJ_FEHUD;                                // offset 0x108, size 0x4

    EAXFrontEnd();
    virtual ~EAXFrontEnd();
    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);
    virtual void Initialize();
    virtual int Play(eMenuSoundTriggers etrigger);
    virtual void Stop(eMenuSoundTriggers etrigger);
    virtual int Play(void *peventst);
    virtual void Update(void *peventst);
    virtual void *GetEventPointer(int neventindex);
    void PlayHydraulics(HydraulicState eType, int iWheel);
    void StopHydraulics(int iWheel);
    void PlayBounce(int iWheel);
    void UpdateDriveOn();
    void SetFEDrivingCarState(bVector3 *car_position, bVector3 *car_velocity, Camera *camera, int view_id);
    void PlayDriveOnSnd(eMenuSoundTriggers etrigger, int view_id, CarType eCarType);
    void EndDriveOnSnd(eMenuSoundTriggers etrigger, int view_id);
    void DestroyDriveOn(DRIVE_ON_STATE SndIndex, DRIVE_ON_CAR_INDEX CarIndex);
    void DestroyAllDriveOnSnds();
};

#endif // _EAXFRONTEND
