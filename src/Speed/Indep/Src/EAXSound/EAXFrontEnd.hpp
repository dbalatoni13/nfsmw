//
//
//
#ifndef EAX_FRONTEND_HPP
#define EAX_FRONTEND_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/FE_AEMS.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"

// Decl: 16
enum DRIVE_ON_STATE {
    DRIVE_ON_NONE = -1,
    DRIVE_ON_IDLEING = 0,
    DRIVE_ON_ENTERING = 1,
    DRIVE_ON_LEAVING = 2,
    NUM_DRIVE_ON_STATES = 3,
};

// Decl: 26
enum DRIVE_ON_CAR_INDEX {
    CAR_1 = 0,
    CAR_2 = 1,
    NUM_CAR_INDEXS = 2,
};

// total size: 0x110
// Decl: 35
class EAXFrontEnd {
  public:
    EAXFrontEnd();
    virtual ~EAXFrontEnd();

    virtual void Initialize();

    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);

    virtual int Play(eMenuSoundTriggers etrigger);
    virtual void Stop(eMenuSoundTriggers etrigger);
    virtual int Play(void *peventst);

    virtual void Update(void *peventst);

    virtual void *GetEventPointer(int neventindex);

    void PlayDriveOnSnd(eMenuSoundTriggers etrigger, int view_id, CarType eCarType);
    void EndDriveOnSnd(eMenuSoundTriggers etrigger, int view_id);
    void DestroyAllDriveOnSnds();
    void UpdateDriveOn();

    void PlayHydraulics(HydraulicState eType, int iWheel);
    void StopHydraulics(int iWheel);

    void PlayBounce(int iWheel);

    void SetFEDrivingCarState(bVector3 *car_position, bVector3 *car_velocity, Camera *camera, int view_id);

    Csis::FX_Hydraulic *m_hydraulicsControls[4]; // offset 0x0, size 0x10, Decl: 58
    Csis::FX_Hydr_Bounce *m_hydraulicsBounce[4]; // offset 0x10, size 0x10, Decl: 59

  private:
    void DestroyDriveOn(DRIVE_ON_STATE SndIndex, DRIVE_ON_CAR_INDEX CarIndex);

    Csis::PlayFrontEndSample *m_pPlayFrontEndSampleHandle;                           // offset 0x20, size 0x4, Decl: 70
    Csis::PlayFrontEndSample_RS *m_pPlayRapSheet;                                    // offset 0x24, size 0x4, Decl: 71
    Csis::FEDriveOn *m_pDriveOnOffSampleHandle[NUM_CAR_INDEXS][NUM_DRIVE_ON_STATES]; // offset 0x28, size 0x18, Decl: 74
    cInterpLine DriveOnFadeOut[NUM_CAR_INDEXS][NUM_DRIVE_ON_STATES];                 // offset 0x40, size 0xA8, Decl: 75
    bool IsEnding[NUM_CAR_INDEXS][NUM_DRIVE_ON_STATES];                              // offset 0xE8, size 0x6, Decl: 76
    DRIVE_ON_STATE DriveONCarState[NUM_CAR_INDEXS];                                  // offset 0x100, size 0x8, Decl: 78
    SFX_Base *m_pSFXOBJ_FEHUD;                                                       // offset 0x108, size 0x4, Decl: 86
};

// total size: 0x30
// Decl: 92
class EAXCommon {
  public:
    EAXCommon();
    virtual ~EAXCommon();

    virtual void Initialize();
    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);
    virtual int Play(eMenuSoundTriggers etrigger);
    virtual void Stop(eMenuSoundTriggers etrigger);
    virtual int Play(void *peventst);
    virtual void Update(void *peventst);
    virtual void *GetEventPointer(int neventindex);

    void Reset();

  private:
    void MsgPlayMiscSound(const struct MMiscSound &message);

  private:
    int m_nAemsPlayHandle[7];                          // offset 0x0, size 0x1C, Decl: 106
    Csis::PlayCommonSample *m_pPlayCommonSampleHandle; // offset 0x1C, size 0x4, Decl: 107
    Csis::FX_Radar *m_pRadar;                          // offset 0x20, size 0x4, Decl: 110
    SFX_Base *m_pSFXOBJ_FEHUD;                         // offset 0x24, size 0x4, Decl: 114
    Hermes::HHANDLER mMsgMiscSound;                    // offset 0x28, size 0x4, Decl: 117
};

#endif
