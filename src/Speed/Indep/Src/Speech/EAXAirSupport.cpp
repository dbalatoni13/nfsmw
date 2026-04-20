#include "EAXAirSupport.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"

EAXAirSupport::EAXAirSupport()
    : EAXCop(0, nullptr, 0, 0) {}

EAXAirSupport::~EAXAirSupport() {}

Type_heli_bailout_type EAXAirSupport::GetCauseOfBailout() {
    static const unsigned int kSoundAIObserverOffset = 0x20C;
    static const unsigned int kObserverWeatherOffset = 0x54;
    Type_heli_bailout_type rval = Type_heli_bailout_type_fuel_low;
    ISimable *simable = ISimable::FindInstance(GetHandle());
    if (simable) {
        IAIHelicopter *heli;
        if (simable->QueryInterface(&heli)) {
            if (heli->GetFuelTimeRemaining() < 8.0f) {
                rval = Type_heli_bailout_type_fuel_low;
            }
        }
    }
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        void *observer = *reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + kSoundAIObserverOffset);
        if (observer && *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(observer) + kObserverWeatherOffset)) {
            rval = Type_heli_bailout_type_flight_conditions;
        }
    }
    if (GetHealth() < 1.0f) {
        rval = Type_heli_bailout_type_damage_sustained;
    }
    return rval;
}

void EAXAirSupport::BullhornArrest() {
    EAXCop::BullhornArrest();
}

void EAXAirSupport::Update() {
    static const unsigned int kSoundAIFocusOffset = 0x140;
    EAXCop::Update();
    SoundAI *ai = SoundAI::Get();
    if (ai && *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + kSoundAIFocusOffset) != 1 && IsActive()) {
        ISimable *simable = ISimable::FindInstance(GetHandle());
        IAIHelicopter *heli;
        if (simable->QueryInterface(&heli)) {
            if (heli->GetFuelTimeRemaining() < 8.0f) {
                IntentToBail();
            }
        }
    }
    GetHandle();
}

void EAXAirSupport::SelfStrategy(int type) {
    EAXCop::SelfStrategy(type);
}

void EAXAirSupport::LostVisual() {
    EAXCop::LostVisual();
}

void EAXAirSupport::Bailout() {
    EAXCop::Bailout();
}

void EAXAirSupport::Spotter() {
    EAXCop::Spotter();
}

void EAXAirSupport::JoinRB() {}

void EAXAirSupport::IntentToBail() {}

void EAXAirSupport::Swarming() {}

void EAXAirSupport::HazardAlert(Type_heli_hazard_alert_type type) {
    if (type) {
        SetActive(true);
    }
}

void EAXAirSupport::Quadrant() {}

void EAXAirSupport::QuadrantMoving() {}
