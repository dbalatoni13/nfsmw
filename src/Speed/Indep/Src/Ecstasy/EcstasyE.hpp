#pragma once

#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"

int eInitEnginePlat();
void epInitViews();
bool IsRainDisabled();
void eFixUpTablesPlat();
EVIEWMODE eGetCurrentViewMode();
void SetupSceneryCullInfoPlat(struct eView *view /* r29 */, struct SceneryCullInfo &info /* r30 */);
