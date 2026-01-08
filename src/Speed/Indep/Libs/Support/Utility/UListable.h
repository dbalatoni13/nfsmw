#ifndef SUPPORT_UTILITY_ULISTABLE_H
#define SUPPORT_UTILITY_ULISTABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <algorithm>
#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "UTLVector.h"
#include <types.h>

namespace UTL {
namespace Collections {

template <typename T, std::size_t U> class Listable {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef value_type const *const_pointer;

    class List : public FixedVector<pointer, U> {
      public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type const *const_pointer;

        // List(const List &);
        List();
        virtual ~List();

        // List &operator=(List &);
    };

    typedef void (*ForEachFunc)(pointer);
    typedef bool (*ComparisonFunc)(pointer, pointer);

  protected:
    Listable() {
        _mTable.push_back((pointer)this);
    }

    void Unlist() {
        typename List::iterator newend = std::remove(_mTable.begin(), _mTable.end(), static_cast<T *>(this));
        if (newend != _mTable.end()) {
            _mTable.erase(newend, _mTable.end());
        }
    }

    ~Listable() {
        Unlist();
    }

  public:
    static void ForEach(ForEachFunc f) {
        std::for_each(_mTable.begin(), _mTable.end(), f);
    }

    static const List &GetList() {
        return _mTable;
    }

    static void Sort(ComparisonFunc pred) {
        std::sort(_mTable.begin(), _mTable.end(), pred);
    }

  private:
    static List _mTable;
};

template <typename T, std::size_t ListSize, typename Enum, std::size_t EnumMax> class ListableSet {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef pointer iterator;
    typedef value_type const *const_iterator;

    class List : public _Storage<pointer, ListSize> {
      public:
        // List(const List &);
        List() {}
        ~List() override {}

        // List &operator=(List &);
    };

  private:
    class _ListSet {
      public:
        _ListSet();
        // _ListSet(_ListSet &);
        ~_ListSet();

        // _ListSet &operator=(const _ListSet &);

        friend class ListableSet;

      private:
        void _add(iterator t, std::size_t idx) {
            _buckets[idx].push_back(t);
        }

        void _remove(iterator t, std::size_t idx) {
            List &bucket = _buckets[idx];
            typename List::iterator newend = std::remove(bucket.begin(), bucket.end(), t);
            if (newend != bucket.end()) {
                bucket.erase(newend, bucket.end());
            }
        }

        List _buckets[EnumMax];
    };

  public:
    typedef void (*ForEachFunc_t)(iterator);

    static int Count(Enum idx);
    static iterator First(Enum idx);
    static iterator Last(Enum idx);

    static void ForEach(Enum idx, ForEachFunc_t f) {
        List &l = _mLists._buckets[idx];
        std::for_each(l.begin(), l.end(), f);
    }

    static const List &GetList(Enum idx) {
        return _mLists._buckets[idx];
    }

    void UnList() {
        for (std::size_t i = 0; i < EnumMax; i++) {
            _mLists._remove(static_cast<iterator>(this), i);
        }
    }

    ~ListableSet() {
        UnList();
    }

    iterator Next(Enum idx) {}

    void AddToList(Enum to) {
        _mLists._add(static_cast<iterator>(this), to);
    }

  private:
    static _ListSet _mLists;
};

template <typename T> class Countable {
    static int _mCount;

  protected:
    Countable() {
        _mCount++;
    }

    ~Countable() {
        _mCount--;
    }

  public:
    static int Count() {
        return _mCount;
    }
};

}; // namespace Collections
}; // namespace UTL

#endif
