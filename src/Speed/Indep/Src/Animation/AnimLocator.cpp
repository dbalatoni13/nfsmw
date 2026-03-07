#include "AnimLocator.hpp"

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
