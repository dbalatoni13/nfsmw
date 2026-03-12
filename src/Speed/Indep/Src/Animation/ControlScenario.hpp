#ifndef ANIMATION_CONTROLSCENARIO_H
#define ANIMATION_CONTROLSCENARIO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimEntity_WorldEntity.hpp"

// total size: 0x4
struct IControlScenario {
    virtual ~IControlScenario() {}

    IControlScenario() {}

    virtual void HandleEventMessage(CWorldAnimEntityTree *entity_tree, unsigned int wa_msg, void *data) {}
};

// total size: 0x4
struct GenericNISControlScenario : public IControlScenario {
    ~GenericNISControlScenario() override {}

    GenericNISControlScenario() {}

    void HandleEventMessage(CWorldAnimEntityTree *entity_tree, unsigned int wa_msg, void *data) override;
};

#endif
