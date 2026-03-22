#ifndef WORLD_TRACK_H
#define WORLD_TRACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"

struct TrackOBB;

void EstablishRemoteCaffeineConnection();
int GetNumTrackOBBs();
TrackOBB *GetTrackOBB(int index);
int LoaderTrackOBB(bChunk *chunk);
int UnloaderTrackOBB(bChunk *chunk);

#endif
