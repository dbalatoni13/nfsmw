#ifndef SUPPORT_UTILITY_UQUEUE_H
#define SUPPORT_UTILITY_UQUEUE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

template <typename T, int U> class UCircularQueue {
    int Size;      // offset 0x0, size 0x4
    int Head;      // offset 0x4, size 0x4
    int Tail;      // offset 0x8, size 0x4
    int MaxSize;   // offset 0xC, size 0x4
    T Elements[U]; // offset 0x10, size 0x258

  public:
    // UCircularQueue() {}

    // void enqueue(const T &insert) {}

    // void dequeue() {}

    // T &tail() {}

    // T &head() {}

    // T &operator[](int i) {
    // int newindex;
    // }

    // void reset() {}

    // int size() const {}
};

#endif
