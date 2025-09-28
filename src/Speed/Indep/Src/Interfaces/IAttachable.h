#ifndef INTTERFACES_IATTACHABLE_H
#define INTTERFACES_IATTACHABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct _type_IAttachableList {
    const char *name() {
        return "IAttachableList";
    }
};

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

struct Attachments {
    // total size: 0x10
    UTL::Std::list<IAttachable *, _type_IAttachableList> mList; // offset 0x0, size 0x8
    IAttachable *mOwner;                                        // offset 0x8, size 0x4

    void *operator new(std::size_t size) {}

    void *operator new(std::size_t size, void *ptr) {}

    void operator delete(void *mem, void *ptr) {}

    void operator delete(void *mem, std::size_t size, const char *name) {}

    Attachments(IAttachable *pOwner) {
        mOwner = pOwner;
    }

    const UTL::Std::list<IAttachable *, _type_IAttachableList> &GetList() const {
        return mList;
    }
};

#endif
