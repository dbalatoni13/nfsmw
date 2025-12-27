#ifndef MAIN_COMMON_EVENT_H
#define MAIN_COMMON_EVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

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

    static struct Message *EmbedField(Event *event, struct Message *ptr);

    // static inline Event *GetCurrentEvent() {}

  private:
    static Event *fgCurrentEvent;  // size: 0x4, address: 0x8041E458
    static bool fgHaltCurrentList; // size: 0x1, address: 0x8041E45C
};

#endif
