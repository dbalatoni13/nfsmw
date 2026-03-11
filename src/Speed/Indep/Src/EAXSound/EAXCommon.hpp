#ifndef _EAXCOMMON
#define _EAXCOMMON

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct MMiscSound;

struct EAXCommon {
    char _pad[0x24];
    SFX_Base *m_pSFXOBJ_FEHUD; // offset 0x24

    EAXCommon();
    ~EAXCommon();
    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);
    virtual void Initialize();
    virtual void Stop(eMenuSoundTriggers etrigger);
    virtual int Play(eMenuSoundTriggers etrigger);
    virtual int Play(void *peventst);
    virtual void Update(void *peventst);
    virtual void *GetEventPointer(int neventindex);
    void MsgPlayMiscSound(const MMiscSound &msg);
};

#endif // _EAXCOMMON
