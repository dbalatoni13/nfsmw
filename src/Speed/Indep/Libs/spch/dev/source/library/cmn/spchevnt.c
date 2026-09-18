#include "./spchi.h"

#include <stdarg.h>

// total size: 0x8
struct SPCHType_FollowData {
    int numEvents;        // offset 0x0, size 0x4
    unsigned short *ID;   // offset 0x4, size 0x4
};

// total size: 0x10
struct VoxEventItem {
    unsigned int entryTime;  // offset 0x0, size 0x4
    unsigned short subTicks; // offset 0x4, size 0x2
    unsigned char pending;   // offset 0x6, size 0x1
    unsigned char channel;   // offset 0x7, size 0x1
    VoxEvent *event;         // offset 0x8, size 0x4
    unsigned int *memParms;  // offset 0xC, size 0x4
};

// total size: 0x140
struct VoxPendingEvents {
    int numPending[8];        // offset 0x0, size 0x20
    int lastAddedEvent[8];    // offset 0x20, size 0x20
    VoxEventItem events[16];  // offset 0x40, size 0x100
};

// total size: 0x8
struct VOXINGAME {
    EventSpec lastEventSpec; // offset 0x0, size 0x4
    int numEventTimes;       // offset 0x4, size 0x4
};

// total size: 0x14
struct SPCH_Callbacks {
    int (*request)(SPCHType_SampleRequestData *);       // offset 0x0, size 0x4
    int (*testRule)(EventSpec *, int, int, int);        // offset 0x4, size 0x4
    void (*setRule)(EventSpec *, int, int, int);        // offset 0x8, size 0x4
    SPCHType_EventRuleResult (*eventRule)(EventSpec *); // offset 0xC, size 0x4
    int (*reparm)(int, unsigned int *);                 // offset 0x10, size 0x4
};

extern SPCH_Callbacks gCallbacks;
extern VoxPendingEvents gVoxEvents;
extern VOXINGAME gVoxInGame[8];
extern int gFilterSetting[8];
extern int gPreLoadTicks;
extern unsigned int gLastTick;
extern unsigned short gLastSubTick;

static int gAddEventStatus = 0;
static int gSPCHPlayStatus = 0;

int SPCHEXT_gettick(void);
void *iSPCH_MemAlloc(unsigned int numBytes);
void iSPCH_MemFree(void *data);
int iSPCH_Rand(int range, int seed);
void iSPCH_ClearSentenceChoiceChannel(unsigned int channel);
int iSPCH_ChooseSingleSentence(int eventIndex);
int iSPCH_OneChosen(unsigned int channel);
int iSPCH_PlayChosen(unsigned int channel);

int iSPCH_FindEventDatInfo(EventSpec *eventSpec, EventDatInfo **datInfo);
VoxEvent *iSPCH_FindEvent(EventSpec *eventSpec);
int iSPCH_FindEventChannel(EventSpec *eventSpec, unsigned int *channel);
int iSPCH_AddEvent(unsigned int *parms);
void iSPCH_ClearEvent(int eventIndex);
void SPCH_PlayLastEvent(unsigned int inChannel);
int SPCH_Choose(unsigned int inChannel);

int SPCH_MakeEventSpec(int projID, int datID, int eventID) {
    EventSpec spec;

    spec.projectID = projID;
    spec.eventDatID = datID;
    spec.eventID = eventID;

    return *(int *)&spec;
}

static VoxEvent *iSPCH_SearchEventDat(VoxData *eventData, int eventType) {
    int i;
    int numEvents;
    VoxEvent *event;

    numEvents = eventData->numEvents;
    for (i = 0; i < numEvents; i++) {
        event = (VoxEvent *)iSPCH_GetOffset16((unsigned char *)eventData,
                                              (unsigned short *)(eventData + 1), i);
        if (event->ID == eventType) {
            return event;
        }
    }

    return 0;
}

