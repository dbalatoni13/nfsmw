#include "EAXAirSupport.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Speech/MiscSpeech.h"
#include "Speed/Indep/Src/Speech/Observer.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

EAXAirSupport::~EAXAirSupport() {}

void EAXAirSupport::Update() {
    EAXCop::Update();
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetFocus() != SoundAI::kPursuitFlow && IsActive()) {
        ISimable *simable = ISimable::FindInstance(GetHandle());
        IAIHelicopter *heli;
        if (simable->QueryInterface(&heli) && heli->GetFuelTimeRemaining() < 8.0f) {
            IntentToBail();
        }
    }
    GetHandle();
}

Csis::Type_heli_bailout_type EAXAirSupport::GetCauseOfBailout() {
    Csis::Type_heli_bailout_type rval = Csis::Type_heli_bailout_type_fuel_low;
    ISimable *simable = ISimable::FindInstance(GetHandle());
    IAIHelicopter *heli;
    if (simable != nullptr && simable->QueryInterface(&heli) && heli->GetFuelTimeRemaining() < 8.0f) {
        rval = Csis::Type_heli_bailout_type_fuel_low;
    }
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetObserver() != nullptr && ai->GetObserver()->WeatherExists()) {
        rval = Csis::Type_heli_bailout_type_flight_conditions;
    }
    if (GetHealth() < 1.0f) {
        rval = Csis::Type_heli_bailout_type_damage_sustained;
    }
    return rval;
}

void EAXAirSupport::SelfStrategy(int type) {
    Csis::HeliSpecific_HeliSelfStrategyStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_self_strategy_type = static_cast<Csis::Type_heli_self_strategy_type>(type);
    SCHEDULE_SPEECH(HeliSpecific_HeliSelfStrategy, data, this);
}

void EAXAirSupport::JoinRB() {
    Csis::HeliSpecific_HeliSelfStrategyStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_self_strategy_type = Csis::Type_heli_self_strategy_type_heli_roadblock;
    SCHEDULE_SPEECH(HeliSpecific_HeliSelfStrategy, data, this);
}

void EAXAirSupport::LostVisual() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::HeliSpecific_HeliLostVisualStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_lost_visual =
        (ai != nullptr && ai->GetObserver()->PlayerInTunnel()) ? Csis::Type_heli_lost_visual_In_tunnel : Csis::Type_heli_lost_visual_Generic;
    SCHEDULE_SPEECH(HeliSpecific_HeliLostVisual, data, this);
}

void EAXAirSupport::IntentToBail() {
    Csis::HeliSpecific_HeliIntentToBailStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_bailout_type = GetCauseOfBailout();
    SCHEDULE_SPEECH(HeliSpecific_HeliIntentToBail, data, this);
}

void EAXAirSupport::Bailout() {
    Csis::HeliSpecific_HeliBailoutStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_bailout_type = GetCauseOfBailout();
    SCHEDULE_SPEECH(HeliSpecific_HeliBailout, data, this);
}

void EAXAirSupport::Swarming() {
    Csis::HeliSpecific_HeliSwarmingStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(HeliSpecific_HeliSwarming, data, this);
}

void EAXAirSupport::Spotter() {
    Csis::HeliSpecific_HeliSpotterStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(HeliSpecific_HeliSpotter, data, this);
}

void EAXAirSupport::HazardAlert(Csis::Type_heli_hazard_alert_type type) {
    Csis::HeliSpecific_HeliHazardAlertStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_hazard_alert_type = type;
    SCHEDULE_SPEECH(HeliSpecific_HeliHazardAlert, data, this);
}

void EAXAirSupport::BullhornArrest() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        Csis::HeliSpecific_HeliBullhornArrestStruct data;
        data.speaker_id = mSpeakerID;
        SCHEDULE_SPEECH(HeliSpecific_HeliBullhornArrest, data, this);
    }
}

void EAXAirSupport::QuadrantMoving() {
    Csis::HeliSpecific_HeliQuadrentMovingStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(HeliSpecific_HeliQuadrentMoving, data, this);
}

void EAXAirSupport::Quadrant() {
    Csis::HeliSpecific_HeliQuadrentStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(HeliSpecific_HeliQuadrent, data, this);
}
