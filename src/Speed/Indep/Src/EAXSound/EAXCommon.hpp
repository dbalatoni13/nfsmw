#ifndef _EAXCOMMON
#define _EAXCOMMON

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"

struct MMiscSound;

struct EAXCommon {
    char _pad[0x24];
    SFX_Base *m_pSFXOBJ_FEHUD; // offset 0x24
    Hermes::HHANDLER mMsgMiscSound; // offset 0x28

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
};

#endif // _EAXCOMMON
