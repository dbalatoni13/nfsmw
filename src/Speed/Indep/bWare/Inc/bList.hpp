#pragma once

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
    return prev_node; // TODO
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

  typedef long (*SortFunc)(bNode *, bNode *);

public:
  bList() {
    // TODO fake match
    bNode **prev = &this->HeadNode.Prev;
    this->HeadNode.Next = &this->HeadNode;
    *prev = &this->HeadNode;
  }
  ~bList() {}
  void InitList() {
    // TODO fake match
    bNode **prev = &this->HeadNode.Prev;
    this->HeadNode.Next = &this->HeadNode;
    *prev = &this->HeadNode;
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
  bNode *RemoveHead();                   // TODO
  bNode *RemoveTail();                   // TODO
  int GetNodeNumber(bNode *node);        // TODO
  int IsInList(bNode *node);             // TODO
  int CountElements();                   // TODO
  bNode *AddSorted(SortFunc check_flip); // TODO
  void Sort(SortFunc check_flip);        // TODO
};

template <typename T> class bTList : public bList {
public:
  bTList() {}
  ~bTList() {}
  T *EndOfList() {
    return (T *)bList::EndOfList();
  }
  T *GetHead() {
    return (T *)bList::GetHead();
  }
  T *GetTail() {
    return (T *)bList::GetTail();
  }
  T *GetNextCircular(T *node) {
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
  T *AddSorted(SortFunc check_flip);
  void Sort(SortFunc check_flip);
};
