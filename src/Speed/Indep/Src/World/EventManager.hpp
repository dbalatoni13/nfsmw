#ifndef WORLD_EVENTMANAGER_H
#define WORLD_EVENTMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct Car;

// total size: 0x20
struct EventTrigger {
    unsigned int NameHash;      // offset 0x0, size 0x4
    unsigned int EventID;       // offset 0x4, size 0x4
    unsigned int Parameter;     // offset 0x8, size 0x4
    int TrackDirectionMask;     // offset 0xC, size 0x4
    float PositionX;            // offset 0x10, size 0x4
    float PositionY;            // offset 0x14, size 0x4
    float PositionZ;            // offset 0x18, size 0x4
    float Radius;               // offset 0x1C, size 0x4

    unsigned int GetNameHash() {
        return NameHash;
    }

    bVector3 *GetPosition() {
        return reinterpret_cast<bVector3 *>(&PositionX);
    }
};

// total size: 0x24
struct emEvent : public bTNode<emEvent> {
    int ReferenceCount;          // offset 0x8, size 0x4
    unsigned int ID;             // offset 0xC, size 0x4
    EventTrigger *pEventTrigger; // offset 0x10, size 0x4
    Car *CarPtr;                 // offset 0x14, size 0x4
    int Parameter0;              // offset 0x18, size 0x4
    int Parameter1;              // offset 0x1C, size 0x4
    int Parameter2;              // offset 0x20, size 0x4
};

void emEventManagerInit();
void emProcessAllEvents();
emEvent **emTriggerEventsInSection(bVector3 *position, int section_number);

#endif
