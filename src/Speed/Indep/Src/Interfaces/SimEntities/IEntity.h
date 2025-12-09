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

    virtual void AttachPhysics(ISimable *object);
    virtual void DetachPhysics();
    virtual ISimable *GetSimable() const;
    virtual const UMath::Vector3 &GetPosition() const;
    virtual bool SetPosition(UMath::Vector3 &position);
    virtual void Kill();
    virtual bool Attach(IUnknown *object);
    virtual bool Detach(IUnknown *object);
    virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const;
};

}; // namespace Sim

#endif
