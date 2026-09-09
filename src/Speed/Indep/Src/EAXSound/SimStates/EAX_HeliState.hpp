#ifndef EAX_HELI_STATE_HPP
#define EAX_HELI_STATE_HPP

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/Src/World/Player.hpp"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// total size: 0xA0
// Decl: 15
class EAX_HeliState : public UTL::Collections::Listable<EAX_HeliState, 10> {
  public:
    // Decl: 17
    static EAX_HeliState *Find(WUID objectid) {
        for (List::const_iterator iter = GetList().begin(); iter != GetList().end(); ++iter) {
            EAX_HeliState *state = *iter;

            if (state->mWorldID == objectid) {
                return state;
            }
        }

        return nullptr;
    }

    // Decl: 28
    EAX_HeliState(const Attrib::Collection *atr, WUID wuid)
        : mVel0(0.0f, 0.0f, 0.0f),         //
          mMovementMode(PHYSICS_MOVEMENT), //
          mPlayerZone(PLAYER_ZONE_NONE),   //
          mAttributes(atr, 0, nullptr),    //
          mSimUpdating(true),              //
          mWorldID(wuid) {
        this->mVel1 = this->mVel0;
        bIdentity(&this->mMatrix);
    }

    ~EAX_HeliState() {} // Decl: 41

    // Decl: 43
    EAX_HeliState *GetState() {
        return this;
    }
    // Decl: 44
    EAX_HeliState *GetDriver() {
        return this;
    }

    // Decl: 46
    const bVector3 *GetForwardVector() {
        return reinterpret_cast<const bVector3 *>(&this->mMatrix.v0);
    }
    // Decl: 47
    bVector3 *GetPosition() {
        return reinterpret_cast<bVector3 *>(&this->mMatrix.v3);
    }
    // Decl: 48
    const bVector2 *GetPosition2D() {
        return reinterpret_cast<const bVector2 *>(this->GetPosition());
    }
    // Decl: 49
    float GetForwardSpeed() {
        return this->mFWSpeed;
    }

    MovementMode GetMovementMode() {
        return this->mMovementMode;
    }

    // Decl: 51
    PlayerZones GetZone() {
        return this->mPlayerZone;
    }

    // const bVector3 *GetAcceleration() {} // Decl: 55

    // Decl: 60
    const bVector3 *GetOldVel() {
        return &this->mVel1;
    }
    // Decl: 61
    const bVector3 *GetVelocity() {
        return &this->mVel0;
    }
    // Decl: 62
    const bVector2 *GetVelocity2D() {
        return reinterpret_cast<const bVector2 *>(this->GetVelocity());
    }
    // Decl: 63
    float GetVelocityMagnitude() {
        return bLength(this->mVel0);
    }
    // Decl: 64
    float GetVelocityMagnitudeMPH() {
        return MPS2MPH(this->GetVelocityMagnitude());
    }

    // Decl: 66
    const bMatrix4 *GetBodyMatrix() {
        return &this->mMatrix;
    }

    // Decl: 69
    Attrib::Instance *GetAttributes() {
        return &this->mAttributes;
    }
    // Decl: 70
    Sound::Context GetContext() {
        return this->mContext;
    }
    // Decl: 71
    bool IsSimUpdating() {
        return this->mSimUpdating;
    }

    bMatrix4 mMatrix;             // offset 0x4, size 0x40, Decl: 74
    bVector3 mVel0;               // offset 0x44, size 0x10, Decl: 75
    bVector3 mVel1;               // offset 0x54, size 0x10, Decl: 76
    bVector3 mAccel;              // offset 0x64, size 0x10, Decl: 77
    float mFWSpeed;               // offset 0x74, size 0x4, Decl: 78
    MovementMode mMovementMode;   // offset 0x78, size 0x4
    PlayerZones mPlayerZone;      // offset 0x7C, size 0x4, Decl: 82
    Attrib::Instance mAttributes; // offset 0x80, size 0x14, Decl: 84
    Sound::Context mContext;      // offset 0x94, size 0x4, Decl: 85
    bool mSimUpdating;            // offset 0x98, size 0x1, Decl: 87
    const WUID mWorldID;          // offset 0x9C, size 0x4, Decl: 88
};

#endif
