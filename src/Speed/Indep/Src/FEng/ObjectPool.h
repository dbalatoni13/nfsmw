#ifndef OBJECTPOOL_H__
#define OBJECTPOOL_H__

#include "FEList.h"
#include "FEngStandard.h"

// File: speed/indep/src/feng/ObjectPool.h
// total size: 0x2020
// Decl: speed/indep/src/feng/ObjectPool.h:20
template <class T, int N> class FEPoolNode : public FEMinNode {
  public:
    T Pool[256];    // offset 0xC, size 0x2000, Decl: speed/indep/src/feng/ObjectPool.h:22
    FEMinList Free; // offset 0x200C, size 0x10, Decl: speed/indep/src/feng/ObjectPool.h:23
    int Used;       // offset 0x201C, size 0x4, Decl: speed/indep/src/feng/ObjectPool.h:24

    FEPoolNode() : Used(0) { // Decl: speed/indep/src/feng/ObjectPool.h:29
        for (int i = 0; i < N; i++) {
            Free.AddTail(&Pool[i]);
        }
    }
    ~FEPoolNode() override { // Decl: speed/indep/src/feng/ObjectPool.h:29
        while (Free.GetNumElements() != 0) {
            Free.RemHead();
        }
    }

    bool Contains(T *pNode) {} // Decl: speed/indep/src/feng/ObjectPool.h:46

    FEPoolNode<T, N> *GetNext() { // Decl: speed/indep/src/feng/ObjectPool.h:51
        return static_cast<FEPoolNode *>(FEMinNode::GetNext());
    }
};

// total size: 0x10
// Decl: speed/indep/src/feng/ObjectPool.h:63
template <class T, int N> class ObjectPool {
  private:
    FEMinList Pools; // offset 0x0, size 0x10, Decl: speed/indep/src/feng/ObjectPool.h:65

  public:
    ObjectPool() {}

    T *AllocSingle() { // Decl: speed/indep/src/feng/ObjectPool.h:73
        FEPoolNode<T, N> *pPool = static_cast<FEPoolNode<T, N> *>(Pools.GetHead());
        while (pPool) {
            if (pPool->Free.GetNumElements() != 0) {
                break;
            }
            pPool = pPool->GetNext();
        }
        if (!pPool) {
            pPool = FNEW FEPoolNode<T, N>();
            Pools.AddHead(pPool);
        }
        T *pNode = static_cast<T *>(pPool->Free.RemHead());
        pPool->Used++;
        return pNode;
    }

    void FreeSingle(T *pNode) { // Decl: speed/indep/src/feng/ObjectPool.h:102
        pNode->~T();

        FEPoolNode<T, N> *pPool = static_cast<FEPoolNode<T, N> *>(Pools.GetHead());
        while (pPool) {
            bool bInPool = false;
            if (pNode >= &pPool->Pool[0]) {
                bInPool = pNode < &pPool->Pool[N];
            }
            if (bInPool) {
                pPool->Free.AddNode(pPool->Free.GetTail(), pNode);
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
