#include "RawEventChannel.h"

namespace EAGL4Anim {

void RawEventChannel::Eval(float previousTime, float currentTime, int &currentIdx, float &cacheCurrentTime, EventHandler **eventHandlers, void *extraData) {
    int numEvents = GetNumEvents();
    Event *events = GetEvents();
    EventHandler *eh;
    int i;

    if (previousTime < cacheCurrentTime) {
        i = currentIdx;

        while (0 <= i && events[i].triggerTime > previousTime) {
            i--;
        }
        currentIdx = i + 1;
    }

    i = currentIdx;

    while (i < numEvents && events[i].triggerTime <= previousTime) {
        i++;
    }

    if (previousTime == currentTime) {
        if (i >= numEvents) {
            currentIdx = numEvents - 1;
        }

        i = currentIdx;

        while (0 <= i && events[i].triggerTime >= currentTime) {
            i--;
        }
        i++;
    } else {
        if (currentTime < previousTime) {
            for (; i < numEvents; i++) {
                eh = eventHandlers[events[i].eventId];

                if (eh) {
                    eh->HandleEvent(currentTime, events[i], extraData);
                }
            }
            i = 0;
        }

        currentIdx = i;

        for (i = currentIdx; i < numEvents && events[i].triggerTime <= currentTime; i++) {
            eh = eventHandlers[events[i].eventId];
            if (eh) {
                eh->HandleEvent(currentTime, events[i], extraData);
            }
        }
    }

    currentIdx = i;
    if (i >= numEvents) {
        currentIdx = numEvents - 1;
    }

    cacheCurrentTime = currentTime;
}

}; // namespace EAGL4Anim
