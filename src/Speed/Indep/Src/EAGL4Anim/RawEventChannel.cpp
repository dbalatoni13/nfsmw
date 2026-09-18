#include "RawEventChannel.h"
#include "FnRawEventChannel.h"

namespace EAGL4Anim {

void RawEventChannel::Eval(float previousTime, float currentTime, int &currentIdx, float &cacheCurrentTime,
                           EventHandler **eventHandlers, void *extraData) {

    int numEvents = GetNumEvents();
    Event *events = GetEvents();
    int i;

    if (previousTime < cacheCurrentTime) {

        i = currentIdx;

        while (i >= 0 && events[i].triggerTime > previousTime) {
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

        if (i >= numEvents) {
            currentIdx = numEvents - 1;
        }

        i = currentIdx;

        while (i >= 0 && events[i].triggerTime >= currentTime) {
            i--;
        }

        currentIdx = i + 1;

    } else if (currentTime < previousTime) {

        while (i < numEvents) {

            EventHandler *eh = eventHandlers[events[i].eventId];

            if (eh != nullptr) {
                eh->HandleEvent(currentTime, events[i], extraData);
            }

            i++;
        }

        currentIdx = 0;
    }

    i = currentIdx;

    while (i < numEvents) {

        if (events[i].triggerTime > currentTime) {
            break;
        }

        EventHandler *eh = eventHandlers[events[i].eventId];

        if (eh != nullptr) {
            eh->HandleEvent(currentTime, events[i], extraData);
        }

        i++;
    }

    currentIdx = i;

    if (i >= numEvents) {
        currentIdx = numEvents - 1;
    }

    cacheCurrentTime = currentTime;
}

}; // namespace EAGL4Anim
