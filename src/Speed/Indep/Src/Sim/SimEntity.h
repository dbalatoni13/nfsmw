#ifndef SIM_SIMENTITY_H
#define SIM_SIMENTITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/SimObject.h"

namespace Sim {

// total size: 0x50
class Entity : public Object, public UTL::Collections::GarbageNode<Sim::Entity, 8>, public IEntity, public IAttachable {
  public:
    Entity();

  private:
    ISimable *mSimable;        // offset 0x48, size 0x4
    Attachments *mAttachments; // offset 0x4C, size 0x4
};

}; // namespace Sim

#endif
