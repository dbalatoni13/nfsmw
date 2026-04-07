#ifndef EAXSOUND_CARSFX_SFXOBJ_WORLDOBJECT_H
#define EAXSOUND_CARSFX_SFXOBJ_WORLDOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

struct WorldObject;
struct SFXObj_WorldObject : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;
};

// TODO: Fill this header with the full WorldObject and SFXObj_WorldObject declarations once
// ISndAttachable ownership is resolved; the current line-lookup evidence only points at the
// STATEMGR_Enviro include site, not at a recovered canonical owner header.

#endif
