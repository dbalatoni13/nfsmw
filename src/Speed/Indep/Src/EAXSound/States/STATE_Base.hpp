#ifndef EAXSOUND_STATES_STATE_BASE_H
#define EAXSOUND_STATES_STATE_BASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

enum eMAINMAPSTATES {
    eMM_MAX_MAIN_MIXSTATES = 13,
    eMM_TRUCK = 12,
    eMM_HELICOPTER = 11,
    eMM_TRAIN = 10,
    eMM_PLANE = 9,
    eMM_DRIVEBY = 8,
    eMM_COLLISION = 7,
    eMM_ENVIRONMENT = 6,
    eMM_TRAFFIC = 5,
    eMM_COPCAR = 4,
    eMM_AIRACECAR = 3,
    eMM_PLAYERCAR = 2,
    eMM_MUSIC = 1,
    eMM_MAIN = 0,
};

struct CSTATE_Base : public AudioMemBase {
    friend struct CSTATEMGR_Base;
    struct StateInfo {
        // total size: 0x10
        int StateID;                       // offset 0x0, size 0x4
        char *stateName;                   // offset 0x4, size 0x4
        StateInfo *baseStateInfo;          // offset 0x8, size 0x4
        CSTATE_Base *(*createState)(void); // offset 0xc, size 0x4
    };

    CSTATE_Base();
    virtual ~CSTATE_Base();
    virtual void Setup(int _m_SFXFlags);
    virtual void PreLoadAssets();
    virtual void Attach();
    virtual void LoadData();
    virtual void UpdateParams(float t);
    virtual void ProcessUpdate();
    virtual bool Detach();
    virtual void Destroy();
    virtual StateInfo *GetStateInfo(void) const;
    virtual char *GetStateName(void) const;

    static StateInfo *GetStaticStateInfo(void) {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(unsigned int allocator);

    bool IsDataLoaded(void);
    void DisconnectMixMap();
    void SafeConnectOrphanObjects();

    void *operator new(size_t s) {
        return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, 0);
    }

  public:
    // total size: 0x44
    CSTATE_Base *m_pNextState;          // offset 0x4, size 0x4
    CSTATE_Base *m_pPreviousState;      // offset 0x8, size 0x4
    struct CSTATEMGR_Base *m_pStateMgr; // offset 0xC, size 0x4
    int m_InstNum;                      // offset 0x10, size 0x4
    eMAINMAPSTATES m_eStateType;        // offset 0x14, size 0x4
    int m_StateInstType;                // offset 0x18, size 0x4
    void *m_pAttachment;                // offset 0x1C, size 0x4
  protected:                            // according to stabs
    SndBase *m_pHeadSFXCTL;             // offset 0x20, size 0x4
    SndBase *m_pHeadSFXObj;             // offset 0x24, size 0x4
    int m_SFXFlags;                     // offset 0x28, size 0x4
    int m_NumLoadedSFXObj;              // offset 0x2C, size 0x4
    int m_NumLoadedSFXCTL;              // offset 0x30, size 0x4
    struct EAX_CarState *m_pCar;        // offset 0x34, size 0x4
    bool bIsAttached;                   // offset 0x38, size 0x1
    float t_CurTime;                    // offset 0x3C, size 0x4
    float t_DeltaTime;                  // offset 0x40, size 0x4

    static StateInfo s_StateInfo;

    void NewSFXObj(int ecarsfx);
    SFXCTL *NewSFXCtrl(int esfxctl);
    void CreateSFXObjs();
    void CreateSFXCtrls();
    SFXCTL *HasCtrlBeenAdded(int esfxctrl);
    void SortSFXCtl();
};

#endif
