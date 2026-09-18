#include "EGPSFinished.hpp"

EGPSFinished::EGPSFinished() : Event(0x10) {}

EGPSFinished::~EGPSFinished() {}

const char *EGPSFinished::GetEventName() const {
    return "EGPSFinished";
}

void EGPSFinished_MakeEvent_Callback(const void *staticData) {
    new EGPSFinished();
}
