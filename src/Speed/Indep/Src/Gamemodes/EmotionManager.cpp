// Emotion system: driver emotional state summaries and pursuit responses.
//
// The entire subsystem was dead-stripped from the shipped GOWE69 build; in the
// zGameModes TU only the type/response string tables survive as the first
// global object (see the key of _GLOBAL_.I.aEmotionalSummaryTypeStrings).
//
// r49: los literales SI estan en el DOL, en 0x803EBD60..0x803EBE94 (el rango
// que splits.txt le da hoy a zGameplay).  Los seis disparadores son CamelCase
// alli, no SCREAMING_SNAKE, y NO hay ningun "Created": el DOL solo trae cinco
// fases (Initial, RampUp, Peak, RampDown, Final).  Ver docs/analisis/r49-promo.md.

#include "Speed/Indep/Src/Gamemodes/EmotionManager.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"


const char *aEmotionalSummaryTypeStrings[EmotionalSummary::MaxTypes] = {
    "Embarrassed",
    "Anxious",
    "Smug",
    "Ecstatic",
};

const char *aEmotionalResponseTriggerStrings[EmotionalResponse::MaxTypes] = {
    "PursuitBegins",
    "PursuitAddsCar",
    "PursuitAddsHeli",
    "PursuitAddsRoadblock",
    "PursuitEnds",
    "Arrested",
};

const char *aEmotionalResponsePhaseStrings[EmotionalResponse::MaxPhases] = {
    0, /* r49: el DOL no trae ningun "Created" entre "Arrested" e "Initial" */
    "Initial",
    "RampUp",
    "Peak",
    "RampDown",
    "Final",
};

const char *GetEmotionalSummaryString(EmotionalSummary::eType type) {
    if (type >= EmotionalSummary::Embarrassed && type < EmotionalSummary::MaxTypes) {
        return aEmotionalSummaryTypeStrings[type];
    }
    return 0;
}

const char *GetEmotionalResponseString(EmotionalResponse::eTrigger trigger) {
    if (trigger >= EmotionalResponse::PursuitBegins && trigger < EmotionalResponse::MaxTypes) {
        return aEmotionalResponseTriggerStrings[trigger];
    }
    return 0;
}

const char *GetEmotionalResponsePhaseString(EmotionalResponse::ePhase phase) {
    if (phase >= EmotionalResponse::Created && phase < EmotionalResponse::MaxPhases) {
        return aEmotionalResponsePhaseStrings[phase];
    }
    return 0;
}

EmotionalSummary::EmotionalSummary()
    : mEmotion(Embarrassed), mEmotionTarget(Embarrassed), mEmotionIntensity(0.0f), mEmotionIntensityTarget(0.0f) {}

EmotionalSummary::~EmotionalSummary() {}

EmotionalSummary::eType EmotionalSummary::GetEmotion() const {
    return mEmotion;
}

float EmotionalSummary::GetIntensity() const {
    return mEmotionIntensity;
}

EmotionalResponse::EmotionalResponse(eTrigger trigger)
    : mTrigger(trigger), mPhase(Created), mPeakIntensity(0.0f), mIntensity(0.0f), mAge(0.0f) {}

EmotionalResponse::~EmotionalResponse() {}

EmotionalResponse::eTrigger EmotionalResponse::GetTrigger() const {
    return mTrigger;
}

EmotionalResponse::ePhase EmotionalResponse::GetPhase() const {
    return mPhase;
}

float EmotionalResponse::GetIntensity() const {
    return mIntensity;
}

EmotionManager::EmotionManager() {
    mSummary.mEmotion = EmotionalSummary::Embarrassed;
    mSummary.mEmotionTarget = EmotionalSummary::Embarrassed;
    mSummary.mEmotionIntensity = 0.0f;
    mSummary.mEmotionIntensityTarget = 0.0f;
}

EmotionManager::~EmotionManager() {}

void EmotionManager::Update(Car *car) {}

const EmotionalSummary &EmotionManager::GetSummary() {
    return mSummary;
}

float EmotionManager::GetEmotionalEvaluation(eEmotionalContext context) {
    return 0.0f;
}

void EmotionManager::SendResponseEvent(EmotionalResponse::eTrigger trigger, float intensity) {}
