#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static char *gMemoryBuffer = nullptr;
static char *gCreationPoint = nullptr;
static char *gDeletionPoint = nullptr;
static unsigned int gHighWaterMem = 0;

Event *EventManager::fgCurrentEvent = nullptr;
bool EventManager::fgHaltCurrentList = false;

void EventManager::Init() {
    gMemoryBuffer = (char *)bMalloc(0x4000, GetVirtualMemoryAllocParams());
    EventManager::Reset();
}

void EventManager::Reset() {
    gDeletionPoint = gMemoryBuffer;
    gCreationPoint = gMemoryBuffer;
    gHighWaterMem = 0;
}

void EventManager::RunEvents() {
    ProfileNode profile_node("EventManager::RunEvents", 0);

    while (EventManager::EventsQueued()) {
        EventManager::fgCurrentEvent = reinterpret_cast<Event *>(gDeletionPoint);
        if (EventManager::fgCurrentEvent != nullptr) {
            delete EventManager::fgCurrentEvent;
        }

        EventManager::fgCurrentEvent = nullptr;
    }

    gDeletionPoint = gMemoryBuffer;
    gCreationPoint = gMemoryBuffer;
}

void EventManager::FireEventList(const CARP::EventList *eventList, bool verbose) {
    bool haltPrevList = EventManager::fgHaltCurrentList;
    const CARP::EventStaticData *event = eventList->Event();

    EventManager::fgHaltCurrentList = false;

    for (int i = 0; i < eventList->fNumEvents && !EventManager::fgHaltCurrentList; i++, event++) {
        void (*callback)(const void *) = RegisterEvent::LookupEvent(event->fEventID);
        if (callback == nullptr) continue;

        callback(event->StaticData());
    }

    EventManager::fgHaltCurrentList = haltPrevList;
}

void EventManager::FireOneEvent(const CARP::EventList *eventList, unsigned int eventToFire, bool verbose) {
    const CARP::EventStaticData *event = eventList->Event();

    for (int i = 0; i < eventList->fNumEvents; i++, event++) {
        if (i == eventToFire) {
            void (*callback)(const void *) = RegisterEvent::LookupEvent(event->fEventID);
            if (callback == nullptr) break;

            callback(event->StaticData());
            break;
        }
    }
}

bool EventManager::ListHasEvent(const CARP::EventList *eventList, unsigned int eventID, const CARP::EventStaticData **foundEvent) {
    const CARP::EventStaticData *event = eventList->Event();

    for (int i = 0; i < eventList->fNumEvents; i++, event++) {
        if (eventID == event->fEventID) {
            if (foundEvent != nullptr) {
                *foundEvent = event;
            }
            return true;
        }
    }

    return false;
}

bool EventManager::EventsQueued() {
    return gCreationPoint < gDeletionPoint;
}

template<>
unsigned int GetEmbeddedObjectSize<const char>(const char *ptr) {
    return bStrLen(ptr) + 1;
}

template<>
unsigned int GetEmbeddedObjectSize<Hermes::Message>(Hermes::Message *ptr) {
    return ptr->GetSize();
}

void *Event::operator new(size_t size) {
    void *result = gCreationPoint;

    gCreationPoint = &gCreationPoint[(size + 0xF) & ~0xF];

    return result;
}

void Event::operator delete(void *ptr, size_t size) {
    gDeletionPoint += *(unsigned int *)ptr;
}
