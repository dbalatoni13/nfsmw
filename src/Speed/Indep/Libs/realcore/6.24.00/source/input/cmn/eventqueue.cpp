#include "eventqueue.h"

namespace RealInput {

EventQueue::EventQueue(unsigned int queueSize) {
    this->mQueueSize = queueSize;
    this->mpQueue = new EventContainer[queueSize];
    int lastSlotI = static_cast<int>(this->mQueueSize) - 1;
    this->mpHead = this->mpQueue;
    this->mpTail = this->mpQueue;

    for (int i = 0; i < lastSlotI; i++) {
        this->mpQueue[i].mpNext = &this->mpQueue[i + 1];
    }
    this->mpQueue[lastSlotI].mpNext = this->mpQueue;
}

EventQueue::~EventQueue() {
    delete[] this->mpQueue;
    this->mQueueSize = 0;
}

void EventQueue::Clear() {
    this->mpTail = this->mpHead;
}

RiResult EventQueue::AddEvent(Event *pEvent) {
    this->mpHead->mEvent = *pEvent;
    if (this->mpHead->mpNext == this->mpTail) {
        return RI_EVENT_QUEUE_FULL;
    }
    this->mpHead = this->mpHead->mpNext;
    return RI_OK;
}

Event *EventQueue::GetEvent() {
    Event *pE = &this->mpTail->mEvent;
    if (this->mpHead == this->mpTail) {
        return nullptr;
    }
    this->mpTail = this->mpTail->mpNext;
    return pE;
}

} // namespace RealInput
