#ifndef EAXSOUND_SIMSTATES_EAX_HELISTATE_H
#define EAXSOUND_SIMSTATES_EAX_HELISTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"

struct EAX_HeliState : public UTL::Collections::Listable<EAX_HeliState, 10> {
    bMatrix4 mMatrix;             // offset 0x4, size 0x40
    bVector3 mVel0;               // offset 0x44, size 0x10
    bVector3 mVel1;               // offset 0x54, size 0x10
    bVector3 mAccel;              // offset 0x64, size 0x10
    float mFWSpeed;               // offset 0x74, size 0x4
    Sound::MovementMode mMovementMode;   // offset 0x78, size 0x4
    Sound::PlayerZones mPlayerZone;      // offset 0x7C, size 0x4
    Attrib::Instance mAttributes; // offset 0x80, size 0x14
    Sound::Context mContext;      // offset 0x94, size 0x4
    bool mSimUpdating;            // offset 0x98, size 0x1
    char _pad_sim[3];
    unsigned int mWorldID;        // offset 0x9C, size 0x4

    EAX_HeliState(const Attrib::Collection *atr, unsigned int wuid);
    ~EAX_HeliState();

    bool IsSimUpdating();
    bVector3 *GetPosition();
    const bVector3 *GetForwardVector();
    const bVector3 *GetVelocity();
    float GetForwardSpeed();
};

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
