#ifndef EAXSOUND_SFXCTL_SFXCTL_ENGINE_H
#define EAXSOUND_SFXCTL_SFXCTL_ENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct SFXCTL_Shifting;
struct SFXCTL_AccelTrans;
struct SFXCTL_3DCarPos;
struct MCountdownDone;
class MNotifyVehicleDestroyed;

struct SFXCTL_Engine : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_Shifting *m_pShiftCtl;                // offset 0x28, size 0x4
    SFXCTL_AccelTrans *m_pAccelTransitionCtl;    // offset 0x2C, size 0x4
    SFXCTL_Physics *m_pPhysicsCtl;               // offset 0x30, size 0x4
    SFXCTL_3DCarPos *m_p3DCarPosCtl;             // offset 0x34, size 0x4
    bVector3 vCarPos;                            // offset 0x38, size 0x10
    Average Trq;                                 // offset 0x48, size 0x28
    Average Rpm;                                 // offset 0x70, size 0x28
    Average VisRpmAvg;                           // offset 0x98, size 0x28
    float tMergeWithPhysicsOffStart;             // offset 0xC0, size 0x4
    bool bPreRace;                               // offset 0xC4, size 0x1
    int m_iEngineVol;                            // offset 0xC8, size 0x4
    bool bIsRedlining;                           // offset 0xCC, size 0x1
    bool bWasRedlining;                          // offset 0xD0, size 0x1
    cInterpLine RedLineSampFactor;               // offset 0xD4, size 0x1C
    cInterpLine RedLineEngFactor;                // offset 0xF0, size 0x1C
    bool bRedliningBounce;                       // offset 0x10C, size 0x1
    float RedlineingVisualOffset;                // offset 0x110, size 0x4
    float m_fEng_RPM;                            // offset 0x114, size 0x4
    float m_fPrevRPM;                            // offset 0x118, size 0x4
    float m_fSmoothedEng_RPM;                    // offset 0x11C, size 0x4
    float m_fEng_Trq;                            // offset 0x120, size 0x4
    float m_fSmoothedEng_Trq;                    // offset 0x124, size 0x4
    int m_Rotation;                              // offset 0x128, size 0x4
    bool m_bIsEngineBlown;                       // offset 0x12C, size 0x1
    int m_DistanceFltr;                          // offset 0x130, size 0x4
    bool bClutchStateOn;                         // offset 0x134, size 0x1
    float m_VOL_LFO;                             // offset 0x138, size 0x4
    float m_RPM_LFO;                             // offset 0x13C, size 0x4
    float m_TRQ_LFO;                             // offset 0x140, size 0x4
    unsigned short m_aglRPM_LFO;                 // offset 0x144, size 0x2
    unsigned short m_aglTRQ_LFO;                 // offset 0x146, size 0x2
    unsigned short m_aglVOL_LFO;                 // offset 0x148, size 0x2
    bool bPlayCompression;                       // offset 0x14C, size 0x1
    cPathLine m_ComppressionRPM;                 // offset 0x150, size 0x8C
    HHANDLER mmsgMVehicleDestroyed;              // offset 0x1DC, size 0x4
    HHANDLER mmsgMCoundown;                      // offset 0x1E0, size 0x4

    SFXCTL_Engine();
    ~SFXCTL_Engine() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void InitSFX() override;
    void UpdateParams(float t) override;
    void MessageVehicleDestroyed(const MNotifyVehicleDestroyed &message);
    void UpdateFilterFX();
    void UpdateCompression(float t);
    virtual void UpdateRPM(float t);
    virtual void UpdateTorque(float t);
    virtual void UpdateVolume(float t);
    virtual void UpdateRedlining(float t);

    virtual float GetEngRPM() { return m_fEng_RPM; }
    virtual float GetSmoothedEngRPM() { return m_fSmoothedEng_RPM; }
    virtual float GetEngTorque() { return m_fEng_Trq; }
    virtual float GetSmoothedEngTorque() { return m_fSmoothedEng_Trq; }
    virtual void UpdateEngineLFO_FX(float t);
    void SetEngTorque(float _torque);
    void SetEngRPM(float _RPM) { m_fEng_RPM = _RPM; }

    void MsgCountdownDone(const MCountdownDone &message);
    void UpdateClutchState();
    bool ShouldTurnOnClutch();
    void SetupSFX(CSTATE_Base *_StateBase) override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *) override;
};

#endif
