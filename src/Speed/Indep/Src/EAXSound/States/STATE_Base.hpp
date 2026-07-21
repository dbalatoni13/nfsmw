//
//
//
//
#ifndef STATE_BASE_HPP
#define STATE_BASE_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"

#define STATE_SHFT(StateEnum) StateEnum << 16                       // Decl: 19
#define STATE_ALLOC(AllocID) (AllocID << 24)                        // Decl: 20
#define STATE_MASK(StateID) ((0x00FF0000 & StateID) >> 16)          // Decl: 21
#define STATE_MASKALLOCTYPE(StateID) ((StateID & 0x0F000000) >> 24) // Decl: 22
#define STATEINST_TYPE_MASK(StateID) (0x0000FFFF & StateID)         // Decl: 23
// Decl: 26
#define DECLARE_STATETYPE()                                                                                                                          \
  protected:                                                                                                                                         \
    static CSTATE_Base::StateInfo s_StateInfo;                                                                                                       \
                                                                                                                                                     \
  public:                                                                                                                                            \
    CSTATE_Base::StateInfo *GetStateInfo() const override;                                                                                           \
    const char *GetStateName() const override;                                                                                                       \
    static CSTATE_Base::StateInfo *GetStaticStateInfo() {                                                                                            \
        return &s_StateInfo;                                                                                                                         \
    }                                                                                                                                                \
    static CSTATE_Base *CreateState(uint32 allocator);

// Decl: 37
#define STATETYPE_IMPLEMENT(theObjectID, theClass, baseClass)                                                                                        \
    CSTATE_Base::StateInfo theClass::s_StateInfo = {(int)theObjectID, #theClass, &baseClass::s_StateInfo, theClass::CreateState};                    \
    CSTATE_Base::StateInfo *theClass::GetStateInfo() const {                                                                                         \
        return &s_StateInfo;                                                                                                                         \
    }                                                                                                                                                \
    const char *theClass::GetStateName() const {                                                                                                     \
        return s_StateInfo.stateName;                                                                                                                \
    }                                                                                                                                                \
    CSTATE_Base *theClass::CreateState(uint32 allocator) {                                                                                           \
        if (allocator == 0)                                                                                                                          \
            return new (GetStaticStateInfo()->stateName, false) theClass;                                                                            \
        else                                                                                                                                         \
            return new (GetStaticStateInfo()->stateName, true) theClass;                                                                             \
    }

// Decl: 46
#define ROOTSTATETYPE_IMPLEMENT(theObjectID, theClass)                                                                                               \
    CSTATE_Base::StateInfo theClass::s_StateInfo = {(int)theObjectID, #theClass, NULL, theClass::CreateState};                                       \
    CSTATE_Base::StateInfo *theClass::GetStateInfo() const {                                                                                         \
        return &s_StateInfo;                                                                                                                         \
    }                                                                                                                                                \
    const char *theClass::GetStateName() const {                                                                                                     \
        return s_StateInfo.stateName;                                                                                                                \
    }                                                                                                                                                \
    CSTATE_Base *theClass::CreateState(uint32 allocator) {                                                                                           \
        return new (GetStaticStateInfo()->stateName) theClass;                                                                                       \
    }

#define STATEINFO(classType) classType::GetStaticStateInfo() // Decl: 53

class SndBase;
class SFXCTL;
class EAX_CarState;
class CSTATEMGR_Base;

// total size: 0x44
class CSTATE_Base : public AudioMemBase {
  public:
    friend class CSTATEMGR_Base; // TODO needed?
    // total size: 0x10
    struct StateInfo {
        int StateID;                         // offset 0x0, size 0x4
        const char *stateName;               // offset 0x4, size 0x4
        StateInfo *baseStateInfo;            // offset 0x8, size 0x4
        CSTATE_Base *(*createState)(uint32); // offset 0xc, size 0x4
    };

    CSTATE_Base();
    virtual ~CSTATE_Base();
    virtual void Setup(int _m_SFXFlags);
    virtual void PreLoadAssets();
    virtual void Attach(void *pAttachment);
    virtual void LoadData();
    virtual void UpdateParams(float t);
    virtual void ProcessUpdate();
    virtual bool Detach();
    virtual void Destroy();

    void SafeConnectOrphanObjects();
    void DisconnectMixMap();

    EAX_CarState *GetPhysCar() {
        return this->m_pCar;
    }

    float GetCurTime() {
        return this->t_CurTime;
    }

    float GetDeltaTime() {
        return this->t_DeltaTime;
    }

  protected:
    static CSTATE_Base::StateInfo s_StateInfo;

  public:
    virtual CSTATE_Base::StateInfo *GetStateInfo() const;
    virtual const char *GetStateName() const;
    static CSTATE_Base::StateInfo *GetStaticStateInfo() {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(uint32 allocator);

    bool IsDataLoaded(void);

    bool IsAttached() {
        return bIsAttached;
    }

    bool IsAttachedToThis(void *testattachment) {
        return testattachment == m_pAttachment;
    }

    // void *operator new(size_t s) {
    //     return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, false);
    // }

  public:
    CSTATE_Base *m_pNextState;     // offset 0x4, size 0x4
    CSTATE_Base *m_pPreviousState; // offset 0x8, size 0x4
    CSTATEMGR_Base *m_pStateMgr;   // offset 0xC, size 0x4
    int m_InstNum;                 // offset 0x10, size 0x4
    eMAINMAPSTATES m_eStateType;   // offset 0x14, size 0x4
    int m_StateInstType;           // offset 0x18, size 0x4
    void *m_pAttachment;           // offset 0x1C, size 0x4

  protected:
    void NewSFXObj(int ecarsfx);
    SFXCTL *NewSFXCtrl(int esfxctl);
    void CreateSFXObjs();
    void CreateSFXCtrls();
    SFXCTL *HasCtrlBeenAdded(int esfxctrl);
    void SortSFXCtl();

    SndBase *m_pHeadSFXCTL; // offset 0x20, size 0x4
    SndBase *m_pHeadSFXObj; // offset 0x24, size 0x4
    int m_SFXFlags;         // offset 0x28, size 0x4
    int m_NumLoadedSFXObj;  // offset 0x2C, size 0x4
    int m_NumLoadedSFXCTL;  // offset 0x30, size 0x4
    EAX_CarState *m_pCar;   // offset 0x34, size 0x4
    bool bIsAttached;       // offset 0x38, size 0x1
    float t_CurTime;        // offset 0x3C, size 0x4
    float t_DeltaTime;      // offset 0x40, size 0x4
};

#endif
