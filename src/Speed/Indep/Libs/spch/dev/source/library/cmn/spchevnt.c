#include "spch/spch.h"

int SPCH_MakeEventSpec(int projID, int datID, int eventID) {
    EventSpec spec;

    spec.eventDatID = datID;
    spec.eventID = eventID;
    spec.projectID = projID;
    return *reinterpret_cast<int *>(&spec);
}

void iSPCH_InitEventDat() {
    int i;

    i = 0;
    do {
        gEventDats[i].eventDat = 0;
        gEventDats[i].channel = 0;
        i++;
    } while (i <= 7);
}

static int iSPCH_CheckLastEventIndex(unsigned int channel) {
    int result;

    result = static_cast<unsigned int>(gVoxEvents.lastAddedEvent[channel]) <= 0xF;
    return result;
}

void iSPCH_InitEventQueue() {
    int i;

    i = 0;
    do {
        gVoxEvents.numPending[i] = 0;
        gVoxEvents.lastAddedEvent[i] = -1;
        i++;
    } while (i <= 7);

    i = 0;
    do {
        gVoxEvents.events[i].pending = 0;
        gVoxEvents.events[i].subTicks = 0;
        gVoxEvents.events[i].entryTime = 0;
        gVoxEvents.events[i].event = 0;
        gVoxEvents.events[i].channel = 0xFF;
        if (gVoxEvents.events[i].memParms != 0) {
            iSPCH_MemFree(gVoxEvents.events[i].memParms);
        }
        gVoxEvents.events[i].memParms = 0;
        i++;
    } while (i < 0x10);
    gLastTick = 0;
    gLastSubTick = 0;
}
