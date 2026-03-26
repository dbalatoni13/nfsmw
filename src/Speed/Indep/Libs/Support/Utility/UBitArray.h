#ifndef SUPPORT_UTILITY_UBITARRAY_H
#define SUPPORT_UTILITY_UBITARRAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

template < typename T, int N > struct BitArray {
    // Number of bits per word
    enum { BITS_PER_WORD = sizeof(T) * 8 };
    // Number of words needed
    enum { NUM_WORDS = (N + BITS_PER_WORD - 1) / BITS_PER_WORD };

    T Words[NUM_WORDS];

    BitArray() {
        for (unsigned int i = 0; i < NUM_WORDS; i++) {
            Words[i] = 0;
        }
    }

    BitArray(const BitArray &src) {
        for (unsigned int i = 0; i < NUM_WORDS; i++) {
            Words[i] = src.Words[i];
        }
    }

    const BitArray &operator=(const BitArray &src) {
        for (unsigned int i = 0; i < NUM_WORDS; i++) {
            Words[i] = src.Words[i];
        }
        return *this;
    }

    void Set(unsigned int index) {
        Words[index >> 5] |= (1 << (index & 0x1f));
    }

    void Clear(unsigned int index) {
        Words[index >> 5] &= ~(1 << (index & 0x1f));
    }

    bool Test(unsigned int index) const {
        return (Words[index >> 5] >> (index & 0x1f)) & 1;
    }

    bool Test() const {
        for (int i = 0; i < static_cast< int >(NUM_WORDS); i++) {
            if (Words[i]) {
                return true;
            }
        }
        return false;
    }

    void Clear() {
        for (unsigned int i = 0; i < NUM_WORDS; i++) {
            Words[i] = 0;
        }
    }
};

#endif
