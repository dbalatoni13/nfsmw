#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEReplay.hpp"

void ICEReplay::ClearRecentlyUsed() {

    ICEReplay::nRecentlyUsedIndex = 0;

    for (int i = 0; i < 3; i++) {

        ICEReplay::RecentlyUsedTracks[i] = 0;
    }
    return;
}
