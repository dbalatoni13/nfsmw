#ifndef WORLD_SCREEN_EFFECTS_H
#define WORLD_SCREEN_EFFECTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

void TickSFX();
void DoTunnelBloom(struct eView *view /* r25 */);
void DoTinting(struct eView *view /* r31 */);
void UpdateAllScreenEFX();
void AccumulationBufferFlushed();
unsigned int QueryFlushAccumulationBuffer();

#endif
