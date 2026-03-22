#include "RawEventChannel.h"

namespace EAGL4Anim {

void RawEventChannel::Eval(float previousTime, float currentTime, int &currentIdx, float &cacheCurrentTime, EventHandler **eventHandlers, void *extraData) {
    int numEvents = GetNumEvents();
    Event *events = GetEvents();
    int i;

    if (previousTime < cacheCurrentTime) {
        i = currentIdx;
        if (i > -1 && previousTime < events[i].triggerTime) {
            float *eventTime = &events[i].triggerTime;

            do {
                i--;
                eventTime -= 4;
                if (i < 0) {
                    break;
                }
            } while (previousTime < *eventTime);
        }
        currentIdx = i + 1;
    }

    i = currentIdx;
    bool validIdx = i < numEvents;

    if (validIdx && events[i].triggerTime <= previousTime) {
        float *eventTime = &events[i].triggerTime;

        do {
            i++;
            validIdx = i < numEvents;
            eventTime += 4;
            if (!validIdx) {
                break;
            }
        } while (*eventTime <= previousTime);
    }

    currentIdx = i;

    if (previousTime == currentTime) {
        if (!validIdx) {
            currentIdx = numEvents - 1;
            i = currentIdx;
        }

        if (i > -1 && currentTime <= events[i].triggerTime) {
            float *eventTime = &events[i].triggerTime;

            do {
                i--;
                eventTime -= 4;
                if (i < 0) {
                    break;
                }
            } while (currentTime <= *eventTime);
        }

        currentIdx = i + 1;
    } else {
        if (previousTime > currentTime) {
            if (validIdx) {
                Event *event = &events[i];
                i = numEvents - i;

                do {
                    EventHandler *eventHandler = eventHandlers[event->eventId];
                    if (eventHandler) {
                        eventHandler->HandleEvent(currentTime, *event, extraData);
                    }
                    i--;
                    event++;
                } while (i != 0);
            }

            currentIdx = 0;
        }

        i = currentIdx;
        validIdx = i < numEvents;
        if (validIdx) {
            Event *event = &events[i];

            do {
                if (currentTime < event->triggerTime) {
                    break;
                }

                EventHandler *eventHandler = eventHandlers[event->eventId];
                if (eventHandler) {
                    eventHandler->HandleEvent(currentTime, *event, extraData);
                }

                i++;
                validIdx = i < numEvents;
                event++;
            } while (validIdx);
        }

        currentIdx = i;
        if (!validIdx) {
            currentIdx = numEvents - 1;
        }
    }

    cacheCurrentTime = currentTime;
}

}; // namespace EAGL4Anim
