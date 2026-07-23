//
#ifndef CARSFX_PRECOLWOOSH_HPP
#define CARSFX_PRECOLWOOSH_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Generated/Messages/MAudioReflection.h"

// total size: 0x64
// Decl: 10
class CARSFX_PreColWoosh : public CARSFX {
  public:
    DECLARE_CREATABLE();

    CARSFX_PreColWoosh();
    ~CARSFX_PreColWoosh() override;

    // Overrides: SndBase
    void InitSFX() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Destroy() override;
    void Detach() override;

    void BailOnWoosh();
    void MsgBarrier(const MAudioReflection &message);
    void MsgBarrierHit(const MAudioReflection &message);

    bool bBarrierDetected; // offset 0x28, size 0x1, Decl: 27
    bool bGoingToCollide;  // offset 0x2C, size 0x1, Decl: 28
    bool bBailOnAll;       // offset 0x30, size 0x1, Decl: 29

    float mDurationActive; // offset 0x34, size 0x4, Decl: 31
    float mResetTime;      // offset 0x38, size 0x4, Decl: 32

    cInterpLine WooshFadeOut; // offset 0x3C, size 0x1C, Decl: 36

    cStichWrapper *m_pWoosh; // offset 0x58, size 0x4, Decl: 39

    Hermes::HHANDLER mMsgBarrier; // offset 0x5C, size 0x4, Decl: 44

    Hermes::HHANDLER mMsgBarrierHit; // offset 0x60, size 0x4, Decl: 47
};

#endif