int iSPCH_FindEventDatInfo(EventSpec *eventSpec, EventDatInfo **datInfo) {
    int i;
    int result = 0;

    for (i = 0; i < 8; i++) {
        if (gEventDats[i].data != 0) {
            if (gEventDats[i].data->datID == eventSpec->eventDatID &&
                gEventDats[i].data->projectID == eventSpec->projectID) {
                *datInfo = &gEventDats[i];
                result = 1;
                break;
            }
        }
    }

    return result;
}

VoxEvent *iSPCH_FindEvent(EventSpec *eventSpec) {
    EventDatInfo *datInfo;
    VoxEvent *event;

    event = 0;
    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo)) {
        event = iSPCH_SearchEventDat(datInfo->data, eventSpec->eventID);
    }

    return event;
}

int iSPCH_FindEventChannel(EventSpec *eventSpec, unsigned int *channel) {
    EventDatInfo *datInfo;
    int result;

    result = 0;

    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo)) {
        *channel = datInfo->channel;
        result = 1;
    }

    return result;
}

int iSPCH_GetDatID(EventSpec *eventSpec, unsigned int *datID) {
    EventDatInfo *datInfo;
    int result;

    result = 0;

    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo)) {
        *datID = datInfo->data->datID;
        result = 1;
    }

    return result;
}

int iSPCH_GetGlobalMatchParmsArray(EventSpec *eventSpec, unsigned char **matchParmArray) {
    int result;
    EventDatInfo *datInfo;

    result = 0;

    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo)) {
        *matchParmArray = iSPCH_GetGlobalMatchParmAddr(datInfo->data);
        result = 1;
    }

    return result;
}

void iSPCH_InitEventDat(void) {
    int i;

    for (i = 0; i < 8; i++) {
        gEventDats[i].data = 0;
        gEventDats[i].channel = 0;
    }
}

int iSPCH_GetFilterLength(EventSpec *eventSpec) {
    EventDatInfo *datInfo;
    int filterLength;

    filterLength = 0;

    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo)) {
        filterLength = datInfo->data->eventFilterLength;
    }

    return filterLength;
}

static inline int iSPCH_TestEventPriority(EventSpec *eventSpec, VoxEvent *event) {
    EventDatInfo *datInfo;
    unsigned short filterPriority;
    int result;

    result = 0;

    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo)) {
        filterPriority = datInfo->data->eventFilterPriority;
        result = (event->priority < filterPriority);
    }

    return result;
}

void iSPCH_InitEventQueue(void) {
    int i;

    for (i = 0; i < 8; i++) {
        gVoxEvents.numPending[i] = 0;
        gVoxEvents.lastAddedEvent[i] = -1;
    }

    for (i = 0; i < 16; i++) {
        gVoxEvents.events[i].pending = 0;
        gVoxEvents.events[i].subTicks = 0;
        gVoxEvents.events[i].entryTime = 0;
        gVoxEvents.events[i].event = 0;

        gVoxEvents.events[i].channel = 0xFF;

        if (gVoxEvents.events[i].memParms != 0) {
            iSPCH_MemFree(gVoxEvents.events[i].memParms);
        }
        gVoxEvents.events[i].memParms = 0;
    }

    gLastTick = 0;
    gLastSubTick = 0;
}

static int iSPCH_FindEventSlot(unsigned int eventPriority, unsigned int inChannel) {
    int i;
    int choice;
    unsigned int timeNow;
    unsigned int expiryTime;
    unsigned int timeSince;
    unsigned int priority;
    unsigned int thisChannel;
    int startEventIndex;

    choice = -1;

    startEventIndex = -1;

    for (i = 0; i < 16; i++) {
        if (gVoxEvents.events[i].pending == 0) {
            choice = i;
            goto decided;
        }
    }

    timeNow = SPCHEXT_gettick();
    for (i = 0; i < 16; i++) {
        expiryTime = gVoxEvents.events[i].event->expiryTime;
        if (expiryTime != 0) {
            timeSince = timeNow - gVoxEvents.events[i].entryTime;

            if (timeSince > expiryTime) {
                if (startEventIndex != i) {
                    iSPCH_ClearEvent(i);
                    choice = i;
                    goto decided;
                }
            }
        }
    }

    for (i = 0; i < 16; i++) {
        priority = gVoxEvents.events[i].event->priority;
        thisChannel = gVoxEvents.events[i].channel;

        if (priority <= eventPriority && thisChannel == inChannel) {
            gVoxEvents.events[i].pending = 0;
            gVoxEvents.numPending[thisChannel]--;
            choice = i;
            goto decided;
        }
    }

decided:
    return choice;
}

