#include "EAXAirSupport.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"

namespace Csis {
enum Type_heli_self_strategy_type {
    Type_heli_self_strategy_type_offset_folloe = 1,
    Type_heli_self_strategy_type_heli_roadblock = 2,
    Type_heli_self_strategy_type_ram = 4,
    Type_heli_self_strategy_type_smokescreen = 8,
    Type_heli_self_strategy_type_rolling_spikes = 16,
};

enum Type_heli_lost_visual {
    Type_heli_lost_visual_In_tunnel = 1,
    Type_heli_lost_visual_behind_building = 2,
    Type_heli_lost_visual_Under_trees = 4,
    Type_heli_lost_visual_Generic = 8,
    Type_heli_lost_visual_Restricted_Airspace = 16,
};

struct HeliSpecific_HeliSelfStrategyStruct {
    int speaker_id;
    Type_heli_self_strategy_type heli_self_strategy_type;
};

struct HeliSpecific_HeliLostVisualStruct {
    int speaker_id;
    Type_heli_lost_visual heli_lost_visual;
};

struct HeliSpecific_HeliIntentToBailStruct {
    int speaker_id;
    Type_heli_bailout_type heli_bailout_type;
};

struct HeliSpecific_HeliBailoutStruct {
    int speaker_id;
    Type_heli_bailout_type heli_bailout_type;
};

struct HeliSpecific_HeliSwarmingStruct {
    int speaker_id;
};

struct HeliSpecific_HeliSpotterStruct {
    int speaker_id;
};

struct HeliSpecific_HeliHazardAlertStruct {
    int speaker_id;
    Type_heli_hazard_alert_type heli_hazard_alert_type;
};

struct HeliSpecific_HeliQuadrentStruct {
    int speaker_id;
};

struct HeliSpecific_HeliQuadrentMovingStruct {
    int speaker_id;
};

struct HeliSpecific_HeliBullhornArrestStruct {
    int speaker_id;
};

extern InterfaceId HeliSpecific_HeliSelfStrategyId;
extern InterfaceId HeliSpecific_HeliLostVisualId;
extern InterfaceId HeliSpecific_HeliIntentToBailId;
extern InterfaceId HeliSpecific_HeliBailoutId;
extern InterfaceId HeliSpecific_HeliSwarmingId;
extern InterfaceId HeliSpecific_HeliSpotterId;
extern InterfaceId HeliSpecific_HeliHazardAlertId;
extern InterfaceId HeliSpecific_HeliQuadrentId;
extern InterfaceId HeliSpecific_HeliQuadrentMovingId;
extern InterfaceId HeliSpecific_HeliBullhornArrestId;

extern FunctionHandle gHeliSpecific_HeliSelfStrategyHandle;
extern FunctionHandle gHeliSpecific_HeliLostVisualHandle;
extern FunctionHandle gHeliSpecific_HeliIntentToBailHandle;
extern FunctionHandle gHeliSpecific_HeliBailoutHandle;
extern FunctionHandle gHeliSpecific_HeliSwarmingHandle;
extern FunctionHandle gHeliSpecific_HeliSpotterHandle;
extern FunctionHandle gHeliSpecific_HeliHazardAlertHandle;
extern FunctionHandle gHeliSpecific_HeliQuadrentHandle;
extern FunctionHandle gHeliSpecific_HeliQuadrentMovingHandle;
extern FunctionHandle gHeliSpecific_HeliBullhornArrestHandle;
}

namespace MiscSpeech {
bool IsVehicleTypeOK();
}

