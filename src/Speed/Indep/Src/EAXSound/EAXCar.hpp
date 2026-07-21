#ifndef EAXCAR_HPP
#define EAXCAR_HPP

#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/acceltrans.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/shiftpattern.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/turbosfx.h"
#include "Speed/Indep/Src/Physics/CarBasics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"

// total size: 0x10
// Decl: 46
struct stCarWorldData {
    bVector3 *pbv3Forward;  // offset 0x0, size 0x4, Decl: 47
    bVector3 *pbv3Position; // offset 0x4, size 0x4, Decl: 48
    float fForwardSpeed;    // offset 0x8, size 0x4, Decl: 50
    int nCarNumber;         // offset 0xC, size 0x4, Decl: 51
};

// Decl: 54
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

// total size: 0x114
// Decl: 71
class EAXCar : public CSTATE_Base {
    DECLARE_STATETYPE();

  public:
    EAXCar();

    // Overrides: AudioMemBase
    ~EAXCar() override;

    // Overrides: CSTATE_Base
    void PreLoadAssets() override {} // Decl: 80
    void Attach(void *pAttachment) override;
    bool Detach() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    virtual void ProcessSoundSphere(uint32 unamehash, int nparamid, bVector3 *pv3pos, float fradius) {} // Decl: 88
    virtual void UpdateCarPhysics() {}                                                                  // Decl: 89

    virtual void StartHonkHorn() {} // Decl: 91
    virtual void StopHonkHorn() {}  // Decl: 92
    virtual bool IsHonking() {}     // Decl: 93

    Attrib::Gen::engineaudio &GetEngineAttributes();

    int m_nHornState; // offset 0x44, size 0x4, Decl: 95

    Attrib::Gen::engineaudio m_FEEngineAttribs; // offset 0x48, size 0x14, Decl: 98

    void SetPhysicsCTLPtr(SFXCTL_Physics *_m_pPhysicsCTL) {} // Decl: 106

    SFXCTL_Physics *GetPhysicsCTL() {} // Decl: 108
    SFXCTL_Physics *m_pPhysicsCTL;     // offset 0x5C, size 0x4, Decl: 109

    float GetPhysTRQ() {
        return this->PhysTRQ;
    } // Decl: 112
    float GetPhysRPM() {
        return this->PhysRPM;
    } // Decl: 113
    bool IsAccelerating() {
        return this->bIsAccelerating;
    } // Decl: 114
    Gear GetCurGear() {
        return CurGear;
    } // Decl: 115
    float GetThrottle() {
        return this->fTrottle;
    } // Decl: 116

    void SetPhysTRQ(float _TRQ) {
        this->PhysTRQ = _TRQ;
    } // Decl: 118
    float PhysTRQ; // offset 0x60, size 0x4, Decl: 119

    void SetPhysRPM(float _RPM) {
        this->PhysRPM = _RPM;
    } // Decl: 121
    float PhysRPM; // offset 0x64, size 0x4, Decl: 122

    void SetIsAccelerating(float _IsAccelerating) {
        this->bIsAccelerating = _IsAccelerating != 0.0f;
    } // Decl: 124
    bool bIsAccelerating; // offset 0x68, size 0x1, Decl: 125

    void SetCurGear(Gear _CurGear) {
        this->CurGear = _CurGear;
    } // Decl: 127
    Gear CurGear; // offset 0x6C, size 0x4, Decl: 128

    void SetThrottle(float _fTrottle) {
        this->fTrottle = _fTrottle;
    } // Decl: 130
    float fTrottle; // offset 0x70, size 0x4, Decl: 131

    void SetFinalAudioRPM(float _RPM) {
        this->m_fAudioRPM = _RPM;
    } // Decl: 133
    float GetFinalAudioRPM() {
        return this->m_fAudioRPM;
    } // Decl: 134
    float m_fAudioRPM; // offset 0x74, size 0x4, Decl: 135

    float t_CurTime;   // offset 0x78, size 0x4, Decl: 137
    float t_DeltaTime; // offset 0x7C, size 0x4, Decl: 138

    bool m_bIsInSoundSphere;   // offset 0x80, size 0x1, Decl: 140
    bVector3 m_v3CurSpherePos; // offset 0x84, size 0x10, Decl: 141
    float m_fSphereRadius;     // offset 0x94, size 0x4, Decl: 142

    DriverInfo *m_pDriverInfo; // offset 0x98, size 0x4, Decl: 144

    eGINSU_ENG_TYPE m_EngineType; // offset 0x9C, size 0x4, Decl: 147

    virtual eAemsUpgradeLevel GetEngineUpgradeLevel() {
        return this->m_EngUGL;
    } // Decl: 149

    eAemsUpgradeLevel m_nTrueEngineUpgradeLevel; // offset 0xA0, size 0x4, Decl: 151

    eAemsUpgradeLevel m_EngUGL;          // offset 0xA4, size 0x4, Decl: 153
    eAemsUpgradeLevel m_TurboUGL;        // offset 0xA8, size 0x4, Decl: 154
    eAemsUpgradeLevel m_NOSUGL;          // offset 0xAC, size 0x4, Decl: 155
    eAemsUpgradeLevel m_TireUGL;         // offset 0xB0, size 0x4, Decl: 156
    eAemsUpgradeLevel m_TransmissionUGL; // offset 0xB4, size 0x4, Decl: 157

    virtual int SFXMessage(eSFXMessageType SFXMessageType, uint32 param1, uint32 param2);

    virtual void UpdatePov();
    int GetPOV() {
        return this->m_PovType;
    } // Decl: 167
    int m_PovType; // offset 0xB8, size 0x4, Decl: 168

    int m_IsDriveCamera; // offset 0xBC, size 0x4, Decl: 170
    bool IsBumperCameraOn();
    bool IsRearCameraOn();
    bool IsHoodCameraOn();

    int GetRotation() {
        return this->m_Rotation;
    } // Decl: 177
    virtual int UpdateRotation();
    int m_Rotation; // offset 0xC0, size 0x4, Decl: 179
    float GetVelocityMagnitudeMPH();

    Attrib::Gen::engineaudio &GetAttributes() {
        return this->mEngineInfo;
    } // Decl: 184
    Attrib::Gen::shiftpattern &GetShiftInfo() {
        return this->mShiftInfo;
    } // Decl: 185
    Attrib::Gen::turbosfx &GetTurboInfo() {
        return this->mTurboInfo;
    } // Decl: 186
    Attrib::Gen::acceltrans &GetAccelInfo() {
        return this->mAccelInfo;
    } // Decl: 187

    Attrib::Gen::engineaudio mEngineInfo; // offset 0xC4, size 0x14, Decl: 193
    Attrib::Gen::shiftpattern mShiftInfo; // offset 0xD8, size 0x14, Decl: 195
    Attrib::Gen::turbosfx mTurboInfo;     // offset 0xEC, size 0x14, Decl: 196
    Attrib::Gen::acceltrans mAccelInfo;   // offset 0x100, size 0x14, Decl: 197

    static Attrib::Gen::shiftpattern *g_ShiftInfo; // size: 0x4, address: 0x80417E2C
    static Attrib::Gen::turbosfx *g_TurboInfo;     // size: 0x4, address: 0x80417E30
};

#endif
