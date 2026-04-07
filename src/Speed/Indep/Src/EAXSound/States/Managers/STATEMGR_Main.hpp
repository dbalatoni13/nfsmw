#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_MAIN_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_MAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

// total size: 0x1C
struct CSTATEMGR_Main : public CSTATEMGR_Base {
    CSTATEMGR_Main();
    ~CSTATEMGR_Main() override;

    void EnterWorld(eSndGameMode esgm) override;
};

#endif
