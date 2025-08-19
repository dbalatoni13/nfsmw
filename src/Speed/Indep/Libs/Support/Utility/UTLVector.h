#ifndef SUPPORT_UTILITY_UTLVECTOR_H
#define SUPPORT_UTILITY_UTLVECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

namespace UTL {
template <typename T, int Alignment = 16> class Vector {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type *iterator;
    typedef const value_type *const_iterator;

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

    const_iterator begin() const {
        return mBegin;
    }

    iterator begin() {
        return mBegin;
    }

    const_iterator end() const {
        return mBegin + mSize;
    }

    iterator end() {
        return mBegin + mSize;
    }

    virtual std::size_t GetGrowSize(std::size_t minSize) const {}

    virtual void OnGrowRequest(std::size_t newSize) {}

    virtual T **VectorAllocVectorSpace(std::size_t num, unsigned int alignment) {}

    virtual void FreeVectorSpace(T **buffer, unsigned int num) {}

    virtual std::size_t GetMaxCapacity() const {}

    void reserve(std::size_t num) {}

    std::size_t indexof(T **pos) {}

    T **erase(T **begIt, T **endIt) {}

  private:
    // total size: 0x10
    T *mBegin;             // offset 0x0, size 0x4
    std::size_t mCapacity; // offset 0x4, size 0x4
    std::size_t mSize;     // offset 0x8, size 0x4
};

template <typename T, std::size_t Size, int Alignment = 16> class FixedVector : public Vector<T, Alignment> {
    // TODO speed considerations for 64 bit
    int mVectorSpace[(sizeof(T) * Size) / sizeof(int)];

  public:
    FixedVector() {}

    virtual ~FixedVector() {}

    virtual std::size_t GetGrowSize(std::size_t minSize) const {}

    virtual T **AllocVectorSpace(std::size_t num, unsigned int alignment) {}

    virtual void FreeVectorSpace(T **buffer, std::size_t) {}

    virtual std::size_t GetMaxCapacity() const {}
};

}; // namespace UTL

#endif
