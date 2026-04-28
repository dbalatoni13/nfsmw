#ifndef UTILITY_UBITARRAY_H
#define UTILITY_UBITARRAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

template <typename T, int N>
struct BitArray {
    static const int kBitsPerWord = sizeof(T) * 8;
    static const int kWordCount = (N + kBitsPerWord - 1) / kBitsPerWord;

    T Words[kWordCount];

    BitArray() {
        for (int i = 0; i < kWordCount; i++) {
            Words[i] = 0;
        }
    }

    const BitArray &operator=(const BitArray &src) {
        for (unsigned int i = 0; i < static_cast<unsigned int>(kWordCount); i++) {
            Words[i] = src.Words[i];
        }
        return *this;
    }

    bool operator!=(const BitArray &other) const {
        for (int i = 0; i < kWordCount; i++) {
            if (Words[i] != other.Words[i]) {
                return true;
            }
        }
        return false;
    }

    bool Test(unsigned int index) const {
        return (Words[index / kBitsPerWord] >> (index % kBitsPerWord)) & 1;
    }

    void Set(unsigned int index) {
        Words[index / kBitsPerWord] |= static_cast<T>(1) << (index % kBitsPerWord);
    }

    void Clear(unsigned int index) {
        Words[index / kBitsPerWord] &= ~(static_cast<T>(1) << (index % kBitsPerWord));
    }

    void Clear() {
        for (int i = 0; i < kWordCount; i++) {
            Words[i] = 0;
        }
    }
};

#endif
