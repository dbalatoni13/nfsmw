#ifndef EAXSOUND_CARSFX_CARSFX_BOTTOMOUT_H
#define EAXSOUND_CARSFX_CARSFX_BOTTOMOUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

struct cStichWrapper;

class CARSFX_BottomOut : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    CARSFX_BottomOut();
    ~CARSFX_BottomOut() override;

    static SndBase *CreateObject(unsigned int allocator);
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    void InitSFX() override;
    void Destroy() override;
    void Detach() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    void LandJumpPlay(float Intensity, bool HardLanding);
    void BottomOutPlay(unsigned int Intensity);

    cStichWrapper *m_pBottomOut;      // offset 0x28, size 0x4
    cStichWrapper *m_pStichLandJump[3]; // offset 0x2C, size 0xC
    float m_Intesity[3];              // offset 0x38, size 0xC
    cStichWrapper *m_pJumpCamCrash;   // offset 0x44, size 0x4
    bool FrontWheelsTouched;          // offset 0x48, size 0x1
    float FrontHangTime;              // offset 0x4C, size 0x4
    bool RearWheelsTouched;           // offset 0x50, size 0x1
    float RearHangTime;               // offset 0x54, size 0x4
    bool RightWheelsTouched;          // offset 0x58, size 0x1
    float RightHangTime;              // offset 0x5C, size 0x4
    bool LeftWheelsTouched;           // offset 0x60, size 0x1
    float LeftHangTime;               // offset 0x64, size 0x4
    bool IsCarLeaningHeavily;         // offset 0x68, size 0x1
};

#endif
