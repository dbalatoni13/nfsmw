#ifndef WORLD_RAIN_H
#define WORLD_RAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

int AmIinATunnel(eView *view, int CheckOverPass);
int AmIinATunnelSlow(eView *view, int CheckOverPass);
void SetRainBase();

inline float Rain::GetCloudIntensity() {
    return this->CloudIntensity;
}

inline float Rain::GetRainIntensity() {
    return this->intensity;
}

inline void Rain::GetPrecipFogColour(unsigned int *r, unsigned int *g, unsigned int *b) {
    *r = this->fogR;
    *g = this->fogG;
    *b = this->fogB;
}

#endif
