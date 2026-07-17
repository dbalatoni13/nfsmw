#ifndef SLINKLIST_H
#define SLINKLIST_H

// total size: 0x8
// Decl: 24
typedef struct CListDNode {
    struct CListDNode *pnext; // offset 0x0, size 0x4
    struct CListDNode *pprev; // offset 0x4, size 0x4
} CListDNode;

// total size: 0x4
// Decl: 96
class CListDStack {
  private:
    CListDNode *phead; // offset 0x0, size 0x4, Decl: 101

  public:
    CListDStack() : phead(0) {} // Decl: 104

    void Reset() {} // Decl: 106

    bool IsEmpty() {} // Decl: 108

    CListDNode *GetHead() {} // Decl: 110

    void Push(CListDNode *pnode) {} // Decl: 112

    CListDNode *Pop() {} // Decl: 129

    void Remove(CListDNode *pnode) {} // Decl: 148
};

#endif
