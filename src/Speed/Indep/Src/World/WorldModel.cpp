#include "./WorldModel.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Ecstasy/eSolid.hpp"

extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;
extern eShaperLightRig ShaperLightsCarsInGame;
extern eShaperLightRig ShaperLightsCharacters;

int elSetupLightContext(eDynamicLightContext *light_context, eShaperLightRig *shaper_lights, bMatrix4 *local_world, bMatrix4 *world_view,
                        bVector4 *camera_world_position, eView *view);
void AdjustQuickDynamicLight(eShaperLightRig *ShaperRigP, bVector3 *MyPosition);

namespace {

void Render(eViewPlatInterface *view, eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, unsigned int flags,
            unsigned int exc_flag) asm("Render__18eViewPlatInterfaceP6eModelP8bMatrix4P13eLightContextUiT2");

void *eFrameMalloc(unsigned int size) {
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
            this->mMin->y += this->mAdjustment->y * scaler;
            this->mMin->z += this->mAdjustment->z * scaler;
            this->mMax->x += this->mAdjustment->x * scaler;
            this->mMax->y += this->mAdjustment->y * scaler;
            this->mMax->z += this->mAdjustment->z * scaler;
        }
    }

    AABBAdjustor(eModel *m, bMatrix4 *adjustment)
        : mMin(0), //
          mMax(0), //
          mAdjustment(0) {
        eSolid *solid = m->GetSolid();

        if (solid != 0 && adjustment != 0) {
            this->mMin = reinterpret_cast<bVector4 *>(&solid->AABBMinX);
            this->mMax = reinterpret_cast<bVector4 *>(&solid->AABBMaxX);
            this->mAdjustment = &adjustment[1].v3;
            this->Adjust(1.0f);
        }
    }

    ~AABBAdjustor() {
        this->Adjust(-1.0f);
    }
};

} // namespace

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
            if (blended_matrices == 0) {
                elSetupLightContext(light_context, &ShaperLightsCarsInGame, frame_matrix, world_view, &camera_world_position, view);
            } else {
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
            }
        }

        if (this->mLightMaterial != 0) {
            render_model->ReplaceLightMaterial(this->mLightMaterialSkinHash, this->mLightMaterial);
        }

        ::Render(view, render_model, frame_matrix, light_context, 0, flags);
    }
}
