#ifndef ANIMATION_ANIMINTERNAL_H
#define ANIMATION_ANIMINTERNAL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.h"

EAGL4::DynamicLoader *AnimBridgeNewDynamicLoader(char *name, char *data, int size);
void AnimBridgeDeleteDynamicLoader(EAGL4::DynamicLoader *pDynLoader);

#endif
