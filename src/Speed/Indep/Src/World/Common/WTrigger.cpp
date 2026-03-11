#include "Speed/Indep/Src/World/WTrigger.h"

#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/World/WCollisionAssets.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

WTrigger::WTrigger() {
    bMemSet(this, 0, sizeof(WTrigger));
}

bool WTrigger::HasEvent(unsigned int eventID, const CARP::EventStaticData** foundEvent) const {
    if (fEvents != nullptr) {
        return EventManager::ListHasEvent(fEvents, eventID, foundEvent);
    }
    return false;
}

bool WTrigger::TestDirection(const UMath::Vector3& vec) const {
    return UMath::Dot(UMath::Vector4To3(fMatRow2Length), vec) >= 0.0f;
}

int LoaderTrigger(bChunk* chunk) {
    return 0;
}

int UnloaderTrigger(bChunk* chunk) {
    return 0;
}

WTriggerManager *WTriggerManager::fgTriggerManager;

WTriggerManager::WTriggerManager()
    : fSilencableEnabled(true) //
    , fIterCount(0) //
    , fgFireOnExitList(nullptr) {
    fgFireOnExitList = new FireOnExitList();
}

WTriggerManager::~WTriggerManager() {
    if (fgFireOnExitList != nullptr) {
        delete fgFireOnExitList;
    }
}

void WTriggerManager::Init() {
    fgTriggerManager = new WTriggerManager();
    Restart();
    for (unsigned int i = 0; i < WCollisionAssets::Get().NumTriggers(); i++) {
        WTrigger &trig = WCollisionAssets::Get().Trigger(i);
        if (trig.fFlags & 0x200) {
            WCollisionAssets::Get().Trigger(i).fFlags &= ~0x400;
        }
    }
}

void WTriggerManager::Restart() {
    fgTriggerManager->fgFireOnExitList->clear();
    Get().EnableSilencables();
}
