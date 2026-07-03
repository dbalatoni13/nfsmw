#include "./WorldModel.hpp"
#include "Speed/Indep/Src/Animation/AnimScene.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"

class AABBAdjustor {
  public:
    void Adjust(float scaler) {
        this->mMin->x += this->mAdjustment->x * scaler;
        this->mMax->x += this->mAdjustment->x * scaler;
        this->mMin->y += this->mAdjustment->y * scaler;
        this->mMax->y += this->mAdjustment->y * scaler;
        this->mMin->z += this->mAdjustment->z * scaler;
        this->mMax->z += this->mAdjustment->z * scaler;
    }

    AABBAdjustor(eModel *m, bMatrix4 *adjustment) {
        if (adjustment != nullptr) {
            this->mMin = reinterpret_cast<bVector4 *>(&m->GetSolid()->AABBMinX);
            this->mMax = reinterpret_cast<bVector4 *>(&m->GetSolid()->AABBMaxX);
            this->mAdjustment = &adjustment[1].v3;
            this->Adjust(1.0f);
        } else {
            this->mMin = nullptr;
            this->mMax = nullptr;
            this->mAdjustment = nullptr;
        }
    }

    ~AABBAdjustor() {
        if (this->mAdjustment != nullptr) {
            this->Adjust(-1.0f);
        }
    }

  private:
    bVector4 *mMin;
    bVector4 *mMax;
    bVector4 *mAdjustment;
};

bTList<WorldModel> WorldModelList;

SlotPool *WorldModelSlotPool = nullptr;
static const int32 DrawWorldModels = 1;

WorldModel::WorldModel(unsigned int name_hash, bMatrix4 *matrix, bool add_lighting) {
    this->pModel = new eModel(name_hash);
    this->pReflectionModel = nullptr;
    this->Construct(nullptr, matrix, nullptr, 0, add_lighting);
}

WorldModel::WorldModel(SpaceNode *spacenode, unsigned int *lod_name_hash, bool add_lighting) {
    this->pModel = new eModel(lod_name_hash[0]);

    if (lod_name_hash[3]) {
        this->pReflectionModel = new eModel(lod_name_hash[3]);
    } else {
        this->pReflectionModel = nullptr;
    }

    this->Construct(spacenode, nullptr, nullptr, 0, add_lighting);
}

WorldModel::WorldModel(const ModelHeirarchy *heirarchy, unsigned int heirarchy_index, bool add_lighting) {
    this->pModel = nullptr;
    this->pReflectionModel = nullptr;
    this->Construct(nullptr, nullptr, heirarchy, heirarchy_index, add_lighting);
}

void WorldModel::Construct(SpaceNode *spacenode, bMatrix4 *matrix, const ModelHeirarchy *heirarchy, unsigned int rootnode, bool add_lighting) {
    this->mDistanceToGameView = 0.0f;
    this->mLastRenderFrame = 0;
    this->mLastVisibleFrame = 0;
    this->mLightMaterial = nullptr;
    this->mLightMaterialSkinHash = 0;

    if ((heirarchy != nullptr) && rootnode < heirarchy->mNumNodes) {
        this->mHeirarchyIndex = rootnode;
        this->mHeirarchy = heirarchy;
        this->mChildVisibility = 0xFFFFFF;
    } else {
        this->mChildVisibility = 0;
        this->mHeirarchyIndex = 0;
        this->mHeirarchy = nullptr;
    }

    this->mInvisibleInside = false;
    this->mEnabled = true;
    this->mRenderInSplitScreen = true;
#ifndef EA_BUILD_A124
    this->mCastsShadow = 1;
#endif
    this->pSpaceNode = spacenode;

    if (spacenode != nullptr) {
        spacenode->Lock();
    }

    if (matrix != nullptr) {
        this->SetMatrix(matrix);
    }

    WorldModelList.AddTail(this);
    this->mAddLighting = add_lighting;
}

WorldModel::~WorldModel() {
    delete this->pModel;
    delete this->pReflectionModel;
    if (this->pSpaceNode != nullptr) {
        this->pSpaceNode->Unlock();
    }
    WorldModelList.Remove(this);
}

eModel *WorldModel::GetModel() {
    if (this->pModel != nullptr) {
        return this->pModel;
    }

    if (this->mHeirarchy != nullptr) {
        return mHeirarchy->GetNodes()[mHeirarchyIndex].mModel;
    }

    return nullptr;
}

