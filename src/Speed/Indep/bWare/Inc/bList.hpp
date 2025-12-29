#ifndef BWARE_BLIST_H
#define BWARE_BLIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

class bNode {
  public:
    bNode *Next; // offset 0x0, size 0x4
    bNode *Prev; // offset 0x4, size 0x4

    bNode() {}

    ~bNode() {}

    bNode *GetNext() {
        return Next;
    }
    bNode *GetPrev() {
        return Prev;
    }

    bNode *AddBefore(bNode *insert_point) {
        bNode *new_prev = insert_point->Prev;
        bNode *new_next = insert_point->Next; // unused
        new_prev->Next = this;
        insert_point->Prev = this;
        this->Prev = new_prev;
        this->Next = insert_point;
        return this; // TODO
    }

    bNode *AddAfter(bNode *insert_point) {
        bNode *new_next = insert_point->Next;
        bNode *new_prev = this->Prev; // unused
        insert_point->Next = this;
        new_next->Prev = this;
        this->Prev = insert_point;
        this->Next = new_next;
        return this; // TODO
    }

    bNode *Remove() {
        bNode *next_node = this->Next;
        bNode *prev_node = this->Prev;
        prev_node->Next = next_node;
        next_node->Prev = prev_node;
        return this;
    }
};

template <typename T> struct bTNode : public bNode {
    bTNode() {}

    ~bTNode() {}

    T *GetNext() {
        return (T *)bNode::GetNext();
    }
    T *GetPrev() {
        return (T *)bNode::GetPrev();
    }
    T *AddAfter(T *insert_point) {
        return (T *)bNode::AddAfter(insert_point);
    }
    T *Remove() {
        return (T *)bNode::Remove();
    }
};

struct bList {
    bNode HeadNode; // offset 0x0, size 0x8

    typedef int (*SortFunc)(bNode *, bNode *);

  public:
    bList() {
        this->HeadNode.Next = &this->HeadNode;
        this->HeadNode.Prev = &this->HeadNode;
    }
    ~bList() {}

    bNode *GetNode(int ordinal_number);
    int TraversebList(bNode *match_node);
    void Sort(SortFunc check_flip);
    void MergeSort(SortFunc cmp);

    void InitList() {
        this->HeadNode.Next = &this->HeadNode;
        this->HeadNode.Prev = &this->HeadNode;
    }
    int IsEmpty() {
        return this->HeadNode.GetNext() == &this->HeadNode; // TODO
    }
    bNode *EndOfList() {
        return &this->HeadNode;
    }
    bNode *GetHead() {
        return this->HeadNode.GetNext();
    }
    bNode *GetTail() {
        return this->HeadNode.GetPrev();
    }
    bNode *GetNextCircular(bNode *node); // TODO
    bNode *GetPrevCircular(bNode *node); // TODO
    bNode *AddHead(bNode *node) {
        return node->AddAfter(&this->HeadNode);
    }
    bNode *AddTail(bNode *node) {
        return node->AddBefore(&this->HeadNode);
    }
    bNode *AddBefore(bNode *insert_point, bNode *node);
    bNode *AddAfter(bNode *insert_point, bNode *node); // TODO
    bNode *Remove(bNode *node) {
        return node->Remove();
    }
    bNode *RemoveHead() {
        return this->GetHead()->Remove();
    }
    bNode *RemoveTail();            // TODO
    int GetNodeNumber(bNode *node); // TODO
    int IsInList(bNode *node) {
        return this->TraversebList(node);
    }
    int CountElements();                   // TODO
    bNode *AddSorted(SortFunc check_flip); // TODO
};

