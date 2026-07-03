#ifndef WORLD_INTERFACES_IVEHICLEDAMAGEBEHAVIOUR_H
#define WORLD_INTERFACES_IVEHICLEDAMAGEBEHAVIOUR_H

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/World/DamageZones.h"

class IVehiclePartDamageBehaviour {
  public:
    IVehiclePartDamageBehaviour() {}

    virtual ~IVehiclePartDamageBehaviour() {}

    virtual void Init();

    virtual void Reset();

    virtual void Update(struct bMatrix4 *worldMatrix);

    virtual void Pose(struct bMatrix4 *worldMatrix);

    virtual void DamageVehicle(const DamageZone::Info &damageInfo);

    virtual struct bMatrix4 *GetPartMatrix(unsigned int slotId);

    virtual bool IsPartHidden(unsigned int slotId);

    virtual void HidePart(unsigned int slotId);

    virtual void UnitTest();
};

#endif
