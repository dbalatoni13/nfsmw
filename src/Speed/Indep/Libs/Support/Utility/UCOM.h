#ifndef SUPPORT_UTILITY_UCOM_H
#define SUPPORT_UTILITY_UCOM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

typedef void *HINTERFACE;

struct _type_UComObject {
    const char *name() {
        return "UComObject";
    }
};

namespace UTL {
namespace COM {

class IUnknown;

// total size: 0x10
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

    class _IList : public UTL::Std::vector<_IPair, _type_UComObject> {
        // int pad;
        // void *_M_start;
        // void *_M_finish;
        // void *_M_end_of_storage;

      public:
        _IList(unsigned int icount);
        ~_IList();
        void Add(HINTERFACE handle, IUnknown *ref);
        IUnknown *Find(HINTERFACE handle);
        void *Find(const IUnknown *pUnk) const;
        void Remove(IUnknown *pUnk);
    };

  public:
    _IList _mInterfaces; // offset 0x0, size 0x10

  protected:
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    Object(unsigned int icount) : _mInterfaces(icount) {}

    ~Object() {}
};

class IUnknown {
  public:
    // total size: 0x8
    Object *_mCOMObject; // offset 0x0, size 0x4

    template <typename T> bool QueryInterface(T **out) {
        HINTERFACE handle = T::_IHandle();

        *out = (T *)_mCOMObject->_mInterfaces.Find(handle);
        return *out != nullptr;
    }

  protected:
    IUnknown(Object *owner, void *handle) {
        _mCOMObject = owner;
        _mCOMObject->_mInterfaces.Add(handle, this);
    }

    virtual ~IUnknown() {
        this->_mCOMObject->_mInterfaces.Remove(this);
    }
};

inline bool ComparePtr(const IUnknown *pUnk1, const IUnknown *pUnk2) {
    return pUnk1 == pUnk2;
}

template <typename T, typename U, typename V> class Factory {

  public:
    static U *CreateInstance(V, T);

    ~Factory() {}
};

} // namespace COM
} // namespace UTL

#endif
