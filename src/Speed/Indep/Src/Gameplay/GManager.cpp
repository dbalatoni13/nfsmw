#include "Speed/Indep/Src/Gameplay/GManager.h"

void GManager::StartBinActivity(GRaceBin *raceBin) {
    GActivity *activity;

    if (!raceBin) {
        return;
    }

    activity = static_cast<GActivity *>(FindInstance(raceBin->GetCollectionKey()));
    if (activity && !activity->GetIsRunning()) {
        activity->Run();
    }
}
