#ifndef REALMEMCARD_IMPL_INTERFACEIMP_H
#define REALMEMCARD_IMPL_INTERFACEIMP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "memcard_interface_impl.h"

namespace Realmc {

extern Interface *gInterface;

struct InterfaceImp : public Interface {
    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, size);
    }

    InterfaceImp(const SystemInterface &iSystem);
    virtual ~InterfaceImp();
    virtual int AddRef();
    virtual int Release();
    virtual void ClearTask() = 0;
    void CheckMessageCompatibility();

    void LockInterfaceMutex();
    void UnlockInterfaceMutex();

    SystemInterface mISystem;
    IMutex *mMutex;
    int mRefcount;
};

} // namespace Realmc

#endif
