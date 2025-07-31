#ifndef SUPPORT_UTILITY_UTLALLOCATOR_H
#define SUPPORT_UTILITY_UTLALLOCATOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

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

    pointer allocate(std::size_t n, const void * = 0) {
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    void deallocate(pointer p, std::size_t) {
        ::operator delete(p);
    }
};

}; // namespace Std
}; // namespace UTL

#endif
