#ifndef MAIN_SCRATCHPTR_H
#define MAIN_SCRATCHPTR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

template <typename T> class ScratchPtr {
  public:
    T &operator*() {
        return **mRef;
    }

    T *operator->() {
        return *mRef;
    }

    ~ScratchPtr();

  private:
    // total size: 0x4
    T **mRef; // offset 0x0, size 0x4
};

#endif
