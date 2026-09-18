#ifndef ULISTABLE_H
#define ULISTABLE_H

#include <algorithm>
#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "UTLVector.h"
#include <types.h>

#define IMPLEMENT_LISTABLE(_class_)                                                                                                                  \
    template <>                                                                                                                                      \
    UTL::Collections::Listable<_class_, _class_::List::Limit>::List UTL::Collections::Listable<_class_, _class_::List::Limit>::_mTable =             \
        UTL::Collections::Listable<_class_, _class_::List::Limit>::List();

#define IMPLEMENT_LISTABLESET(TYPE, ENUMERATORTYPE, NUMBUCKETS)                                                                                      \
    template <>                                                                                                                                      \
    UTL::Collections::ListableSet<TYPE, TYPE::List::Limit, ENUMERATORTYPE, NUMBUCKETS>::_ListSet                                                     \
        UTL::Collections::ListableSet<TYPE, TYPE::List::Limit, ENUMERATORTYPE, NUMBUCKETS>::_mLists =                                                \
            UTL::Collections::ListableSet<TYPE, TYPE::List::Limit, ENUMERATORTYPE, NUMBUCKETS>::_ListSet();


#define IMPLEMENT_COUNTABLE(TYPE) template <> int UTL::Collections::Countable<TYPE>::_mCount = 0;
#define IMPLEMENT_SINGLETON(TYPE) template <> TYPE *UTL::Collections::Singleton<TYPE>::mInstance = NULL;

namespace UTL {
namespace Collections {

template <typename T, int U> class Listable {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef value_type const *const_pointer;

    class List : public _Storage<pointer, U> {
      public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type const *const_pointer;

        const static int Limit = U;

        List() {
            this->reserve(U);
        }
        ~List() override {}

        // List &operator=(List &);
    };

    static int Count() {
        return _mTable.size();
    }

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
    static ForEachFunc ForEach(ForEachFunc f) {
        return std::for_each(_mTable.begin(), _mTable.end(), f);
    }

    static const List &GetList() {
        return _mTable;
    }

    template <typename Compare> static void Sort(Compare pred) {
        std::sort(_mTable.begin(), _mTable.end(), pred);
    }

  private:
    static List _mTable;
};

template <typename T, int ListSize, typename Enum, std::size_t EnumMax> class ListableSet {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef pointer iterator;
    typedef value_type const *const_iterator;

    class List : public _Storage<pointer, ListSize> {
      public:
        List() {}
        ~List() override {}

        // List &operator=(List &);
    };

  private:
    class _ListSet {
      public:
        _ListSet() {
            for (unsigned int i = 0; i < EnumMax; i++) {
                _buckets[i].reserve(ListSize);
            }
        }
        ~_ListSet() {}

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
    static int Count(Enum idx);
    static iterator First(Enum idx);
    static iterator Last(Enum idx);

    template <typename Functor> static Functor ForEach(Enum idx, Functor f) {
        List &l = _mLists._buckets[idx];
        return std::for_each(l.begin(), l.end(), f);
    }

    template <typename Functor> static iterator FindIf(Enum idx, Functor f) {
        List &l = _mLists._buckets[idx];
        typename List::iterator iter = std::find_if(l.begin(), l.end(), f);
        if (iter != l.end()) {
            return *iter;
        }
        return nullptr;
    }

    static const List &GetList(Enum idx) {
        return _mLists._buckets[idx];
    }

    void UnList() {
        for (std::size_t i = 0; i < EnumMax; i++) {
            _mLists._remove(static_cast<iterator>(this), i);
        }
    }

    void UnList(Enum from) {
        _mLists._remove(static_cast<iterator>(this), from);
    }

    ~ListableSet() {
        UnList();
    }

    iterator Next(Enum idx) const {
        const List &list = GetList(idx);
        typename List::const_iterator iter = std::find(list.begin(), list.end(), static_cast<const_iterator>(this));

        if (iter == list.end()) {
            return nullptr;
        }

        ++iter;

        if (iter == list.end()) {
            return nullptr;
        }

        return *iter;
    }

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

// NOTE: defined out of class on purpose. An in-class body would be implicitly
// inline and GCC 2.9 would expand it into every caller instead of emitting the
// standalone ListableSet<...>::Count/First/Last symbols the original ships.
template <typename T, int ListSize, typename Enum, std::size_t EnumMax> int ListableSet<T, ListSize, Enum, EnumMax>::Count(Enum idx) {
    return GetList(idx).size();
}

template <typename T, int ListSize, typename Enum, std::size_t EnumMax> T *ListableSet<T, ListSize, Enum, EnumMax>::First(Enum idx) {
    const List &list = GetList(idx);
    if (list.size() != 0) {
        return *list.begin();
    }
    return nullptr;
}

template <typename T, int ListSize, typename Enum, std::size_t EnumMax> T *ListableSet<T, ListSize, Enum, EnumMax>::Last(Enum idx) {
    const List &list = GetList(idx);
    if (list.size() != 0) {
        return list[list.size() - 1];
    }
    return nullptr;
}

}; // namespace Collections
}; // namespace UTL

#endif
