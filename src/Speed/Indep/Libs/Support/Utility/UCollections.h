#ifndef SUPPORT_UTILITY_UCOLLECTIONS_H
#define SUPPORT_UTILITY_UCOLLECTIONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"

struct _type_UContainer {
    const char *name() {
        return "UContainer";
    }
};

namespace UTL {
namespace Collections {

struct _KeyedNode {
    // total size: 0x8
    unsigned int Handle; // offset 0x0, size 0x4
    void *Ref;           // offset 0x4, size 0x4
};

template <typename T, std::size_t Size> class _Storage : public FixedVector<T, Size, 16> {
  public:
    _Storage() {}

    ~_Storage() {}
};

template <typename T> class Singleton {
  public:
    static T *Get() {
        return mInstance;
    }

    static bool Exists() {
        return mInstance != nullptr;
    }

  protected:
    Singleton() {}
    ~Singleton() {}

  private:
    static T *mInstance;
};

template <typename HandleT, typename U, std::size_t Size> class Instanceable {
    class _List : public FixedVector<_KeyedNode, Size, 16> {};

    unsigned int _mHandle;

    static unsigned int _mHNext;
    static _List _mList;

  public:
    Instanceable() {}

    HandleT GetInstanceHandle() const {
        return reinterpret_cast<HandleT>(_mHandle);
    }
};

template <typename T, std::size_t V> class GarbageNode {
    struct Collector {
        // void _Add(T *) {}

        // void _Remove(T *) {}

        // bool _Release(T *) {}

      public:
        Collector() {}

        // void Collect() {}

        // void Shutdown() {}
    };

    bool _mDirty;
    static Collector _mCollector;

    // T *_This() {}

  protected:
    GarbageNode() {}
    ~GarbageNode() {}

  public:
    // bool ReleaseGC() {}

    // bool IsDirty() {}

    static Collector &GetGC() {
        return _mCollector;
    }
};

template <typename T, typename Tag> class Container {
  public:
    class Elements {
      public:
        Elements();
        ~Elements();
    };

  private:
    Elements _mElements;
};

}; // namespace Collections
}; // namespace UTL

#endif
