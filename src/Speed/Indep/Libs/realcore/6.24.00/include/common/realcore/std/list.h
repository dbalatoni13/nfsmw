#ifndef REALCORE_COMMON_STD_LIST_H
#define REALCORE_COMMON_STD_LIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

struct ListNodeS {
    ListNodeS *next;
};

template <class T> class ListSingleIterator {
  private:
    T *node;

  public:
    ListSingleIterator() : node(nullptr) {}
    ListSingleIterator(T *_node) : node(_node) {}

    void operator++(int) {
        if (this->node != nullptr) {
            this->node = static_cast<T *>(this->node->next);
        }
    }

    bool operator!=(const ListSingleIterator<T> &rhs) const {
        return this->node != rhs.node;
    }

    T *operator*() {
        return this->node;
    }
};

template <class T> class ListSingle {
  private:
    T *head;
    T *tail;
    unsigned int nNodes;

  public:
    ListSingle();
    ~ListSingle();
    void Push(T *node);
    void PushTail(T *node);
    T *Pop();
    T *Head();
    unsigned int GetCount();
    void InsertAfter(T *node, T *newnode);
    bool Remove(T *node, T *prev);
    ListSingleIterator<T> Begin();

  private:
    void VerifyList();
};

#include "../impl/std/list.inl"

#endif
