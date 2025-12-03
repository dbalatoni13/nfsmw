#ifndef ECSTASY_ECSTASY_E_H
#define ECSTASY_ECSTASY_E_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EcstasyData.hpp"

class eView;
class eRenderTarget;

int eInitEnginePlat();
void epInitViews();
bool IsRainDisabled();
void eFixUpTablesPlat();
EVIEWMODE eGetCurrentViewMode();
void SetupSceneryCullInfoPlat(eView *view, struct SceneryCullInfo &info);
void SetParticleSystemStats(int num_particles, int max_num_particles, int num_particle_textures, int max_num_particle_textures, int num_emitters,
                            int max_num_emitters, int num_groups, int max_num_groups);
void eWaitUntilRenderingDone();

void eSetScissor(int xOrig, int yOrig, int wd, int ht);

#endif
