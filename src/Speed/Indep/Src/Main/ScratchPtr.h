#ifndef MAIN_SCRATCHPTR_H
#define MAIN_SCRATCHPTR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "dolphin/types.h"

// total size: 0x4
template <typename T> class ScratchPtr {
  public:
    static void Push(void *workspace);
    static void Pop();

    T **_Alloc();
    ScratchPtr();
    ~ScratchPtr();

    const T &operator*() const {
        return **mRef;
    }

    T &operator*() {
        return **mRef;
    }

    const T *operator->() const {
        return *mRef;
    }

    T *operator->() {
        return *mRef;
    }

  private:
    static void *mWorkSpace;
    static T *mPointer[64]; // TODO how to get the correct size based on the class??
    static T mRAMBuffer[64];

    T **mRef; // offset 0x0, size 0x4
};

// UNSOLVED
template <typename T> T **ScratchPtr<T>::_Alloc() {
    for (int i = 0; i < sizeof(mPointer) / sizeof(T *); ++i) {
        if (!mPointer[i]) {
            T *spbuffer;
            if (!mWorkSpace) {
                spbuffer = &mRAMBuffer[i];
            } else {
                spbuffer = &reinterpret_cast<T *>(mWorkSpace)[i];
            }
            // TODO
            mPointer[i] = spbuffer;
            return &mPointer[i];
        }
    }
    return nullptr;
}

template <typename T> ScratchPtr<T>::ScratchPtr() {
    mRef = nullptr;
    mRef = _Alloc();
    new (*mRef) T();
}

template <typename T> void ScratchPtr<T>::Push(void *workspace) {}

template <typename T> void ScratchPtr<T>::Pop() {}

#endif