void WorldModel::AttachReplacementTextureTable(eReplacementTextureTable *replacement_texture_table, int num_textures) {
    if (this->pModel != nullptr) {
        this->pModel->AttachReplacementTextureTable(replacement_texture_table, num_textures, 0);
    }

    if (this->pReflectionModel != nullptr) {
        this->pReflectionModel->AttachReplacementTextureTable(replacement_texture_table, num_textures, 0);
    }
}

// STRIPPED
uint32 WorldModel::GetNameHash() {}

// STRIPPED
const char *WorldModel::GetName() {}

void WorldModel::GetLocalBoundingBox(bVector3 *min_ext, bVector3 *max_ext) {
    eModel *model = this->GetModel();

    if (model != nullptr) {
        model->GetBoundingBox(min_ext, max_ext);
    } else {
        *max_ext = *min_ext = bVector3(0.0f, 0.0f, 0.0f);
    }
}

void InitWorldModels() {
    WorldModelSlotPool = bNewSlotPool(sizeof(WorldModel), 128, "WorldModelSlotPool", 0);
}

void CloseWorldModels() {
    if (!WorldModelList.IsEmpty()) {
        while (!WorldModelList.IsEmpty()) {
            WorldModel *w = WorldModelList.GetHead();
            delete w;
        }
    }

    bDeleteSlotPool(WorldModelSlotPool);
    WorldModelSlotPool = nullptr;
}

void WorldModel::RenderNode(const ModelHeirarchy *heirarchy, unsigned int nodeIndex, eView *view, int exc_flag, bMatrix4 *blended_matrices,
                            const bMatrix4 *matrix) {
    const ModelHeirarchy::Node *node = &heirarchy->GetNodes()[nodeIndex];

    if ((node->mModel != nullptr) && (node->mModel->GetSolid() != nullptr)) {
        this->RenderModel(node->mModel, view, exc_flag, blended_matrices, matrix);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        if (this->mHeirarchyIndex != nodeIndex || (this->mChildVisibility & (1 << i))) {
            if ((heirarchy->GetNodes()[node->mChildIndex + i].mFlags & ModelHeirarchy::F_INTERNAL) == 0) {
                this->RenderNode(heirarchy, node->mChildIndex + i, view, exc_flag, blended_matrices, matrix);
            }
        }
    }
}

void WorldModel::RenderModel(eModel *render_model, eView *view, int exc_flag, bMatrix4 *blended_matrices, const bMatrix4 *matrix) {
    unsigned int flags = exc_flag;
    AABBAdjustor adjustor(render_model, blended_matrices);
    bMatrix4 *frame_matrix = static_cast<bMatrix4 *>(eFrameMalloc(sizeof(bMatrix4)));

    if (frame_matrix != nullptr) {
        *frame_matrix = *matrix;
        if (flags & 0x800) {
            frame_matrix->v2.z = -frame_matrix->v2.z;
        }

        eDynamicLightContext *light_context = nullptr;
        if (mAddLighting) {
            light_context = static_cast<eDynamicLightContext *>(eFrameMalloc(sizeof(eDynamicLightContext) + 0xC)); // TODO why +0xC?
            if (light_context == nullptr) {
                return;
            }

            bVector4 camera_world_position;
            bVector3 *eye = view->GetCamera()->GetPosition();
            Camera *camera = view->GetCamera();
            bMatrix4 *world_view = camera->GetCameraMatrix();

            camera_world_position.x = eye->x;
            camera_world_position.y = eye->y;
            camera_world_position.z = eye->z;
            camera_world_position.w = 1.0f;

            bMatrix4 *actual_frame_matrix;
            bMatrix4 moved_frame_matrix;
            if (blended_matrices != nullptr) {
                bVector4 pelvis_pos = blended_matrices[1].v3;

                eMulVector(&pelvis_pos, frame_matrix, &pelvis_pos);
                moved_frame_matrix = *frame_matrix;
                moved_frame_matrix.v3 = pelvis_pos;
                actual_frame_matrix = &moved_frame_matrix;
                AdjustQuickDynamicLight(&ShaperLightsCharacters, reinterpret_cast<bVector3 *>(&camera_world_position));
                elSetupLightContext(light_context, &ShaperLightsCharacters, actual_frame_matrix, world_view, &camera_world_position, view);
                ShaperLightsCharacters.NumOverideSlots = 0;
            } else {
                elSetupLightContext(light_context, &ShaperLightsCarsInGame, frame_matrix, world_view, &camera_world_position, view);
            }
        }

        if (this->mLightMaterial != nullptr) {
            render_model->ReplaceLightMaterial(this->mLightMaterialSkinHash, this->mLightMaterial);
        }

        view->Render(render_model, frame_matrix, light_context, 0, blended_matrices);
    }
}

