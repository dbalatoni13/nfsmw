#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

#define HELI_FUEL_CRITICAL_TIME 8.0f // Decl: 21

EAXAirSupport::~EAXAirSupport() {}

void EAXAirSupport::Update() {
    this->EAXCop::Update();
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr && ai->GetFocus() != 1 && this->IsActive()) {
        ISimable *simable = ISimable::FindInstance(this->GetHandle());
        IAIHelicopter *heli;
        if (simable->QueryInterface(&heli)) {
            if (heli->GetFuelTimeRemaining() < HELI_FUEL_CRITICAL_TIME) {
                this->IntentToBail();
            }
        }
    }
    this->GetHandle();
}

Csis::Type_heli_bailout_type EAXAirSupport::GetCauseOfBailout() {
    Csis::Type_heli_bailout_type rval = Csis::Type_heli_bailout_type_fuel_low;
    ISimable *simable = ISimable::FindInstance(this->GetHandle());
    IAIHelicopter *heli;
    if (simable != nullptr) {
        if (simable->QueryInterface(&heli)) {
            if (heli->GetFuelTimeRemaining() < 8.0f) {
                rval = Csis::Type_heli_bailout_type_fuel_low;
            }
        }
    }
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr) {
        if (ai->GetObserver() != nullptr && ai->GetObserver()->WeatherExists()) {
            rval = Csis::Type_heli_bailout_type_flight_conditions;
        }
    }
    if (this->GetHealth() < 1.0f) {
        rval = Csis::Type_heli_bailout_type_damage_sustained;
    }
    return rval;
}

void EAXAirSupport::SelfStrategy(int type) {
    Csis::HeliSpecific_HeliSelfStrategyStruct data;
    data.speaker_id = this->mSpeakerID;
    data.heli_self_strategy_type = static_cast<Csis::Type_heli_self_strategy_type>(type);
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliSelfStrategyId, Csis::gHeliSpecific_HeliSelfStrategyHandle, this);
}

void EAXAirSupport::JoinRB() {
    Csis::HeliSpecific_HeliSelfStrategyStruct data;
    data.speaker_id = this->mSpeakerID;
    data.heli_self_strategy_type = Csis::Type_heli_self_strategy_type_heli_roadblock;
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliSelfStrategyId, Csis::gHeliSpecific_HeliSelfStrategyHandle, this);
}

void EAXAirSupport::LostVisual() {
    SoundAI *ai = SoundAI::Get();
    Csis::HeliSpecific_HeliLostVisualStruct data;
    data.speaker_id = this->mSpeakerID;
    data.heli_lost_visual =
        ai != nullptr && ai->GetObserver()->PlayerInTunnel() ? Csis::Type_heli_lost_visual_In_tunnel : Csis::Type_heli_lost_visual_Generic;
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliLostVisualId, Csis::gHeliSpecific_HeliLostVisualHandle, this);
}

void EAXAirSupport::IntentToBail() {
    Csis::HeliSpecific_HeliIntentToBailStruct data;
    data.speaker_id = this->mSpeakerID;
    data.heli_bailout_type = this->GetCauseOfBailout();
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliIntentToBailId, Csis::gHeliSpecific_HeliIntentToBailHandle, this);
}

void EAXAirSupport::Bailout() {
    Csis::HeliSpecific_HeliBailoutStruct data;
    data.speaker_id = this->mSpeakerID;
    data.heli_bailout_type = this->GetCauseOfBailout();
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliBailoutId, Csis::gHeliSpecific_HeliBailoutHandle, this);
}

void EAXAirSupport::Swarming() {
    Csis::HeliSpecific_HeliSwarmingStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliSwarmingId, Csis::gHeliSpecific_HeliSwarmingHandle, this);
}

void EAXAirSupport::Spotter() {
    Csis::HeliSpecific_HeliSpotterStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliSpotterId, Csis::gHeliSpecific_HeliSpotterHandle, this);
}

void EAXAirSupport::HazardAlert(Csis::Type_heli_hazard_alert_type type) {
    Csis::HeliSpecific_HeliHazardAlertStruct data;
    data.speaker_id = this->mSpeakerID;
    data.heli_hazard_alert_type = type;
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliHazardAlertId, Csis::gHeliSpecific_HeliHazardAlertHandle, this);
}

void EAXAirSupport::BullhornArrest() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        Csis::HeliSpecific_HeliBullhornArrestStruct data;
        data.speaker_id = this->mSpeakerID;
        Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliBullhornArrestId, Csis::gHeliSpecific_HeliBullhornArrestHandle, this);
    }
}

void EAXAirSupport::QuadrantMoving() {
    Csis::HeliSpecific_HeliQuadrentMovingStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliQuadrentMovingId, Csis::gHeliSpecific_HeliQuadrentMovingHandle, this);
}

void EAXAirSupport::Quadrant() {
    Csis::HeliSpecific_HeliQuadrentStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::HeliSpecific_HeliQuadrentId, Csis::gHeliSpecific_HeliQuadrentHandle, this);
}
