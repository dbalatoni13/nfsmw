#include "RawEventChannel.h"

namespace EAGL4Anim {

void RawEventChannel::Eval(float previousTime, float currentTime, int &currentIdx, float &cacheCurrentTime, EventHandler **eventHandlers, void *extraData) {
    int numEvents = GetNumEvents();
    Event *events = GetEvents();
    EventHandler *eh;
    int i;

    if (previousTime < cacheCurrentTime) {
        i = currentIdx;

        while (i > -1 && previousTime < events[i].triggerTime) {
            i--;
        }
        currentIdx = i + 1;
    }

    i = currentIdx;

    while (i < numEvents && events[i].triggerTime <= previousTime) {
        i++;
    }

    currentIdx = i;

    if (previousTime == currentTime) {
        if (numEvents <= i) {
            currentIdx = numEvents - 1;
        }

        i = currentIdx;

        while (i > -1 && currentTime <= events[i].triggerTime) {
            i--;
        }
        i++;
    } else {
        if (previousTime > currentTime) {
            for (; i < numEvents; i++) {
                eh = eventHandlers[events[i].eventId];

                if (eh) {
                    eh->HandleEvent(currentTime, events[i], extraData);
                }
            }
            i = 0;
        }

        currentIdx = i;

        for (i = currentIdx; i < numEvents; i++) {
            if (currentTime < events[i].triggerTime) {
                break;
            }

            eh = eventHandlers[events[i].eventId];
            if (eh) {
                eh->HandleEvent(currentTime, events[i], extraData);
            }
        }
    }

    currentIdx = i;
    if (numEvents <= i) {
        currentIdx = numEvents - 1;
    }

    cacheCurrentTime = currentTime;
}

}; // namespace EAGL4Anim
