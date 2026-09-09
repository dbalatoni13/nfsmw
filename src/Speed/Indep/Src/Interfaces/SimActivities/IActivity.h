#ifndef IACTIVITY_H
#define IACTIVITY_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

namespace Sim {

// total size: 0x10
class IActivity : public UTL::COM::IUnknown,
                  public UTL::COM::Factory<Param, IActivity, UCrc32>,
                  public UTL::Collections::Instanceable<HACTIVITY, IActivity, 40> {
  public:
    DECL_INTERFACE(IActivity);

    virtual void Release() = 0;
    virtual bool Attach(IUnknown *object) = 0;
    virtual bool Detach(IUnknown *object) = 0;
    virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const = 0;
};

}; // namespace Sim

#define BIND_ACTIVITY_FACTORY(_TYPE_) Sim::IActivity::Prototype _##_TYPE_(UCrc32(#_TYPE_), _TYPE_::Construct);

#endif
