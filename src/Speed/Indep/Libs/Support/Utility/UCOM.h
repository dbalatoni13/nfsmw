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

    Object(std::size_t icount) : _mInterfaces(icount) {}

    ~Object() {}
};

// total size: 0x8
class IUnknown {
  public:
    template <typename T> bool QueryInterface(T **out) {
        HINTERFACE handle = T::_IHandle();

        *out = (T *)_mCOMObject->_mInterfaces.Find(handle);
        return *out != nullptr;
    }

    // TODO is this a thing?
    template <typename T> bool QueryInterface(const T **out) const {
        HINTERFACE handle = T::_IHandle();

        *out = (T *)_mCOMObject->_mInterfaces.Find(handle);
        return *out != nullptr;
    }

    friend bool ComparePtr(const IUnknown *pUnk1, const IUnknown *pUnk2);

  protected:
    IUnknown(Object *owner, void *handle) {
        _mCOMObject = owner;
        _mCOMObject->_mInterfaces.Add(handle, this);
    }

    virtual ~IUnknown() {
        _mCOMObject->_mInterfaces.Remove(this);
    }

  private:
    Object *_mCOMObject; // offset 0x0, size 0x4
};

template <typename T> inline T *QueryInterface(IUnknown *pUnk) {
    T *ptr = nullptr;
    if (pUnk) {
        pUnk->QueryInterface(&ptr);
    }

    return ptr;
}

inline bool ComparePtr(const IUnknown *pUnk1, const IUnknown *pUnk2) {
    if (pUnk1 && pUnk2) {
        return pUnk1->_mCOMObject == pUnk2->_mCOMObject;
    }
    return !pUnk1 && !pUnk2;
}

inline void ValidatePtr(const IUnknown *pUnk) {}

template <typename T> inline bool Is(const IUnknown *pUnk) {
    const T *ptr;
    return pUnk && pUnk->QueryInterface(&ptr);
}

template <typename T, typename U, typename V> class Factory {

  public:
    static U *CreateInstance(V, T);

    Factory() {}

    ~Factory() {}
};

} // namespace COM
} // namespace UTL

#endif
