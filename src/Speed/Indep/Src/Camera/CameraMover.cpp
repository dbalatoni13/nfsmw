#include "Speed\Indep\Src\Camera\CameraMover.hpp"

bool DoesCameraTypeDisablePreculler(CameraMoverTypes type) {
    if (type == CM_DEBUG_WORLD) {
        return true;
    }
    return type == CM_TRACK_CAR;
}

CameraMover::CameraMover(int view_id, CameraMoverTypes type) {

    WCollider *pWVar1;
    bool doesCameraTypeDisablePreculler = false;
    Camera *m_eViews;

    pWVar1 = WCollider::Create(0, WCollider::kColliderShape_Sphere, 0x1c, 0);
    this->mWPos.fUsageCount = this->mWPos.fUsageCount;
    this->mWPos.fYOffset = 0.025;
    this->mCollider = pWVar1;

    (this->mWPos).fFace.fSurface.fFlags = '\0';
    (this->mWPos).fFace.fSurface.fSurface = '\0';
    this->mWPos.fUsageCount = 0;

    (this->mWPos).fFace.fPt0.x = 0;
    (this->mWPos).fFace.fPt0.z = 0;
    (this->mWPos).fFace.fPt0.y = 0;
    (this->mWPos).fFace.fPt1.x = 0;
    (this->mWPos).fFace.fPt1.z = 0;
    (this->mWPos).fFace.fPt1.y = 0;
    (this->mWPos).fFace.fPt2.x = 0;
    (this->mWPos).fFace.fPt2.z = 0;
    (this->mWPos).fFace.fPt2.y = 0;

    this->Type = type;
    (this->mWPos).fSurface = 0x0;
    this->ViewID = view_id;
    this->Enabled = 0;
    this->fAccumulatedClearance = 0;
    this->fAccumulatedAdjust = 0;
    this->fSavedAdjust = 0;
    (this->vSavedForward).x = 0;
    (this->vSavedForward).z = 0;
    (this->vSavedForward).y = 0;
    if (view_id == -1) {
        this->RenderDash = 0;
        this->pView = (eView *)0x0;
        this->pCamera = (Camera *)0x0;
    } else {
        this->pView = eViews + view_id;
        m_eViews = eViews[view_id].pCamera;
        this->pCamera = m_eViews;
        (m_eViews->CurrentKey).FarZ = 12000.0;
        this->RenderDash = this->pCamera->RenderDash;
        this->Enable();
    }
    doesCameraTypeDisablePreculler = DoesCameraTypeDisablePreculler(this->Type);
    if (doesCameraTypeDisablePreculler) {
        DisablePrecullerCounter++;
    }
}

void CameraMover::Enable() {
    if (this->Enabled == 0) {
        this->Enabled = 1;
        if (Camera::StopUpdating == 0) {
            this->pCamera->RenderDash = this->RenderDash;
        }
        pView->AttachCameraMover(this);
        (this->pCamera->CurrentKey).NearZ = 0.5;
    }
}

void CameraMover::ComputeBankedUpVector(bVector3 *up, bVector3 *eye, bVector3 *look, bAngle bank) {

    bVector3 diff;

    diff.x = look->x - eye->x;
    diff.z = look->z - eye->z;
    diff.y = look->y - eye->y;

    bVector3 axis;
    bNormalize(&axis, &diff);

    bMatrix4 rotationMatrix;
    eCreateAxisRotationMatrix(&rotationMatrix, *&axis, bank);
    // defaultVec.y = 0.0;
    // defaultVec.z = 1.0;
    // defaultVec.x = 0.0;
    bVector3 defaultVec = bVector3(0.0, 0.0, 1.0);
    eMulVector(up, &rotationMatrix, &defaultVec);
    return;
}

void CameraMover::Disable()

{
    if (this->Enabled != 0) {
        this->Enabled = 0;
        this->RenderDash = this->pCamera->RenderDash;
        this->pView->UnattachCameraMover(this);
    }
    return;
}

CameraMover::~CameraMover() {

    WCollider::Destroy(mCollider);

    if (DoesCameraTypeDisablePreculler(Type)) {
        DisablePrecullerCounter--;
    }
    Disable();
}


void CameraMover::Update(float dT) {
    return;
}

void CameraMover::Render(eView *view) {
    return;
}