static int iSPCH_CheckLastEventIndex(unsigned int channel) {
    int result;

    result = gVoxEvents.lastAddedEvent[channel];

    return (unsigned int)result < 16;
}

int iSPCH_AddEvent(unsigned int *parms) {
    VoxEvent *event;
    unsigned int priority;
    unsigned int tick;
    int choice;
    int frequency;
    int testFreq;
    unsigned int channel;
    int result;
    EventSpec eventSpec;

    result = 0;

    if (gAddEventStatus != 0) {
        goto abort;
    }
    gAddEventStatus = 1;

    if (parms == 0) {
        goto abort;
    }

    *(int *)&eventSpec = parms[0];
    event = iSPCH_FindEvent(&eventSpec);
    if (event == 0) {
        goto abort;
    }

    if (!iSPCH_FindEventChannel(&eventSpec, &channel)) {
        goto abort;
    }

    frequency = event->frequency;
    testFreq = iSPCH_Rand(100, -1);
    if (testFreq > frequency) {
        goto abort;
    }

    priority = event->priority;
    choice = iSPCH_FindEventSlot(priority, channel);

    if (choice < 0) {
        goto abort;
    }

    tick = SPCHEXT_gettick();
    if (tick == gLastTick) {
        gLastSubTick++;
    } else {
        gLastSubTick = 0;
    }
    gLastTick = tick;

    gVoxEvents.events[choice].event = event;
    gVoxEvents.events[choice].channel = channel;
    gVoxEvents.events[choice].subTicks = gLastSubTick;
    gVoxEvents.events[choice].entryTime = tick;

    if (gVoxEvents.events[choice].memParms != 0) {
        iSPCH_MemFree(gVoxEvents.events[choice].memParms);
    }

    gVoxEvents.events[choice].memParms = parms;

    gVoxEvents.numPending[channel]++;

    gVoxEvents.events[choice].pending = 1;

    gVoxEvents.lastAddedEvent[channel] = choice;

    if (VoxEvent_GetInterruptFlag(event)) {
        SPCH_PlayLastEvent(channel);
    }

    result = 1;

abort:
    gAddEventStatus = 0;

    return result;
}

int SPCH_AddEventV(int eventID, int numArgs, ...) {
    int i;
    unsigned int numBytes;
    int result;
    unsigned int *parms;
    va_list parmlist;

    result = 0;

    va_start(parmlist, numArgs);

    numBytes = numArgs * 4;
    parms = (unsigned int *)iSPCH_MemAlloc(numBytes + 4);
    if (parms != 0) {
        parms[0] = eventID;
        for (i = 1; i <= numArgs; i++) {
            parms[i] = va_arg(parmlist, unsigned int);
        }

        result = iSPCH_AddEvent(parms);
        if (result == 0) {
            iSPCH_MemFree(parms);
        }
    }

    return result;
}

static void iSPCH_InitFollowData(SPCHType_FollowData *followData) {
    followData->numEvents = 0;
    followData->ID = 0;
}

static int iSPCH_EventInFollowGroup(unsigned short eventType, SPCHType_FollowData *followData) {
    int followValid;
    int i;

    followValid = 0;

    for (i = 0; i < followData->numEvents; i++) {
        if (followData->ID[i] == eventType) {
            followValid = 1;
            break;
        }
    }

    return followValid;
}

