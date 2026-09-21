#ifndef RCMP_MVRCON_H
#define RCMP_MVRCON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// Intrusive list primitives from the original RCMP mvrcon.h owner.
namespace RCMP {

struct DLNode {
    inline DLNode()
        : Next(reinterpret_cast<DLNode *>(3))
        , Prev(reinterpret_cast<DLNode *>(3)) {
    }

    inline ~DLNode() {
        this->Next = reinterpret_cast<DLNode *>(7);
        this->Prev = reinterpret_cast<DLNode *>(7);
    }

    inline DLNode *GetNext() {
        return this->Next;
    }

    inline DLNode *GetPrev() {
        return this->Prev;
    }

    inline DLNode *Remove() {
        DLNode *next_node;
        DLNode *prev_node;

        next_node = this->Next;
        prev_node = this->Prev;
        prev_node->Next = next_node;
        next_node->Prev = prev_node;
        this->Next = reinterpret_cast<DLNode *>(11);
        this->Prev = reinterpret_cast<DLNode *>(11);
        return this;
    }

    inline DLNode *AddAfter(DLNode *insert_point) {
        DLNode *new_prev;
        DLNode *new_next;

        new_prev = insert_point;
        new_next = insert_point->Next;
        new_prev->Next = this;
        new_next->Prev = this;
        this->Next = new_next;
        this->Prev = new_prev;
        return this;
    }

    inline DLNode *AddBefore(DLNode *insert_point) {
        DLNode *new_next;
        DLNode *new_prev;

        new_next = insert_point;
        new_prev = new_next->Prev;
        new_prev->Next = this;
        new_next->Prev = this;
        this->Next = new_next;
        this->Prev = new_prev;
        return this;
    }

    DLNode *Next;
    DLNode *Prev;
};

struct DLList {
    inline void InitList() {
        this->HeadNode.Next = &this->HeadNode;
        this->HeadNode.Prev = &this->HeadNode;
    }

    inline DLList() {
        this->HeadNode.Next = &this->HeadNode;
        this->HeadNode.Prev = &this->HeadNode;
    }

    inline ~DLList() {
    }

    inline DLNode *GetHead() {
        return this->HeadNode.GetNext();
    }

    inline int IsEmpty() {
        return this->HeadNode.GetNext() == &this->HeadNode;
    }

    inline DLNode *RemoveHead() {
        return this->GetHead()->Remove();
    }

    inline DLNode *AddHead(DLNode *node) {
        return node->AddAfter(&this->HeadNode);
    }

    inline DLNode *AddTail(DLNode *node) {
        return node->AddBefore(&this->HeadNode);
    }

    DLNode HeadNode;
};

template <class T> struct TDLNode : public DLNode {
    inline TDLNode()
        : DLNode() {
    }

    inline T *Remove() {
        this->DLNode::Remove();
        return static_cast<T *>(this);
    }
};

template <class T> struct TDLList : public DLList {
    inline TDLList() {
        this->InitList();
    }

    inline T *AddHead(DLNode *node) {
        return static_cast<T *>(DLList::AddHead(node));
    }

    inline T *RemoveHead() {
        return static_cast<T *>(DLList::RemoveHead());
    }

    inline T *AddTail(DLNode *node) {
        return static_cast<T *>(DLList::AddTail(node));
    }
};


} // namespace RCMP

#endif
