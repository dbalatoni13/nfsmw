#include "AnimEntity_BasicCharacter.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct BoneMapping {
    int LeftFoot;
    int RightFoot;
    int LeftHand;
    int RightHand;
    int Head;
};

extern BoneMapping BoneMap[];
extern unsigned int skel_ROOT_hash;
extern int RenderCharacterShadows;

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

CBasicCharacterAnimEntity::~CBasicCharacterAnimEntity() {
    Purge();
}

bool CBasicCharacterAnimEntity::Init(void *init_data, SpaceNode *parent_space_node) {
    BasicCharacterAnimEntityInfo *info = reinterpret_cast<BasicCharacterAnimEntityInfo *>(init_data);
    unsigned int ctrl_flags;
    CAnimSkeleton *skeleton;
    bool use_bone_map = false;

    Purge();

    mTypeID = info->mTypeID;
    mThisInstanceNameHash = info->mThisInstanceNameHash;
    mSpaceNode = CreateSpaceNode(nullptr);
    mSpaceNode->SetParent(parent_space_node);
    mSpaceNode->SetNameHash(mThisInstanceNameHash);
    mSpaceNode->SetLocalMatrix(&info->mLocalMatrix);
    mWorldModel = new WorldModel(mSpaceNode, info->mLODNameHash, true);
    mAnimCtrl = new ("CAnimCtrl") CAnimCtrl();
    mAnimCtrl->SetNameHash(mSpaceNode->GetNameHash());
    mAnimCtrl->SetTimeScale(info->mPlaySpeed);
    if (mAnimCtrl->GetTimeScale() == 0.0f) {
        mAnimCtrl->SetTimeScale(1.0f);
    }

    ctrl_flags = info->mPlayFlags & 0x20;
    if (info->mPlayFlags & 0x40) {
        ctrl_flags = 0x60;
    }
    if (info->mPlayFlags & 0x10) {
        ctrl_flags |= 0x10;
    } else {
        ctrl_flags |= 8;
    }
    ctrl_flags |= info->mPlayFlags & 0x80;
    ctrl_flags |= info->mPlayFlags & 0x100;
    ctrl_flags |= info->mPlayFlags & 0x200;
    ctrl_flags |= info->mPlayFlags & 0x400;

    if (info->mSkelNameHash == 0) {
        skeleton = GetSkeletonFromList(skel_ROOT_hash);
        if (skeleton == nullptr) {
            return false;
        }
        mAnimCtrl->GetAnimPart()->Init(skeleton);
        if (ctrl_flags != 0) {
            ctrl_flags |= 2;
        } else {
            ctrl_flags = 0x2A;
        }
    } else {
        skeleton = GetSkeletonFromList(info->mSkelNameHash);
        if (skeleton == nullptr) {
            return false;
        }
        mAnimCtrl->GetAnimPart()->Init(skeleton);
        if (ctrl_flags == 0) {
            ctrl_flags = 0x29;
        } else {
            ctrl_flags |= 1;
        }
    }
    mAnimCtrl->SetFlags(ctrl_flags);

    if (info->mTransAnimNameHash != 0) {
        mAnimCtrl->CreateFnAnimFromNamehash(info->mTransAnimNameHash, 0);
    }
    if (info->mQuatsAnimNameHash != 0) {
        mAnimCtrl->CreateFnAnimFromNamehash(info->mQuatsAnimNameHash, 1);
    }
    if (info->mScaleAnimNameHash != 0) {
        mAnimCtrl->CreateFnAnimFromNamehash(info->mScaleAnimNameHash, 2);
    }
    if (info->mSkelNameHash != 0 && info->mSkelAnimNameHash != 0) {
        use_bone_map = true;
        mAnimCtrl->CreateFnAnimFromNamehash(info->mSkelAnimNameHash, 0);
        mAnimCtrl->SetTimeScale(mAnimCtrl->GetTimeScale() * 0.5f);
    }

    if (!mAnimCtrl->GetAllocated()) {
        return false;
    }

    if (info->mPlayFlags & 0x40) {
        mAnimCtrl->SetLoopRange(info->mLoopRangeStart, info->mLoopRangeEnd);
    }

    ctrl_flags |= 0x80;
    mAnimCtrl->SetMasterDelayTime(info->mPlayDelay);
    mAnimCtrl->SetFlags(ctrl_flags);

    if (mKeepOnGround) {
        bVector3 pelvis_position;
        float ground_elevation;
        float world_z;

        mAnimCtrl->UpdateAnimPose(true);
        FindWorldBonePosition(1, &pelvis_position);
        world_z = mSpaceNode->GetWorldMatrix()->v3.z;
        UMath::Vector3 point;
        point.x = -pelvis_position.y;
        point.y = pelvis_position.z;
        point.z = pelvis_position.x;
        if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(point, ground_elevation, nullptr)) {
            bMatrix4 local_matrix(*mSpaceNode->GetLocalMatrix());
            mPreviousElevation = ground_elevation;
            mHavePreviousElevation = true;
            local_matrix.v3.z += (mPreviousElevation - world_z) + 0.05f;
            mSpaceNode->SetLocalMatrix(&local_matrix);
        }
    }

    if (use_bone_map) {
        mBoneMapType = -1;
        if (mAnimCtrl->GetAnimPart()->GetNumGlobalMatrices() == 0x30) {
            mBoneMapType = 2;
            if (info->mSkelNameHash == bStringHash("Bip23")) {
                mBoneMapType = 3;
            }
        }
    }

    return true;
}

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

