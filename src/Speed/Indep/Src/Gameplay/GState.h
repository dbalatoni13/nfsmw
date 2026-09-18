#ifndef GAMEPLAY_GSTATE_H
#define GAMEPLAY_GSTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

// total size: 0x28
class GState : public GRuntimeInstance {
  public:
    GState(const unsigned int &stateKey);
    ~GState() override;

    GameplayObjType GetType() const override {
        return kGameplayObjType_State;
    }

    bool IsTerminalState() const {
        return bStrCmp(Name(), "done") == 0;
    }
};

#endif
