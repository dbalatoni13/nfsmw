#ifndef PHYSICS_SMACKABLETRIGGER_H
#define PHYSICS_SMACKABLETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/World/WTrigger.h"

struct HMODEL__;

struct SmackableTrigger {
    SmackableTrigger(HMODEL__ *hmodel, bool virgin, const UMath::Matrix4 &objectmatrix,
                     const UMath::Vector3 &dim, unsigned int extra_flags);
    ~SmackableTrigger();
    void Fire();
    void Disable();
    void Enable();
    bool IsEnabled() const;
    void GetObjectMatrix(UMath::Matrix4 &matrix) const;
    void Move(const UMath::Matrix4 &matrix, const UMath::Vector3 &dim, bool virgin);

    void *operator new(std::size_t size) { return gFastMem.Alloc(size, nullptr); }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    WTrigger *mTrigger;
    Event::StaticData *mEventData;
};

#endif
