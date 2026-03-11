#include "Speed/Indep/Src/World/WCollisionAssets.h"

#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/WCollider.h"
#include "Speed/Indep/Src/World/WCollisionPack.h"
#include "Speed/Indep/Src/World/WGridManagedDynamicElem.h"
#include "Speed/Indep/Src/World/WTrigger.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

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
      fManagedCollisionInstances(new CollisionInstanceMap), //
      fManagedCollisionInstancesInd(0x8000),   //
      fStaticCollisionObjects(nullptr),        //
      fStaticCollisionObjectsCount(0),         //
      fManagedCollisionObjects(new CollisionObjectMap), //
      fManagedCollisionObjectsInd(0x8000),     //
      fNumPackLoadCallbacks(0),                //
      fStaticTriggers(nullptr),                //
      fStaticTriggersCount(0) {
    unsigned int i;

    for (i = 0; i <= 3; ++i) {
        fPackLoadCallback[i] = nullptr;
    }

    mCollisionPackList = new WCollisionPack *[0xA8C];
    for (i = 0; i <= 0xA8B; ++i) {
        mCollisionPackList[i] = nullptr;
    }
}

WCollisionAssets::~WCollisionAssets() {
    unsigned int i;

    for (i = 0; i <= 0xA8B; ++i) {
        if (mCollisionPackList[i] != nullptr) {
            delete mCollisionPackList[i];
            mCollisionPackList[i] = nullptr;
        }
    }

    delete[] mCollisionPackList;
    mCollisionPackList = nullptr;

    if (fManagedCollisionInstances != nullptr) {
        CollisionInstanceMap::iterator it;
        for (it = fManagedCollisionInstances->begin(); it != fManagedCollisionInstances->end(); ++it) {
            delete it->second;
        }
        delete fManagedCollisionInstances;
    }
    fManagedCollisionInstances = nullptr;

    if (fManagedCollisionObjects != nullptr) {
        CollisionObjectMap::iterator it;
        for (it = fManagedCollisionObjects->begin(); it != fManagedCollisionObjects->end(); ++it) {
            delete it->second;
        }
        delete fManagedCollisionObjects;
    }
    fManagedCollisionObjects = nullptr;
}

void WCollisionAssets::Shutdown() {
    if (sWCollisionAssets != nullptr) {
        delete sWCollisionAssets;
    }

    sWCollisionAssets = nullptr;
    WTriggerManager::Shutdown();
    WGridManagedDynamicElem::Shutdown();
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
                WCollisionArticle *cArt = const_cast<WCollisionArticle *>(cInst->fCollisionArticle);
                if (cArt != nullptr) {
                    int j = 0;
                    unsigned char *edge = reinterpret_cast<unsigned char *>(cArt);
                    edge += cArt->fStripsSize + 0x10;

                    if (j < cArt->fNumEdges) {
                        do {
                            edge[0xD] |= exclusionFlags;
                            ++j;
                            edge += 0x20;
                        } while (j < cArt->fNumEdges);
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
    unsigned int objectInd = fManagedCollisionObjectsInd++;
    (*fManagedCollisionObjects)[objectInd] = obj;
    return objectInd;
}
