#ifndef MAIN_EVENT_H
#define MAIN_EVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

char *gCreationPoint = nullptr;
char *gDeletionPoint = nullptr;

class Event {
  public:
    virtual ~Event();
    virtual const char *GetEventName() {
        return "";
    }

    void *operator new(size_t size);
    void operator delete(void *ptr);

    struct StaticData {
        unsigned int fEventID;
    };

  private:
    unsigned int fEventSize;
};

#endif
