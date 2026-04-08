#ifndef EAXSOUND_CARSFX_CARSFX_PRECOLWOOSH_H
#define EAXSOUND_CARSFX_CARSFX_PRECOLWOOSH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/Generated/Messages/MAudioReflection.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x64
struct CARSFX_PreColWoosh : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    bool bBarrierDetected;        // offset 0x28, size 0x1
    bool bGoingToCollide;         // offset 0x2C, size 0x1
    bool bBailOnAll;              // offset 0x30, size 0x1
    float mDurationActive;        // offset 0x34, size 0x4
    float mResetTime;             // offset 0x38, size 0x4
    cInterpLine WooshFadeOut;     // offset 0x3C, size 0x1C
    cStichWrapper *m_pWoosh;      // offset 0x58, size 0x4
    Hermes::HHANDLER mMsgBarrier; // offset 0x5C, size 0x4
    Hermes::HHANDLER mMsgBarrierHit; // offset 0x60, size 0x4

    CARSFX_PreColWoosh();
    ~CARSFX_PreColWoosh() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);

    void InitSFX() override;
    void Destroy() override;
    void MsgBarrier(const MAudioReflection &message);
    void MsgBarrierHit(const MAudioReflection &message);
    void BailOnWoosh();
    void Detach() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
};

#endif
