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

struct _type_vector {
    const char *name() {
        return "vector";
    };
};

struct _type_list {
    const char *name() {
        return "list";
    }
};

struct _type_map {
    const char *name() {
        return "map";
    };
};

struct _type_set {
    const char *name() {
        return "set";
    }
};

namespace UTL {
namespace Std {

template <typename T, typename Tag> struct vector : public std::vector<T, UTL::Std::Allocator<T, Tag> > {
    void push_back(const typename std::vector<T, UTL::Std::Allocator<T, Tag> >::value_type &_Val) {
        std::vector<T, UTL::Std::Allocator<T, Tag> >::push_back(_Val);
    }

    typename std::vector<T, UTL::Std::Allocator<T, Tag> >::reference operator[](size_t _Pos) {
        return *(std::vector<T, UTL::Std::Allocator<T, Tag> >::begin() + _Pos);
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
