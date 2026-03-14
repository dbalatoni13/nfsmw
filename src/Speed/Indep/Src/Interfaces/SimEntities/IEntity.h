#ifndef INTERFACES_SIMENTITIES_IENTITY_H
#define INTERFACES_SIMENTITIES_IENTITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

enum eEntityList {
    ENTITY_ALL = 0,
    ENTITY_PLAYERS = 1,
    ENTITY_AI = 2,
    ENTITY_COPS = 3,
    ENTITY_MAX = 4,
};

namespace Sim {

class IEntity : public UTL::COM::IUnknown,
                public UTL::Collections::ListableSet<Sim::IEntity, 8, eEntityList, ENTITY_MAX>,
                public UTL::COM::Factory<Sim::Param, Sim::IEntity, UCrc32> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IEntity(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IEntity() {}

    virtual void AttachPhysics(ISimable *object) = 0;
    virtual void DetachPhysics() = 0;
    virtual ISimable *GetSimable() const = 0;
    virtual const UMath::Vector3 &GetPosition() const = 0;
    virtual bool SetPosition(const UMath::Vector3 &position) = 0;
    virtual void Kill() = 0;
    virtual bool Attach(IUnknown *object) = 0;
    virtual bool Detach(IUnknown *object) = 0;
    virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const = 0;
};

}; // namespace Sim

#endif
