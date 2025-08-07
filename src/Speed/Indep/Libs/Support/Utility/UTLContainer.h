#ifndef SUPPORT_UTILITY_CONTAINER_H
#define SUPPORT_UTILITY_CONTAINER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <map>
#include <set>
#include <vector>

#include "UTLAllocator.h"

struct _type_vector {
    const char *name() {
        return "vector";
    };
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
namespace Container {
template <typename T, typename Tag> struct vector : public std::vector<T, UTL::Std::Allocator<T, Tag> > {};
// template <typename Key, typename T, typename Tag> struct map : public std::map<Key, T, std::less<Key>, UTL::Std::Allocator<T, _type_vector> > {};
// template <typename T, typename Tag> struct set : public std::set<T, std::less<T>, UTL::Std::Allocator<T, _type_set> > {};

// temporary
// template <typename T, typename Tag> struct vector : public std::vector<T> {};
template <typename Key, typename T, typename Tag> struct map : public std::map<Key, T> {};
template <typename T, typename Tag> struct set : public std::set<T> {};

}; // namespace Container
}; // namespace UTL

#endif
