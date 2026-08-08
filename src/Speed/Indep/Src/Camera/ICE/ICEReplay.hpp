#ifndef CAMERA_ICE_ICEREPLAY_H
#define CAMERA_ICE_ICEREPLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"

namespace ICEReplay {
static int nRecentlyUsedIndex;
static ICETrack RecentlyUsedTracks[3];

void ClearRecentlyUsed(void)

{

    ICEReplay::nRecentlyUsedIndex = 0;

    for (int i = 0; i < 3; i++) {

        ICEReplay::RecentlyUsedTracks[i].Next = nullptr; // nullptr?
    }
    return;
}
} // namespace ICEReplay
#endif
