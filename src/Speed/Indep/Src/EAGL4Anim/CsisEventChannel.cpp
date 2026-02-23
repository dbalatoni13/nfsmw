#include "CsisEventChannel.h"

namespace EAGL4Anim {

void CsisEventChannel::HandleEvents(float currentTime, EventHandler **eventHandlers, void *extraData, CsisEvent &event) {
    CsisDictionary *csisDict = GetCsisDictionary();

    Event newEvent;
    newEvent.eventId = csisDict->GetEventId(event.mGlobalIndex);
    newEvent.triggerTime = event.triggerTime;
    newEvent.duration = 0.0f;
    newEvent.parameter = 0.0f;

    int numParams = csisDict->GetEventNumParams(event.mGlobalIndex);

    CsisData csisData;
    int eventParams[255];
    csisData.SetInterfaceCRC(csisDict->GetEventInterfaceCRC(event.mGlobalIndex));
    csisData.mUserData = extraData;
    csisData.SetNumParameters(numParams);
    csisData.SetParams(eventParams);

    unsigned short *eventDataIndices = GetCsisDataIndex();
    unsigned char *eventDataBuffer = GetCsisData();
    unsigned char *eventData = &eventDataBuffer[eventDataIndices[event.mLocalIndex]];

    unsigned int bitCount = 0;
    int p;

    for (p = 0; p < numParams; p++) {
        int numBits = csisDict->GetEventParamNumBits(event.mGlobalIndex, static_cast<unsigned short>(p));

        unsigned int paramBitCount = 0;
        unsigned int unpackBuffer = 0;

        do {
            int bitLeft = 8 - bitCount;
            int actualBits = numBits;
            if (numBits > bitLeft) {
                actualBits = bitLeft;
            }
            numBits -= actualBits;

            unsigned int mask = (1 << actualBits) - 1;
            unsigned int bitChunk = (*eventData >> bitCount) & mask;

            bitCount += actualBits;
            unpackBuffer |= bitChunk << paramBitCount;
            paramBitCount += actualBits;

            if (bitCount > 7) {
                bitCount = 0;
                eventData++;
            }
        } while (numBits > 0);

        eventParams[p] = csisDict->GetEventParamStartValue(event.mGlobalIndex, static_cast<unsigned short>(p)) + unpackBuffer;
    }

    EventHandler *eh = eventHandlers[newEvent.eventId];
    if (eh) {
        eh->HandleEvent(currentTime, newEvent, &csisData);
    }
}

void CsisEventChannel::Eval(float previousTime, float currentTime, int &currentIdx, float &cacheCurrentTime, EventHandler **eventHandlers,
                            void *extraData) {
    int numEvents = GetNumEvents();
    CsisEvent *events = GetEvents();
    int i;
    if (previousTime < cacheCurrentTime) {
        for (i = currentIdx; i >= 0; i--) {
            if (events[i].triggerTime <= previousTime) {
                break;
            }
        }
        currentIdx = i + 1;
    }
    for (i = currentIdx; i < numEvents; i++) {
        if (events[i].triggerTime > previousTime) {
            break;
        }
    }
    currentIdx = i;

    if (previousTime == currentTime) {
        if (i >= numEvents) {
            currentIdx = numEvents - 1;
        }
        for (i = currentIdx; i >= 0; i--) {
            if (events[i].triggerTime < currentTime) {
                break;
            }
        }
        currentIdx = i + 1;
    } else if (currentTime < previousTime) {
        for (; i < numEvents; i++) {
            HandleEvents(currentTime, eventHandlers, extraData, events[i]);
        }
        currentIdx = 0;
    }

    for (i = currentIdx; i < numEvents; i++) {
        if (events[i].triggerTime <= currentTime) {
            HandleEvents(currentTime, eventHandlers, extraData, events[i]);
        } else {
            break;
        }
    }

    currentIdx = i;

    if (i >= numEvents) {
        currentIdx = numEvents - 1;
    }

    cacheCurrentTime = currentTime;
}

}; // namespace EAGL4Anim
