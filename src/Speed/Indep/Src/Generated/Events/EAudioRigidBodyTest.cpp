#include "EAudioRigidBodyTest.hpp"

EAudioRigidBodyTest::EAudioRigidBodyTest() : Event(0x10) {}

EAudioRigidBodyTest::~EAudioRigidBodyTest() {}

const char *EAudioRigidBodyTest::GetEventName() const {
    return "EAudioRigidBodyTest";
}

void EAudioRigidBodyTest_MakeEvent_Callback(const void *staticData) {
    new EAudioRigidBodyTest();
}
