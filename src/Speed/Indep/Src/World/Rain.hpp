#ifndef WORLD_RAIN_H
#define WORLD_RAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

int AmIinATunnel(eView *view, int CheckOverPass);
int AmIinATunnelSlow(eView *view, int CheckOverPass);

#endif
