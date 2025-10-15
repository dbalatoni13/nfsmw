#ifndef SUPPORT_UTILITY_ULISTABLE_H
#define SUPPORT_UTILITY_ULISTABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <algorithm>
#include <cstddef>

#include "UTLVector.h"

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

        List(const List &);
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

    void Unlist() {}

    ~Listable() {
        Unlist();
    }

  public:
    ForEachFunc ForEach(ForEachFunc f) {}

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

    class List : public FixedVector<pointer, ListSize> {
      public:
        List(const List &);
        List();
        virtual ~List();

        // List &operator=(List &);
    };

    typedef void (*ForEachFunc_t)(iterator);

    static int Count(Enum idx);
    static iterator First(Enum idx);
    static iterator Last(Enum idx);

    ~ListableSet() {}

    void UnList() {}

    iterator Next(Enum idx) {}

    ForEachFunc_t ForEach(Enum idx, ForEachFunc_t f) {}

    void AddToList(Enum to) {}

    static const List &GetList(Enum idx) {
        return _mLists._buckets[idx];
    }

  private:
    class _ListSet {
      public:
        // how to access _buckets without making it public?
        List _buckets[EnumMax];

        void _add(iterator, unsigned int);

        void _remove(iterator, unsigned int);

        _ListSet();
        // _ListSet(_ListSet &);
        ~_ListSet();

        // _ListSet &operator=(const _ListSet &);
    };

    static _ListSet _mLists;
};

template <typename T> class Countable {
    static int _mCount;

  protected:
    Countable() {}
    ~Countable() {}

  public:
    static int Count() {
        return _mCount;
    }
};

}; // namespace Collections
}; // namespace UTL

#endif
