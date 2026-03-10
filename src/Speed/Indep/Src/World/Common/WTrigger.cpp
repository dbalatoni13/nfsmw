#include "Speed/Indep/Src/World/WTrigger.h"

#include "Speed/Indep/Src/Main/Event.h"
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
