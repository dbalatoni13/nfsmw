#include "Speed/Indep/Src/World/WCollisionAssets.h"

#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/Track.hpp"
#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/World/WCollision.h"
#include "Speed/Indep/Src/World/WCollisionPack.h"
#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
#include "Speed/Indep/Src/World/WTrigger.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#define COLLISION_PACK_MAX (2700)

struct ManagedCollisionInstance {
    ManagedCollisionInstance(WCollisionInstance *cInst, unsigned int trigInd)
        : mCInst(cInst), //
          mTriggerInd(trigInd) {}

    WCollisionInstance *mCInst;
    unsigned int mTriggerInd;
};

WCollisionAssets::WCollisionAssets()
    : fStaticCollisionInstances(nullptr),                                                //
      fStaticCollisionInstancesCount(0),                                                 //
      fManagedCollisionInstances(new ("CollisionInstanceList", 0) CollisionInstanceMap), //
      fManagedCollisionInstancesInd(0x8000),                                             //
      fStaticCollisionObjects(nullptr),                                                  //
      fStaticCollisionObjectsCount(0),                                                   //
      fManagedCollisionObjects(new ("CollisionObjectList", 0) CollisionObjectMap),       //
      fManagedCollisionObjectsInd(0x8000),                                               //
      fNumPackLoadCallbacks(0) {
    for (unsigned int onCallback = 0; onCallback < 4; ++onCallback) {
        fPackLoadCallback[onCallback] = nullptr;
    }

    fStaticTriggers = nullptr;
    fStaticTriggersCount = 0;
    fStaticCollisionObjects = nullptr;

    mCollisionPackList = new ("WCollisionPack", 0) WCollisionPack *[COLLISION_PACK_MAX];
    for (int ix = 0; ix < COLLISION_PACK_MAX; ++ix) {
        mCollisionPackList[ix] = nullptr;
    }
}

WCollisionAssets::~WCollisionAssets() {
    for (int ix = 0; ix < COLLISION_PACK_MAX; ++ix) {
        if (mCollisionPackList[ix]) {
            delete mCollisionPackList[ix];
            mCollisionPackList[ix] = nullptr;
        }
    }

    if (mCollisionPackList) {
        delete[] mCollisionPackList;
        mCollisionPackList = nullptr;
    }

    delete fManagedCollisionInstances;
    fManagedCollisionInstances = nullptr;

    for (CollisionObjectMap::iterator iter = fManagedCollisionObjects->begin(); iter != fManagedCollisionObjects->end(); ++iter) {
        WCollisionObject *obj = (*iter).second;
        delete obj;
    }
    delete fManagedCollisionObjects;
    fManagedCollisionObjects = nullptr;
}

void WCollisionAssets::Init(const UGroup *mapGroup, const UData *triggerUData) {
    if (!mapGroup) {
        sWCollisionAssets = new ("WCollisionAssets", 0) WCollisionAssets();
        WTriggerManager::Init();
        WGrid::Init(nullptr);
        WGridManagedDynamicElem::Init();
        return;
    }
    sWCollisionAssets = new ("WCollisionAssets", 0) WCollisionAssets();
    if (!triggerUData) {
        sWCollisionAssets->fStaticTriggers = nullptr;
        sSavedTriggerData = nullptr;
        sOriginalTriggerData = nullptr;
    }
    WTriggerManager::Init();
    WGrid::Init(mapGroup);

    for (unsigned int num = GetNumTrackOBBs() - 1; num != -1; --num) {
        TrackOBB *tobb = GetTrackOBB(num);
        UMath::Matrix4 mat = *reinterpret_cast<const UMath::Matrix4 *>(&tobb->Matrix);
        bConvertToBond(reinterpret_cast<bMatrix4 &>(mat), tobb->Matrix);

        UMath::Vector3 dim;
        bConvertToBond(dim, tobb->Dims);
        dim.x = bAbs(dim.x);
        dim.y = bAbs(dim.y);
        dim.z = bAbs(dim.z);
        WCollisionObject *obj = Get().CreateObject(dim, mat, false);
    }
}

