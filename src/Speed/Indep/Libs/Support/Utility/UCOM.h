#ifndef SUPPORT_UTILITY_UCOM_H
#define SUPPORT_UTILITY_UCOM_H

#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace UTL {
namespace COM {

class IUnknown;

class Object {
    struct _IPair {
        void *handle;
        IUnknown *ref;

        _IPair(void *h, IUnknown *r) {
            this->handle = h;
            this->ref = r;
        }
        // bool operator<(const _IPair &rhs) {}
    };

    struct _IList // : public std::vector<_IPair, _type_UComObject>
    {
        int pad;
        void *_M_start;
        void *_M_finish;
        void *_M_end_of_storage;

        // _IList(unsigned int icount) {}

        // ~_IList() {}

        // void Add(void *handle, IUnknown *ref) {}

        // IUnknown *Find(void *handle) {}

        // void *Find(const IUnknown *pUnk) const {}

        // void Remove(IUnknown *pUnk) {}
    };

    // total size: 0x10
    _IList _mInterfaces; // offset 0x0, size 0x10

  protected:
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }
};

class IUnknown {
    // total size: 0x8
    Object *_mCOMObject; // offset 0x0, size 0x4

  protected:
    IUnknown(Object *owner, void *handle) {}

    virtual ~IUnknown() {}
};

template <typename T, typename U, typename V> class Factory {

  public:
    static U *CreateInstance(V, T);
};

} // namespace COM
} // namespace UTL

#endif
