#ifndef ECSTASY_ECSTASY_E_H
#define ECSTASY_ECSTASY_E_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"

int eInitEnginePlat();
void epInitViews();
bool IsRainDisabled();
void eFixUpTablesPlat();
EVIEWMODE eGetCurrentViewMode();
void SetupSceneryCullInfoPlat(struct eView *view /* r29 */, struct SceneryCullInfo &info /* r30 */);

#endif
