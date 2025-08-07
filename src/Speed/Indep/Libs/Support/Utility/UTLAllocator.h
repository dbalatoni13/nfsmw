#ifndef SUPPORT_UTILITY_UTLALLOCATOR_H
#define SUPPORT_UTILITY_UTLALLOCATOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/bWare/Inc/bMemory.hpp"

namespace UTL {
namespace Std {
template <typename T, typename Tag> struct Allocator {
    typedef T value_type;
    typedef T *pointer;

    template <typename U> struct rebind {
        typedef Allocator<U, Tag> other;
    };

    Allocator() {}
    template <typename U, typename V> Allocator(const Allocator<U, Tag> &) {}

    pointer allocate(std::size_t n) {
        if (n != 0) {
            return reinterpret_cast<pointer>(gFastMem.Alloc(n * sizeof(T), NULL));
        }
        return nullptr;
    }

    pointer allocate(std::size_t n, std::size_t, std::size_t) {
        if (n != 0) {
            return reinterpret_cast<pointer>(gFastMem.Alloc(n * sizeof(T), NULL));
        }
        return nullptr;
    }

    void deallocate(pointer p, std::size_t n) {
        if (p) {
            gFastMem.Free(p, n * sizeof(T), nullptr);
        }
    }
};

}; // namespace Std
}; // namespace UTL

#endif
