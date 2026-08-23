#include "spch/spch.h"
#include <stdarg.h>

static int gSPCHPlayStatus = 0;
int gAddEventStatus = 0;

extern int iSPCH_OneChosen(unsigned int inChannel);
extern int iSPCH_PlayChosen(unsigned int inChannel);
extern int SPCH_Choose(unsigned int inChannel);
static int iSPCH_ChooseEvent(unsigned int inChannel);
static SPCHType_EventRuleResult iSPCH_Callback_EventRule(EventSpec *eventSpec);
extern void iSPCH_ClearSentenceChoiceChannel(unsigned int inChannel);
extern int iSPCH_ChooseSingleSentence(int eventIndex);
static void iSPCH_ClearOldEvents(int eventIndex);
extern unsigned int SPCHEXT_gettick();
extern unsigned short iSPCH_Rand(int max, int randHandle);
extern void *iSPCH_MemAlloc(unsigned int numBytes);
extern int gFilterSetting[8];
extern int gPreLoadTicks;

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

static VoxEvent *iSPCH_SearchEventDat(VoxData *eventData, int eventType) {
    int i;
    int numEvents;
    VoxEvent *event;

    i = 0;
    numEvents = eventData->numEvents;
    if (i < numEvents) {
        do {
            event = reinterpret_cast<VoxEvent *>(iSPCH_GetOffset16(reinterpret_cast<unsigned char *>(eventData), reinterpret_cast<unsigned short *>(eventData + 1), i));
            if (event->ID == eventType) {
                return event;
            }
            i++;
        } while (i < numEvents);
    }
    return 0;
}

int iSPCH_FindEventDatInfo(EventSpec *eventSpec, EventDatInfo **datInfo) {
    int i;
    int result;

    result = 0;
    i = 0;
    do {
        if (gEventDats[i].eventDat != 0) {
            if (gEventDats[i].eventDat->datID == eventSpec->eventDatID) {
                if (gEventDats[i].eventDat->projectID == eventSpec->projectID) {
                    *datInfo = &gEventDats[i];
                    result = 1;
                    break;
                }
            }
        }
        i++;
    } while (i <= 7);
    return result;
}

inline int iSPCH_TestEventPriority(EventSpec *eventSpec, VoxEvent *event) {
    EventDatInfo *datInfo;
    unsigned short filterPriority;
    int result;

    result = 0;
    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo) != 0) {
        filterPriority = datInfo->eventDat->eventFilterPriority;
        result = event->priority < filterPriority;
    }
    return result;
}

int iSPCH_GetDatID(EventSpec *eventSpec, unsigned int *datID) {
    EventDatInfo *datInfo;
    int result;

    result = 0;
    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo) != 0) {
        result = 1;
        *datID = datInfo->eventDat->datID;
    }
    return result;
}

int iSPCH_GetGlobalMatchParmsArray(EventSpec *eventSpec, unsigned char **matchParmArray) {
    int result;
    EventDatInfo *datInfo;

    result = 0;
    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo) != 0) {
        result = 1;
        *matchParmArray = iSPCH_GetGlobalMatchParmAddr(datInfo->eventDat);
    }
    return result;
}

int iSPCH_GetFilterLength(EventSpec *eventSpec) {
    EventDatInfo *datInfo;
    int filterLength;

    filterLength = 0;
    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo) != 0) {
        filterLength = datInfo->eventDat->eventFilterLength;
    }
    return filterLength;
}

VoxEvent *iSPCH_FindEvent(EventSpec *eventSpec) {
    EventDatInfo *datInfo;
    VoxEvent *event;

    event = 0;
    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo) != 0) {
        event = iSPCH_SearchEventDat(datInfo->eventDat, eventSpec->eventID);
    }
    return event;
}

int iSPCH_FindEventChannel(EventSpec *eventSpec, unsigned int *channel) {
    EventDatInfo *datInfo;
    int result;

    result = 0;
    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo) != 0) {
        result = 1;
        *channel = datInfo->channel;
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
    i = 0;
    while (i < followData->numEvents) {
        if (followData->ID[i] == eventType) {
            followValid = 1;
            break;
        }
        i++;
    }
    return followValid;
}