extern void ScheduleSpeech_HeliSelfStrategy(Csis::HeliSpecific_HeliSelfStrategyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis35HeliSpecific_HeliSelfStrategyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliLostVisual(Csis::HeliSpecific_HeliLostVisualStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33HeliSpecific_HeliLostVisualStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliIntentToBail(Csis::HeliSpecific_HeliIntentToBailStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis35HeliSpecific_HeliIntentToBailStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliBailout(Csis::HeliSpecific_HeliBailoutStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis30HeliSpecific_HeliBailoutStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliSwarming(Csis::HeliSpecific_HeliSwarmingStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31HeliSpecific_HeliSwarmingStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliSpotter(Csis::HeliSpecific_HeliSpotterStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis30HeliSpecific_HeliSpotterStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliHazardAlert(Csis::HeliSpecific_HeliHazardAlertStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis34HeliSpecific_HeliHazardAlertStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliBullhornArrest(Csis::HeliSpecific_HeliBullhornArrestStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis37HeliSpecific_HeliBullhornArrestStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliQuadrentMoving(Csis::HeliSpecific_HeliQuadrentMovingStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis37HeliSpecific_HeliQuadrentMovingStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_HeliQuadrent(Csis::HeliSpecific_HeliQuadrentStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31HeliSpecific_HeliQuadrentStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");

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
    Csis::HeliSpecific_HeliSelfStrategyStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_self_strategy_type = static_cast<Csis::Type_heli_self_strategy_type>(type);
    ScheduleSpeech_HeliSelfStrategy(data, Csis::HeliSpecific_HeliSelfStrategyId, Csis::gHeliSpecific_HeliSelfStrategyHandle, this);
}

void EAXAirSupport::LostVisual() {
    Csis::HeliSpecific_HeliLostVisualStruct data;
    SoundAI *ai = SoundAI::Get();
    Csis::Type_heli_lost_visual lostVisual = Csis::Type_heli_lost_visual_Generic;
    data.speaker_id = mSpeakerID;
    if (ai) {
        void *observer = *reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x20C);
        if (*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(observer) + 0x58)) {
            lostVisual = Csis::Type_heli_lost_visual_In_tunnel;
        }
    }
    data.heli_lost_visual = lostVisual;
    ScheduleSpeech_HeliLostVisual(data, Csis::HeliSpecific_HeliLostVisualId, Csis::gHeliSpecific_HeliLostVisualHandle, this);
}

void EAXAirSupport::Bailout() {
    Csis::HeliSpecific_HeliBailoutStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_bailout_type = GetCauseOfBailout();
    ScheduleSpeech_HeliBailout(data, Csis::HeliSpecific_HeliBailoutId, Csis::gHeliSpecific_HeliBailoutHandle, this);
}

void EAXAirSupport::Spotter() {
    Csis::HeliSpecific_HeliSpotterStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_HeliSpotter(data, Csis::HeliSpecific_HeliSpotterId, Csis::gHeliSpecific_HeliSpotterHandle, this);
}

void EAXAirSupport::JoinRB() {
    Csis::HeliSpecific_HeliSelfStrategyStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_self_strategy_type = Csis::Type_heli_self_strategy_type_heli_roadblock;
    ScheduleSpeech_HeliSelfStrategy(data, Csis::HeliSpecific_HeliSelfStrategyId, Csis::gHeliSpecific_HeliSelfStrategyHandle, this);
}

void EAXAirSupport::IntentToBail() {
    Csis::HeliSpecific_HeliIntentToBailStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_bailout_type = GetCauseOfBailout();
    ScheduleSpeech_HeliIntentToBail(data, Csis::HeliSpecific_HeliIntentToBailId, Csis::gHeliSpecific_HeliIntentToBailHandle, this);
}

void EAXAirSupport::Swarming() {
    Csis::HeliSpecific_HeliSwarmingStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_HeliSwarming(data, Csis::HeliSpecific_HeliSwarmingId, Csis::gHeliSpecific_HeliSwarmingHandle, this);
}

void EAXAirSupport::HazardAlert(Csis::Type_heli_hazard_alert_type type) {
    Csis::HeliSpecific_HeliHazardAlertStruct data;
    data.speaker_id = mSpeakerID;
    data.heli_hazard_alert_type = type;
    ScheduleSpeech_HeliHazardAlert(data, Csis::HeliSpecific_HeliHazardAlertId, Csis::gHeliSpecific_HeliHazardAlertHandle, this);
}

void EAXAirSupport::Quadrant() {
    Csis::HeliSpecific_HeliQuadrentStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_HeliQuadrent(data, Csis::HeliSpecific_HeliQuadrentId, Csis::gHeliSpecific_HeliQuadrentHandle, this);
}

void EAXAirSupport::QuadrantMoving() {
    Csis::HeliSpecific_HeliQuadrentMovingStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_HeliQuadrentMoving(data, Csis::HeliSpecific_HeliQuadrentMovingId, Csis::gHeliSpecific_HeliQuadrentMovingHandle, this);
}

void EAXAirSupport::BullhornArrest() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        Csis::HeliSpecific_HeliBullhornArrestStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech_HeliBullhornArrest(data, Csis::HeliSpecific_HeliBullhornArrestId, Csis::gHeliSpecific_HeliBullhornArrestHandle, this);
    }
}
