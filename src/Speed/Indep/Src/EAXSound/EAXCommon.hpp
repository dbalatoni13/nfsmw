#ifndef _EAXCOMMON
#define _EAXCOMMON

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"

struct MMiscSound;
struct PlayCommonSample;
struct FX_Radar;

// total size: 0x30
struct EAXCommon {
    int m_nAemsPlayHandle[7];                  // offset 0x0, size 0x1C
    PlayCommonSample *m_pPlayCommonSampleHandle; // offset 0x1C, size 0x4
    FX_Radar *m_pRadar;                        // offset 0x20, size 0x4
    SFX_Base *m_pSFXOBJ_FEHUD;                 // offset 0x24, size 0x4
    Hermes::HHANDLER mMsgMiscSound;            // offset 0x28, size 0x4

    EAXCommon();
    virtual ~EAXCommon();
    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);
    virtual void Initialize();
    virtual int Play(eMenuSoundTriggers etrigger);
    virtual void Stop(eMenuSoundTriggers etrigger);
    virtual int Play(void *peventst);
    virtual void Update(void *peventst);
    virtual void *GetEventPointer(int neventindex);
    void MsgPlayMiscSound(const MMiscSound &msg);
    void Reset();
};

#endif // _EAXCOMMON
