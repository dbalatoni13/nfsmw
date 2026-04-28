#include "./WorldModel.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"

extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern float lbl_8040CD80;
extern float lbl_8040CD84;
extern float lbl_8040CD88;
extern float lbl_8040CD8C;
extern float lbl_8040CD90;
extern float lbl_8040CD94;
extern eShaperLightRig ShaperLightsCarsInGame;
extern eShaperLightRig ShaperLightsCharacters;
extern int bBoundingBoxIsInside(const bVector3 *bbox_min, const bVector3 *bbox_max, const bVector3 *point, float extra_width);
extern void RenderAnimSceneEffects(eView *view, int exc_flag) asm("RenderAnimSceneEffects__FP5eViewi");

bTList<WorldModel> WorldModelList;

int elSetupLightContext(eDynamicLightContext *light_context, eShaperLightRig *shaper_lights, bMatrix4 *local_world, bMatrix4 *world_view,
                        bVector4 *camera_world_position, eView *view);
void AdjustQuickDynamicLight(eShaperLightRig *ShaperRigP, bVector3 *MyPosition);

namespace {

void Render(eViewPlatInterface *view, eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, unsigned int flags,
            unsigned int exc_flag) asm("Render__18eViewPlatInterfaceP6eModelP8bMatrix4P13eLightContextUiT2");

inline void *eFrameMalloc(unsigned int size) {
    unsigned char *address = CurrentBufferPos;

    if (CurrentBufferEnd <= CurrentBufferPos + size) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += size;
        address = 0;
    } else {
        CurrentBufferPos += size;
    }

    return address;
}

struct AABBAdjustor {
    bVector4 *mMin;
    bVector4 *mMax;
    bVector4 *mAdjustment;

    void Adjust(float scaler) {
        if (this->mAdjustment != 0) {
            this->mMin->x += this->mAdjustment->x * scaler;
            this->mMax->x += this->mAdjustment->x * scaler;
            this->mMin->y += this->mAdjustment->y * scaler;
            this->mMax->y += this->mAdjustment->y * scaler;
            this->mMin->z += this->mAdjustment->z * scaler;
            this->mMax->z += this->mAdjustment->z * scaler;
        }
    }

    AABBAdjustor(eModel *m, bMatrix4 *adjustment) {
        if (adjustment != 0) {
            eSolid *solid = m->GetSolid();

            if (solid != 0) {
                this->mMin = reinterpret_cast<bVector4 *>(&solid->AABBMinX);
                this->mMax = reinterpret_cast<bVector4 *>(&solid->AABBMaxX);
                this->mAdjustment = &adjustment[1].v3;
                this->Adjust(1.0f);
                return;
            }
        }
        this->mMin = 0;
        this->mMax = 0;
        this->mAdjustment = 0;
    }

    ~AABBAdjustor() {
        this->Adjust(-1.0f);
    }
};

} // namespace

WorldModel::WorldModel(unsigned int name_hash, bMatrix4 *matrix, bool add_lighting) {
    eModel *model = static_cast<eModel *>(bOMalloc(eModelSlotPool));
    model->NameHash = 0;
    model->Solid = 0;
    model->Init(name_hash);
    this->pModel = model;
    this->pReflectionModel = 0;
    this->Construct(0, matrix, 0, 0, add_lighting);
}

WorldModel::WorldModel(SpaceNode *spacenode, unsigned int *lod_name_hash, bool add_lighting) {
    eModel *model = static_cast<eModel *>(bOMalloc(eModelSlotPool));
    unsigned int model_name_hash = *lod_name_hash;
    model->NameHash = 0;
    model->Solid = 0;
    model->Init(model_name_hash);
    this->pModel = model;

    if (lod_name_hash[3] != 0) {
        eModel *reflection_model = static_cast<eModel *>(bOMalloc(eModelSlotPool));
        unsigned int reflection_name_hash = lod_name_hash[3];
        reflection_model->NameHash = 0;
        reflection_model->Solid = 0;
        reflection_model->Init(reflection_name_hash);
        this->pReflectionModel = reflection_model;
    } else {
        this->pReflectionModel = 0;
    }

    this->Construct(spacenode, 0, 0, 0, add_lighting);
}