static const float AmbientCharacterLightRed = 0x42800000;
static const float AmbientCharacterLightGreen = 0x42800000;
static const float AmbientCharacterLightBlue = 0x42800000;

static const float WorldObjectMaximumRadius = 35.0f;

// TODO how to initialize these?
bVector4 delta2;
bVector4 delta66;

void WorldModel::Render(eView *view, int exc_flag) {
    if (this->mLastRenderFrame != eGetFrameCounter()) {
        this->mLastRenderFrame = eGetFrameCounter();
        this->mDistanceToGameView = 999999.0f;
    }

    CameraMover *mover = view->GetCameraMover();

    if ((mover != nullptr) && eIsGameViewID(view->GetID())) {
        this->mDistanceToGameView = UMath::Min(this->mDistanceToGameView, mover->GetDistanceTo(reinterpret_cast<bVector3 *>(&this->GetMatrix()->v3)));
    }

    eModel *render_model = this->GetModel();
    if (exc_flag & 0x800) {
        if (this->pReflectionModel != nullptr) {
            render_model = this->pReflectionModel;
        } else {
            return;
        }
    }

    if ((render_model == nullptr) || (render_model->GetSolid() == nullptr)) {
        return;
    }

    if (exc_flag & 0x200000) {
#ifndef EA_BUILD_A124
        if (!this->mCastsShadow) {
            return;
        }
#endif
        if (exc_flag & 0x2000) {
            if (render_model->GetSolid()->NumBones) {
                return;
            }
        } else if (render_model->GetSolid()->NumBones == 0) {
            return;
        }
    }

    bMatrix4 *matrix;
    bMatrix4 *blended_matrices = nullptr;
    bMatrix4 clip_matrix;
    if (this->pSpaceNode != nullptr) {
        matrix = this->pSpaceNode->GetWorldMatrix();
        blended_matrices = this->pSpaceNode->GetBlendingMatrices();
        if (blended_matrices != nullptr) {
            bMulMatrix(&clip_matrix, matrix, &blended_matrices[1]);
        } else {
            bCopy(&clip_matrix, matrix);
        }
    } else {
        matrix = this->GetMatrix();
        bCopy(&clip_matrix, matrix);
    }

    const bVector3 *position = reinterpret_cast<const bVector3 *>(&clip_matrix.v3);
    if (view->GetPixelSize(position, WorldObjectMaximumRadius) < view->GetPixelMinSize()) {
        return;
    }

    if (view->IsVisible(render_model, &clip_matrix)) {
        const ModelHeirarchy *heirarchy = this->GetHeirarchy();
        if (heirarchy != nullptr) {
            this->RenderNode(heirarchy, this->mHeirarchyIndex, view, exc_flag, blended_matrices, matrix);
        } else {
            this->RenderModel(render_model, view, exc_flag, blended_matrices, matrix);
        }

        mLastVisibleFrame = eGetFrameCounter();
    }
}

static const int DisableWorldObjectReflections = 0;

void RenderWorldModels(eView *view, int exc_flag) {
    ProfileNode profile_node("TODO", 0);

    if (!DrawWorldModels || (DisableWorldObjectReflections && (exc_flag & 0x1000) != 0)) {
        return;
    }

    bool split_screen = eGetCurrentViewMode() == EVIEWMODE_TWOH;

    for (WorldModel *w = WorldModelList.GetHead(); w != WorldModelList.EndOfList(); w = w->GetNext()) {
        if (w->IsEnabled() && (!split_screen || w->CanRenderInSplitScreen())) {
            if (w->InvisibleInside()) {
                bMatrix4 matrix;
                bVector3 camera_pos;
                bVector3 box_min;
                bVector3 box_max;

                bCopy(&matrix, w->GetMatrix(), w->GetPosition());
                eInvertTransformationMatrix(&matrix, &matrix);
                w->GetLocalBoundingBox(&box_min, &box_max);
                eMulVector(&camera_pos, &matrix, view->GetCamera()->GetPosition());

                if (bBoundingBoxIsInside(&box_min, &box_max, &camera_pos, 0.0f) != 0) {
                    continue;
                }
            }

            w->Render(view, exc_flag);
        }
    }

    RenderAnimSceneEffects(view, exc_flag);
}

// STRIPPED
bool AnyWorldModelsInBox(bVector3 *pMin, bVector3 *pMax) {}
