#ifndef OBJECTPOOL_H__
#define OBJECTPOOL_H__

#include "FEList.h"
#include "FEngStandard.h"

// total size: 0x2020
// Decl: 20
template <class T, int N> class FEPoolNode : public FEMinNode {
  public:
    T Pool[N];      // offset 0xC, size 0x2000, Decl: 22
    FEMinList Free; // offset 0x200C, size 0x10, Decl: 23
    int Used;       // offset 0x201C, size 0x4, Decl: 24

    FEPoolNode() : Used(0) { // Decl: 29
        for (int i = 0; i < N; i++) {
            Free.AddTail(&Pool[i]);
        }
    }
    ~FEPoolNode() override { // Decl: 29
        while (Free.GetNumElements() != 0) {
            Free.RemHead();
        }
    }

    bool Contains(T *pNode) { // Decl: 46
        return pNode >= &Pool[0] && pNode < &Pool[N];
    }

    FEPoolNode<T, N> *GetNext() { // Decl: 51
        return static_cast<FEPoolNode *>(FEMinNode::GetNext());
    }
};

// total size: 0x10
// Decl: 63
template <class T, int N> class ObjectPool {
  private:
    FEMinList Pools; // offset 0x0, size 0x10, Decl: 65

  public:
    ObjectPool() {}

    T *AllocSingle() { // Decl: 73
        FEPoolNode<T, N> *pPool = static_cast<FEPoolNode<T, N> *>(Pools.GetHead());
        T *pNode;
        while (pPool) {
            if (pPool->Free.GetNumElements() != 0) {
                break;
            }
            pPool = pPool->GetNext();
        }
        if (pPool == nullptr) {
            pPool = FNEW FEPoolNode<T, N>();
            Pools.AddHead(pPool);
        }
        pNode = static_cast<T *>(pPool->Free.RemHead());
        pPool->Used++;
        return pNode;
    }

    void FreeSingle(T *pNode) { // Decl: 102
        FEPoolNode<T, N> *pPool = static_cast<FEPoolNode<T, N> *>(Pools.GetHead());
        while (pPool) {
            if (pPool->Contains(pNode)) {
                pPool->Free.AddTail(pNode);
                pPool->Used--;
                if (pPool->Used == 0) {
                    Pools.RemNode(pPool);
                    if (pPool) {
                        delete pPool;
                    }
                }
                return;
            }
            pPool = pPool->GetNext();
        }
    }
};

#endif
