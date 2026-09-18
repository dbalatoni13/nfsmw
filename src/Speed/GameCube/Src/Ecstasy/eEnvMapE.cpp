#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eEnvMap.hpp"
#include "Speed/GameCube/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/GameCube/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <dolphin.h>
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/GameCube/Src/Ecstasy/EcstasyEx.hpp"

extern cSphereMap SphereMap;

// El juego solo tiene un env map: vive aqui como objeto global y se reparte
// por puntero.

bMatrix4 hack_man_matrix;
bVector3 envmap_fakeup(0.0f, 0.0f, 1.0f);
eEnvMap TheOnlyEnvMap;
cSphereMap SphereMap;

float EnvMapScreenZ = 64.0f;

float HackDirArray[2][6][4] = {
    {
        {-1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
    },
    {
        {-1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
    },
};

float HackDirArrayUp[2][6][4] = {
    {
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f, 0.0f},
    },
    {
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f, 0.0f},
    },
};

int EnableEnvMapFacesNum[2] = {4, 6};

int EnableEnvMapFaces[2][6][6] = {
    {
        {1, 1, 0, 0, 1, 0},
        {1, 0, 1, 0, 1, 0},
        {1, 0, 0, 1, 1, 0},
        {1, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
    },
    {
        {1, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 1},
    },
};

int eInitEnvMap() {
    extern int _GxInitialized;

    if (!_GxInitialized) {
        return 0;
    }

    SphereMap.Init(SphereMap.CUBEFACE_SIZE, SphereMap.CUBEFACE_SIZE, SphereMap.CUBEFACE_SIZE, SphereMap.CUBEFACE_SIZE,
                   SphereMap.CUBEFACE_SIZE, SphereMap.CUBEFACE_SIZE);

    eEnvMap *envmap = eGetEnvMap();

    for (int face = 0; face < 6; face++) {
        int view_id = EVIEW_ENVMAP0F + face;
        int target_id = TARGET_ENVMAP0F + face;
        eView *view = eGetView(view_id, true);
        eRenderTarget *target = eGetRenderTarget(target_id);

        view->SetCamera(reinterpret_cast<Camera *>(&envmap->Cameras[face]));
        view->SetRenderTarget(target, 0);
        view->SetActive(1);

        target->SetActive(1);

        envmap->Views[face] = view;
    }

    return 1;
}

void eEnvMap::UpdateCameras(bVector3 *viewer_world_position, bVector3 *envmap_world_position) {
    bMatrix4 camera_basis;
    bVector3 a(0.0f, 0.0f, 0.0f);
    bVector3 b(1.0f, 0.0f, 0.0f);

    eCreateLookAtMatrix(&camera_basis, a, b, envmap_fakeup);

    camera_basis.v0.w = 0.0f;
    camera_basis.v1.w = 0.0f;
    camera_basis.v2.w = 0.0f;
    camera_basis.v3.x = 0.0f;
    camera_basis.v3.y = 0.0f;
    camera_basis.v3.z = 0.0f;
    camera_basis.v3.w = 1.0f;

    bMatrix4 camera_lookat_car;

    eCreateLookAtMatrix(&camera_lookat_car, *viewer_world_position, *envmap_world_position, envmap_fakeup);

    camera_lookat_car.v0.w = 0.0f;
    camera_lookat_car.v1.w = 0.0f;
    camera_lookat_car.v2.w = 0.0f;
    camera_lookat_car.v3.x = 0.0f;
    camera_lookat_car.v3.y = 0.0f;
    camera_lookat_car.v3.z = 0.0f;
    camera_lookat_car.v3.w = 1.0f;

    hack_man_matrix = camera_lookat_car;

    bMatrix4 orientation;

    eTransposeMatrix(&orientation, &camera_basis);

    eMulMatrix(&orientation, &camera_lookat_car, &orientation);

    orientation.v0.w = 0.0f;
    orientation.v1.w = 0.0f;
    orientation.v2.w = 0.0f;
    orientation.v3.x = 0.0f;
    orientation.v3.y = 0.0f;
    orientation.v3.z = 0.0f;
    orientation.v3.w = 1.0f;

    eTransposeMatrix(&orientation, &orientation);

    int direction_array_index;
    int direction_matrix_index;

    if (IsGameFlowInFrontEnd()) {
        direction_array_index = 0;
        direction_matrix_index = 2;
    } else {
        direction_array_index = 1;
        direction_matrix_index = 2;
    }

    EVIEWMODE view_mode = eGetCurrentViewMode();
    int single_player = view_mode != EVIEWMODE_ONE && view_mode != EVIEWMODE_ONE_RVM;
    int num_enable_slots = EnableEnvMapFacesNum[single_player];
    int *face_enable_state = EnableEnvMapFaces[single_player][eFrameCounter % num_enable_slots];

    float near_z = 0.5f;
    float far_z = 10000.0f;

    for (int face = 0; face < 6; face++) {
        eView *view = this->Views[face];

        if (view) {
            int view_active = face_enable_state[face];

            view->SetActive(view_active != 0);

            Camera *camera = view->GetCamera();

            if (camera) {
                bVector3 look;
                bVector3 up;
                bMatrix4 matrix;

                eMulVector(&up, &orientation, reinterpret_cast<const bVector3 *>(HackDirArrayUp[direction_array_index][face]));
                eMulVector(&look, &orientation, reinterpret_cast<const bVector3 *>(HackDirArray[direction_array_index][face]));

                bAdd(&look, &look, envmap_world_position);

                eCreateLookAtMatrix(&matrix, *envmap_world_position, look, up);

                if (direction_matrix_index == face) {
                    hack_man_matrix = matrix;
                }

                camera->SetCameraMatrix(matrix, RealTimeElapsed);
                camera->SetRenderDash(0);
                camera->SetFocalDistance(0.0f);
                camera->SetNearZ(near_z);
                camera->SetFarZ(far_z);
                camera->SetFieldOfView(0x4000);
            }
        }
    }
}

eEnvMap *eGetEnvMap() {
    return &TheOnlyEnvMap;
}

void eDisplayEnvRenderTargets(eView *view) {

    ePoly poly;

    // El volcado de los seis render targets se quedo fuera de la version final.

}
