#ifndef WORLD_WCOLLISIONASSETS_H
#define WORLD_WCOLLISIONASSETS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "WGridManagedDynamicElem.h"
#include "WTrigger.h"
#include "WCollisionPack.h"
#include "WCollision.h"

struct ManagedCollisionInstance;

// total size: 0x10
struct CollisionInstanceMap : public UTL::Std::map<unsigned int, ManagedCollisionInstance *, _type_map> {};

// total size: 0x10
struct CollisionObjectMap : public UTL::Std::map<unsigned int, WCollisionObject *, _type_map> {};

// total size: 0x3C
class WCollisionAssets {
  public:
    static void Init(const UGroup *mapGroup, const UData *triggerUData);
    static void Shutdown();
    static void Restart();

    WCollisionAssets();
    ~WCollisionAssets();
    void SetExclusionFlags(WCollisionPack *collisionPack);
    void SetExclusionFlags();
    void AddPackLoadCallback(void (*callback)(int, bool));
    void RemovePackLoadCallback(void (*callback)(int, bool));
    void LoadCollisionPack(bChunk *chunk);
    void UnLoadCollisionPack(bChunk *chunk);
    const WCollisionInstance *GetStaticInstance(WGridNodeElemTag ind);
    const WCollisionInstance *Instance(WGridNodeElemTag ind) const;
    const WCollisionObject *Object(WGridNodeElemTag ind) const;
    unsigned int AddObject(WCollisionObject *obj);
    WCollisionObject *CreateObject(const UMath::Vector3 &dim, const UMath::Matrix4 &mat, bool dynamicFlag);
    WTrigger &Trigger(uintptr_t tag) const;
    void AddTrigger(WTrigger *trig);
    void RemoveTrigger(WTrigger *trigger);
    // bool FindElemIndex(WGridNode_ElemType type, const void *ptr, WGridNodeElemTag &ind);

    static WCollisionAssets &Get() {
        return *sWCollisionAssets;
    }

    // static bool Exists() {}

    unsigned int NumTriggers() const {
        return fStaticTriggersCount;
    }

    static WCollisionAssets *sWCollisionAssets;       // size: 0x4, address: 0x80438F58
    static unsigned int sTriggerDataSize;             // size: 0x4, address: 0xFFFFFFFF
    static const CARP::Trigger *sOriginalTriggerData; // size: 0x4, address: 0x80438F60
    static const CARP::Trigger *sSavedTriggerData;    // size: 0x4, address: 0x80438F64
    static WCollisionPack **mCollisionPackList;       // size: 0x4, address: 0x80438F68

    const WCollisionInstance *fStaticCollisionInstances; // offset 0x0, size 0x4
    unsigned int fStaticCollisionInstancesCount;         // offset 0x4, size 0x4
    CollisionInstanceMap *fManagedCollisionInstances;    // offset 0x8, size 0x4
    unsigned int fManagedCollisionInstancesInd;          // offset 0xC, size 0x4
    const WCollisionObject *fStaticCollisionObjects;     // offset 0x10, size 0x4
    unsigned int fStaticCollisionObjectsCount;           // offset 0x14, size 0x4
    CollisionObjectMap *fManagedCollisionObjects;        // offset 0x18, size 0x4
    unsigned int fManagedCollisionObjectsInd;            // offset 0x1C, size 0x4
    unsigned int fNumPackLoadCallbacks;                  // offset 0x20, size 0x4
    void (*fPackLoadCallback[4])(int, bool);             // offset 0x24, size 0x10
    const WTrigger *fStaticTriggers;                     // offset 0x34, size 0x4
    unsigned int fStaticTriggersCount;                   // offset 0x38, size 0x4
};

#endif