void WCollisionAssets::Shutdown() {
    delete sWCollisionAssets;
    sWCollisionAssets = nullptr;

    WTriggerManager::Shutdown();
    WGridManagedDynamicElem::Shutdown();
    WGrid::Shutdown();
}

void WCollisionAssets::SetExclusionFlags(WCollisionPack *collisionPack) {
    if (!collisionPack) {
        return;
    }
    const WCollisionInstance *cInst;
    unsigned int ind = 0;

    while (true) {
        cInst = collisionPack->Instance(static_cast<unsigned short>(ind));
        if (!cInst) {
            break;
        }

        if (ind >= collisionPack->InstanceCount()) {
            break;
        }

        unsigned int flags = 0;
        if (cInst->fGroupNumber != 0) {
            flags = 0xC0;
        }

        if (cInst->fCollisionArticle) {
            if (cInst->fCollisionArticle->fNumStrips == 0) {
                cInst->fFlags |= flags;
            }
        }

        if (flags != 0) {
            if (cInst->fCollisionArticle) {
                const WCollisionBarrier *barrier = cInst->fCollisionArticle->GetBarrier(0u);
                for (int i = 0; i < cInst->fCollisionArticle->fNumEdges; ++i, barrier = barrier->Next()) {
                    const_cast<WSurface &>(barrier->GetWSurface()).FlagsRef() |= flags;
                }
            }
        }

        ind++;
    }
}

void WCollisionAssets::SetExclusionFlags() {
    for (unsigned int sectionId = 0; sectionId < 0x400; ++sectionId) {
        WCollisionPack *collisionPack = mCollisionPackList[sectionId];
        SetExclusionFlags(collisionPack);
    }

    WCollider::InvalidateAllCachedData();
}

void WCollisionAssets::AddPackLoadCallback(void (*callback)(int, bool)) {
    if (fNumPackLoadCallbacks > 3) {
        return;
    }

    fPackLoadCallback[fNumPackLoadCallbacks++] = callback;
}

void WCollisionAssets::RemovePackLoadCallback(void (*callback)(int, bool)) {
    unsigned int onCallback = 0;

    while (onCallback < fNumPackLoadCallbacks) {
        if (fPackLoadCallback[onCallback] == callback) {
            if (onCallback < fNumPackLoadCallbacks - 1) {
                fPackLoadCallback[onCallback] = fPackLoadCallback[fNumPackLoadCallbacks - 1];
            } else {
                fPackLoadCallback[onCallback] = nullptr;
            }
            fNumPackLoadCallbacks--;
        } else {
            onCallback++;
        }
    }
}

void WCollisionAssets::LoadCollisionPack(bChunk *chunk) {
    bChunkCarpHeader *chunk_header = reinterpret_cast<bChunkCarpHeader *>(chunk->GetAlignedData(16));
    int sectionId = chunk_header->GetSectionNumber();
    if (!chunk_header->IsResolved()) {
        bPlatEndianSwap(&sectionId);
    }
    if (!mCollisionPackList[sectionId]) {
        mCollisionPackList[sectionId] = new ("WCollisionPack") WCollisionPack(chunk);
        for (unsigned int onCallback = 0; onCallback < fNumPackLoadCallbacks; ++onCallback) {
            if (fPackLoadCallback[onCallback]) {
                fPackLoadCallback[onCallback](sectionId, true);
            }
        }
    }
    SetExclusionFlags(mCollisionPackList[sectionId]);
}

void WCollisionAssets::UnLoadCollisionPack(bChunk *chunk) {
    bChunkCarpHeader *chunk_header = reinterpret_cast<bChunkCarpHeader *>(chunk->GetAlignedData(16));
    int sectionId = chunk_header->GetSectionNumber();
    if (mCollisionPackList[sectionId]) {
        for (unsigned int onCallback = 0; onCallback < fNumPackLoadCallbacks; ++onCallback) {
            if (fPackLoadCallback[onCallback]) {
                fPackLoadCallback[onCallback](sectionId, false);
            }
        }
        delete mCollisionPackList[sectionId];
        mCollisionPackList[sectionId] = nullptr;
    }
}

