#ifndef FENG_OBJECTPOOL_H
#define FENG_OBJECTPOOL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <new>
#include "FEList.h"
#include "FEngStandard.h"

void FEngFree(void* ptr);

template <class T, int N>
struct FEPoolNode : public FEMinNode {
    T Pool[N];
    FEMinList Free;
    int Used;

    inline FEPoolNode() : Used(0) {
        for (int i = 0; i < N; i++) {
            Free.AddTail(&Pool[i]);
        }
    }

    ~FEPoolNode() override;

    inline FEPoolNode* GetNext() { return static_cast<FEPoolNode*>(FEMinNode::GetNext()); }
};

template <class T, int N>
FEPoolNode<T, N>::~FEPoolNode() {
    while (Free.GetNumElements() != 0) {
        Free.RemHead();
    }
}

template <class T, int N>
struct ObjectPool {
    FEMinList Pools;

    inline T* AllocSingle() {
        FEPoolNode<T, N>* pPool = static_cast<FEPoolNode<T, N>*>(Pools.GetHead());
        while (pPool) {
            if (pPool->Free.GetNumElements() != 0) {
                break;
            }
            pPool = pPool->GetNext();
        }
        if (!pPool) {
            pPool = FENG_NEW FEPoolNode<T, N>();
            Pools.AddHead(pPool);
        }
        T* pNode = static_cast<T*>(pPool->Free.RemHead());
        pPool->Used++;
        return pNode;
    }

    inline void FreeSingleNoDestroy(T* pNode) {
        FEPoolNode<T, N>* pPool = static_cast<FEPoolNode<T, N>*>(Pools.GetHead());
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

    inline void FreeSingle(T* pNode) {
        pNode->~T();
        FreeSingleNoDestroy(pNode);
    }
};

#endif
