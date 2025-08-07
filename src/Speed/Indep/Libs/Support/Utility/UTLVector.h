#ifndef SUPPORT_UTILITY_UTLVECTOR_H
#define SUPPORT_UTILITY_UTLVECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

namespace UTL {
template <typename T, int Alignment> class Vector {
    // total size: 0x10
    T *mBegin;             // offset 0x0, size 0x4
    std::size_t mCapacity; // offset 0x4, size 0x4
    std::size_t mSize;     // offset 0x8, size 0x4
                           // _vptr; // offset 0xC, size 0x4

  public:
    void Init() {}

    Vector() {
        this->Init();
    }

    virtual ~Vector() {}

    std::size_t capacity() const {}

    std::size_t size() const {}

    void pop_back() {}

    void push_back(T *const &val) {}

    void make_empty() {}

    void clear() {}

    T *const *begin() const {}

    T *const *end() const {}

    virtual std::size_t GetGrowSize(std::size_t minSize) const {}

    virtual void OnGrowRequest(std::size_t newSize) {}

    virtual T **VectorAllocVectorSpace(unsigned int num, std::size_t alignment) {}

    virtual void FreeVectorSpace(T **buffer, unsigned int num) {}

    virtual std::size_t GetMaxCapacity() const {}

    void reserve(std::size_t num) {}

    std::size_t indexof(T **pos) {}

    T **erase(T **begIt, T **endIt) {}
};

template <typename T, std::size_t Size, int Alignment> class FixedVector : public Vector<T, Alignment> {
    // TODO speed considerations for 64 bit
    int mVectorSpace[(sizeof(T) * Size) / sizeof(int)];

  public:
    FixedVector() {}

    virtual ~FixedVector() {}

    virtual std::size_t GetGrowSize(std::size_t minSize) const {}

    virtual T **AllocVectorSpace(unsigned int num, std::size_t alignment) {}

    virtual void FreeVectorSpace(T **buffer, unsigned int num) {}

    virtual std::size_t GetMaxCapacity() const {}
};

template <typename T, std::size_t Size> class _Storage : public FixedVector<T, Size, 16> {
    _Storage() {}

    ~_Storage() {}
};

}; // namespace UTL

#endif
