#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_BASE_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_BASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct CSTATEMGR_Base : public AudioMemBase {
  protected:
    // total size: 0x1C
    float m_CurTime;                  // offset 0x4, size 0x4
    float m_DeltaTime;                // offset 0x8, size 0x4
    enum eMAINMAPSTATES m_eStateType; // offset 0xC, size 0x4
    CSTATE_Base *m_pHeadStateObj;     // offset 0x10, size 0x4
    int m_CurNumStates;               // offset 0x14, size 0x4
    bool bIsInitialized;              // offset 0x18, size 0x1

    SFX_Base *CreateSFX(int Instance, int SFXObjID);
    SFXCTL *CreateSFXCTL(int Instance, int SFXCtrlID);
    CSTATE_Base *CreateState(int StateInstType, int _SFXFlags);

  public:
    CSTATEMGR_Base();
    virtual ~CSTATEMGR_Base();
    virtual void Initialize(eMAINMAPSTATES _m_eStateType);
    virtual void EnterWorld(eSndGameMode esgm);
    virtual CSTATE_Base *GetFreeState(void *ObjectPtr);
    virtual void UpdateParams(float t);
    virtual void ProcessUpdate();
    virtual void ExitWorld();
    virtual bool IsDataLoaded();

    int GetAttachedStateCount(void);
    CSTATE_Base *GetStateObj(int nInstance);
    CSTATE_Base *GetStateObj(void *testattachment);

    void DisconnectMixMap();
    void SafeConnectOrphanObjects();

    static void RegisterSFXCTL(SndBase::TypeInfo *typeinfo);
    static void RegisterSTATE(CSTATE_Base::StateInfo *stateinfo);
    static void RegisterSFX(SndBase::TypeInfo *typeinfo);

    static void InitClassLists(void);
    static void ClearClassLists(void);

    static bPList<SndBase::TypeInfo> m_SFXClassList;
    static bPList<SndBase::TypeInfo> m_SFXCTRLClassList;
    static bPList<CSTATE_Base::StateInfo> m_STATEClassList;
};

#endif
