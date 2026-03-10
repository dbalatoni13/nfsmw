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
    UCircularQueue() : Size(0), Head(0), Tail(0), MaxSize(U) {}

    void enqueue(const T &insert) {
        Elements[Head] = insert;
        int new_head = Head + 1;
        Head = new_head;
        if (MaxSize - 1 < new_head) {
            Head = 0;
        }
        int new_size = Size + 1;
        Size = new_size;
        if (MaxSize < new_size) {
            int new_tail = Tail + 1;
            Tail = new_tail;
            if (MaxSize - 1 < new_tail) {
                Tail = 0;
            }
            Size = MaxSize;
        }
    }

    void dequeue() {
        int new_tail = Tail + 1;
        Tail = new_tail;
        if (MaxSize - 1 < new_tail) {
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
        Head = 0;
        Tail = 0;
    }

    int size() const {
        return Size;
    }
};

#endif