static int iSPCH_ChooseEventSearch(SPCHType_FollowData *followData, unsigned int inChannel) {
    int i;
    unsigned int timeNow;
    unsigned int timeSince;
    VoxEvent *event;
    EventSpec eventSpec;
    int priority;
    int highestPriority;
    int choice;
    unsigned int choiceTimeSince;
    unsigned short choiceSubTicks;
    int expired;
    int priorityFiltered;
    int followValid;

    choice = -1;

    timeNow = SPCHEXT_gettick() + gPreLoadTicks;

    highestPriority = 0;

    choiceTimeSince = -1;
    choiceSubTicks = 0;

    for (i = 0; i < 16; i++) {
        if (gVoxEvents.events[i].pending != 0 && gVoxEvents.events[i].channel == inChannel) {
            expired = 0;
            priorityFiltered = 0;
            followValid = 1;

            event = gVoxEvents.events[i].event;

            if (followData != 0) {
                followValid = iSPCH_EventInFollowGroup(event->ID, followData);
            }

            timeSince = timeNow - gVoxEvents.events[i].entryTime;

            if (event->expiryTime != 0) {
                if (timeSince > event->expiryTime) {
                    expired = 1;
                }
            }

            if (gFilterSetting[inChannel] == 1) {
                if (VoxEvent_GetFilterPriorityFlag(event)) {
                    *(int *)&eventSpec = gVoxEvents.events[i].memParms[0];
                    priorityFiltered = iSPCH_TestEventPriority(&eventSpec, event);
                }
            }

            if (expired) {
                iSPCH_ClearEvent(i);
            } else {
                if (followValid && !priorityFiltered) {
                    priority = event->priority;
                    if (priority > highestPriority) {
                        choice = i;
                        choiceTimeSince = timeSince;
                        choiceSubTicks = gVoxEvents.events[choice].subTicks;
                        highestPriority = priority;
                    } else if (priority == highestPriority) {
                        if (timeSince < choiceTimeSince ||
                            (timeSince == choiceTimeSince &&
                             gVoxEvents.events[i].subTicks > choiceSubTicks)) {
                            choice = i;
                            choiceTimeSince = timeSince;
                            choiceSubTicks = gVoxEvents.events[choice].subTicks;
                        }
                    }
                }
            }
        }
    }

    return choice;
}

static inline int VoxEvent_GetFollowEventData(VoxEvent *event, SPCHType_FollowData *followData) {
    int result;
    unsigned char *ptr;

    result = 0;

    if (VoxEvent_GetFollowGroupFlag(event)) {
        result = 1;

        ptr = iSPCH_GetContextDataAddr(event);
        ptr += (VoxEvent_GetNumContexts(event) * 3 + 3) & ~3;

        if (VoxEvent_GetExactParmMatchFlag(event)) {
            ptr += ExactMatch_NumBytes(event->numSentences);
        }

        followData->numEvents = ptr[0];
        followData->ID = (unsigned short *)(ptr + 2);
    }

    return result;
}

static int iSPCH_ChooseEvent(unsigned int inChannel) {
    VoxEvent *lastEvent;
    int choice;
    SPCHType_FollowData followData;
    int followFlag;

    followFlag = 0;

    iSPCH_InitFollowData(&followData);

    if (gVoxInGame[inChannel].lastEventSpec.eventID != 0xFFFF) {
        lastEvent = iSPCH_FindEvent(&gVoxInGame[inChannel].lastEventSpec);
        if (lastEvent != 0) {
            followFlag = VoxEvent_GetFollowEventData(lastEvent, &followData);
        }
    }

    if (followFlag) {
        choice = iSPCH_ChooseEventSearch(&followData, inChannel);
    } else {
        choice = iSPCH_ChooseEventSearch(0, inChannel);
    }

    if (followFlag && choice < 0) {
        choice = iSPCH_ChooseEventSearch(0, inChannel);
    }

    return choice;
}

void iSPCH_ClearEvent(int eventIndex) {
    int channel;

    if ((unsigned int)eventIndex > 15) {
        goto abort;
    }

    if (gVoxEvents.events[eventIndex].memParms != 0) {
        iSPCH_MemFree(gVoxEvents.events[eventIndex].memParms);
        gVoxEvents.events[eventIndex].memParms = 0;
    }

    gVoxEvents.events[eventIndex].pending = 0;

    channel = gVoxEvents.events[eventIndex].channel;

    if (eventIndex == gVoxEvents.lastAddedEvent[channel]) {
        gVoxEvents.lastAddedEvent[channel] = -1;
    }

    gVoxEvents.numPending[channel]--;

    if (gVoxEvents.numPending[channel] < 0) {
        gVoxEvents.numPending[channel] = 0;
    }

abort:
    return;
}

