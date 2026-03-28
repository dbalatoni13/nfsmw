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
    /* 0x28 */ SFXCTL_Shifting *m_pShiftCtl;
    /* 0x2c */ SFXCTL_AccelTrans *m_pAccelTransitionCtl;
    /* 0x30 */ struct SFXCTL_Physics *m_pPhysicsCtl;
    /* 0x34 */ SFXCTL_3DCarPos *m_p3DCarPosCtl;
    /* 0x38 */ bVector3 vCarPos;
    /* 0x48 */ Average Trq;
    /* 0x70 */ Average Rpm;
    /* 0x98 */ Average VisRpmAvg;
    /* 0xc0 */ float tMergeWithPhysicsOffStart;
    /* 0xc4 */ bool bPreRace;
    /* 0xc8 */ int m_iEngineVol;
    /* 0xcc */ bool bIsRedlining;
    /* 0xd0 */ bool bWasRedlining;
    /* 0xd4 */ cInterpLine RedLineSampFactor;
    /* 0xf0 */ cInterpLine RedLineEngFactor;
    /* 0x10c */ bool bRedliningBounce;
    /* 0x110 */ float RedlineingVisualOffset;
    /* 0x114 */ float m_fEng_RPM;
    /* 0x118 */ float m_fPrevRPM;
    /* 0x11c */ float m_fSmoothedEng_RPM;
    /* 0x120 */ float m_fEng_Trq;
    /* 0x124 */ float m_fSmoothedEng_Trq;
    /* 0x128 */ int m_Rotation;
    /* 0x12c */ bool m_bIsEngineBlown;
    /* 0x130 */ int m_DistanceFltr;
    /* 0x134 */ bool bClutchStateOn;
    /* 0x138 */ float m_VOL_LFO;
    /* 0x13c */ float m_RPM_LFO;
    /* 0x140 */ float m_TRQ_LFO;
    /* 0x144 */ unsigned short m_aglRPM_LFO;
    /* 0x146 */ unsigned short m_aglTRQ_LFO;
    /* 0x148 */ unsigned short m_aglVOL_LFO;
    /* 0x14c */ bool bPlayCompression;
    /* 0x150 */ cPathLine m_ComppressionRPM;
    /* 0x1dc */ HHANDLER mmsgMVehicleDestroyed;
    /* 0x1e0 */ HHANDLER mmsgMVehicleDestroyed2;

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