WorldModel::WorldModel(const ModelHeirarchy *heirarchy, unsigned int heirarchy_index, bool add_lighting) {
    this->pModel = 0;
    this->pReflectionModel = 0;
    this->Construct(0, 0, heirarchy, heirarchy_index, add_lighting);
}

void WorldModel::Construct(SpaceNode *spacenode, bMatrix4 *matrix, const ModelHeirarchy *heirarchy, unsigned int rootnode, bool add_lighting) {
    this->mDistanceToGameView = lbl_8040CD80;
    this->mLastRenderFrame = 0;
    this->mLastVisibleFrame = 0;
    this->mLightMaterial = 0;
    this->mLightMaterialSkinHash = 0;

    if (heirarchy != 0 && rootnode < heirarchy->mNumNodes) {
        this->mHeirarchyIndex = rootnode;
        this->mHeirarchy = heirarchy;
        this->mChildVisibility = 0xFFFFFF;
    } else {
        this->mChildVisibility = 0;
        this->mHeirarchyIndex = 0;
        this->mHeirarchy = 0;
    }

    this->mInvisibleInside = false;
    this->mEnabled = true;
    this->mRenderInSplitScreen = true;
    this->mCastsShadow = 1;
    this->pSpaceNode = spacenode;

    if (spacenode != 0) {
        spacenode->Lock();
    }

    if (matrix != 0) {
        this->SetMatrix(matrix);
    }

    WorldModelList.AddTail(this);
    this->mAddLighting = add_lighting;
}

WorldModel::~WorldModel() {
    if (this->pModel != 0) {
        delete this->pModel;
    }

    if (this->pReflectionModel != 0) {
        delete this->pReflectionModel;
    }

    if (this->pSpaceNode != 0) {
        this->pSpaceNode->Unlock();
    }

    this->Remove();
}

eModel *WorldModel::GetModel() {
    if (this->pModel != 0) {
        return this->pModel;
    }

    if (this->mHeirarchy != 0) {
        return this->mHeirarchy->GetNodes()[this->mHeirarchyIndex].mModel;
    }

    return 0;
}

void WorldModel::AttachReplacementTextureTable(eReplacementTextureTable *replacement_texture_table, int num_textures) {
    if (this->pModel != 0) {
        this->pModel->AttachReplacementTextureTable(replacement_texture_table, num_textures, 0);
    }

    if (this->pReflectionModel != 0) {
        this->pReflectionModel->AttachReplacementTextureTable(replacement_texture_table, num_textures, 0);
    }
}

void WorldModel::GetLocalBoundingBox(bVector3 *min_ext, bVector3 *max_ext) {
    eModel *model = this->GetModel();

    if (model != 0) {
        model->GetBoundingBox(min_ext, max_ext);
    } else {
        bVector3 zero;
        bFill(&zero, 0.0f, 0.0f, 0.0f);
        bCopy(min_ext, &zero);
        bCopy(max_ext, &zero);
    }
}

void InitWorldModels() {
    WorldModelSlotPool = bNewSlotPool(0x88, 0x80, "WorldModelSlotPool", 0);
}

void CloseWorldModels() {
    WorldModel *end = WorldModelList.EndOfList();

    if (WorldModelList.GetHead() != end) {
        for (;;) {
            WorldModel *world_model = WorldModelList.GetHead();
            if (world_model == end) {
                break;
            }
            if (world_model != 0) {
                delete world_model;
            }
        }
    }

    bDeleteSlotPool(WorldModelSlotPool);
    WorldModelSlotPool = 0;
}

