#include "AnimEntity_BasicCharacter.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
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
    unsigned int play_flags = 0;
    BasicCharacterAnimEntityInfo *info = reinterpret_cast<BasicCharacterAnimEntityInfo *>(init_data);

    Purge();

    mTypeID = info->mTypeID;
    mThisInstanceNameHash = info->mThisInstanceNameHash;
    asm("cmpw 7, %0, %1" : : "r"(mThisInstanceNameHash), "r"(info->mParentInstanceNameHash) : "cr7");
    if (info->mParentInstanceNameHash == 0) {
        mSpaceNode = CreateSpaceNode(nullptr);
    } else {
        mSpaceNode = CreateSpaceNode(nullptr);
    }
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

    if (info->mPlayFlags & 0x40) {
        play_flags |= 0x60;
    } else if (info->mPlayFlags & 0x20) {
        play_flags |= 0x20;
    }
    if (info->mPlayFlags & 0x10) {
        play_flags |= 0x10;
    } else {
        play_flags |= 8;
    }
    if (info->mPlayFlags & 0x80) {
        play_flags |= 0x80;
    }
    if (info->mPlayFlags & 0x100) {
        play_flags |= 0x100;
    }
    if (info->mPlayFlags & 0x200) {
        play_flags |= 0x200;
    }
    if (info->mPlayFlags & 0x400) {
        play_flags |= 0x400;
    }

    CAnimPart *anim_part = mAnimCtrl->GetAnimPart();
    CAnimSkeleton *skel;
    if (info->mSkelNameHash != 0) {
        skel = GetSkeletonFromList(info->mSkelNameHash);
        if (skel == nullptr) {
            goto init_fail;
        }
        anim_part->Init(skel);
        if (play_flags == 0) {
            mAnimCtrl->SetFlags(0x29);
        } else {
            mAnimCtrl->SetFlags(play_flags | 1);
        }
    } else {
        skel = GetSkeletonFromList(skel_ROOT_hash);
        if (skel == nullptr) {
            goto init_fail;
        }
        anim_part->Init(skel);
        if (play_flags == 0) {
            mAnimCtrl->SetFlags(0x2A);
        } else {
            mAnimCtrl->SetFlags(play_flags | 2);
        }
    }

    if (skel == nullptr) {
    init_fail:
        return false;
    }

    if (info->mTransAnimNameHash != 0) {
        mAnimCtrl->CreateFnAnimFromNamehash(info->mTransAnimNameHash, 0);
    }
    if (info->mQuatsAnimNameHash != 0) {
        mAnimCtrl->CreateFnAnimFromNamehash(info->mQuatsAnimNameHash, 1);
    }
    if (info->mScaleAnimNameHash != 0) {
        mAnimCtrl->CreateFnAnimFromNamehash(info->mScaleAnimNameHash, 2);
    }
    bool skeletal_animation = false;
    if (info->mSkelNameHash != 0 && info->mSkelAnimNameHash != 0) {
        skeletal_animation = true;
        mAnimCtrl->CreateFnAnimFromNamehash(info->mSkelAnimNameHash, 0);
        mAnimCtrl->SetTimeScale(mAnimCtrl->GetTimeScale() * 0.5f);
    }

    if (!mAnimCtrl->GetAllocated()) {
        return false;
    }

    if (mAnimCtrl) {
        if (info->mPlayFlags & 0x40) {
            mAnimCtrl->SetLoopRange(info->mLoopRangeStart, info->mLoopRangeEnd);
        }
        CAnimCtrl *ctrl = mAnimCtrl;
        ctrl->SetMasterDelayTime(info->mPlayDelay);
        ctrl->SetFlags(0x80);
        if (mAnimCtrl != nullptr && mAnimCtrl->GetFlags() == 8) {
            bBreak();
        }
    }

    if (mKeepOnGround && mAnimCtrl) {
        bVector3 pelvis_position;

        mAnimCtrl->UpdateAnimPose(true);
        FindWorldBonePosition(1, &pelvis_position);
        float non_adjusted_z = mSpaceNode->GetWorldMatrix()->v3.z;
        eUnSwizzleWorldVector(pelvis_position, pelvis_position);
        float ground_elevation;
        if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(
                *reinterpret_cast<UMath::Vector3 *>(&pelvis_position), ground_elevation, nullptr)) {
            mPreviousElevation = ground_elevation;
            mHavePreviousElevation = true;
            bMatrix4 local_matrix(*mSpaceNode->GetLocalMatrix());
            local_matrix.v3.z += (mPreviousElevation - non_adjusted_z) + 0.05f;
            mSpaceNode->SetLocalMatrix(&local_matrix);
        }
    }

    if (skeletal_animation && mAnimCtrl != nullptr && anim_part != nullptr) {
        if (anim_part->GetNumGlobalMatrices() == 0x30) {
            if (info->mSkelNameHash == bStringHash("Bip23")) {
                mBoneMapType = 3;
            } else {
                mBoneMapType = 2;
            }
        } else {
            mBoneMapType = -1;
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
        bVector3 last_pos(*reinterpret_cast<bVector3 *>(&mSpaceNode->GetLocalMatrix()->v3));

        mAnimCtrl->AdvanceAnimTime(time_step);
        mAnimCtrl->UpdateAnimPose(true);

        bMatrix4 *global_matrices = reinterpret_cast<bMatrix4 *>(mAnimCtrl->GetAnimPart()->GetGlobalMatrices());
        if (mAnimCtrl->GetFlags() & 1) {
            mSpaceNode->SetBlendingMatrices(global_matrices);
            if (mKeepOnGround) {
                bVector3 pelvis_position;
                FindWorldBonePosition(1, &pelvis_position);
                float non_adjusted_z = mSpaceNode->GetWorldMatrix()->v3.z;
                eUnSwizzleWorldVector(pelvis_position, pelvis_position);
                pelvis_position.y += 2.0f;
                float ground_elevation;
                if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(
                        *reinterpret_cast<UMath::Vector3 *>(&pelvis_position), ground_elevation, nullptr)) {
                    if (mHavePreviousElevation) {
                        mPreviousElevation = ground_elevation * 0.5f + mPreviousElevation * 0.5f;
                    } else {
                        mHavePreviousElevation = true;
                        mPreviousElevation = ground_elevation;
                    }
                    mSpaceNode->GetLocalMatrix()->v3.z += (mPreviousElevation - non_adjusted_z) + 0.05f;
                }
            } else {
                bMatrix4 local_matrix(*mSpaceNode->GetLocalMatrix());
                bMatrix4 bip_matrix;
                bMulMatrix(&bip_matrix, &local_matrix, &global_matrices[1]);
                mSpaceNode->SetLocalMatrix(&local_matrix);
            }
        } else {
            bMatrix4 local_matrix(*mSpaceNode->GetLocalMatrix());
            bMatrix4 the_matrix;
            mSpaceNode->SetBlendingMatrices(nullptr);
            bIdentity(&the_matrix);
            bMulMatrix(&the_matrix, &local_matrix, &global_matrices[1]);
            mSpaceNode->SetLocalMatrix(&the_matrix);
        }

        if (time_step > 0.0001f) {
            bVector3 new_pos(*reinterpret_cast<bVector3 *>(&mSpaceNode->GetLocalMatrix()->v3));
            bVector3 diff = new_pos - last_pos;
            float inv_time_step = 1.0f / time_step;
            bScale(&diff, &diff, inv_time_step);
            mSpaceNode->SetLocalVelocity(&diff);
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
        bVector2 parallel;
        bVector2 perpendicular;
        bVector2 left0;
        bVector2 left1;
        bVector2 right0;
        bVector2 right1;

        FindWorldBonePosition(BoneMap[mBoneMapType].LeftFoot, &left_foot);
        FindWorldBonePosition(BoneMap[mBoneMapType].RightFoot, &right_foot);
        bFill(&parallel, 1.0f, 0.0f);
        if (left_foot.x != right_foot.x || left_foot.y != right_foot.y) {
            parallel.x = left_foot.x - right_foot.x;
            parallel.y = left_foot.y - right_foot.y;
            bNormalize(&parallel, &parallel);
        }

        bFill(&perpendicular, -parallel.y, parallel.x);
        parallel *= 0.25f;
        perpendicular *= 0.35f;

        bFill(&left0, left_foot.x, left_foot.y);
        bFill(&left1, left_foot.x, left_foot.y);
        bFill(&right0, right_foot.x, right_foot.y);
        bFill(&right1, right_foot.x, right_foot.y);
        left0.x += parallel.x + perpendicular.x;
        left0.y += parallel.y + perpendicular.y;
        left1.x += parallel.x - perpendicular.x;
        left1.y += parallel.y - perpendicular.y;
        right0.x -= parallel.x - perpendicular.x;
        right0.y -= parallel.y - perpendicular.y;
        right1.x -= parallel.x + perpendicular.x;
        right1.y -= parallel.y + perpendicular.y;

        ePoly shadow_poly;
        float ground = mSpaceNode->GetWorldMatrix()->v3.z + 0.01f;
        shadow_poly.Vertices[0] = bVector3(left1.x, left1.y, ground);
        shadow_poly.Vertices[1] = bVector3(left0.x, left0.y, ground);
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
        shadow_poly.UVs[0][2] = 1.0f;
        shadow_poly.UVs[0][3] = 0.0f;
        shadow_poly.UVs[1][0] = 1.0f;
        shadow_poly.UVs[1][1] = 1.0f;
        shadow_poly.UVs[1][2] = 0.0f;
        shadow_poly.UVs[1][3] = 1.0f;
        view->Render(&shadow_poly, CharacterShadowTexture, eGetIdentityMatrix(), 0, 0.0f);
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
