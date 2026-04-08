#ifndef EAXSOUND_CARSFX_SFXOBJ_TRUCKFX_H
#define EAXSOUND_CARSFX_SFXOBJ_TRUCKFX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_TrafficFX.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"

struct FX_TRUCK_FX;

struct SFXObj_TruckFX : public CARSFX {
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

    SFXObj_TruckFX();
    ~SFXObj_TruckFX() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);

    void Destroy() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void InitSFX() override;
    void Detach() override;

    FX_TRUCK_FX *m_pTruckFX; // offset 0x28, size 0x4
    float m_fSpeed;          // offset 0x2C, size 0x4
    bool m_bStopped;         // offset 0x30, size 0x1
};

struct CARSFX_TruckWoosh : public CARSFX_TrafficWoosh {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    CARSFX_TruckWoosh();
    ~CARSFX_TruckWoosh() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);

    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void InitSFX() override;
    bool IsPlayerCarInRadius() override;
    void UpdateParams(float t) override;
    eDRIVE_BY_TYPE GetWooshSample() override;

  private:
    SFXCTL_3DCarPos *m_p3DTrailerPos; // offset 0x30, size 0x4
    bVector3 m_vTrailerPos;           // offset 0x34, size 0x10
    bVector3 m_vTrailerVel;           // offset 0x44, size 0x10
    WorldConn::Reference m_TrailerRef; // offset 0x54, size 0x10
};

struct SFXCTL_3DTrailerPos : public SFXCTL_3DCarPos {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    SFXCTL_3DTrailerPos() {}
    ~SFXCTL_3DTrailerPos() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);
};

#endif
