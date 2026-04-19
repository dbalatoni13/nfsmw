#include "AnimEntity_Prop.hpp"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void CPropAnimEntity::EndianSwapEntityData(void *data, int size) {
    PropAnimEntityInfo *info = reinterpret_cast<PropAnimEntityInfo *>(data);
#ifndef EA_BUILD_A124
    bPlatEndianSwap(&info->mThisInstanceNameHash);
    bPlatEndianSwap(&info->mParentInstanceNameHash);
    bPlatEndianSwap(&info->mLocalMatrix);
    bPlatEndianSwap(&info->mLODNameHash[0]);
    bPlatEndianSwap(&info->mLODNameHash[1]);
    bPlatEndianSwap(&info->mLODNameHash[2]);
    bPlatEndianSwap(&info->mLODNameHash[3]);
#endif
}

CPropAnimEntity::CPropAnimEntity()
    : mWorldModel(nullptr),     //
      mKeepOnGround(true),      //
      mTypeID(0),               //
      mThisInstanceNameHash(0), //
      mSpaceNode(nullptr) {}

CPropAnimEntity::~CPropAnimEntity() {
    Purge();
}

// Functionally matching
bool CPropAnimEntity::Init(void *init_data, SpaceNode *parent_space_node) {
    Purge();
    uint32 play_flags;
    PropAnimEntityInfo *info = reinterpret_cast<PropAnimEntityInfo *>(init_data);
    mTypeID = info->mTypeID;
    mThisInstanceNameHash = info->mThisInstanceNameHash;

    if (mThisInstanceNameHash == info->mParentInstanceNameHash) {
        // maybe a debug print
    }

    if (info->mParentInstanceNameHash == 0 || mThisInstanceNameHash == info->mParentInstanceNameHash) {
        mSpaceNode = CreateSpaceNode(nullptr);
    } else {
        mSpaceNode = CreateSpaceNode(nullptr);
    }
    mSpaceNode->SetParent(parent_space_node);
    mSpaceNode->SetNameHash(mThisInstanceNameHash);
    mSpaceNode->SetLocalMatrix(&info->mLocalMatrix);
    mWorldModel = new WorldModel(mSpaceNode, info->mLODNameHash, true);

    return true;
}

void CPropAnimEntity::Purge() {
    if (mWorldModel) {
        delete mWorldModel;
        mWorldModel = nullptr;
    }
    if (mSpaceNode) {
        DeleteSpaceNode(this->mSpaceNode);
    }
}

void CPropAnimEntity::SetTime(float time) {}

void CPropAnimEntity::UpdateTimeStep(float time_step) {}
