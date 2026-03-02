#include "AnimEntity_BasicCharacter.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

TextureInfo *CharacterShadowTexture = nullptr;

void InitCharacterEffects() {
    CharacterShadowTexture = GetTextureInfo(bStringHash("CHARACTERSHADOW"), true, false);
}

void CloseCharacterEffects() {
    CharacterShadowTexture = nullptr;
}

void CBasicCharacterAnimEntity::EndianSwapEntityData(void *data, int size) {
    BasicCharacterAnimEntityInfo *info = reinterpret_cast<BasicCharacterAnimEntityInfo *>(data);
    bPlatEndianSwap(&info->mThisInstanceNameHash);
    bPlatEndianSwap(&info->mParentInstanceNameHash);
    bPlatEndianSwap(&info->mLocalMatrix);
    bPlatEndianSwap(&info->mLODNameHash[0]);
    bPlatEndianSwap(&info->mLODNameHash[1]);
    bPlatEndianSwap(&info->mLODNameHash[2]);
    bPlatEndianSwap(&info->mLODNameHash[3]);
    bPlatEndianSwap(&info->mTransAnimNameHash);
    bPlatEndianSwap(&info->mQuatsAnimNameHash);
    bPlatEndianSwap(&info->mScaleAnimNameHash);
    bPlatEndianSwap(&info->mSkelAnimNameHash);
    bPlatEndianSwap(&info->mSkelNameHash);
    bPlatEndianSwap(&info->mPlayFlags);
    bPlatEndianSwap(&info->mPlaySpeed);
    bPlatEndianSwap(&info->mPlayDelay);
    bPlatEndianSwap(&info->mWorldObjectTypeNameHash);
    bPlatEndianSwap(&info->mLoopRangeStart);
    bPlatEndianSwap(&info->mLoopRangeEnd);
    bPlatEndianSwap(&info->mPad1);
    bPlatEndianSwap(&info->mPad2);
}

CBasicCharacterAnimEntity::CBasicCharacterAnimEntity()
    : mDrawShadow(true),             //
      mTypeID(0),                    //
      mThisInstanceNameHash(0),      //
      mSpaceNode(nullptr),           //
      mWorldModel(nullptr),          //
      mAnimCtrl(nullptr),            //
      mKeepOnGround(true),           //
      mHavePreviousElevation(false), //
      mBoneMapType(-1) {}

CBasicCharacterAnimEntity::~CBasicCharacterAnimEntity() {}

void CBasicCharacterAnimEntity::Purge() {
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

void CBasicCharacterAnimEntity::SetTime(float time) {
    if (mAnimCtrl) {
        mAnimCtrl->SetEvalTime(0.0f);
    }
    UpdateTimeStep(time);
}

void CBasicCharacterAnimEntity::FindWorldBonePosition(int bone, bVector3 *position) {
    bMatrix4 world_matrix = *mSpaceNode->GetWorldMatrix();
    bMatrix4 *global_matrices = reinterpret_cast<bMatrix4 *>(mAnimCtrl->GetAnimPart()->GetGlobalMatrices());
    EAGL4Anim::Skeleton *skeleton = mAnimCtrl->GetAnimPart()->GetSkeleton()->GetEAGLSkeleton();
    EAGL4Anim::BoneData *bone_data = &skeleton->GetBoneData(bone);
    bMatrix4 invert_invert_bone_matrix;
    bMatrix4 bone_matrix;
    bInvertMatrix(&invert_invert_bone_matrix, reinterpret_cast<bMatrix4 *>(&bone_data->mInvBaseMatrix));
    bMulMatrix(&bone_matrix, &global_matrices[bone], &invert_invert_bone_matrix);
    bCopy(position, reinterpret_cast<bVector3 *>(&bone_matrix.v3));
    bMulMatrix(position, &world_matrix, position);
}
