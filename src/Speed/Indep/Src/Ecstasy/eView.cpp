#include "./Ecstasy.hpp"
#include "EcstasyData.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

eView eViews[22];

eView::eView() {
    bMemSet(this, 0, sizeof(*this));
    this->CameraMoverList.InitList();
    this->FovBias = 1.0f;
    this->SetID(this - eViews);
    this->SetPlatInfo(eViewPlatInterface::GimmeMyViewPlatInfo(this->ID));
    this->PixelMinSize = 4;
    this->ScreenEffects = nullptr;
    this->Precipitation = nullptr;
    this->facePixelation = nullptr;
}

eVisibleState eView::GetVisibleState(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world) {
    eVisibleState vis_state = this->GetVisibleStateSB(aabb_min, aabb_max, local_world);
}

eVisibleState eView::GetVisibleState(eModel *model, bMatrix4 *local_world) {
    bVector3 min;
    bVector3 max;
    model->GetBoundingBox(&min, &max);
    return this->GetVisibleState(&min, &max, local_world);
}

int eView::GetPixelSize(float radius, float distance) {
    float pixel_size;

    if (distance > radius / 2.0f) {
        pixel_size = (radius * this->H) / distance;
    } else {
        pixel_size = 2.0f * this->H;
    }
    return static_cast<int>(pixel_size);
}

int eView::GetPixelSize(const bVector3 *position, float radius) {
    Camera *camera = this->GetCamera();
    bVector3 *cam_position = camera->GetPosition();
    bVector3 *cam_direction = camera->GetDirection();
    float dir_x = position->x - cam_position->x;
    float dir_y = position->y - cam_position->y;
    float dir_z = position->z - cam_position->z;
    float pixel_size;

    if (dir_x * cam_direction->x + dir_y * cam_direction->y + dir_z * cam_direction->z < -radius) {
        return 0;
    } else {
        float distance_ahead = bSqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
        float distance_away = distance_ahead - radius;
        if (distance_away > radius / 2) {
            pixel_size = (radius * this->H) / distance_away;
        } else {
            pixel_size = 2 * this->H;
        }
    }
    return static_cast<int>(pixel_size);
}

// UNSOLVED
int eView::GetPixelSize(const bVector3 *bbox_min, const bVector3 *bbox_max) {
    Camera *camera = this->GetCamera();
    bVector3 *cam_position = camera->GetPosition();
    bVector3 *cam_direction = camera->GetDirection();

    float pos_x = bbox_min->x - bbox_max->x;
    float pos_y = bbox_min->y - bbox_max->y;
    float pos_z = bbox_min->z - bbox_max->z;

    float radius = bSqrt(pos_x * pos_x + pos_y * pos_y + pos_z * pos_z) * 0.5f;
    float rad_x = (bbox_min->x + bbox_max->x) * 0.5f;
    float rad_y = (bbox_min->y + bbox_max->y) * 0.5f;
    float rad_z = (bbox_min->z + bbox_max->z) * 0.5f;

    float dir_x = rad_x - cam_position->x;
    float dir_y = rad_y - cam_position->y;
    float dir_z = rad_z - cam_position->z;
    float pixel_size;

    if (dir_x * cam_direction->x + dir_y * cam_direction->y + dir_z * cam_direction->z < -radius) {
        return 0;
    } else {
        float distance_ahead = bSqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
        float distance_away = distance_ahead - radius;
        rad_y = this->H;
        if (distance_away > radius) {
            rad_y = (radius * this->H) / distance_away;
        }
        pixel_size = rad_y;
    }
    return static_cast<int>(pixel_size);
}

void eView::BiasMatrixForZSorting(bMatrix4 *pL2W, float zBias) {
    if (this->pCamera) {
        bMatrix4 m;
        bIdentity(&m);

        bVector3 *pObjPosition = reinterpret_cast<bVector3 *>(&pL2W->v3);
        bVector3 *pCamPosition = this->pCamera->GetPosition();
        m.v0.x = zBias;
        m.v1.y = zBias;
        m.v2.z = zBias;

        bVector3 v = *pObjPosition - *pCamPosition;
        v *= zBias;
        v += *pCamPosition;
        *reinterpret_cast<bVector3 *>(&m.v3) = v;
        m.v3.w = 1.0f;
        bFill(&pL2W->v3, 0.0f, 0.0f, 0.0f, 1.0f);
        eMulMatrix(pL2W, pL2W, &m);
    }
}

void eView::AttachCameraMover(CameraMover *camera_mover) {
    this->CameraMoverList.AddHead(camera_mover);
    eUpdateViewMode();
}

void eView::UnattachCameraMover(CameraMover *camera_mover) {
    this->CameraMoverList.Remove(camera_mover);
    eUpdateViewMode();
}