// STRIPPED
const WCollisionInstance *WCollisionAssets::GetStaticInstance(unsigned int ind) {}

const WCollisionInstance *WCollisionAssets::Instance(unsigned int ind) const {
    unsigned short sectionId = static_cast<unsigned short>(ind >> 16);
    if (sectionId >= COLLISION_PACK_MAX) {
        return nullptr;
    }

    WCollisionPack *collisionPack = mCollisionPackList[sectionId];
    if (collisionPack) {
        return collisionPack->Instance(static_cast<unsigned short>(ind));
    }

    return nullptr;
}

const WCollisionObject *WCollisionAssets::Object(unsigned int ind) const {
    unsigned short sectionId = static_cast<unsigned short>(ind >> 16);
    if (sectionId >= COLLISION_PACK_MAX) {
        return nullptr;
    }

    if (ind > 0x7FFF) {
        return (*fManagedCollisionObjects)[ind];
    }

    if (mCollisionPackList[sectionId]) {
        return mCollisionPackList[sectionId]->Object(static_cast<unsigned short>(ind));
    }

    return nullptr;
}

unsigned int WCollisionAssets::AddObject(WCollisionObject *obj) {
    unsigned int objectInd = fManagedCollisionObjectsInd;
    fManagedCollisionObjectsInd = objectInd + 1;
    (*fManagedCollisionObjects)[objectInd] = obj;
    return objectInd;
}

WCollisionObject *WCollisionAssets::CreateObject(const UMath::Vector3 &dim, const UMath::Matrix4 &mat, bool dynamicFlag) {
    WCollisionObject *obj = new ("WCollisionObject", 0) WCollisionObject;
    obj->fFlags = 0;
    obj->fMat = mat;
    if (dynamicFlag) {
        obj->fFlags |= 1;
    }
    obj->fRenderInstanceInd = 0;
    obj->fFlags |= 0x40;
    obj->fPosRadius.x = mat.v3.x;
    obj->fPosRadius.y = mat.v3.y - dim.y;
    obj->fPosRadius.z = mat.v3.z;
    obj->fType = 0;
    obj->fDimensions = UMath::Vector4Make(dim, 1.0f);
    obj->fDimensions.w = 1.0f;
    obj->fPosRadius.w = UMath::Sqrt(obj->fDimensions.x * obj->fDimensions.x + obj->fDimensions.z * obj->fDimensions.z);
    obj->fSurface.fFlags = 0;
    obj->fSurface.fSurface = 0;

    unsigned int objectInd = AddObject(obj);
    WGridManagedDynamicElem::AddElem(nullptr, &obj->fPosRadius, WGrid_kObject, objectInd);

    if (dynamicFlag) {
        WGridManagedDynamicElem::DynamicElemList().push_back(
            WGridManagedDynamicElem(&obj->fPosRadius, &mat.v3, WGridNodeElem(objectInd, WGrid_kObject)));
    }

    return obj;
}

WTrigger &WCollisionAssets::Trigger(uintptr_t tag) const {
    return *reinterpret_cast<WTrigger *>(tag);
}

void WCollisionAssets::AddTrigger(WTrigger *trig) {
    trig->UpdatePos(UMath::Vector4To3(trig->fPosRadius), reinterpret_cast<uintptr_t>(trig));
}

// UNSOLVED gotta use trigger->fFlags
void WCollisionAssets::RemoveTrigger(WTrigger *trigger) {
    UMath::Vector4 oldPosRad = trigger->fPosRadius;

    WGridManagedDynamicElem::AddElem(&oldPosRad, nullptr, WGrid_kTrigger, reinterpret_cast<uintptr_t>(trigger));
    if (trigger && ((static_cast<unsigned int>(reinterpret_cast<unsigned char *>(trigger)[0x12]) << 8) & 0x100) == 0) {
        delete trigger;
    }
}

#undef COLLISION_PACK_MAX
