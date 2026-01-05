#ifndef WORLD_WCOLLISIONASSETS_H
#define WORLD_WCOLLISIONASSETS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "WCollision.h"

// total size: 0x3C
class WCollisionAssets {
  public:
    static void Init(const UGroup *mapGroup, const UData *triggerUData);
    static void Shutdown();
    static void Restart();

    WCollisionAssets();
    ~WCollisionAssets();
    void SetExclusionFlags(struct WCollisionPack *collisionPack);
    void SetExclusionFlags();
    void AddPackLoadCallback(void (*callback)(int, bool));
    void RemovePackLoadCallback(void (*callback)(int, bool));
    void LoadCollisionPack(bChunk *chunk);
    void UnLoadCollisionPack(bChunk *chunk);
    const WCollisionInstance *GetStaticInstance(unsigned int ind);
    const WCollisionInstance *Instance(unsigned int ind) const;
    const WCollisionObject *Object(unsigned int ind) const;
    unsigned int AddObject(WCollisionObject *obj);
    struct WCollisionObject *CreateObject(UMath::Vector3 &dim, const UMath::Matrix4 &mat, bool dynamicFlag);
    struct WTrigger &Trigger(unsigned int tag) const;
    void AddTrigger(struct WTrigger *trig);
    void RemoveTrigger(struct WTrigger *trigger);
    // bool FindElemIndex(WGridNode_ElemType type, const void *ptr, unsigned int &ind);

    static WCollisionAssets &Get() {
        return *sWCollisionAssets;
    }

    // static bool Exists() {}

    // unsigned int NumTriggers() const {}

    static WCollisionAssets *sWCollisionAssets;        // size: 0x4, address: 0x80438F58
    static unsigned int sTriggerDataSize;              // size: 0x4, address: 0xFFFFFFFF
    static const struct Trigger *sOriginalTriggerData; // size: 0x4, address: 0x80438F60
    static const struct Trigger *sSavedTriggerData;    // size: 0x4, address: 0x80438F64
    static struct WCollisionPack **mCollisionPackList; // size: 0x4, address: 0x80438F68

    const WCollisionInstance *fStaticCollisionInstances;     // offset 0x0, size 0x4
    unsigned int fStaticCollisionInstancesCount;             // offset 0x4, size 0x4
    struct CollisionInstanceMap *fManagedCollisionInstances; // offset 0x8, size 0x4
    unsigned int fManagedCollisionInstancesInd;              // offset 0xC, size 0x4
    const WCollisionObject *fStaticCollisionObjects;         // offset 0x10, size 0x4
    unsigned int fStaticCollisionObjectsCount;               // offset 0x14, size 0x4
    struct CollisionObjectMap *fManagedCollisionObjects;     // offset 0x18, size 0x4
    unsigned int fManagedCollisionObjectsInd;                // offset 0x1C, size 0x4
    unsigned int fNumPackLoadCallbacks;                      // offset 0x20, size 0x4
    void (*fPackLoadCallback[4])(int, bool);                 // offset 0x24, size 0x10
    const struct WTrigger *fStaticTriggers;                  // offset 0x34, size 0x4
    unsigned int fStaticTriggersCount;                       // offset 0x38, size 0x4
};

#endif
