#ifndef SPEECH_EAXAIRSPUPPORT_H
#define SPEECH_EAXAIRSPUPPORT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EAXCop.h"

// TODO move
enum Type_heli_hazard_alert_type {
    Type_heli_hazard_alert_type_windy_roads = 1,
    Type_heli_hazard_alert_type_approaching_highway = 2,
    Type_heli_hazard_alert_type_approaching_tunnel = 4,
    Type_heli_hazard_alert_type_approaching_city = 8,
    Type_heli_hazard_alert_type_approaching_airport = 16,
    Type_heli_hazard_alert_type_approaching_blimp = 32,
};

enum Type_heli_bailout_type {
    Type_heli_bailout_type_flight_conditions = 1,
    Type_heli_bailout_type_low_ammo = 2,
    Type_heli_bailout_type_fuel_low = 4,
    Type_heli_bailout_type_damage_sustained = 8,
};

// total size: 0x84
struct EAXAirSupport : public EAXCop {
    Type_heli_bailout_type GetCauseOfBailout();
    EAXAirSupport(); // TODO remove ig
    // EAXAirSupport(int speakerID, HSIMABLE handle) {}

    // Virtual overrides
    //  AudioMemBase
    ~EAXAirSupport() override;

    // EAXCop
    void BullhornArrest() override;
    bool IsHeli() override {}

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
    virtual void HazardAlert(Type_heli_hazard_alert_type type);
    virtual void Quadrant();
    virtual void QuadrantMoving();
};

#endif
