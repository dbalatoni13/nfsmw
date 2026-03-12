#ifndef _EAXFRONTEND
#define _EAXFRONTEND

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct PlayFrontEndSample;
struct PlayFrontEndSample_loop;
struct FEDriveOn;
struct FX_Hydraulic;
struct FX_Hydr_Bounce;
struct bVector3;
struct Camera;

struct EAXFrontEnd {
    char _pad[0x108];
    SFX_Base *m_pSFXOBJ_FEHUD; // offset 0x108

    EAXFrontEnd();
    virtual ~EAXFrontEnd();
    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);
    virtual void Initialize();
    virtual int Play(eMenuSoundTriggers etrigger);
    virtual void Stop(eMenuSoundTriggers etrigger);
    virtual int Play(void *peventst);
    virtual void Update(void *peventst);
    virtual void *GetEventPointer(int neventindex);
    void UpdateDriveOn();
    void SetFEDrivingCarState(bVector3 *, bVector3 *, Camera *, int);
    void DestroyAllDriveOnSnds();
};

#endif // _EAXFRONTEND