void CBasicCharacterAnimEntity::UpdateTimeStep(float time_step) {
    if (mAnimCtrl != nullptr) {
        bVector3 last_pos;
        last_pos.x = mSpaceNode->GetLocalMatrix()->v3.x;
        last_pos.y = mSpaceNode->GetLocalMatrix()->v3.y;
        last_pos.z = mSpaceNode->GetLocalMatrix()->v3.z;

        mAnimCtrl->AdvanceAnimTime(time_step);
        mAnimCtrl->UpdateAnimPose(true);

        bMatrix4 *global_matrices = reinterpret_cast<bMatrix4 *>(mAnimCtrl->GetAnimPart()->GetGlobalMatrices());
        if ((mAnimCtrl->GetFlags() & 1) == 0) {
            bMatrix4 local_matrix(*mSpaceNode->GetLocalMatrix());
            bMatrix4 the_matrix;

            mSpaceNode->SetBlendingMatrices(nullptr);
            bIdentity(&the_matrix);
            bMulMatrix(&the_matrix, &local_matrix, &global_matrices[1]);
            mSpaceNode->SetLocalMatrix(&the_matrix);
        } else {
            mSpaceNode->SetBlendingMatrices(global_matrices);
            if (!mKeepOnGround) {
                bMatrix4 local_matrix(*mSpaceNode->GetLocalMatrix());
                bMatrix4 the_matrix;

                bMulMatrix(&the_matrix, &local_matrix, &global_matrices[1]);
                mSpaceNode->SetLocalMatrix(&local_matrix);
            } else {
                bVector3 pelvis_position;
                float ground_elevation;

                FindWorldBonePosition(1, &pelvis_position);
                float world_z = mSpaceNode->GetWorldMatrix()->v3.z;
                UMath::Vector3 point;
                point.x = -pelvis_position.y;
                point.y = pelvis_position.z + 2.0f;
                point.z = pelvis_position.x;
                if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(point, ground_elevation, nullptr)) {
                    if (!mHavePreviousElevation) {
                        mHavePreviousElevation = true;
                        mPreviousElevation = ground_elevation;
                    } else {
                        mPreviousElevation = ground_elevation * 0.5f + mPreviousElevation * 0.5f;
                    }
                    mSpaceNode->GetLocalMatrix()->v3.z += (mPreviousElevation - world_z) + 0.05f;
                }
            }
        }

        if (time_step > 0.0001f) {
            bVector3 new_pos;
            bVector3 diff;
            float inv_time_step = 1.0f / time_step;

            new_pos.x = mSpaceNode->GetLocalMatrix()->v3.x;
            new_pos.y = mSpaceNode->GetLocalMatrix()->v3.y;
            new_pos.z = mSpaceNode->GetLocalMatrix()->v3.z;
            diff.x = (new_pos.x - last_pos.x) * inv_time_step;
            diff.y = (new_pos.y - last_pos.y) * inv_time_step;
            diff.z = (new_pos.z - last_pos.z) * inv_time_step;
            mSpaceNode->SetLocalVelocity(&diff);
            mSpaceNode->SetDirty();
        }
    }
}

