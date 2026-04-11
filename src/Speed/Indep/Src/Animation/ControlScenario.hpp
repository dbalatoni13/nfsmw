#ifndef ANIMATION_CONTROLSCENARIO_H
#define ANIMATION_CONTROLSCENARIO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimEntity_WorldEntity.hpp"

// total size: 0x4
class IControlScenario {
  public:
    virtual ~IControlScenario() {}

    IControlScenario() {}

    virtual void HandleEventMessage(CWorldAnimEntityTree *entity_tree, unsigned int wa_msg, void *data) {}
};

// total size: 0x4
class GenericNISControlScenario : public IControlScenario {
  public:
    ~GenericNISControlScenario() override {}

    GenericNISControlScenario() {}

    void HandleEventMessage(CWorldAnimEntityTree *entity_tree, unsigned int wa_msg, void *data) override;
};

#endif
