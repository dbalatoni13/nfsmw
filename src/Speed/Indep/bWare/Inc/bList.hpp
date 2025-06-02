#pragma once

#include <types.h>

class bNode {
public:
  bNode *Next;
  bNode *Prev;

  bNode *GetNext() { return Next; }
  bNode *GetPrev() { return Prev; }
};

template <typename T> struct bTNode : public bNode {};

struct bList {
  bNode HeadNode;

  typedef long (*SortFunc)(bNode *, bNode *);

public:
  bList() {
    this->HeadNode.Next = &this->HeadNode;
    this->HeadNode.Prev = &this->HeadNode;
  }
  ~bList() {}
  void InitList() {
    this->HeadNode.Next = &this->HeadNode;
    this->HeadNode.Prev = &this->HeadNode;
  }
  int IsEmpty() { return HeadNode.GetNext() != &HeadNode; }
  bNode *EndOfList(); // TODO
  bNode *GetHead() { return &HeadNode; }
  bNode *GetTail();                                   // TODO
  bNode *GetNextCircular(bNode *node);                // TODO
  bNode *GetPrevCircular(bNode *node);                // TODO
  bNode *AddHead(bNode *node);                        // TODO
  bNode *AddTail(bNode *node);                        // TODO
  bNode *AddBefore(bNode *insert_point, bNode *node); // TODO
  bNode *AddAfter(bNode *insert_point, bNode *node);  // TODO
  bNode *Remove(bNode *node);                         // TODO
  bNode *RemoveHead();                                // TODO
  bNode *RemoveTail();                                // TODO
  int GetNodeNumber(bNode *node);                     // TODO
  int IsInList(bNode *node);                          // TODO
  int CountElements();                                // TODO
  bNode *AddSorted(SortFunc check_flip);              // TODO
  void Sort(SortFunc check_flip);                     // TODO
};

template <typename T> class bTList : public bList {
public:
  bTList() {}
  ~bTList() {}
  T *EndOfList() { return (T *)bList::EndOfList(); }
  T *GetHead() { return (T *)bList::GetHead(); }
  T *GetTail() { return (T *)bList::GetTail(); }
  T *GetNextCircular(T *node) { return (T *)bList::GetNextCircular(); }
  T *GetPrevCircular(bNode *node) { return (T *)bList::GetPrevCircular(); }
  T *AddHead(bNode *node) { return (T *)bList::AddHead(node); }
  T *AddTail(bNode *node) { return (T *)bList::AddTail(); }
  T *AddBefore(bNode *insert_point, bNode *node) { return (T *)bList::AddBefore(insert_point, node); }
  T *AddAfter(bNode *insert_point, bNode *node) { return (T *)bList::AddAfter(insert_point, node); }
  T *Remove(bNode *node) { return (T *)bList::Remove(node); }
  T *RemoveHead() { return (T *)bList::RemoveHead(); }
  T *RemoveTail() { return (T *)bList::RemoveTail(); }
  T *AddSorted(SortFunc check_flip);
  void Sort(SortFunc check_flip);
};
