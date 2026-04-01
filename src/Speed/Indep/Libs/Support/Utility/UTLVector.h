#ifndef SUPPORT_UTILITY_UTLVECTOR_H
#define SUPPORT_UTILITY_UTLVECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

namespace UTL {
template <typename T> class RepeatSequencer {
  public:
    explicit RepeatSequencer(const T &t)
        : mValue(&t) {}

    const T &operator[](unsigned int) const {
        return *mValue;
    }

  private:
    const T *mValue;
};

template <typename T, int Alignment = 16> class Vector {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef value_type *iterator;
    typedef value_type *reverse_iterator;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef const value_type *const_iterator;
    typedef const value_type *const_reverse_iterator;
    typedef unsigned int size_type;
    typedef int difference_type;

  public:
    void Init() {}

    Vector() {
        mBegin = nullptr;
        mCapacity = 0;
        mSize = 0;
        Init();
    }

    virtual ~Vector() {}

    size_type capacity() const {
        return mCapacity;
    }

    size_type size() const {
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

    void push_back(value_type const &val) {
        if (size() >= capacity()) {
            reserve(GetGrowSize(size() + 1));
        }
        new (&mBegin[size()]) T(val);
        mSize++;
    }

    iterator insert(iterator position, const_reference val) {
        size_type posIndex = position - mBegin;
        RepeatSequencer<const value_type> sequencer(val);
        insert_sequence(position, 1, sequencer);
        return mBegin + posIndex;
    }

    void pop_back() {
        mSize = size() - 1;
    }

    reference operator[](size_type idx) {
        return mBegin[idx];
    }

    const_reference operator[](size_type idx) const {
        return mBegin[idx];
    }

    void reserve(size_type num) {
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

    void make_empty() {
        int num = size();
        for (int ii = 0; ii < num; ii++) {
            pop_back();
        }

        if (mBegin) {
            FreeVectorSpace(mBegin, mCapacity);
            mBegin = nullptr;
            mCapacity = 0;
            mSize = 0;
        }
    }

    void clear() {
        make_empty();
        Init();
    }

    size_type indexof(pointer pos) {
        size_type index = pos - mBegin;
        return index;
    }

    iterator erase(iterator begIt, iterator endIt) {
        size_type iPos = indexof(begIt);
        size_type num = endIt - begIt;
        for (iterator it = begIt; it != endIt; ++it) {
            value_type &obj = *it;
            obj.~T();
        }

        for (size_type ii = 0; ii < size() - (iPos + num); ++ii) {
            size_type src = iPos + num + ii;
            size_type dest = iPos + ii;

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
    void insert_sequence(iterator pos, unsigned int num, RepeatSequencer<const value_type> &sequencer) {
        pointer oldBuffer = mBegin;
        size_type oldSize = size();
        size_type oldCapacity = capacity();
        size_type iPos = pos - oldBuffer;
        size_type newSize = oldSize + num;

        if (newSize > oldCapacity) {
            OnGrowRequest(newSize);

            oldBuffer = mBegin;
            oldSize = size();
            oldCapacity = capacity();
            iPos = pos - oldBuffer;
            newSize = oldSize + num;

            if (newSize > oldCapacity) {
                mBegin = AllocVectorSpace(newSize, Alignment);
                mCapacity = newSize;
            }
        }

        mSize = newSize;

        if (oldBuffer && oldBuffer != mBegin) {
            for (size_type ii = 0; ii < iPos; ++ii) {
                pointer dst = mBegin + ii;
                if (dst) {
                    new (dst) T(oldBuffer[ii]);
                }
            }
        }

        if (oldSize != iPos) {
            for (size_type ii = 0; ii < oldSize - iPos; ++ii) {
                pointer dst = mBegin + (newSize - ii);
                --dst;
                if (dst) {
                    pointer src = oldBuffer + (oldSize - ii);
                    --src;
                    new (dst) T(*src);
                }
            }
        }

        for (size_type ii = 0; ii < num; ++ii) {
            pointer dst = mBegin + iPos + ii;
            if (dst) {
                new (dst) T(sequencer[ii]);
            }
        }

        if (oldBuffer && oldBuffer != mBegin) {
            FreeVectorSpace(oldBuffer, oldCapacity);
        }
    }

    // Unfinished
    virtual pointer AllocVectorSpace(size_type num, unsigned int alignment) {
        return nullptr;
    }

    virtual void FreeVectorSpace(pointer buffer, size_type num) {}

    virtual size_type GetGrowSize(size_type minSize) const {
        size_type grown = mCapacity + ((mCapacity + 1) >> 1);
        return grown >= minSize ? grown : minSize;
    }

    // Unfinished
    virtual size_type GetMaxCapacity() const {
        return 0x7fffffff;
    }

    virtual void OnGrowRequest(size_type newSize) {}

  private:
    // total size: 0x10
    pointer mBegin;      // offset 0x0, size 0x4
    size_type mCapacity; // offset 0x4, size 0x4
    size_type mSize;     // offset 0x8, size 0x4
};

template <typename T, int Size, int Alignment = 16> class FixedVector : public Vector<T, Alignment> {
  public:
    FixedVector() {}

    ~FixedVector() override {
        // clang is being annoying
        Vector<T, Alignment>::clear();
    }

    // TODO also put the typedefs here according to the dwarf?

  protected:
    // Unfinished
    virtual unsigned int GetGrowSize(unsigned int minSize) const {
        (void)minSize;
        return Size;
    }

    // Unfinished
    virtual typename Vector<T, Alignment>::pointer AllocVectorSpace(unsigned int num, unsigned int alignment) {
        (void)num;
        (void)alignment;
        return reinterpret_cast<typename Vector<T, Alignment>::pointer>(mVectorSpace);
    }

    virtual void FreeVectorSpace(typename Vector<T, Alignment>::pointer buffer, unsigned int) {}

    // Unfinished
    virtual unsigned int GetMaxCapacity() const {
        return Size;
    }

  private:
    // TODO speed considerations for 64 bit
    int mVectorSpace[(sizeof(typename Vector<T, Alignment>::value_type) * Size) / sizeof(int)];
};

}; // namespace UTL

#endif
