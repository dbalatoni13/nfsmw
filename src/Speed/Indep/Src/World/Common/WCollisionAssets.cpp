#include "Speed/Indep/Src/World/WCollisionAssets.h"

#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/World/WCollisionPack.h"
#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
#include "Speed/Indep/Src/World/WTrigger.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct TrackOBB {
    unsigned int TypeNameHash;
    unsigned int Pad[3];
    bMatrix4 Matrix;
    bVector3 Dims;
};

int GetNumTrackOBBs();
TrackOBB *GetTrackOBB(int index);

struct ManagedCollisionInstance {
    ManagedCollisionInstance(WCollisionInstance *cInst, unsigned int trigInd)
        : mCInst(cInst), //
          mTriggerInd(trigInd) {}

    WCollisionInstance *mCInst;
    unsigned int mTriggerInd;
};

WCollisionAssets::WCollisionAssets()
    : fStaticCollisionInstances(nullptr),      //
      fStaticCollisionInstancesCount(0),       //
      fManagedCollisionInstances(new (__FILE__, __LINE__) CollisionInstanceMap), //
      fManagedCollisionInstancesInd(0x8000),   //
      fStaticCollisionObjects(nullptr),        //
      fStaticCollisionObjectsCount(0),         //
      fManagedCollisionObjects(new (__FILE__, __LINE__) CollisionObjectMap), //
      fManagedCollisionObjectsInd(0x8000),     //
      fNumPackLoadCallbacks(0) {
    unsigned int onCallback;

    for (onCallback = 0; onCallback <= 3; ++onCallback) {
        fPackLoadCallback[onCallback] = nullptr;
    }

    fStaticTriggers = nullptr;
    fStaticTriggersCount = 0;
    fStaticCollisionObjects = nullptr;

    mCollisionPackList = new WCollisionPack *[0xA8C];
    int ix;
    for (ix = 0; ix <= 0xA8B; ++ix) {
        mCollisionPackList[ix] = nullptr;
    }
}

WCollisionAssets::~WCollisionAssets() {
    int ix;

    for (ix = 0; ix <= 0xA8B; ++ix) {
        if (mCollisionPackList[ix] != nullptr) {
            delete mCollisionPackList[ix];
            mCollisionPackList[ix] = nullptr;
        }
    }

    if (mCollisionPackList != nullptr) {
        delete[] mCollisionPackList;
        mCollisionPackList = nullptr;
    }

    delete fManagedCollisionInstances;
    fManagedCollisionInstances = nullptr;

    for (CollisionObjectMap::iterator iter = fManagedCollisionObjects->begin(); iter != fManagedCollisionObjects->end(); ++iter) {
        delete iter->second;
    }
    delete fManagedCollisionObjects;
    fManagedCollisionObjects = nullptr;
}

