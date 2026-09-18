#include "Speed/Indep/Src/Camera/Movers/Rearview.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Rumble.hpp"

float RVMnearz = 0.75f;
float RVMfarz = 192.0f;

static const int RVMRotX = 0x4000;
static const int RVMRotY = 0x4000;
static const int RVMRotZ = 0;
static const int RVMfov = 0x4e20;

static const bVector3 RVMOffsetInCar(0.0f, 0.5f, 0.0f); // size: 0x10, address: 0x8045AF58

RearViewMirrorCameraMover::RearViewMirrorCameraMover(int view_id, CameraAnchor *car)
    : CameraMover(view_id, CM_REAR_VIEW_MIRROR) {
    pCar = car;

    pCamera->SetRenderDash(0);
}

RearViewMirrorCameraMover::~RearViewMirrorCameraMover() {}

void RearViewMirrorCameraMover::Update(float dT) {
    if (FEDatabase->GetGameplaySettings()->RearviewOn) {
        bMatrix4 m;
        bMatrix4 tbod;
        bMatrix4 CarRotMat;
        bMatrix4 rvm_matrix;

        eIdentity(&m);

        m.v3.x = -pCar->GetGeometryPosition()->x;
        m.v3.y = -pCar->GetGeometryPosition()->y;
        m.v3.z = -pCar->GetGeometryPosition()->z;

        CarRotMat = *pCar->GetGeometryOrientation();

        eTransposeMatrix(&tbod, &CarRotMat);

        eRotateX(&tbod, &tbod, RVMRotX);
        eRotateY(&tbod, &tbod, RVMRotY);
        eRotateZ(&tbod, &tbod, RVMRotZ);

        eMulMatrix(&rvm_matrix, &m, &tbod);

        rvm_matrix.v3.x += RVMOffsetInCar.x;
        rvm_matrix.v3.y += RVMOffsetInCar.y;
        rvm_matrix.v3.z += RVMOffsetInCar.z;

        pCamera->SetFieldOfView(RVMfov);
        pCamera->SetNearZ(RVMnearz);
        pCamera->SetFarZ(RVMfarz);

        ApplyCameraShake(ViewID, &rvm_matrix);

        pCamera->SetCameraMatrix(rvm_matrix, dT);
    }
}
