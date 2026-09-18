#include "EAudioSmackableTest.hpp"

EAudioSmackableTest::EAudioSmackableTest(float pTestRadius) : Event(0x10), fTestRadius(pTestRadius) {}

EAudioSmackableTest::~EAudioSmackableTest() {}

const char *EAudioSmackableTest::GetEventName() const {
    return "EAudioSmackableTest";
}

void EAudioSmackableTest_MakeEvent_Callback(const void *staticData) {
    new EAudioSmackableTest(((EAudioSmackableTest::StaticData *)staticData)->fTestRadius);
}
