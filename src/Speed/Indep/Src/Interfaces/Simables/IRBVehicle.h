#ifndef IRBVEHICLE_H
#define IRBVEHICLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/collisionreactions.h"

enum eInvulnerablitiy {
    INVULNERABLE_NONE = 0,
    INVULNERABLE_FROM_MANUAL_RESET = 1,
    INVULNERABLE_FROM_RESET = 2,
    INVULNERABLE_FROM_CONTROL_SWITCH = 3,
    INVULNERABLE_FROM_PHYSICS_SWITCH = 4,
};

class IRBVehicle : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IRBVehicle);

    virtual void SetCollisionMass(float mass) = 0;
    virtual void SetCollisionCOG(const UMath::Vector3 &cog) = 0;
    virtual void EnableObjectCollisions(bool enable) = 0;
    virtual void SetInvulnerability(eInvulnerablitiy state, float time) = 0;
    virtual eInvulnerablitiy GetInvulnerability() const = 0;
    virtual void SetPlayerReactions(const Attrib::Gen::collisionreactions &reactions) = 0;
    virtual const Attrib::Gen::collisionreactions &GetPlayerReactions() const = 0;
};

#endif
