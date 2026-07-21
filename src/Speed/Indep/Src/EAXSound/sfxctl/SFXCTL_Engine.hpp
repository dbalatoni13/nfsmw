//
#ifndef SFXCTL_ENGINE_H
#define SFXCTL_ENGINE_H

#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/Generated/Messages/MCountdownDone.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyVehicleDestroyed.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

class SFXCTL_Shifting;
class SFXCTL_AccelTrans;
class SFXCTL_Physics;
class SFXCTL_3DCarPos;

// total size: 0x1E4
// Decl: 17
class SFXCTL_Engine : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_Engine();
    ~SFXCTL_Engine() override;

    // Overrides: SndBase
    void UpdateParams(float t) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;

    SFXCTL_Shifting *m_pShiftCtl;             // offset 0x28, size 0x4, Decl: 36
    SFXCTL_AccelTrans *m_pAccelTransitionCtl; // offset 0x2C, size 0x4, Decl: 37
    SFXCTL_Physics *m_pPhysicsCtl;            // offset 0x30, size 0x4, Decl: 38
    SFXCTL_3DCarPos *m_p3DCarPosCtl;          // offset 0x34, size 0x4, Decl: 39

    bool IsActive() {} // Decl: 43

    virtual void UpdateRPM(float t);
    virtual void UpdateTorque(float t);
    virtual void UpdateVolume(float t);
    virtual void UpdateRedlining(float t);

    bVector3 vCarPos;                // offset 0x38, size 0x10, Decl: 47
    Average Trq;                     // offset 0x48, size 0x28, Decl: 53
    Average Rpm;                     // offset 0x70, size 0x28, Decl: 54
    Average VisRpmAvg;               // offset 0x98, size 0x28, Decl: 55
    float tMergeWithPhysicsOffStart; // offset 0xC0, size 0x4, Decl: 56
    bool bPreRace;                   // offset 0xC4, size 0x1, Decl: 57
    int m_iEngineVol;                // offset 0xC8, size 0x4, Decl: 68
    bool bIsRedlining;               // offset 0xCC, size 0x1, Decl: 70
    bool bWasRedlining;              // offset 0xD0, size 0x1, Decl: 71
    cInterpLine RedLineSampFactor;   // offset 0xD4, size 0x1C, Decl: 72
    cInterpLine RedLineEngFactor;    // offset 0xF0, size 0x1C, Decl: 73
    bool bRedliningBounce;           // offset 0x10C, size 0x1, Decl: 74
    float RedlineingVisualOffset;    // offset 0x110, size 0x4, Decl: 75

    void SetEngRPM(float _RPM) {
        this->m_fEng_RPM = _RPM;
    } // Decl: 78
    virtual float GetEngRPM() {
        return this->m_fEng_RPM;
    } // Decl: 79
    float m_fEng_RPM;         // offset 0x114, size 0x4, Decl: 80
    float m_fPrevRPM;         // offset 0x118, size 0x4, Decl: 81
    float m_fSmoothedEng_RPM; // offset 0x11C, size 0x4, Decl: 82
    virtual float GetSmoothedEngRPM() {
        return this->m_fSmoothedEng_RPM;
    } // Decl: 83

    void SetEngTorque(float _torque) {
        _torque += this->m_TRQ_LFO;
        this->m_fEng_Trq = _torque;
        this->m_fSmoothedEng_Trq = this->m_fSmoothedEng_Trq * 0.95f + _torque * 0.05f;
    } // Decl: 86
    virtual float GetEngTorque() {
        return this->m_fEng_Trq;
    } // Decl: 87
    float m_fEng_Trq;         // offset 0x120, size 0x4, Decl: 88
    float m_fSmoothedEng_Trq; // offset 0x124, size 0x4, Decl: 89
    virtual float GetSmoothedEngTorque() {
        return this->m_fSmoothedEng_Trq;
    } // Decl: 90

    int m_Rotation; // offset 0x128, size 0x4, Decl: 92

    bool m_bIsEngineBlown; // offset 0x12C, size 0x1, Decl: 94

    int m_DistanceFltr; // offset 0x130, size 0x4, Decl: 96

    void UpdateFilterFX();
    void UpdateClutchState();
    bool ShouldTurnOnClutch();

    virtual void UpdateEngineLFO_FX(float t);
    void UpdateCompression(float t);

    void MessageVehicleDestroyed(const MNotifyVehicleDestroyed &message);

    void MsgCountdownDone(const MCountdownDone &message);

    bool bClutchStateOn;                    // offset 0x134, size 0x1, Decl: 101
    float m_VOL_LFO;                        // offset 0x138, size 0x4, Decl: 110
    float m_RPM_LFO;                        // offset 0x13C, size 0x4, Decl: 111
    float m_TRQ_LFO;                        // offset 0x140, size 0x4, Decl: 112
    bAngle m_aglRPM_LFO;                    // offset 0x144, size 0x2, Decl: 114
    bAngle m_aglTRQ_LFO;                    // offset 0x146, size 0x2, Decl: 115
    bAngle m_aglVOL_LFO;                    // offset 0x148, size 0x2, Decl: 116
    bool bPlayCompression;                  // offset 0x14C, size 0x1, Decl: 125
    cPathLine m_ComppressionRPM;            // offset 0x150, size 0x8C, Decl: 126
    Hermes::HHANDLER mmsgMVehicleDestroyed; // offset 0x1DC, size 0x4, Decl: 129
    Hermes::HHANDLER mmsgMCoundown;         // offset 0x1E0, size 0x4, Decl: 132
};

#endif
