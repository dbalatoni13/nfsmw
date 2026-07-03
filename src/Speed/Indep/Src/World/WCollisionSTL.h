#ifndef WORLD_WCOLLISIONSTL_H
#define WORLD_WCOLLISIONSTL_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "WCollision.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

DECLARE_CONTAINER_TYPE(WCollisionWarnVector);
DECLARE_CONTAINER_TYPE(WCollisionVector);

// TODO move these?
template <typename T, std::size_t N> class WCollisionWarnVector : public UTL::Std::vector<T, _type_WCollisionWarnVector> {};

struct WCollisionInstanceCacheList : public WCollisionWarnVector<const WCollisionInstance *, 80> {
    // total size: 0x10
};

template <typename T> class WCollisionVector : public UTL::Std::vector<T, _type_WCollisionVector> {};

// total size: 0x10
struct WCollisionBarrierList : public WCollisionVector<WCollisionBarrierListEntry> {
    USE_FASTALLOC(WCollisionBarrierList);
};

// total size: 0x10
struct WCollisionTriBlock : public WCollisionVector<WCollisionTri> {
    USE_FASTALLOC(WCollisionTriBlock);
};

// total size: 0x14
struct WCollisionTriList : public WCollisionVector<WCollisionTriBlock *> {
    USE_FASTALLOC(WCollisionTriList);

    WCollisionTriList() : mCurrBlock(nullptr) {}

    ~WCollisionTriList() {
        clear_all();
    }

    void clear_all() {
        for (iterator i = begin(); i != end(); ++i) {
            delete *i;
        }
        clear();
        mCurrBlock = nullptr;
    }

    void add_tri(const WCollisionTri &tri) {
        if (mCurrBlock == nullptr || mCurrBlock->size() == mCurrBlock->capacity()) {
            mCurrBlock = new WCollisionTriBlock();
            mCurrBlock->reserve(21);
            push_back(mCurrBlock);
        }
        mCurrBlock->push_back(tri);
    }

    // TODO doesn't exist on PS2
    void reserve_total() {
        reserve(8);
    }

    WCollisionTriBlock *mCurrBlock; // offset 0x10, size 0x4
};

struct WCollisionObjectList : public WCollisionVector<const WCollisionObject *> {
    USE_FASTALLOC(WCollisionObjectList);
};

#endif
