#ifndef SUPPORT_UTILITY_UCOLLECTIONS_H
#define SUPPORT_UTILITY_UCOLLECTIONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"

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
  protected:
    Singleton() {}
    ~Singleton() {}

  public:
    // static T *Get() {}

    // static bool Exists() {}
};

template <typename T, typename U, int V> class Instanceable {
    class _List : public FixedVector<_KeyedNode, 32, 16> {};

    unsigned int _mHandle;

    static unsigned int _mHNext;
    static _List _mList;

  public:
    Instanceable() {}

    // T GetInstanceHandle() const {}
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

}; // namespace Collections
}; // namespace UTL

#endif
