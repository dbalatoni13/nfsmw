#ifndef INTTERFACES_IATTACHABLE_H
#define INTTERFACES_IATTACHABLE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

DECLARE_CONTAINER_TYPE(IAttachableList);

class IAttachable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IAttachable);

    typedef UTL::Std::list<IAttachable *, _type_IAttachableList> List;

    virtual bool Attach(IUnknown *pOther);
    virtual bool Detach(IUnknown *pOther);
    virtual bool IsAttached(const IUnknown *pOther) const;
    virtual void OnAttached(IAttachable *pOther);
    virtual void OnDetached(IAttachable *pOther);
    virtual const List *GetAttachments() const;
};

#endif
