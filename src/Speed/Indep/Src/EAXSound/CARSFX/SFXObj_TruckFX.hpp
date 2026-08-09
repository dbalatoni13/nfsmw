//
#ifndef SFXOBJ_TRUCKFX_HPP
#define SFXOBJ_TRUCKFX_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_TrafficFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"

// total size: 0x34
// Decl: 8
class SFXObj_TruckFX : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_TruckFX();
    ~SFXObj_TruckFX() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void Detach() override;

  private:
    Csis::FX_TRUCK_FX *m_pTruckFX; // offset 0x28, size 0x4, Decl: 30
    float m_fSpeed;                // offset 0x2C, size 0x4, Decl: 31
    bool m_bStopped;               // offset 0x30, size 0x1, Decl: 32
};

// total size: 0x64
// Decl: 39
class CARSFX_TruckWoosh : public CARSFX_TrafficWoosh {
  public:
    DECLARE_CREATABLE();
    CARSFX_TruckWoosh();
    ~CARSFX_TruckWoosh() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void InitSFX() override;
    void UpdateParams(float t) override;

    // Overrides: CARSFX_TrafficWoosh
    bool IsPlayerCarInRadius() override;
    eDRIVE_BY_TYPE GetWooshSample() override;

    SFXCTL_3DCarPos *m_p3DTrailerPos;  // offset 0x30, size 0x4, Decl: 53
    bVector3 m_vTrailerPos;            // offset 0x34, size 0x10, Decl: 55
    bVector3 m_vTrailerVel;            // offset 0x44, size 0x10, Decl: 56
    WorldConn::Reference m_TrailerRef; // offset 0x54, size 0x10, Decl: 57
};

// total size: 0x64
// Decl: 63
class SFXCTL_3DTrailerPos : SFXCTL_3DCarPos {
  public:
    DECLARE_CREATABLE();
};

#endif