void WorldModel::RenderNode(const ModelHeirarchy *heirarchy, unsigned int nodeIndex, eView *view, int exc_flag, bMatrix4 *blended_matrices,
                            const bMatrix4 *matrix) {
    const ModelHeirarchy::Node *node = &heirarchy->GetNodes()[nodeIndex];

    if (node->mModel != 0 && node->mModel->GetSolid() != 0) {
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
    unsigned int flags = static_cast<unsigned int>(exc_flag);
    AABBAdjustor adjustor(render_model, blended_matrices);
    bMatrix4 *frame_matrix = static_cast<bMatrix4 *>(eFrameMalloc(sizeof(bMatrix4)));

    if (frame_matrix != 0) {
        eDynamicLightContext *light_context = 0;

        *frame_matrix = *matrix;
        if ((flags & 0x800) != 0) {
            frame_matrix->v2.z = -frame_matrix->v2.z;
        }

        if (this->mAddLighting) {
            light_context = static_cast<eDynamicLightContext *>(eFrameMalloc(0x130));
            if (light_context == 0) {
                return;
            }

            Camera *camera = view->GetCamera();
            bVector3 *eye = camera->GetPosition();
            bMatrix4 *world_view = camera->GetCameraMatrix();
            bVector4 camera_world_position;

            camera_world_position.x = eye->x;
            camera_world_position.y = eye->y;
            camera_world_position.z = eye->z;
            camera_world_position.w = 1.0f;
            if (blended_matrices != 0) {
                bMatrix4 *actual_frame_matrix;
                bMatrix4 moved_frame_matrix;
                bVector4 pelvis_pos = blended_matrices[1].v3;

                eMulVector(&pelvis_pos, frame_matrix, &pelvis_pos);
                moved_frame_matrix = *frame_matrix;
                camera_world_position = pelvis_pos;
                actual_frame_matrix = &moved_frame_matrix;
                AdjustQuickDynamicLight(&ShaperLightsCharacters, reinterpret_cast<bVector3 *>(&camera_world_position));
                elSetupLightContext(light_context, &ShaperLightsCharacters, actual_frame_matrix, world_view, &camera_world_position, view);
                ShaperLightsCharacters.NumOverideSlots = 0;
            } else {
                elSetupLightContext(light_context, &ShaperLightsCarsInGame, frame_matrix, world_view, &camera_world_position, view);
            }
        }

        if (this->mLightMaterial != 0) {
            render_model->ReplaceLightMaterial(this->mLightMaterialSkinHash, this->mLightMaterial);
        }

        ::Render(view, render_model, frame_matrix, light_context, 0, flags);
    }
}

void WorldModel::Render(eView *view, int exc_flag) {
    if (this->mLastRenderFrame != eFrameCounter) {
        this->mLastRenderFrame = eFrameCounter;
        this->mDistanceToGameView = lbl_8040CD80;
    }

    CameraMover *camera_mover = 0;
    if (!view->CameraMoverList.IsEmpty()) {
        camera_mover = view->CameraMoverList.GetHead();
    }

    if (camera_mover != 0 && (view->GetID() - 1U) < 3U) {
        const bMatrix4 *world_matrix = &this->mMatrix;

        if (this->pSpaceNode != 0) {
            world_matrix =
                reinterpret_cast<const bMatrix4 *>(reinterpret_cast<const unsigned char *>(this->pSpaceNode) + 0x50);
        }

        {
            bVector3 *camera_position = camera_mover->GetPosition();
            bVector3 delta;
            float distance_sq;
            float distance_scale = lbl_8040CD90;

            delta.y = camera_position->y - world_matrix->v3.y;
            delta.x = camera_position->x - world_matrix->v3.x;
            delta.z = camera_position->z - world_matrix->v3.z;
            distance_sq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
            if (lbl_8040CD84 < distance_sq) {
                distance_scale = bSqrt(distance_sq);
            }

            if (this->mDistanceToGameView < distance_scale) {
                distance_scale = this->mDistanceToGameView;
            }
            this->mDistanceToGameView = distance_scale;
        }
    }

    eModel *render_model = this->GetModel();
    if ((static_cast<unsigned int>(exc_flag) & 0x800) != 0 && (render_model = this->pReflectionModel) == 0) {
        return;
    }
    if (render_model == 0) {
        return;
    }

    eSolid *solid = render_model->Solid;
    if (solid == 0) {
        return;
    }

    if ((static_cast<unsigned int>(exc_flag) & 0x200000) != 0) {
        if (!this->mCastsShadow) {
            return;
        }
        if ((static_cast<unsigned int>(exc_flag) & 0x2000) != 0) {
            if (*reinterpret_cast<char *>(reinterpret_cast<unsigned char *>(solid) + 0x18) != '\0') {
                return;
            }
        } else if (*reinterpret_cast<char *>(reinterpret_cast<unsigned char *>(solid) + 0x18) == '\0') {
            return;
        }
    }

    bMatrix4 world_matrix;
    bMatrix4 *blended_matrices = 0;
    const bMatrix4 *render_matrix;
    if (this->pSpaceNode != 0) {
        SpaceNode *space_node = this->pSpaceNode;

        if (*reinterpret_cast<unsigned int *>(reinterpret_cast<unsigned char *>(space_node) + 0xE4) != 0) {
            space_node->Update();
        }
        render_matrix = reinterpret_cast<const bMatrix4 *>(reinterpret_cast<const unsigned char *>(space_node) + 0x10);
        blended_matrices = *reinterpret_cast<bMatrix4 **>(reinterpret_cast<unsigned char *>(this->pSpaceNode) + 0xE8);
        if (blended_matrices != 0) {
            bMulMatrix(&world_matrix, render_matrix, &blended_matrices[1]);
            goto have_world_matrix;
        }
    } else {
        render_matrix = &this->mMatrix;
    }
 
    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(render_matrix), *reinterpret_cast<Mtx44 *>(&world_matrix));

    have_world_matrix:
    if (view->GetPixelSize(reinterpret_cast<const bVector3 *>(&world_matrix.v3), lbl_8040CD94) < view->PixelMinSize) {
        return;
    }

    {
        int visible = view->GetVisibleState(render_model, &world_matrix) != 0;

        if (visible == 0) {
            return;
        }
    }

    if (this->mHeirarchy != 0) {
        this->RenderNode(this->mHeirarchy, this->mHeirarchyIndex, view, exc_flag, blended_matrices, render_matrix);
    } else {
        this->RenderModel(render_model, view, exc_flag, blended_matrices, render_matrix);
    }

    this->mLastVisibleFrame = eFrameCounter;
}

