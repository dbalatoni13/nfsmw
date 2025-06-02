#include "./Ecstasy.hpp"
#include "EcstasyData.hpp"
#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

eView eViews[16];

eView::eView() {
  bMemSet(this, 0, sizeof(*this));
  this->CameraMoverList.InitList();
  this->FovBias = 1.0f;
  this->SetID(this - eViews);
  this->SetPlatInfo(eViewPlatInterface::GimmeMyViewPlatInfo(this->ID));
}

eView::~eView() {}

eVisibleState eView::GetVisibleState(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world) {
  eVisibleState vis_state = this->GetVisibleStateSB(aabb_min, aabb_max, local_world);
  if (vis_state == EVISIBLESTATE_PARTIAL) {
    this->GetVisibleStateGB(aabb_min, aabb_max, local_world);
  }
}

eVisibleState eView::GetVisibleState(eModel *model, bMatrix4 *local_world) {
  bVector3 min;
  bVector3 max;
  model->GetBoundingBox(&min, &max);
  return this->GetVisibleState(&min, &max, local_world);
}

int eView::GetPixelSize(float radius, float distance) {
  float pixel_size;

  if (distance > radius / 2) {
    pixel_size = (radius * this->H) / distance;
  } else {
    pixel_size = 2 * this->H;
  }
  return pixel_size;
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
    float distance_ahead = sqrtf(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
    float distance_away = distance_ahead - radius;
    if (distance_away > radius / 2) {
      pixel_size = (radius * this->H) / distance_away;
    } else {
      pixel_size = 2 * this->H;
    }
  }
  return pixel_size;
}

int eView::GetPixelSize(const bVector3 *bbox_min, const bVector3 *bbox_max) {
  Camera *camera = this->GetCamera();
  bVector3 *cam_position = camera->GetPosition();
  bVector3 *cam_direction = camera->GetDirection();
  float pos_x = bbox_min->x;
  float pos_y = bbox_min->y;
  float pos_z = bbox_min->z;
  float rad_x = bbox_max->x;
  float rad_y = bbox_max->y;
  float rad_z = bbox_max->z;

  float radius = sqrtf(rad_x * rad_x + rad_y * rad_y + rad_z * rad_z);

  float dir_x = pos_x - cam_position->x;
  float dir_y = pos_y - cam_position->y;
  float dir_z = pos_z - cam_position->z;
  float pixel_size;

  if (dir_x * cam_direction->x + dir_y * cam_direction->y + dir_z * cam_direction->z < -radius) {
    return 0;
  } else {
    float distance_ahead = sqrtf(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
    float distance_away = distance_ahead - radius;
    rad_y = this->H;
    if (distance_away > radius) {
      rad_y = (radius * this->H) / distance_away;
    }
    pixel_size = rad_y;
  }
  return pixel_size;
}

eView *eGetView(int view_id) { return &eViews[view_id]; }
