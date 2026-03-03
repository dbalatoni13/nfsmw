#include "AnimEntity_WorldEntity.hpp"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/Src/World/WorldModel.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "WorldAnimInstanceDirectory.hpp"

// TODO move? they are static though
static int NumWorldAnimEntities;           // size: 0x4, address: 0x804156F4
static int MaxNumWorldAnimEntities;        // size: 0x4, address: 0x804156F8
static int NumWorldAnimEntityTrees;        // size: 0x4, address: 0x804156FC
static int MaxNumWorldAnimEntityTrees;     // size: 0x4, address: 0x80415700
static int NumWorldAnimEntityTreeInfos;    // size: 0x4, address: 0x80415704
static int MaxNumWorldAnimEntityTreeInfos; // size: 0x4, address: 0x80415708

void *CWorldAnimEntity::operator new(size_t size, const char *debug_name) {
    NumWorldAnimEntities++;
    if (NumWorldAnimEntities > MaxNumWorldAnimEntities) {
        MaxNumWorldAnimEntities = NumWorldAnimEntities;
    }
    return bOMalloc(TheWorldAnimInstanceDirectory.GetWorldAnimEntitySlotPool());
}

void CWorldAnimEntity::operator delete(void *ptr) {
    NumWorldAnimEntities--;
    bFree(TheWorldAnimInstanceDirectory.GetWorldAnimEntitySlotPool(), ptr);
}

void *CWorldAnimEntityTree::operator new(size_t size, const char *debug_name) {
    NumWorldAnimEntityTrees++;
    if (NumWorldAnimEntityTrees > MaxNumWorldAnimEntityTrees) {
        MaxNumWorldAnimEntityTrees = NumWorldAnimEntityTrees;
    }
    return bOMalloc(TheWorldAnimInstanceDirectory.GetWorldAnimEntityTreeSlotPool());
}

void CWorldAnimEntityTree::operator delete(void *ptr) {
    NumWorldAnimEntityTrees--;
    bFree(TheWorldAnimInstanceDirectory.GetWorldAnimEntityTreeSlotPool(), ptr);
}

void *WorldAnimEntityTreeInfo::operator new(size_t size, const char *debug_name) {
    NumWorldAnimEntityTreeInfos++;
    if (NumWorldAnimEntityTreeInfos > MaxNumWorldAnimEntityTreeInfos) {
        MaxNumWorldAnimEntityTreeInfos = NumWorldAnimEntityTreeInfos;
    }
    return bOMalloc(TheWorldAnimInstanceDirectory.GetWorldAnimEntityTreeInfoSlotPool());
}

void WorldAnimEntityTreeInfo::operator delete(void *ptr) {
    NumWorldAnimEntityTreeInfos--;
    bFree(TheWorldAnimInstanceDirectory.GetWorldAnimEntityTreeInfoSlotPool(), ptr);
}

void CWorldAnimEntity::EndianSwapEntityData(void *data, int size) {
    WorldAnimEntityInfo *info;
    bPlatEndianSwap(&info->mTypeID);
    bPlatEndianSwap(&info->mThisInstanceNameHash);
    bPlatEndianSwap(&info->mParentInstanceNameHash);
    bPlatEndianSwap(&info->mModelHash);
    bPlatEndianSwap(&info->mLODB);
    bPlatEndianSwap(&info->mLODZ);
    bPlatEndianSwap(&info->mAnimTreeHash);
    bPlatEndianSwap(&info->mAnimNameHash);
    bPlatEndianSwap(&info->mAnimContentFlags);
    bPlatEndianSwap(&info->mParentIndex);
    bPlatEndianSwap(&info->mLocalMatrix);
}

CWorldAnimEntity::CWorldAnimEntity()
    : mParentInstanceNameHash(0), //
      mTypeID(0),                 //
      mThisInstanceNameHash(0),   //
      mSpaceNode(nullptr),        //
      mWorldModel(nullptr),       //
      mAnimCtrl(nullptr) {}

CWorldAnimEntity::~CWorldAnimEntity() {}

uint32 GetAnimChannelHash(uint32 anim_hash, uint32 dof) {
    if (dof == 0) {
        return bStringHash("_t", anim_hash);
    } else if (dof == 1) {
        return bStringHash("_q", anim_hash);
    } else if (dof == 2) {
        return bStringHash("_s", anim_hash);
    } else {
        return 0;
    }
}

void CWorldAnimEntity::Purge() {
    if (mAnimCtrl) {
        mAnimCtrl->GetAnimPart()->Purge();
        mAnimCtrl->Cleanup();
        delete mAnimCtrl;
        mAnimCtrl = nullptr;
    }
    if (mWorldModel) {
        delete mWorldModel;
        mWorldModel = nullptr;
    }
    if (mSpaceNode) {
        DeleteSpaceNode(mSpaceNode);
    }
}

void CWorldAnimEntity::Play() {
    if (mAnimCtrl) {
        mAnimCtrl->Play();
    }
}

void CWorldAnimEntity::Pause() {
    if (mAnimCtrl) {
        mAnimCtrl->Pause();
    }
}

void CWorldAnimEntity::Stop() {
    if (mAnimCtrl) {
        mAnimCtrl->Stop();
    }
}

bool CWorldAnimEntity::IsPlaying() {
    if (mAnimCtrl) {
        return mAnimCtrl->IsPlaying();
    }
    return false;
}

void CWorldAnimEntity::SetTime(float time) {
    if (mAnimCtrl) {
        mAnimCtrl->SetEvalTime(0.0f);
    }
    // TODO is this right?
    UpdateTimeStep(time);
}

// void CWorldAnimEntity::UpdateTimeStep(float time_step) {}