void RenderWorldModels(eView *view, int exc_flag) {
    int view_mode = eGetCurrentViewMode();

    for (WorldModel *world_model = WorldModelList.GetHead(); world_model != WorldModelList.EndOfList(); world_model = world_model->GetNext()) {
        unsigned char *world_model_bytes = reinterpret_cast<unsigned char *>(world_model);
        unsigned int *world_model_words = reinterpret_cast<unsigned int *>(world_model);

        if (world_model_words[10] != 0 && (view_mode != 3 || world_model_words[11] != 0)) {
            if (world_model_words[12] != 0) {
                const bMatrix4 *matrix = reinterpret_cast<const bMatrix4 *>(world_model_bytes + 0x40);
                const bVector3 *position = reinterpret_cast<const bVector3 *>(world_model_bytes + 0x70);
                SpaceNode *space_node = *reinterpret_cast<SpaceNode **>(world_model_bytes + 0x3C);

                if (space_node != 0) {
                    matrix = reinterpret_cast<const bMatrix4 *>(reinterpret_cast<const unsigned char *>(space_node) + 0x50);
                    position = reinterpret_cast<const bVector3 *>(reinterpret_cast<const unsigned char *>(space_node) + 0x80);
                }

                bMatrix4 local_matrix;
                bVector3 bbox_min;
                bVector3 bbox_max;
                bVector3 local_camera_position;

                local_matrix.v0 = matrix->v0;
                local_matrix.v1 = matrix->v1;
                local_matrix.v2 = matrix->v2;
                local_matrix.v3.x = position->x;
                local_matrix.v3.y = position->y;
                local_matrix.v3.z = position->z;
                local_matrix.v3.w = 1.0f;

                eInvertTransformationMatrix(&local_matrix, &local_matrix);
                world_model->GetLocalBoundingBox(&bbox_min, &bbox_max);
                eMulVector(&local_camera_position, &local_matrix, reinterpret_cast<const bVector3 *>(reinterpret_cast<const unsigned char *>(view->pCamera) + 0x40));

                if (bBoundingBoxIsInside(&bbox_min, &bbox_max, &local_camera_position, 0.0f) != 0) {
                    continue;
                }
            }

            world_model->Render(view, exc_flag);
        }
    }

    RenderAnimSceneEffects(view, exc_flag);
}
