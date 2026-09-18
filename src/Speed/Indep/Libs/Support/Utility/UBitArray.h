#ifndef UBITARRAY_H
#define UBITARRAY_H

#include "Speed/Indep/bWare/Inc/bTypes.hpp"

template <typename T, int N> struct BitArray {
    static const int kBitsPerWord = sizeof(T) * 8; // TODO this doesn't exist
    static const int kNumWords = (N + kBitsPerWord - 1) / kBitsPerWord;
    T Words[(N + kBitsPerWord - 1) / kBitsPerWord];

    BitArray() {
        for (unsigned int i = 0; i < NUM_ELEMENTS(this->Words); i++) {
            Words[i] = 0;
        }
    }

    BitArray(const BitArray &src) {
        for (unsigned int i = 0; i < NUM_ELEMENTS(this->Words); i++) {
            Words[i] = src.Words[i];
        }
    }

    const BitArray &operator=(const BitArray &src) {
        for (unsigned int i = 0; i < NUM_ELEMENTS(this->Words); i++) {
            Words[i] = src.Words[i];
        }
        return *this;
    }

    bool operator!=(const BitArray &other) const {
        for (int i = 0; i < NUM_ELEMENTS(this->Words); i++) {
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
        for (unsigned int i = 0; i < NUM_ELEMENTS(this->Words); i++) {
            Words[i] = 0;
        }
    }

    bool AnySet() const {
        for (int i = 0; i < kNumWords; i++) {
            if (Words[i] != 0) {
                return true;
            }
        }
        return false;
    }
};

#endif
