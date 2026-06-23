#ifndef WORLD_INTERFACES_IVEHICLEDAMAGEBEHAVIOUR_H
#define WORLD_INTERFACES_IVEHICLEDAMAGEBEHAVIOUR_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/World/DamageZones.h"

class IVehiclePartDamageBehaviour {
  public:
    IVehiclePartDamageBehaviour() {}

    virtual ~IVehiclePartDamageBehaviour() {}

    virtual void Init() = 0;
    virtual void Reset() = 0;
    virtual void Update(struct bMatrix4 *worldMatrix) = 0;
    virtual void Pose(struct bMatrix4 *worldMatrix) = 0;
    virtual void DamageVehicle(const DamageZone::Info &damageInfo) = 0;
    virtual struct bMatrix4 *GetPartMatrix(unsigned int slotId) = 0;
    virtual bool IsPartHidden(unsigned int slotId) = 0;
    virtual void HidePart(unsigned int slotId) = 0;
    virtual void UnitTest() = 0;
};

#endif
