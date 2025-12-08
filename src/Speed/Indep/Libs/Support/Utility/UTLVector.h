#ifndef SUPPORT_UTILITY_UTLVECTOR_H
#define SUPPORT_UTILITY_UTLVECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
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
        Init();
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

    std::size_t indexof(pointer pos) {
        std::size_t index = pos - mBegin;
        return index;
    }

    iterator erase(iterator begIt, iterator endIt) {
        std::size_t iPos = indexof(begIt);
        std::size_t num = endIt - begIt;
        for (iterator it = begIt; it != endIt; ++it) {
            value_type &obj = *it;
            obj.~T();
        }

        for (std::size_t ii = 0; ii < size() - (iPos + num); ++ii) {
            std::size_t src = iPos + num + ii;
            std::size_t dest = iPos + ii;

            new (&mBegin[dest]) T(mBegin[src]);
        }
        mSize = size() - num;
        return end();
    }

    iterator erase(iterator pos) {
        if (pos == pos + 1) {
            return nullptr;
        }
        return erase(pos, pos + 1);
    }

  protected:
    virtual pointer AllocVectorSpace(std::size_t num, unsigned int alignment) {}

    virtual void FreeVectorSpace(pointer buffer, std::size_t num) {}

    virtual std::size_t GetGrowSize(std::size_t minSize) const {
        return UMath::Max(minSize, mCapacity + ((mCapacity + 1) >> 1)); // TODO is this right?
    }

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

    virtual typename Vector<T, Alignment>::pointer AllocVectorSpace(std::size_t num, unsigned int alignment) {}

    virtual void FreeVectorSpace(typename Vector<T, Alignment>::pointer buffer, std::size_t) {}

    virtual std::size_t GetMaxCapacity() const {}

  private:
    // TODO speed considerations for 64 bit
    int mVectorSpace[(sizeof(typename Vector<T, Alignment>::value_type) * Size) / sizeof(int)];
};

}; // namespace UTL

#endif
