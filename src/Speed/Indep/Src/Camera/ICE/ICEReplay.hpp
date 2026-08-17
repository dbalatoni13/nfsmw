#ifndef CAMERA_ICE_ICEREPLAY_H
#define CAMERA_ICE_ICEREPLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"

namespace ICEReplay {
static int nRecentlyUsedIndex;
static int RecentlyUsedTracks[3];

void ClearRecentlyUsed(void);
} // namespace ICEReplay
#endif
