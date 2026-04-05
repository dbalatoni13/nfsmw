#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_ENVIRO_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_ENVIRO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

struct WorldObject;

// total size: 0x24
struct CSTATEMGR_Enviro : public CSTATEMGR_Base {
    UTL::Std::list<WorldObject *, _type_list> m_WorldObjects; // offset 0x1C, size 0x8

    CSTATEMGR_Enviro();
};

#endif
