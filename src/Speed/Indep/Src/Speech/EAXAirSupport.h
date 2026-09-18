#ifndef SPEECH_EAXAIRSPUPPORT_H
#define SPEECH_EAXAIRSPUPPORT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EAXCop.h"

// total size: 0x84
struct EAXAirSupport : public EAXCop {
    Csis::Type_heli_bailout_type GetCauseOfBailout();
    EAXAirSupport(); // TODO remove ig
    EAXAirSupport(int speakerID, HSIMABLE handle) : EAXCop(speakerID, handle, 64, 1) {}

    // Virtual overrides
    //  AudioMemBase
    ~EAXAirSupport() override;

    // EAXCop
    void BullhornArrest() override;
    bool IsHeli() override { return true; }

    // EAXCharacter
    void Update() override;

    //  EAXCop
    void SelfStrategy(int type) override;
    void LostVisual() override;
    void Bailout() override;
    void Spotter() override;

    // Virtual functions
    virtual void JoinRB();
    virtual void IntentToBail();
    virtual void Swarming();
    virtual void HazardAlert(Csis::Type_heli_hazard_alert_type type);
    virtual void Quadrant();
    virtual void QuadrantMoving();
};

#endif
