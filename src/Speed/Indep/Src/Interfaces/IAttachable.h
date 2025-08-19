#ifndef INTTERFACES_IATTACHABLE_H
#define INTTERFACES_IATTACHABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct IAttachable : public UTL::COM::IUnknown {
    IAttachable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IAttachable();

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual bool Attach(IUnknown *pOther);

    virtual bool Detach(IUnknown *pOther);

    virtual bool IsAttached(const IUnknown *pOther) const;

    virtual void OnAttached(IAttachable *pOther);
};

#endif
