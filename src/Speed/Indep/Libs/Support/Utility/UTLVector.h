#ifndef SUPPORT_UTILITY_UTLVECTOR_H
#define SUPPORT_UTILITY_UTLVECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

namespace UTL {
template <typename T, unsigned int Alignment = 16> class Vector {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer; // TODO is it value_type const ?
    typedef value_type *iterator;
    typedef value_type const *const_iterator;

  public:
    void Init() {}

    Vector() {
        this->Init();
    }

    virtual ~Vector() {}

    std::size_t capacity() const {
        return mCapacity;
    }

    std::size_t size() const {
        return mSize;
    }

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

    void reserve(std::size_t num) {
        if (num > capacity()) {
            OnGrowRequest(num);
            pointer oldBuffer = mBegin;
            int oldSize = size();
            int oldCapacity = capacity();

            mBegin = AllocVectorSpace(num, Alignment);
            mCapacity = num;
            if (oldBuffer != mBegin) {
                mSize = 0;
                for (int ii = 0; ii < oldSize; ++ii) {
                    push_back(oldBuffer[ii]);
                }
                if (oldBuffer) {
                    FreeVectorSpace(oldBuffer, oldCapacity);
                }
            }
        }
    }

    void pop_back() {}

    void push_back(value_type const &val) {
        if (size() >= capacity()) {
            reserve(GetGrowSize(size() + 1));
        }
        new (&mBegin[size()]) T(val);
        mSize++;
    }

    void make_empty() {}

    void clear() {}

    std::size_t indexof(pointer pos) {}

    pointer erase(iterator begIt, iterator endIt) {}

  protected:
    virtual pointer AllocVectorSpace(std::size_t num, unsigned int alignment) {}

    virtual void FreeVectorSpace(pointer buffer, std::size_t num) {}

    virtual std::size_t GetGrowSize(std::size_t minSize) const {}

    virtual std::size_t GetMaxCapacity() const {}

    virtual void OnGrowRequest(std::size_t newSize) {}

  private:
    // total size: 0x10
    pointer mBegin;        // offset 0x0, size 0x4
    std::size_t mCapacity; // offset 0x4, size 0x4
    std::size_t mSize;     // offset 0x8, size 0x4
};

template <typename T, std::size_t Size, unsigned int Alignment = 16> class FixedVector : public Vector<T, Alignment> {
  public:
    FixedVector() {}

    virtual ~FixedVector() {}

  protected:
    virtual std::size_t GetGrowSize(std::size_t minSize) const {}

    virtual pointer AllocVectorSpace(std::size_t num, unsigned int alignment) {}

    virtual void FreeVectorSpace(pointer buffer, std::size_t) {}

    virtual std::size_t GetMaxCapacity() const {}

  private:
    // TODO speed considerations for 64 bit
    int mVectorSpace[(sizeof(value_type) * Size) / sizeof(int)];
};

}; // namespace UTL

#endif
