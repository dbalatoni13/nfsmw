#ifndef ANIMATION_GENERICNISCONTROLSCENARIO_H
#define ANIMATION_GENERICNISCONTROLSCENARIO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct GenericNISControlScenarioInfo {
    float pos;              // offset 0x0, size 0x4
    float len;              // offset 0x4, size 0x4
    unsigned int entity_hash; // offset 0x8, size 0x4
};

#endif
