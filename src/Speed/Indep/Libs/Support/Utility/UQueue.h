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
    T Elements[U]; // offset 0x10, size varies

  public:
    UCircularQueue() : Size(0), Head(-1), Tail(0), MaxSize(U) {}

    void enqueue(const T &insert) {
        Head = Head + 1;
        if (Head > MaxSize - 1) {
            Head = 0;
        }
        Size = Size + 1;
        if (Size > MaxSize) {
            Tail = Tail + 1;
            if (Tail > MaxSize - 1) {
                Tail = 0;
            }
            Size = MaxSize;
        }
        Elements[Head] = insert;
    }

    void dequeue() {
        Tail = Tail + 1;
        if (Tail > MaxSize - 1) {
            Tail = 0;
        }
        Size = Size - 1;
    }

    T &tail() {
        return Elements[Tail];
    }

    T &head() {
        return Elements[Head];
    }

    T &operator[](int i) {
        int newindex = Head - i;
        if (newindex < 0) {
            newindex = newindex + MaxSize;
        }
        return Elements[newindex];
    }

    void reset() {
        Size = 0;
        Head = -1;
        Tail = 0;
    }

    int size() const {
        return Size;
    }
};

#endif
