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
    static EAX_HeliState *Find(WUID objectid) {} // Decl: 17

    EAX_HeliState(const Attrib::Collection *atr, WUID wuid)
        : mAttributes(atr, 0, nullptr), //
          mWorldID(wuid) {}             // Decl: 28

    ~EAX_HeliState() {} // Decl: 41

    EAX_HeliState *GetState() {}  // Decl: 43
    EAX_HeliState *GetDriver() {} // Decl: 44

    const bVector3 *GetForwardVector() {
        return reinterpret_cast<const bVector3 *>(&this->mMatrix.v0);
    } // Decl: 46
    bVector3 *GetPosition() {
        return reinterpret_cast<bVector3 *>(&this->mMatrix.v3);
    } // Decl: 47
    const bVector2 *GetPosition2D() {} // Decl: 48
    float GetForwardSpeed() {}         // Decl: 49

    MovementMode GetMovementMode() {}

    PlayerZones GetZone() {} // Decl: 51

    const bVector3 *GetAcceleration() {} // Decl: 55

    const bVector3 *GetOldVel() {} // Decl: 60
    const bVector3 *GetVelocity() {
        return &this->mVel0;
    } // Decl: 61
    const bVector2 *GetVelocity2D() {} // Decl: 62
    float GetVelocityMagnitude() {}    // Decl: 63
    float GetVelocityMagnitudeMPH() {} // Decl: 64

    const bMatrix4 *GetBodyMatrix() {} // Decl: 66

    Attrib::Instance *GetAttributes() {} // Decl: 69
    Sound::Context GetContext() {}       // Decl: 70
    bool IsSimUpdating() {
        return this->mSimUpdating;
    } // Decl: 71

    ALIGNVEC bMatrix4 mMatrix;     // offset 0x4, size 0x40, Decl: 74
    ALIGNVEC bVector3 mVel0;       // offset 0x44, size 0x10, Decl: 75
    ALIGNVEC bVector3 mVel1;       // offset 0x54, size 0x10, Decl: 76
    ALIGNVEC bVector3 mAccel;      // offset 0x64, size 0x10, Decl: 77
    float mFWSpeed;               // offset 0x74, size 0x4, Decl: 78
    MovementMode mMovementMode;   // offset 0x78, size 0x4
    PlayerZones mPlayerZone;      // offset 0x7C, size 0x4, Decl: 82
    Attrib::Instance mAttributes; // offset 0x80, size 0x14, Decl: 84
    Sound::Context mContext;      // offset 0x94, size 0x4, Decl: 85
    bool mSimUpdating;            // offset 0x98, size 0x1, Decl: 87
    const WUID mWorldID;          // offset 0x9C, size 0x4, Decl: 88
};

#endif
