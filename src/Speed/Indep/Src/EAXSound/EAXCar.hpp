#ifndef EAXSOUND_EAXCAR_H
#define EAXSOUND_EAXCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct stShiftPair {
    short RPM;
    short Time;
};

#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/acceltrans.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/shiftpattern.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/turbosfx.h"

struct SFXCTL_Physics;
struct DriverInfo;

enum eGINSU_ENG_TYPE {
    eGINSU_ENG_AEMS = 0,
    eGINSU_ENG_SINGLE = 1,
    eGINSU_ENG_DUAL = 2,
};

enum eSFXMessageType {
    SFX_NONE = 0,
    SFX_CHANGEGEAR = 1,
    SFX_SHIFT_UP = 2,
    SFX_SHIFT_DOWN = 3,
    SFX_NITROUS = 4,
    SFX_BOTTOMOUT = 5,
    SFX_TRUNKBOUNCE = 6,
    SFX_NIS_PATTERN = 7,
    SFX_NIS_REV = 8,
};

struct EAXCar : public CSTATE_Base {
    EAXCar();
    virtual ~EAXCar();

    virtual void PreLoadAssets() override;
    virtual void Attach(void *pAttachment) override;
    virtual void UpdateParams(float t) override;
    virtual void ProcessUpdate() override;
    virtual bool Detach() override;
    virtual StateInfo *GetStateInfo(void) const override;
    virtual const char *GetStateName(void) const override;

    virtual void ProcessSoundSphere(unsigned int unamehash, int nparamid, bVector3 *pv3pos, float fradius);
    virtual void UpdateCarPhysics();
    virtual void StartHonkHorn();
    virtual void StopHonkHorn();
    virtual bool IsHonking();
    virtual eAemsUpgradeLevel GetEngineUpgradeLevel();
    virtual int SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2);
    virtual void UpdatePov();
    virtual int UpdateRotation();

    Attrib::Gen::engineaudio &GetAttributes() { return mEngineInfo; }
    Attrib::Gen::shiftpattern &GetShiftInfo() { return mShiftInfo; }
    Attrib::Gen::turbosfx &GetTurboInfo() { return mTurboInfo; }
    Attrib::Gen::acceltrans &GetAccelInfo() { return mAccelInfo; }
    SFXCTL_Physics *GetPhysicsCTL() { return m_pPhysicsCTL; }
    bool IsAccelerating() { return bIsAccelerating; }
    Sound::Gear GetCurGear() { return static_cast<Sound::Gear>(CurGear); }
    float GetPhysRPM() { return PhysRPM; }
    float GetThrottle() { return fTrottle; }
    float GetPhysTRQ() { return PhysTRQ; }
    float GetVelocityMagnitudeMPH();
    int GetPOV() { return m_PovType; }
    void SetPhysTRQ(float _TRQ) { PhysTRQ = _TRQ; }
    void SetPhysRPM(float _RPM) { PhysRPM = _RPM; }
    void SetIsAccelerating(float _IsAccelerating) { bIsAccelerating = _IsAccelerating != 0.0f; }
    void SetCurGear(Sound::Gear _CurGear) { CurGear = _CurGear; }
    void SetThrottle(float _fTrottle) { fTrottle = _fTrottle; }
    float GetFinalAudioRPM() { return m_fAudioRPM; }

    static StateInfo *GetStaticStateInfo(void) {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(unsigned int allocator);

    void *operator new(size_t s, unsigned int allocator) {
        if (allocator != 0) {
            return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, true);
        } else {
            return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, false);
        }
    }

    void *operator new(size_t s) {
        return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, 0);
    }

    void *operator new(size_t, void *p) { return p; }

    static StateInfo s_StateInfo;

    int m_nHornState;                         // offset 0x44
    Attrib::Gen::engineaudio m_FEEngineAttribs; // offset 0x48
    SFXCTL_Physics *m_pPhysicsCTL;            // offset 0x5C
    float PhysTRQ;                            // offset 0x60
    float PhysRPM;                            // offset 0x64
    bool bIsAccelerating;                     // offset 0x68
    int CurGear;                              // offset 0x6C
    float fTrottle;                           // offset 0x70
    float m_fAudioRPM;                        // offset 0x74
    float m_CurTime;                          // offset 0x78
    float m_DeltaTime;                        // offset 0x7C
    bool m_bIsInSoundSphere;                  // offset 0x80
    bVector3 m_v3CurSpherePos;                // offset 0x84
    float m_fSphereRadius;                    // offset 0x94
    DriverInfo *m_pDriverInfo;                // offset 0x98
    eGINSU_ENG_TYPE m_EngineType;             // offset 0x9C
    eAemsUpgradeLevel m_nTrueEngineUpgradeLevel; // offset 0xA0
    eAemsUpgradeLevel m_EngUGL;               // offset 0xA4
    eAemsUpgradeLevel m_TurboUGL;             // offset 0xA8
    eAemsUpgradeLevel m_NOSUGL;               // offset 0xAC
    eAemsUpgradeLevel m_TireUGL;              // offset 0xB0
    eAemsUpgradeLevel m_TransmissionUGL;      // offset 0xB4
    int m_PovType;                            // offset 0xB8
    int m_IsDriveCamera;                      // offset 0xBC
    int m_Rotation;                           // offset 0xC0
    Attrib::Gen::engineaudio mEngineInfo;     // offset 0xC4
    Attrib::Gen::shiftpattern mShiftInfo;     // offset 0xD8
    Attrib::Gen::turbosfx mTurboInfo;         // offset 0xEC
    Attrib::Gen::acceltrans mAccelInfo;       // offset 0x100
};

#endif
