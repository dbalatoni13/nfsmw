#ifndef SUPPORT_UTILITY_UCOLLECTIONS_H
#define SUPPORT_UTILITY_UCOLLECTIONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"

#include <algorithm>

struct _type_UContainer {
    const char *name() {
        return "UContainer";
    }
};

namespace UTL {
namespace Collections {

// total size: 0x8
struct _KeyedNode {
    static _KeyedNode *Search(_KeyedNode *begin_iter, _KeyedNode *end_iter, uintptr_t handle);

    _KeyedNode(uintptr_t handle, void *refrence) : Handle(handle), Ref(refrence) {}

    uintptr_t Handle; // offset 0x0, size 0x4
    void *Ref;        // offset 0x4, size 0x4
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
    Singleton() {
        mInstance = static_cast<T *>(this);
    }

    ~Singleton() {
        mInstance = nullptr;
    }

  private:
    static T *mInstance;
};

template <typename Handle, typename T, std::size_t Size> class Instanceable {
  public:
    Instanceable() {
        _mHandle = ++_mHNext;
        _mList.push_back(_KeyedNode(_mHandle, static_cast<T *>(this)));
    }

    ~Instanceable() {
        _KeyedNode *node = _KeyedNode::Search(_mList.begin(), _mList.end(), _mHandle);
        _mList.erase(node);
    }

    static T *FindInstance(Handle handle) {
        if (handle) {
            _KeyedNode *node = _KeyedNode::Search(_mList.begin(), _mList.end(), reinterpret_cast<uintptr_t>(handle));
            if (node) {
                return static_cast<T *>(node->Ref);
            }
        }
        return nullptr;
    }

    Handle GetInstanceHandle() const {
        return reinterpret_cast<Handle>(_mHandle);
    }

  private:
    class _List : public FixedVector<_KeyedNode, Size, 16> {};

    static uintptr_t _mHNext;
    static _List _mList;

    uintptr_t _mHandle; // offset 0x0, size 0x4
};

template <typename T, std::size_t Size> class GarbageNode {
  public:
    class Collector {
      public:
        // total size: 0x8
        struct _Node {
            _Node(T *r) : myptr(r), refcount(1) {}

            bool operator==(const _Node &n) const {
                return n.myptr == myptr;
            }

            T *myptr;     // offset 0x0, size 0x4
            int refcount; // offset 0x4, size 0x4
        };

        Collector() {}

        // void Collect() {}

        // void Shutdown() {}

        friend class GarbageNode;

      private:
        void _Add(T *ptr) {
            _mClean.push_back(_Node(ptr));
            _mCount++;
        }

        /**
         * @brief Finds a node based on its pointer and removes it from both the clean container and the dirty one.
         *
         * @param ptr
         * @return true
         * @return false
         */
        void _Remove(T *ptr) {
            typename _Storage<_Node, Size>::iterator iter = std::remove(_mClean.begin(), _mClean.end(), _Node(ptr));
            if (iter != _mClean.end()) {
                _mClean.erase(iter, _mClean.end());
            }

            iter = std::remove(_mDirty.begin(), _mDirty.end(), _Node(ptr));
            if (iter != _mDirty.end()) {
                _mDirty.erase(iter, _mDirty.end());
            }
            _mCount--;
        }

        /**
         * @brief Finds a clean node based on its pointer, sets its reference count to 0 and adds it to the dirty container.
         *
         * @param ptr
         * @return true if the node was found
         * @return false otherwise
         */
        bool _Release(T *ptr) {
            for (typename _Storage<_Node, Size>::iterator iter = _mClean.begin(); iter != _mClean.end(); ++iter) {
                if (iter->refcount != 0 && iter->myptr == ptr) {
                    iter->refcount = 0;
                    _mDirty.push_back(_Node(ptr));
                    return true;
                }
            }
            return false;
        }

        _Storage<_Node, Size> _mDirty; // offset 0x0, size 0x150
        _Storage<_Node, Size> _mClean; // offset 0x150, size 0x150
        unsigned int _mCount;          // offset 0x2A0, size 0x4
    };

    bool ReleaseGC() {
        _mDirty = true;
        _mCollector._Release(_This());
    }

    bool IsDirty() {
        return _mDirty;
    }

    static Collector &GetGC() {
        return _mCollector;
    }

  protected:
    GarbageNode() {
        _mDirty = false;
        _mCollector._Add(_This());
    }

    ~GarbageNode() {
        _mCollector._Remove(_This());
    }

  private:
    T *_This() {
        return static_cast<T *>(this);
    }

    bool _mDirty;
    static Collector _mCollector;
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
