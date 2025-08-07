#ifndef SUPPORT_UTILITY_ULISTABLE_H
#define SUPPORT_UTILITY_ULISTABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

namespace UTL {
namespace Collections {

template <typename T, std::size_t U> struct Listable {
    struct List {};

    typedef void (*ForEachFunc_t)(T *);

    Listable() {}

    ~Listable() {}

    void Unlist() {}

    ForEachFunc_t ForEach(ForEachFunc_t f) {}

    const List &GetList() {}
};

template <typename T, std::size_t Tsize, typename U, std::size_t Usize> struct ListableSet {
    struct List {};

    typedef void (*ForEachFunc_t)(T *);

    T *First(U idx);
    T *Last(U idx);

    ~ListableSet() {}

    void UnList() {}

    T *Next(U idx) {}

    ForEachFunc_t ForEach(U idx, ForEachFunc_t f) {}

    void AddToList(U to) {}

    const List &GetList(U idx) {}
};

}; // namespace Collections
}; // namespace UTL

#endif
