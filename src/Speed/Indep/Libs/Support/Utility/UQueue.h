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
    UCircularQueue() {
        this->Size = 0;
        this->Head = -1;
        this->Tail = 0;
        this->MaxSize = 50;
    }

    void enqueue(const T &insert) {
        if (++this->Head > this->MaxSize - 1) {
            this->Head = 0;
        }

        if (++this->Size > this->MaxSize) {
            if (++this->Tail > this->MaxSize - 1) {
                this->Tail = 0;
            }
            this->Size = this->MaxSize;
        }

        this->Elements[this->Head] = insert;
    }

    void dequeue() {
        if (++this->Tail > this->MaxSize - 1) {
            this->Tail = 0;
        }
        this->Size--;
    }

    T &tail() {
        return this->Elements[this->Tail];
    }

    T &head() {
        return this->Elements[this->Head];
    }

    // T &operator[](int i) {
    // int newindex;
    // }

    void reset() {
        this->Size = 0;
        this->Head = -1;
        this->Tail = 0;
    }

    int size() const {
        return this->Size;
    }
};

#endif
