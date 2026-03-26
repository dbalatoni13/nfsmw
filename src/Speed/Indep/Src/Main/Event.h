#ifndef MAIN_EVENT_H
#define MAIN_EVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

#include "Speed/Indep/Src/Misc/Hermes.h"

extern char *gCreationPoint;
extern char *gDeletionPoint;

class Event {
  public:
    struct StaticData {
        unsigned int fEventID;
    };

    void *operator new(size_t size);
    void operator delete(void *ptr, std::size_t size);

    virtual ~Event() {}

    virtual const char *GetEventName();

    Event(std::size_t size) : fEventSize(size) {}

    std::size_t fEventSize;
};

// total size: 0x10
struct EventStaticData {
    unsigned int fEventID;   // offset 0x0, size 0x4
    unsigned int fEventSize; // offset 0x4, size 0x4
    unsigned int fDataOffset; // offset 0x8, size 0x4
    unsigned int fPad;       // offset 0xC, size 0x4
};

// total size: 0x10
struct EventList {
    unsigned int fNumEvents; // offset 0x0, size 0x4
    unsigned int fPad[3];    // offset 0x4, size 0xC
};

// total size: 0x1
class EventManager {
  public:
    static void BindMessagesToLua(struct HandlerVector &handlers);

    static void Init();

    static void Shutdown();

    static void Reset();

    static void RunEvents();

    static void FireEventList(const struct EventList *eventList, bool verbose);

    static void FireOneEvent(const struct EventList *eventList, unsigned int eventToFire, bool verbose);

    static bool ListHasEvent(const struct EventList *eventList, unsigned int eventID, const struct EventStaticData **foundEvent);

    static void AbortCurrentEventList();

    static bool EventsQueued();

    static const char *EmbedField(Event *event, const char *ptr);

    static Hermes::Message *EmbedField(Event *event, Hermes::Message *ptr);

    // static inline Event *GetCurrentEvent() {}

  private:
    static Event *fgCurrentEvent;  // size: 0x4, address: 0x8041E458
    static bool fgHaltCurrentList; // size: 0x1, address: 0x8041E45C
};

#endif
