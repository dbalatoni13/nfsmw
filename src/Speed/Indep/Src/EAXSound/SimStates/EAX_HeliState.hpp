#ifndef EAXSOUND_SIMSTATES_EAX_HELISTATE_H
#define EAXSOUND_SIMSTATES_EAX_HELISTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

inline EAX_HeliState::~EAX_HeliState() {}

inline bool EAX_HeliState::IsSimUpdating() {
    return mSimUpdating == true;
}

inline bVector3 *EAX_HeliState::GetPosition() {
    return static_cast<bVector3 *>(static_cast<void *>(&mMatrix.v3));
}

inline const bVector3 *EAX_HeliState::GetForwardVector() {
    return static_cast<const bVector3 *>(static_cast<const void *>(&mMatrix.v0));
}

inline const bVector3 *EAX_HeliState::GetVelocity() {
    return &mVel0;
}

inline float EAX_HeliState::GetForwardSpeed() {
    return mFWSpeed;
}

#endif
