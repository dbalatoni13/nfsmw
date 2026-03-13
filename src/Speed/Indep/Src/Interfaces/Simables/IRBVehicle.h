#ifndef INTERFACES_SIMABLES_IRBVEHICLE_H
#define INTERFACES_SIMABLES_IRBVEHICLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/collisionreactions.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

class IRBVehicle : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IRBVehicle(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IRBVehicle() {}

    virtual void SetCollisionMass(float mass) = 0;
    virtual void SetCollisionCOG(const UMath::Vector3 &cog) = 0;
    virtual void EnableObjectCollisions(bool enable) = 0;
    virtual void SetInvulnerability(eInvulnerablitiy state, float time) = 0;
    virtual eInvulnerablitiy GetInvulnerability() const = 0;
    virtual void SetPlayerReactions(const Attrib::Gen::collisionreactions &reactions) = 0;
    virtual const Attrib::Gen::collisionreactions &GetPlayerReactions() const = 0;
};

#endif
