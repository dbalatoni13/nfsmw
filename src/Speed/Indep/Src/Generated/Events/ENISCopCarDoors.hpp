#ifndef EVENTS_ENISCOPCARDOORS_H
#define EVENTS_ENISCOPCARDOORS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x18
class ENISCopCarDoors : public Event {
  public:
    // total size: 0x14
    struct StaticData : public Event::StaticData {
        int fCarDoor;             // offset: 0x4, size 0x4
        float fCarDoorPos;        // offset: 0x8, size 0x4
        float fCarDoorAnimLength; // offset: 0xc, size 0x4
        float fCarDoorEndPos;     // offset: 0x10, size 0x4
    };

    // enum { kEventID = 0 };

    ENISCopCarDoors(int pCarDoor, float pCarDoorPos, float pCarDoorAnimLength, float pCarDoorEndPos);

    ~ENISCopCarDoors() override;

    const char *GetEventName() override;

  private:
    int fCarDoor;             // offset: 0x8, size 0x4
    float fCarDoorPos;        // offset: 0xc, size 0x4
    float fCarDoorAnimLength; // offset: 0x10, size 0x4
    float fCarDoorEndPos;     // offset: 0x14, size 0x4
};

void ENISCopCarDoors_MakeEvent_Callback(const void *staticData);

#endif