template <typename T> class bTList : public bList {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;

    typedef int (*SortFuncT)(pointer, pointer);

    bTList() {}

    ~bTList() {
        while (!this->IsEmpty()) {
            delete this->RemoveHead();
        }
    }

    T *EndOfList() {
        return (T *)bList::EndOfList();
    }

    T *GetHead() {
        return (T *)bList::GetHead();
    }

    T *GetTail() {
        return (T *)bList::GetTail();
    }

    T *GetNextCircular(bNode *node) {
        return (T *)bList::GetNextCircular(node);
    }

    T *GetPrevCircular(bNode *node) {
        return (T *)bList::GetPrevCircular(node);
    }

    T *AddHead(bNode *node) {
        return (T *)bList::AddHead(node);
    }

    T *AddTail(bNode *node) {
        return (T *)bList::AddTail(node);
    }

    T *AddBefore(bNode *insert_point, bNode *node) {
        return (T *)bList::AddBefore(insert_point, node);
    }

    T *AddAfter(bNode *insert_point, bNode *node) {
        return (T *)bList::AddAfter(insert_point, node);
    }

    T *Remove(bNode *node) {
        return (T *)bList::Remove(node);
    }

    T *RemoveHead() {
        return (T *)bList::RemoveHead();
    }

    T *RemoveTail() {
        return (T *)bList::RemoveTail();
    }

    T *AddSorted(SortFuncT check_flip) {
        // TODO
    }

    T *GetNode(int ordinal_number) {
        return (T *)bList::GetNode(ordinal_number);
    }

    void Sort(SortFuncT check_flip) {
        bList::Sort((SortFunc)check_flip);
    }

    void DeleteAllElements() {
        while (!this->IsEmpty()) {
            delete this->RemoveHead();
        }
    }

    // total size: 0x8
    class iterator {
      private:
        pointer _Ptr; // offset 0x0, size 0x4
        bList *_Lst;  // offset 0x4, size 0x4

        void validate() {}

      public:
        iterator() {}

        iterator(pointer ptr, bList *list) {
            _Ptr = ptr;
            _Lst = list;
        }

        iterator &operator--() {
            validate();
            return _Ptr->GetPrev();
        }

        iterator &operator--(int) {
            validate();
            iterator tmp = _Ptr;
            _Ptr = _Ptr->GetPrev();
            return tmp;
        }

        iterator &operator++() {
            validate();
            return _Ptr->GetNext();
        }

        iterator &operator++(int) {
            validate();
            iterator tmp = _Ptr;
            _Ptr = _Ptr->GetNext();
            return tmp;
        }

        pointer &operator*() {
            validate();
            return _Ptr;
        }

        bool operator==(const iterator &rhs) {
            return _Ptr == rhs._Ptr;
        }

        bool operator!=(const iterator &rhs) {
            return _Ptr != rhs._Ptr;
        }
    };

    iterator begin() {
        return iterator(GetHead(), this);
    }

    iterator end() {
        return iterator(EndOfList(), this);
    }
};

// total size: 0xC
class bPNode : public bTNode<bPNode> {
  public:
    static void *Malloc();
    static void Free(void *ptr);

    bPNode(void *object) {
        this->Object = object;
    }

    bPNode *GetObject() {
        return reinterpret_cast<bPNode *>(Object);
    }

    void *GetpObject() {
        return Object;
    }

    void *operator new(size_t size) {
        return Malloc();
    }

    void operator delete(void *ptr) {
        Free(ptr);
    }

  private:
    void *Object; // offset 0x8, size 0x4
};

template <typename T> class bPList : public bTList<bPNode> {
  public:
    bPNode *AddHead(T *object) {
        return (bPNode *)bList::AddHead(new bPNode(object));
    }

    bPNode *AddTail(T *object) {
        return (bPNode *)bList::AddTail(new bPNode(object));
    }

    void Remove(bNode *node) {
        bList::Remove(node);
        delete node;
    }

    void RemoveHead() {
        delete reinterpret_cast<bPNode *>(bList::RemoveHead());
    }

    void RemoveTail() {
        bList::RemoveTail();
    }
};

template <typename T> class bSNode {
  public:
    T *GetNext() {
        return Next;
    }

  private:
    T *Next;
};

template <typename T> class bSList {
  public:
    // Functions
    bSList() {}

    int IsEmpty() {}

    T *RemoveHead() {}

    ~bSList() {}

    T *GetHead() {
        return Head;
    }

    T *EndOfList() {
        return (T *)this;
    }

    T *AddTail(T *node) {}

  private:
    T *Head; // offset 0x0, size 0x4
    T *Tail; // offset 0x4, size 0x4
};

void bPListInit(int num_expected_bpnodes);

#endif
