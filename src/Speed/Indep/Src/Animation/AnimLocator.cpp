#include "AnimLocator.hpp"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WWorldMath.h"
#include "Speed/Indep/Src/World/WWorldPos.h"

bVector3 gNISSceneOrigin;
unsigned short gNISSceneAngle;

void CAnimLocator::SetAnimOriginPosition(const bVector3 &nis_scene_origin, unsigned short pnis_scene_angle) {
    gNISSceneOrigin.x = nis_scene_origin.x;
    gNISSceneOrigin.y = nis_scene_origin.y;
    gNISSceneOrigin.z = nis_scene_origin.z;
    gNISSceneAngle = pnis_scene_angle;
}

bool CAnimLocator::GetAnimOriginPosition(bVector3 *nis_scene_origin, unsigned short *pnis_scene_angle, bool at_start_line) {
    if (at_start_line) {
        nis_scene_origin->x = gNISSceneOrigin.x;
        nis_scene_origin->y = gNISSceneOrigin.y;
        nis_scene_origin->z = gNISSceneOrigin.z;
        *pnis_scene_angle = gNISSceneAngle;
    } else {
        nis_scene_origin->x = 0.0f;
        nis_scene_origin->y = 0.0f;
        nis_scene_origin->z = 0.0f;
        *pnis_scene_angle = static_cast<unsigned short>(at_start_line);
    }
    return true;
}

bool CAnimLocator::GetInitialAnimMatricies(bMatrix4 *scene_rotation_matrix, bMatrix4 *scene_translation_matrix, bool at_start_line) {}

bool ANIM_GetWorldHeight(const UMath::Vector3 &pt, float &height, UMath::Vector3 &norm) {
    WWorldPos temp(0.025f);
    temp.FindClosestFace(pt, true);

    if (temp.OnValidFace()) {
        temp.UNormal(&norm);
        height = WWorldMath::GetPlaneY(norm, UMath::Vector4To3(temp.FacePoint(0)), pt);
        return true;
    } else {
        UMath::Vector4 seg[2];
        seg[1] = UMath::Vector4Make(pt, 1.0f);
        seg[0] = seg[1];
        seg[1].y += 4000.0f;

        WCollisionMgr::WorldCollisionInfo cInfo;
        WCollisionMgr(0, 3).CheckHitWorld(seg, cInfo, 1);

        if (!cInfo.HitSomething()) {
            return false;
        }
        if (cInfo.fType != 1) {
            return false;
        }

        height = cInfo.fCollidePt.y;
        norm.x = -cInfo.fNormal.x;
        norm.y = -cInfo.fNormal.y;
        norm.z = -cInfo.fNormal.z;
        return true;
    }
}