static void iSPCH_ClearOldEvents(int oldest) {
    int i;
    unsigned int oldestTime;
    unsigned int oldestSubTick;
    unsigned int eventTime;
    unsigned int eventSubTick;
    unsigned int channel;

    oldestTime = gVoxEvents.events[oldest].entryTime;
    oldestSubTick = gVoxEvents.events[oldest].subTicks;
    channel = gVoxEvents.events[oldest].channel;

    for (i = 0; i < 16; i++) {
        if (i != oldest && gVoxEvents.events[i].pending != 0 &&
            gVoxEvents.events[i].channel == channel) {
            eventTime = gVoxEvents.events[i].entryTime;
            eventSubTick = gVoxEvents.events[i].subTicks;

            if (eventTime < oldestTime ||
                (eventTime == oldestTime && eventSubTick < oldestSubTick)) {
                if (!VoxEvent_GetKeepTillExpiresFlag(gVoxEvents.events[i].event)) {
                    iSPCH_ClearEvent(i);
                }
            }
        }
    }
}

static SPCHType_EventRuleResult iSPCH_Callback_EventRule(EventSpec *eventSpec) {
    SPCHType_EventRuleResult result;

    result = kSPCH_EventRule_OK;
    if (gCallbacks.eventRule != 0) {
        result = gCallbacks.eventRule(eventSpec);
    }

    return result;
}

void SPCH_PlayLastEvent(unsigned int inChannel) {
    int choiceIndex;
    int validSentence;

    if (gSPCHPlayStatus != 0) {
        goto abort;
    }
    gSPCHPlayStatus = 1;

    if (inChannel > 7) {
        goto abort;
    }

    if (!iSPCH_CheckLastEventIndex(inChannel)) {
        goto abort;
    }

    choiceIndex = gVoxEvents.lastAddedEvent[inChannel];

    iSPCH_ClearSentenceChoiceChannel(inChannel);

    validSentence = iSPCH_ChooseSingleSentence(choiceIndex);

    iSPCH_ClearEvent(choiceIndex);

    if (validSentence) {
        iSPCH_PlayChosen(inChannel);
    }

abort:
    gSPCHPlayStatus = 0;
}

int SPCH_Play(unsigned int inChannel) {
    int numPhrases;

    numPhrases = 0;

    if (gSPCHPlayStatus != 0) {
        goto abort;
    }
    gSPCHPlayStatus = 1;

    if (iSPCH_OneChosen(inChannel) || SPCH_Choose(inChannel)) {
        numPhrases = iSPCH_PlayChosen(inChannel);
    }

abort:
    gSPCHPlayStatus = 0;

    return numPhrases;
}

int SPCH_Choose(unsigned int inChannel) {
    int choice;
    SPCHType_EventRuleResult eventOK;
    int foundEvent;
    EventSpec eventSpec;

    foundEvent = 0;

    if (inChannel > 7) {
        goto abort;
    }

    if (gVoxEvents.numPending[inChannel] == 0) {
        goto abort;
    }

    do {
        choice = iSPCH_ChooseEvent(inChannel);
        if (choice < 0) {
            goto abort;
        }

        *(int *)&eventSpec = gVoxEvents.events[choice].memParms[0];

        eventOK = iSPCH_Callback_EventRule(&eventSpec);
        if (eventOK == kSPCH_EventRule_OK) {
            iSPCH_ClearSentenceChoiceChannel(inChannel);
            foundEvent = iSPCH_ChooseSingleSentence(choice);
            if (foundEvent != 0) {
                break;
            }
            iSPCH_ClearEvent(choice);
        } else {
            iSPCH_ClearEvent(choice);
        }
    } while (foundEvent == 0);

    iSPCH_ClearOldEvents(choice);
    iSPCH_ClearEvent(choice);

abort:
    return foundEvent;
}