void CBasicCharacterAnimEntity::RenderEffects(eView *view, int is_reflection) {
    if (mBoneMapType == 3) {
        bVector3 head;
        FindWorldBonePosition(BoneMap[3].Head, &head);
        FacePixelation::SetLocation(head);
    }

    if (RenderCharacterShadows && mDrawShadow && !is_reflection && CharacterShadowTexture != nullptr && mBoneMapType != -1) {
        bVector3 left_foot;
        bVector3 right_foot;
        bVector2 parallel(1.0f, 0.0f);
        bVector2 perpendicular;
        bVector2 left0;
        bVector2 left1;
        bVector2 right0;
        bVector2 right1;
        ePoly shadow_poly;
        float ground;

        FindWorldBonePosition(BoneMap[mBoneMapType].LeftFoot, &left_foot);
        FindWorldBonePosition(BoneMap[mBoneMapType].RightFoot, &right_foot);
        if (left_foot.x != right_foot.x || left_foot.y != right_foot.y) {
            parallel.x = left_foot.x - right_foot.x;
            parallel.y = left_foot.y - right_foot.y;
            bNormalize(&parallel, &parallel);
        }

        perpendicular.x = -parallel.y;
        perpendicular.y = parallel.x;
        parallel *= 0.25f;
        perpendicular *= 0.35f;

        left0 = bVector2(left_foot.x + parallel.x - perpendicular.x, left_foot.y + parallel.y - perpendicular.y);
        left1 = bVector2(left_foot.x + parallel.x + perpendicular.x, left_foot.y + parallel.y + perpendicular.y);
        right0 = bVector2(right_foot.x - parallel.x + perpendicular.x, right_foot.y - parallel.y + perpendicular.y);
        right1 = bVector2(right_foot.x - parallel.x - perpendicular.x, right_foot.y - parallel.y - perpendicular.y);

        ground = mSpaceNode->GetWorldMatrix()->v3.z + 0.01f;
        shadow_poly.Vertices[0] = bVector3(left0.x, left0.y, ground);
        shadow_poly.Vertices[1] = bVector3(left1.x, left1.y, ground);
        shadow_poly.Vertices[2] = bVector3(right0.x, right0.y, ground);
        shadow_poly.Vertices[3] = bVector3(right1.x, right1.y, ground);
        shadow_poly.Colours[0][0] = 0x80;
        shadow_poly.Colours[0][1] = 0x80;
        shadow_poly.Colours[0][2] = 0x80;
        shadow_poly.Colours[0][3] = 0x80;
        shadow_poly.Colours[1][0] = 0x80;
        shadow_poly.Colours[1][1] = 0x80;
        shadow_poly.Colours[1][2] = 0x80;
        shadow_poly.Colours[1][3] = 0x80;
        shadow_poly.Colours[2][0] = 0x80;
        shadow_poly.Colours[2][1] = 0x80;
        shadow_poly.Colours[2][2] = 0x80;
        shadow_poly.Colours[2][3] = 0x80;
        shadow_poly.Colours[3][0] = 0x80;
        shadow_poly.Colours[3][1] = 0x80;
        shadow_poly.Colours[3][2] = 0x80;
        shadow_poly.Colours[3][3] = 0x80;
        shadow_poly.UVs[0][0] = 0.0f;
        shadow_poly.UVs[0][1] = 0.0f;
        shadow_poly.UVs[1][0] = 1.0f;
        shadow_poly.UVs[1][1] = 0.0f;
        shadow_poly.UVs[2][0] = 1.0f;
        shadow_poly.UVs[2][1] = 1.0f;
        shadow_poly.UVs[3][0] = 0.0f;
        shadow_poly.UVs[3][1] = 1.0f;
        view->Render(&shadow_poly, CharacterShadowTexture, &eMathIdentityMatrix, 0, 0.0f);
    }
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
