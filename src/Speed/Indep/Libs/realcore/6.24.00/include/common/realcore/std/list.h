#ifndef REALCORE_COMMON_STD_LIST_H
#define REALCORE_COMMON_STD_LIST_H

// total size: 0x4
struct ListNodeS {
    ListNodeS *next; // offset 0x0, size 0x4
};

template <class T> struct ListSingle;
template <class T> struct ListSingleIterator;

// Relleno hasta la linea 87: el mapa de lineas del volcado situa el ctor de
// ListSingle en 89-91 y su dtor en 96-111.
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// total size: 0xC
template <class T> struct ListSingle {
    ListSingle() {
        head = tail = 0;
        nNodes = 0;
    }

    ~ListSingle() {
        ListNodeS *node;
        ListNodeS *nextnode;

        node = head;
        nextnode = node;

        while (node != 0 && node->next != 0) {
            nextnode = node->next;
            node->next = 0;
            node = nextnode;
        }

        head = tail = 0;
        nNodes = 0;
    }

    T *Head() {
        return head;
    }

    ListSingleIterator<T> Begin();

    T *Pop();
    void Push(T *node);
    void PushTail(T *node);
    void InsertAfter(T *node, T *newnode);
    bool Remove(T *node, T *prev);

    T *head;             // offset 0x0, size 0x4
    T *tail;             // offset 0x4, size 0x4
    unsigned int nNodes; // offset 0x8, size 0x4
};

// Relleno hasta la linea 170: operator++ del iterador cae en 176-177.
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// total size: 0x4
template <class T> struct ListSingleIterator {
    ListSingleIterator() {
        node = 0;
    }

    ListSingleIterator(T *_node) {
        node = _node;
    }

    T *operator*() {
        return node;
    }

    void operator++(int) {
        if (node != 0) {
            node = (T *)node->next;
        }
    }

    T *node; // offset 0x0, size 0x4
};

template <class T> inline ListSingleIterator<T> ListSingle<T>::Begin() {
    return ListSingleIterator<T>(head);
}

#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/impl/std/list.inl"

#endif