void iSPCH_ClearEvent(int eventIndex) {
    int channel;

    if (static_cast<unsigned int>(eventIndex) > 0xF) {
        goto abort;
    }
    if (gVoxEvents.events[eventIndex].memParms != 0) {
        iSPCH_MemFree(gVoxEvents.events[eventIndex].memParms);
        gVoxEvents.events[eventIndex].memParms = 0;
    }
    channel = gVoxEvents.events[eventIndex].channel;
    gVoxEvents.events[eventIndex].pending = 0;
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

int SPCH_Play(unsigned int inChannel) {
    int numPhrases;

    numPhrases = 0;
    if (gSPCHPlayStatus != 0) {
        goto abort;
    }
    gSPCHPlayStatus = 1;
    if (iSPCH_OneChosen(inChannel) != 0 || SPCH_Choose(inChannel) != 0) {
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
    if (inChannel >= 8) {
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
        eventSpec = *reinterpret_cast<EventSpec *>(gVoxEvents.events[choice].memParms);
        eventOK = static_cast<SPCHType_EventRuleResult>(iSPCH_Callback_EventRule(&eventSpec));
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
    i = 0;
    do {
        if (i != oldest && gVoxEvents.events[i].pending != 0 && gVoxEvents.events[i].channel == channel) {
            eventTime = gVoxEvents.events[i].entryTime;
            eventSubTick = gVoxEvents.events[i].subTicks;
            if (eventTime < oldestTime || (eventTime == oldestTime && eventSubTick < oldestSubTick)) {
                if (VoxEvent_GetKeepTillExpiresFlag(gVoxEvents.events[i].event) == 0) {
                    iSPCH_ClearEvent(i);
                }
            }
        }
        i++;
    } while (i <= 0xF);
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
    if (inChannel >= 8) {
        goto abort;
    }
    if (iSPCH_CheckLastEventIndex(inChannel) == 0) {
        goto abort;
    }
    choiceIndex = gVoxEvents.lastAddedEvent[inChannel];
    iSPCH_ClearSentenceChoiceChannel(inChannel);
    validSentence = iSPCH_ChooseSingleSentence(choiceIndex);
    iSPCH_ClearEvent(choiceIndex);
    if (validSentence != 0) {
        iSPCH_PlayChosen(inChannel);
    }
abort:
    gSPCHPlayStatus = 0;
}

static int iSPCH_FindEventSlot(unsigned int eventPriority, unsigned int inChannel) {
    int i;
    int choice;
    unsigned int timeNow, expiryTime;
    unsigned int timeSince;
    unsigned int priority;
    unsigned int thisChannel;
    int startEventIndex;

    choice = -1;
    startEventIndex = choice;
    i = 0;
    do {
        if (gVoxEvents.events[i].pending == 0) {
            choice = i;
            goto decided;
        }
        i++;
    } while (i <= 0xF);
    timeNow = SPCHEXT_gettick();
    i = 0;
    do {
        expiryTime = gVoxEvents.events[i].event->expiryTime;
        timeSince = timeNow - gVoxEvents.events[i].entryTime;
        if (expiryTime != 0 && timeSince > expiryTime && i != startEventIndex) {
            choice = i;
            iSPCH_ClearEvent(i);
            goto decided;
        }
        i++;
    } while (i <= 0xF);
    i = 0;
    do {
        thisChannel = gVoxEvents.events[i].channel;
        priority = gVoxEvents.events[i].event->priority;
        if (eventPriority >= priority && thisChannel == inChannel) {
            gVoxEvents.events[i].pending = 0;
            gVoxEvents.numPending[thisChannel]--;
            choice = i;
            goto decided;
        }
        i++;
    } while (i <= 0xF);
decided:
    return choice;
}

static int iSPCH_ChooseEventSearch(SPCHType_FollowData *followData, unsigned int inChannel) {
    int i;
    unsigned int timeNow, timeSince;
    VoxEvent *event;
    EventSpec eventSpec;
    int priority, highestPriority;
    int choice;
    unsigned int choiceTimeSince;
    unsigned short choiceSubTicks;
    int expired, priorityFiltered;
    int followValid;

    timeNow = SPCHEXT_gettick();
    choice = -1;
    timeNow += gPreLoadTicks;
    highestPriority = 0;
    choiceTimeSince = -1;
    choiceSubTicks = 0;
    i = 0;
    do {
        if (gVoxEvents.events[i].pending != 0) {
            if (gVoxEvents.events[i].channel == inChannel) {
                expired = 0;
                event = gVoxEvents.events[i].event;
                priorityFiltered = 0;
                followValid = 1;
                if (followData != 0) {
                    followValid = iSPCH_EventInFollowGroup(event->ID, followData);
                }
                timeSince = timeNow - gVoxEvents.events[i].entryTime;
                if (event->expiryTime != 0 && event->expiryTime < timeSince) {
                    expired = 1;
                }
                if (gFilterSetting[inChannel] == 1 && VoxEvent_GetFilterPriorityFlag(event) != 0) {
                    eventSpec = *reinterpret_cast<EventSpec *>(gVoxEvents.events[i].memParms);
                    priorityFiltered = iSPCH_TestEventPriority(&eventSpec, event);
                }
                if (expired) {
                    iSPCH_ClearEvent(i);
                } else if (followValid != 0 && priorityFiltered == 0) {
                    if (highestPriority < event->priority) {
                        choice = i;
                        choiceTimeSince = timeSince;
                        highestPriority = event->priority;
                        choiceSubTicks = gVoxEvents.events[i].subTicks;
                    } else if (event->priority == highestPriority) {
                        if (timeSince < choiceTimeSince || (timeSince == choiceTimeSince && choiceSubTicks < gVoxEvents.events[i].subTicks)) {
                            choice = i;
                            choiceTimeSince = timeSince;
                            choiceSubTicks = gVoxEvents.events[i].subTicks;
                        }
                    }
                }
            }
        }
        i++;
    } while (i <= 0xF);
    return choice;
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
    if (followFlag != 0) {
        choice = iSPCH_ChooseEventSearch(&followData, inChannel);
    } else {
        choice = iSPCH_ChooseEventSearch(0, inChannel);
    }
    if (followFlag != 0 && choice < 0) {
        choice = iSPCH_ChooseEventSearch(0, inChannel);
    }
    return choice;
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
    eventSpec = *reinterpret_cast<EventSpec *>(parms);
    event = iSPCH_FindEvent(&eventSpec);
    if (event == 0) {
        goto abort;
    }
    if (iSPCH_FindEventChannel(&eventSpec, &channel) == 0) {
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
    gVoxEvents.events[choice].event = event;
    gVoxEvents.events[choice].channel = static_cast<unsigned char>(channel);
    gVoxEvents.events[choice].subTicks = gLastSubTick;
    gVoxEvents.events[choice].entryTime = tick;
    gLastTick = tick;
    if (gVoxEvents.events[choice].memParms != 0) {
        iSPCH_MemFree(gVoxEvents.events[choice].memParms);
    }
    gVoxEvents.events[choice].memParms = parms;
    gVoxEvents.numPending[channel]++;
    gVoxEvents.events[choice].pending = 1;
    gVoxEvents.lastAddedEvent[channel] = choice;
    if (VoxEvent_GetInterruptFlag(event) != 0) {
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
    numBytes = numArgs * 4 + 4;
    parms = reinterpret_cast<unsigned int *>(iSPCH_MemAlloc(numBytes));
    if (parms == 0) {
        goto abort;
    }
    parms[0] = eventID;
    i = 1;
    if (i <= numArgs) {
        do {
            parms[i] = va_arg(parmlist, unsigned int);
            i++;
        } while (i <= numArgs);
    }
    result = iSPCH_AddEvent(parms);
    if (result == 0) {
        iSPCH_MemFree(parms);
    }
abort:
    va_end(parmlist);
    return result;
}
