#ifndef ANIMATION_GENERICNISCONTROLSCENARIO_H
#define ANIMATION_GENERICNISCONTROLSCENARIO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"
#include "Speed/Indep/Src/Animation/ControlScenario.hpp"

struct GenericNISControlScenarioInfo {
    float pos;          // offset 0x0, size 0x4
    float len;          // offset 0x4, size 0x4
    uint32 entity_hash; // offset 0x8, size 0x4
};

// total size: 0x4
class GenericNISControlScenario : public IControlScenario {
  public:
    ~GenericNISControlScenario() override {}

    GenericNISControlScenario() {}

    void HandleEventMessage(CWorldAnimEntityTree *entity_tree, uint32 wa_msg, void *data) override;
};

#endif
