#ifndef CARSFX_BOTTOMOUT_HPP
#define CARSFX_BOTTOMOUT_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"

// total size: 0x6C
// Decl: 8
class CARSFX_BottomOut : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_BottomOut();
    ~CARSFX_BottomOut() override;

    // Overrides: SndBase
    void InitSFX() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Destroy() override;
    void Detach() override;

    void BottomOutPlay(uint32 Intensity);
    void LandJumpPlay(float Intensity, bool HardLanding);

    cStichWrapper *m_pBottomOut;        // offset 0x28, size 0x4, Decl: 30
#ifdef EA_BUILD_A124
    cStichWrapper *m_pStichLandJump;    // offset 0x2C, size 0x4
#else
    cStichWrapper *m_pStichLandJump[3]; // offset 0x2C, size 0xC, Decl: 31
    float m_Intesity[3];                // offset 0x38, size 0xC, Decl: 32
#endif

#ifdef EA_BUILD_A124
    cStichWrapper *m_pJumpCamCrash; // offset 0x30, size 0x4
#else
    cStichWrapper *m_pJumpCamCrash; // offset 0x44, size 0x4, Decl: 34
#endif

    bool FrontWheelsTouched; // offset 0x48, size 0x1, Decl: 37
    float FrontHangTime;     // offset 0x4C, size 0x4, Decl: 38

    bool RearWheelsTouched; // offset 0x50, size 0x1, Decl: 40
    float RearHangTime;     // offset 0x54, size 0x4, Decl: 41

    bool RightWheelsTouched; // offset 0x58, size 0x1, Decl: 43
    float RightHangTime;     // offset 0x5C, size 0x4, Decl: 44

    bool LeftWheelsTouched; // offset 0x60, size 0x1, Decl: 46
    float LeftHangTime;     // offset 0x64, size 0x4, Decl: 47

    bool IsCarLeaningHeavily; // offset 0x68, size 0x1, Decl: 50
};

#endif
