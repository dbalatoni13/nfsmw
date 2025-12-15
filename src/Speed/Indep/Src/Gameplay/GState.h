#ifndef GAMEPLAY_GSTATE_H
#define GAMEPLAY_GSTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"

// total size: 0x28
class GState : public GRuntimeInstance {
  public:
    GState(const Attrib::Key &stateKey);
};

#endif
