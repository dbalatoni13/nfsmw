#ifndef INTTERFACES_IATTACHABLE_H
#define INTTERFACES_IATTACHABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

DECLARE_CONTAINER_TYPE(IAttachableList);

class IAttachable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IAttachable);

    typedef UTL::Std::list<IAttachable *, _type_IAttachableList> List;

    virtual bool Attach(IUnknown *pOther) = 0;
    virtual bool Detach(IUnknown *pOther) = 0;
    virtual bool IsAttached(const IUnknown *pOther) const = 0;
    virtual void OnAttached(IAttachable *pOther) = 0;
    virtual void OnDetached(IAttachable *pOther) = 0;
    virtual const List *GetAttachments() const = 0;
};

#endif
