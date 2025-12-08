#ifndef INTTERFACES_IATTACHABLE_H
#define INTTERFACES_IATTACHABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct _type_IAttachableList {
    const char *name() {
        return "IAttachableList";
    }
};

struct IAttachable : public UTL::COM::IUnknown {
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IAttachable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IAttachable() {}

    virtual bool Attach(IUnknown *pOther);

    virtual bool Detach(IUnknown *pOther);

    virtual bool IsAttached(const IUnknown *pOther) const;

    virtual void OnAttached(IAttachable *pOther);
};

#endif
