#include "EChangeState.hpp"

#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"

EChangeState::EChangeState(unsigned int pActivityKey, unsigned int pStateKey) : Event(0x10), fActivityKey(pActivityKey), fStateKey(pStateKey) {
}

EChangeState::~EChangeState() {
    GActivity *activity = static_cast<GActivity *>(GManager::Get().FindInstance(fActivityKey));
    GState *state = static_cast<GState *>(GManager::Get().FindInstance(fStateKey));

    if (activity && state) {
        activity->EnterState(state);
    }
}

const char *EChangeState::GetEventName() const {
    return "EChangeState";
}

void EChangeState_MakeEvent_Callback(const void *staticData) {
    new EChangeState(((EChangeState::StaticData *) staticData)->fActivityKey, ((EChangeState::StaticData *) staticData)->fStateKey);
}
