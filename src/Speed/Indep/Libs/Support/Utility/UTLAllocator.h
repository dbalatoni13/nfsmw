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
    typedef std::size_t size_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef std::ptrdiff_t difference_type;

    template <typename U> struct rebind {
        typedef Allocator<U, Tag> other;
    };

    Allocator() {}
    ~Allocator() {}
    template <typename U> Allocator(const Allocator<U, Tag> &) {}

    pointer allocate(std::size_t n) {
        return n != 0 ? reinterpret_cast<pointer>(gFastMem.Alloc(n * sizeof(T), nullptr)) : nullptr;
    }

    void deallocate(pointer p, std::size_t n) {
        if (p) {
            gFastMem.Free(p, n * sizeof(T), nullptr);
        }
    }

    // TODO are these three right?
    size_type max_size() const {
        return static_cast<size_type>(-1) / sizeof(value_type);
    }

    void construct(pointer p, const_reference value) {
        new ((void *)p) value_type(value); // placement new
    }

    void destroy(pointer p) {
        p->~T();
    }
};

}; // namespace Std
}; // namespace UTL

#endif
