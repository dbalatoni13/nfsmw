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
    IRBVehicle(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IRBVehicle() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual void SetCollisionMass(float mass);
    virtual void SetCollisionCOG(const UMath::Vector3 &cog);
    virtual void EnableObjectCollisions(bool enable);
    virtual void SetInvulnerability(eInvulnerablitiy state, float time);
    virtual eInvulnerablitiy GetInvulnerability() const;
    virtual void SetPlayerReactions(const Attrib::Gen::collisionreactions &reactions);
    virtual const Attrib::Gen::collisionreactions &GetPlayerReactions() const;
};

#endif
