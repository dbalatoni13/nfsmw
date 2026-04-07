#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_AICAR_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_AICAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"

// total size: 0x24
struct CSTATEMGR_AICar : public CSTATEMGR_CarState {
    static bool bUsingGinsu;

    CSTATEMGR_AICar();
    ~CSTATEMGR_AICar() override;

    void UpdateParams(float t) override;
    void EnterWorld(eSndGameMode esgm) override;
    static void QueueSlots();

  private:
    void DebugDisplayAIConnections();
};

#endif
