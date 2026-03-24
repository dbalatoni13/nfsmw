#ifndef WORLD_SCREEN_EFFECTS_H
#define WORLD_SCREEN_EFFECTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

inline float ScreenEffectDB::GetIntensity(ScreenEffectType type) {
    return SE_data[type].intensity;
}

inline float ScreenEffectDB::GetDATA(ScreenEffectType type, int index) {
    return SE_data[type].data[index];
}

inline void ScreenEffectDB::SetDATA(ScreenEffectType type, float data, int index) {
    SE_data[type].data[index] = data;
}

void TickSFX();
void DoTunnelBloom(struct eView *view /* r25 */);
void DoTinting(struct eView *view /* r31 */);
void UpdateAllScreenEFX();
void FlushAccumulationBuffer();
void AccumulationBufferFlushed();
unsigned int QueryFlushAccumulationBuffer();

#endif
