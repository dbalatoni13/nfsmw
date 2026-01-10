#ifndef AI_AIPURSUIT_H
#define AI_AIPURSUIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"

bool IsValidPursuitCarName(const char *name);
const char *GetRandomValidCopCar();
Attrib::Gen::pursuitlevels *GetGlobalPursuitLevelAttrib();

#endif
