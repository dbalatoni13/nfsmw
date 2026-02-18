#ifndef INTTERFACES_IATTACHABLE_H
#define INTTERFACES_IATTACHABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

DECLARE_CONTAINER_TYPE(IAttachableList);

struct IAttachable : public UTL::COM::IUnknown {
    typedef UTL::Std::list<IAttachable *, _type_IAttachableList> List;

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IAttachable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IAttachable() {}

    virtual bool Attach(IUnknown *pOther);
    virtual bool Detach(IUnknown *pOther);
    virtual bool IsAttached(const IUnknown *pOther) const;
    virtual void OnAttached(IAttachable *pOther);
    virtual void OnDetached(IAttachable *pOther);
    virtual const List *GetAttachments() const;
};

#endif
