#ifndef SIM_ACTIVITIES_CAREERGAME_H
#define SIM_ACTIVITIES_CAREERGAME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "QuickGame.h"

// total size: 0xA0
class CareerGame : public QuickGame {
  public:
    static Sim::IActivity *Construct(Sim::Param params);

    CareerGame(Sim::Param params);

    // Virtual functions
    // IUnknown
    ~CareerGame() override;
};

#endif
