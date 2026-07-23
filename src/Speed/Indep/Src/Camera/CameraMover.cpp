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
    mWPos.fUsageCount = mWPos.fUsageCount;
    mWPos.fYOffset = 0.025;
    mCollider = pWVar1;

    mWPos.fFace.fSurface.fFlags = '\0';
    mWPos.fFace.fSurface.fSurface = '\0';
    mWPos.fUsageCount = 0;

    mWPos.fFace.fPt0.x = 0;
    mWPos.fFace.fPt0.z = 0;
    mWPos.fFace.fPt0.y = 0;
    mWPos.fFace.fPt1.x = 0;
    mWPos.fFace.fPt1.z = 0;
    mWPos.fFace.fPt1.y = 0;
    mWPos.fFace.fPt2.x = 0;
    mWPos.fFace.fPt2.z = 0;
    mWPos.fFace.fPt2.y = 0;

    Type = type;
    mWPos.fSurface = 0x0;
    ViewID = view_id;
    Enabled = 0;
    fAccumulatedClearance = 0;
    fAccumulatedAdjust = 0;
    fSavedAdjust = 0;
    vSavedForward.x = 0;
    vSavedForward.z = 0;
    vSavedForward.y = 0;
    if (view_id == -1) {
        RenderDash = 0;
        pView = (eView *)0x0;
        pCamera = (Camera *)0x0;
    } else {
        pView = eViews + view_id;
        m_eViews = eViews[view_id].pCamera;
        pCamera = m_eViews;
        m_eViews->CurrentKey.FarZ = 12000.0;
        RenderDash = pCamera->RenderDash;
        Enable();
    }
    doesCameraTypeDisablePreculler = DoesCameraTypeDisablePreculler(Type);
    if (doesCameraTypeDisablePreculler) {
        DisablePrecullerCounter++;
    }
}

void CameraMover::Enable() {
    if (Enabled == 0) {
        Enabled = 1;
        if (Camera::StopUpdating == 0) {
            pCamera->RenderDash = RenderDash;
        }
        pView->AttachCameraMover(this);
        (pCamera->CurrentKey).NearZ = 0.5;
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
    if (Enabled != 0) {
        Enabled = 0;
        RenderDash = pCamera->RenderDash;
        pView->UnattachCameraMover(this);
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


void CameraMover::HandheldNoise(
    bMatrix4* world_to_camera,
    float f_scale,
    bool useWorldTimer)
{
    bVector4 v_frequency;
    bVector4 v_magnitude;

    if (f_scale <= 0.0f)
        return;

    bVector4* pfreq = &v_frequency;

    pfreq->x = CameraNoiseHandheldFrequency.x;
    pfreq->y = CameraNoiseHandheldFrequency.y;

    float ampW = CameraNoiseHandheldAmplitude.w;
    float ampY = CameraNoiseHandheldAmplitude.y;

    if (useWorldTimer) {
    }

    float ampZ = CameraNoiseHandheldAmplitude.z;

    ampW *= f_scale;

    pfreq->z = CameraNoiseHandheldFrequency.z;

    ampY *= f_scale;

    float ampX = CameraNoiseHandheldAmplitude.x;
    float freqW = CameraNoiseHandheldFrequency.w;

    ampX *= f_scale;

    v_magnitude.x = ampX;
    v_magnitude.y = ampY;
    v_magnitude.z = ampZ * f_scale;
    v_magnitude.w = ampW;

    float fx = pfreq->x;
    float fy = pfreq->y;
    float fz = pfreq->z;

    float ax = v_magnitude.x;
    float ay = v_magnitude.y;
    float az = v_magnitude.z;
    float aw = v_magnitude.w;

    pCamera->CurrentKey.NoiseFrequency1.w = freqW;
    pCamera->CurrentKey.NoiseFrequency1.x = fx;
    pCamera->CurrentKey.NoiseFrequency1.y = fy;
    pCamera->CurrentKey.NoiseFrequency1.z = fz;

    pCamera->CurrentKey.NoiseAmplitude1.x = ax;
    pCamera->CurrentKey.NoiseAmplitude1.w = aw;
    pCamera->CurrentKey.NoiseAmplitude1.y = ay;
    pCamera->CurrentKey.NoiseAmplitude1.z = az;

    int packedTime =
        useWorldTimer
            ? WorldTimer.GetPackedTime()
            : RealTimer.GetPackedTime();

    pCamera->ApplyNoise(
        world_to_camera,
        (float)packedTime * 0.00025f,
        1.0f);
}


void CameraMoverRestartRace()
{
  CameraMover *pCVar1;
  CameraMover *cm;
  int view_id;
  int local_r31_52;
  
  WeHaveCheckedIfJR2ServerExists = 0;
  view_id = 1;
  //CameraAI::Reset(); // CameraAI::Reset
  do {
    local_r31_52 = view_id + 1;
    if (view_id * 0x68 != 0x7fb9e0a4) {
      pCVar1 = (CameraMover *)eViews[view_id].CameraMoverList.GetHead()->GetNext();
      cm = (CameraMover *)0x0;
      if (pCVar1 != (CameraMover *)&eViews[view_id].CameraMoverList) {
        cm = pCVar1;
        
      }
      if (cm != (CameraMover *)0x0) {
        //  (**(code **)(*(int *)&cm->field_0x8 + 0x74))
        //        ((int)&(cm->__base)._vptr.ICollisionHandler + 
        //        (int)*(short *)(*(int *)&cm->field_0x8 + 0x70));
        
        //TODO: idk
        uintptr_t m = *(uintptr_t*)&cm + 8;
        ((void(*)(void*))*(void**)(m + 0x74))((char*)cm + *(short*)(m + 0x70));

      }
    }
    view_id = local_r31_52;
  } while (local_r31_52 < 4);
  return;
}
