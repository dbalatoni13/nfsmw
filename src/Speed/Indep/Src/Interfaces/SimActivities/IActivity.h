#ifndef IACTIVITY_H
#define IACTIVITY_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

// TODO move to SimTypes.h
struct HACTIVITY__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

// TODO move to SimTypes.h
typedef HACTIVITY__ *HACTIVITY;

namespace Sim {

// total size: 0x10
class IActivity : public UTL::COM::IUnknown,
                  public UTL::COM::Factory<Param, IActivity, UCrc32>,
                  public UTL::Collections::Instanceable<HACTIVITY, IActivity, 40> {
    DECL_INTERFACE(IActivity);

    virtual void Release();
    virtual bool Attach(IUnknown *object);
    virtual bool Detach(IUnknown *object);
    virtual const UTL::Std::list<IAttachable *, _type_IAttachableList> *GetAttachments() const;
};

}; // namespace Sim

#define BIND_ACTIVITY_FACTORY(_TYPE_) Sim::IActivity::Prototype _##_TYPE_(UCrc32(#_TYPE_), _TYPE_::Construct);

#endif
