#include "EAXCharacter.h"

EAXCharacter::EAXCharacter(int sID, HSIMABLE wID, int bID, int cID)
    : mCallsign(bID, cID) {
    mSpeakerID = sID;
    mHandle = wID;
    mPos.x = UMath::Vector3::kZero.x;
    mPos.z = UMath::Vector3::kZero.z;
    mPos.y = UMath::Vector3::kZero.y;
    mDistance = 0.0f;
    mHealth = 1.0f;
    *reinterpret_cast<unsigned int *>(&mSuspectLOS) = 0;
    mSpeed = 0.0f;
    *reinterpret_cast<unsigned int *>(&mDestroyed) = 0;
    *reinterpret_cast<unsigned int *>(&mActive) = 0;
}

EAXCharacter::~EAXCharacter() {}

void *EAXCharacter::operator new(unsigned int size) {
    return AudioMemBase::operator new(size);
}

void EAXCharacter::operator delete(void *ptr) {
    AudioMemBase::operator delete(ptr);
}

void EAXCharacter::Ack() {}

void EAXCharacter::Deny() {}

void EAXCharacter::InterruptStatic() {}

void EAXCharacter::InterruptExpletive() {}

void EAXCharacter::InterruptComposedLow() {}

void EAXCharacter::InterruptComposedHigh() {}

void EAXCharacter::DriverHistory() {}

void EAXCharacter::HeatJump(Type_heat_level heat) {
    if (heat) {
        mLastEvent = static_cast<int>(heat);
    }
}

void EAXCharacter::Update() {}

void EAXCharacter::InterruptViolent() {}

void EAXCharacter::Reset() {
    mTimeLastSpoken = 0;
    mLastEvent = 0;
    mSpeed = 0.0f;
    mDistance = 0.0f;
    mDestroyed = false;
    mActive = false;
    mSuspectLOS = false;
}
