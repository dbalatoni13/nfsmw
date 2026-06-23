#ifndef SCRATCHPPTR_H
#define SCRATCHPPTR_H

#include <types.h>

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
    T **mRef; // offset 0x0, size 0x4

    static void *mWorkSpace;
    static T *mPointer[T::MaxInstances];
    static T mRAMBuffer[T::MaxInstances];
};

// UNSOLVED
template <typename T> T **ScratchPtr<T>::_Alloc() {
    for (int i = 0; i < sizeof(mPointer) / sizeof(T *); ++i) {
        if (mPointer[i] == nullptr) {
            T *spbuffer;
            if (mWorkSpace == nullptr) {
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

#define IMPLEMENT_SCRATCHPTR(DATATYPE)                                                                                                               \
    template <> void *ScratchPtr<DATATYPE>::mWorkSpace = NULL;                                                                                       \
    template <> ALIGNVEC DATATYPE ScratchPtr<DATATYPE>::mRAMBuffer[DATATYPE::MaxInstances] = {};                                                     \
    template <> ALIGNVEC DATATYPE *ScratchPtr<DATATYPE>::mPointer[DATATYPE::MaxInstances] = {};

#endif
