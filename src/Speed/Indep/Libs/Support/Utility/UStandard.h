#ifndef SUPPORT_UTILITY_USTANDARD_H
#define SUPPORT_UTILITY_USTANDARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <list>
#include <map>
#include <set>
#include <vector>

#include "UTLAllocator.h"

// clang-format off
#define DECLARE_CONTAINER_TYPE(Type)\
struct _type_##Type { \
    const char* name() {\
        return #Type; \
    }\
};
// clang-format on

DECLARE_CONTAINER_TYPE(vector);
DECLARE_CONTAINER_TYPE(list);
DECLARE_CONTAINER_TYPE(map);
DECLARE_CONTAINER_TYPE(set);

namespace UTL {
namespace Std {

template <typename T, typename Tag> struct vector : public std::vector<T, UTL::Std::Allocator<T, Tag> > {
  private:
    typedef std::vector<T, UTL::Std::Allocator<T, Tag> > _Base;

  public:
    void push_back(const typename _Base::value_type &_Val) {
        _Base::push_back(_Val);
    }

    typename _Base::iterator insert(typename _Base::iterator _Where, typename _Base::const_reference _Val) {
        return _Base::insert(_Where, _Val);
    }

    typename _Base::reference operator[](typename _Base::size_type _Pos) {
        return *(_Base::begin() + _Pos);
    }
};

template <typename T, typename Tag> struct list : public std::list<T, UTL::Std::Allocator<T, Tag> > {};
template <typename Key, typename T, typename Tag> struct map : public std::map<Key, T, std::less<Key>, UTL::Std::Allocator<T, Tag> > {};
template <typename T, typename Tag> struct set : public std::set<T, std::less<T>, UTL::Std::Allocator<T, Tag> > {};

// template <typename T, typename Tag> struct vector : public std::vector<T> {};
// template <typename Key, typename T, typename Tag> struct map : public std::map<Key, T> {};
// template <typename T, typename Tag> struct set : public std::set<T> {};

}; // namespace Std
}; // namespace UTL

#endif