void WCollisionAssets::Init(const UGroup *mapGroup, const UData *triggerUData) {
    if (mapGroup == nullptr) {
        sWCollisionAssets = new WCollisionAssets();
        WTriggerManager::Init();
        WGrid::Init(nullptr);
        WGridManagedDynamicElem::Init();
    } else {
        sWCollisionAssets = new WCollisionAssets();
        if (triggerUData == nullptr) {
            sWCollisionAssets->fStaticTriggers = nullptr;
            sOriginalTriggerData = nullptr;
            sSavedTriggerData = nullptr;
        }
        WTriggerManager::Init();
        WGrid::Init(mapGroup);
        unsigned int num = GetNumTrackOBBs();
        for (num = num - 1; num != static_cast<unsigned int>(-1); --num) {
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
}

void WCollisionAssets::Shutdown() {
    if (sWCollisionAssets != nullptr) {
        delete sWCollisionAssets;
    }

    sWCollisionAssets = nullptr;
    if (WTriggerManager::fgTriggerManager != nullptr) {
        delete WTriggerManager::fgTriggerManager;
    }
    WTriggerManager::fgTriggerManager = nullptr;
    WGridManagedDynamicElem::fgManagedDynamicElemList.clear();
    WGrid::Shutdown();
}

void WCollisionAssets::SetExclusionFlags(WCollisionPack *collisionPack) {
    if (collisionPack != nullptr) {
        unsigned int i = 0;

        while (true) {
            WCollisionInstance *cInst = const_cast<WCollisionInstance *>(collisionPack->Instance(static_cast<unsigned short>(i)));
            if (cInst == nullptr) {
                break;
            }

            if (i >= collisionPack->mInstanceNum) {
                break;
            }

            unsigned int exclusionFlags = 0;
            if (cInst->fGroupNumber != 0) {
                exclusionFlags = 0xC0;
            }

            if (cInst->fCollisionArticle != nullptr) {
                if (cInst->fCollisionArticle->fNumStrips == 0) {
                    cInst->fFlags |= exclusionFlags;
                }
            }

            unsigned int next = i + 1;
            if (exclusionFlags != 0) {
                const WCollisionArticle *cArt = cInst->fCollisionArticle;
                if (cArt != nullptr) {
                    const WCollisionBarrier *barrier = cArt->GetBarrier(0u);
                    int j = 0;
                    if (j < cArt->fNumEdges) {
                        do {
                            const_cast<WSurface &>(barrier->GetWSurface()).FlagsRef() |= exclusionFlags;
                            ++j;
                            barrier = barrier->Next();
                        } while (j < cInst->fCollisionArticle->fNumEdges);
                    }
                }
            }

            i = next;
        }
    }
}

void WCollisionAssets::SetExclusionFlags() {
    unsigned int i = 0;

    do {
        SetExclusionFlags(mCollisionPackList[i]);
        ++i;
    } while (i <= 0x3FF);

    WCollider::InvalidateAllCachedData();
}

void WCollisionAssets::AddPackLoadCallback(void (*callback)(int, bool)) {
    unsigned int numPackLoadCallbacks = fNumPackLoadCallbacks;
    if (numPackLoadCallbacks > 3) {
        return;
    }

    fPackLoadCallback[numPackLoadCallbacks] = callback;
    fNumPackLoadCallbacks = numPackLoadCallbacks + 1;
}

void WCollisionAssets::RemovePackLoadCallback(void (*callback)(int, bool)) {
    unsigned int i = 0;

    while (i < fNumPackLoadCallbacks) {
        if (fPackLoadCallback[i] == callback) {
            if (i < fNumPackLoadCallbacks - 1) {
                fPackLoadCallback[i] = fPackLoadCallback[fNumPackLoadCallbacks - 1];
            } else {
                fPackLoadCallback[i] = nullptr;
            }
            --fNumPackLoadCallbacks;
        } else {
            ++i;
        }
    }
}

const WCollisionInstance *WCollisionAssets::Instance(unsigned int ind) const {
    unsigned short sectionId = static_cast<unsigned short>(ind >> 0x10);
    if (sectionId > 0xA8B) {
        return nullptr;
    }

    WCollisionPack *collisionPack = mCollisionPackList[sectionId];
    if (collisionPack != nullptr) {
        return collisionPack->Instance(static_cast<unsigned short>(ind));
    }

    return nullptr;
}

const WCollisionObject *WCollisionAssets::Object(unsigned int ind) const {
    unsigned short sectionId = static_cast<unsigned short>(ind >> 0x10);
    if (sectionId > 0xA8B) {
        return nullptr;
    }

    if (ind > 0x7FFF) {
        return (*fManagedCollisionObjects)[ind];
    }

    WCollisionPack *collisionPack = mCollisionPackList[sectionId];
    if (collisionPack != nullptr) {
        return collisionPack->Object(static_cast<unsigned short>(ind));
    }

    return nullptr;
}

WTrigger &WCollisionAssets::Trigger(unsigned int tag) const {
    return *reinterpret_cast<WTrigger *>(tag);
}

void WCollisionAssets::AddTrigger(WTrigger *trig) {
    trig->UpdatePos(UMath::Vector4To3(trig->fPosRadius), reinterpret_cast<unsigned int>(trig));
}

void WCollisionAssets::RemoveTrigger(WTrigger *trigger) {
    UMath::Vector4 oldPosRad = trigger->fPosRadius;

    WGridManagedDynamicElem::AddElem(&oldPosRad, nullptr, WGrid_kTrigger, reinterpret_cast<unsigned int>(trigger));
    if (trigger != nullptr && ((static_cast<unsigned int>(reinterpret_cast<unsigned char *>(trigger)[0x12]) << 8) & 0x100) == 0) {
        delete trigger;
    }
}

void WCollisionAssets::LoadCollisionPack(bChunk *chunk) {
    bChunkCarpHeader *chunk_header = reinterpret_cast<bChunkCarpHeader *>(chunk->GetAlignedData(16));
    int sectionId = chunk_header->GetSectionNumber();
    if (!chunk_header->IsResolved()) {
        bPlatEndianSwap(&sectionId);
    }
    if (mCollisionPackList[sectionId] == nullptr) {
        mCollisionPackList[sectionId] = new WCollisionPack(chunk);
        for (unsigned int onCallback = 0; onCallback < fNumPackLoadCallbacks; ++onCallback) {
            if (fPackLoadCallback[onCallback] != nullptr) {
                fPackLoadCallback[onCallback](sectionId, true);
            }
        }
    }
    SetExclusionFlags(mCollisionPackList[sectionId]);
}

void WCollisionAssets::UnLoadCollisionPack(bChunk *chunk) {
    bChunkCarpHeader *chunk_header = reinterpret_cast<bChunkCarpHeader *>(chunk->GetAlignedData(16));
    int sectionId = chunk_header->GetSectionNumber();
    if (mCollisionPackList[sectionId] != nullptr) {
        for (unsigned int onCallback = 0; onCallback < fNumPackLoadCallbacks; ++onCallback) {
            if (fPackLoadCallback[onCallback] != nullptr) {
                fPackLoadCallback[onCallback](sectionId, false);
            }
        }
        delete mCollisionPackList[sectionId];
        mCollisionPackList[sectionId] = nullptr;
    }
}

unsigned int WCollisionAssets::AddObject(WCollisionObject *obj) {
    unsigned int objectInd = fManagedCollisionObjectsInd;
    fManagedCollisionObjectsInd = objectInd + 1;
    (*fManagedCollisionObjects)[objectInd] = obj;
    return objectInd;
}

WCollisionObject *WCollisionAssets::CreateObject(const UMath::Vector3 &dim, const UMath::Matrix4 &mat, bool dynamicFlag) {
    WCollisionObject *obj = new (__FILE__, __LINE__) WCollisionObject;
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
        WGridNodeElem elem(objectInd, WGrid_kObject);
        WGridManagedDynamicElem dynElem(&obj->fPosRadius, &mat.v3, elem);
        WGridManagedDynamicElem::DynamicElemList().push_back(dynElem);
    }

    return obj;
}
