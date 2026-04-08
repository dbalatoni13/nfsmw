#ifndef EAXSOUND_CARSFX_CARSFX_SKIDS_H
#define EAXSOUND_CARSFX_CARSFX_SKIDS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

struct FX_SKID;
struct SFXCTL_Wheel;

struct SFXCTL_3DLeftWheelPos : public SFXCTL_3DObjPos {
  public:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    SFXCTL_3DLeftWheelPos() {}
    ~SFXCTL_3DLeftWheelPos() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);
};

struct SFXCTL_3DRightWheelPos : public SFXCTL_3DObjPos {
  public:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    SFXCTL_3DRightWheelPos() {}
    ~SFXCTL_3DRightWheelPos() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);
};

struct CARSFX_Skids : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    CARSFX_Skids();
    ~CARSFX_Skids() override;

    static SndBase *CreateObject(unsigned int allocator);
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    void Destroy() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Detach() override;
    void UpdateMixerOutputs() override;
    void ProcessUpdate() override;
    void SetupLoadData() override;

    FX_SKID *m_pSkidControl;                    // offset 0x28, size 0x4
    int SkidType;                               // offset 0x2C, size 0x4
    int m_SkidAzimuth[2];                       // offset 0x30, size 0x8
    SFXCTL_Wheel *m_pWheelCtl;                  // offset 0x38, size 0x4
    SFXCTL_3DLeftWheelPos *m_pLeftWheelPos;     // offset 0x3C, size 0x4
    SFXCTL_3DRightWheelPos *m_pRightWheelPos;   // offset 0x40, size 0x4
    bool mDataLoaded;                           // offset 0x44, size 0x1
};

struct CARSFX_TrafficSkids : public CARSFX_Skids {
  public:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    CARSFX_TrafficSkids();
    ~CARSFX_TrafficSkids() override;

    static SndBase *CreateObject(unsigned int allocator);

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    void Detach() override;
    int GetController(int Index) override;
};

#endif
