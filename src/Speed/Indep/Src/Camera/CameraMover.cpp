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



//TODO: rename variables, refactor ghidra pseudocods
void CameraMover::HandheldNoise (bMatrix4 *world_to_camera,float f_scale,bool useWorldTimer)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  Camera *pCVar9;
  bVector4 v_frequency;
  bVector4 v_magnitude;
  
  fVar7 = CameraNoiseHandheldFrequency.z;
  fVar6 = CameraNoiseHandheldFrequency.y;
  fVar5 = CameraNoiseHandheldFrequency.x;
  if (f_scale > 0.0) {
    fVar1 = CameraNoiseHandheldAmplitude.w * f_scale;
    fVar2 = CameraNoiseHandheldAmplitude.y * f_scale;
    fVar3 = CameraNoiseHandheldAmplitude.z * f_scale;
    fVar4 = CameraNoiseHandheldAmplitude.x * f_scale;
    pCVar9 = this->pCamera;
    (pCVar9->CurrentKey).NoiseFrequency1.w = CameraNoiseHandheldFrequency.w;
    (pCVar9->CurrentKey).NoiseFrequency1.x = fVar5;
    (pCVar9->CurrentKey).NoiseFrequency1.y = fVar6;
    (pCVar9->CurrentKey).NoiseFrequency1.z = fVar7;
    pCVar9 = this->pCamera;
    (pCVar9->CurrentKey).NoiseAmplitude1.x = fVar4;
    (pCVar9->CurrentKey).NoiseAmplitude1.w = fVar1;
    (pCVar9->CurrentKey).NoiseAmplitude1.y = fVar2;
    (pCVar9->CurrentKey).NoiseAmplitude1.z = fVar3;
    iVar8 = RealTimer.GetPackedTime();
    if (useWorldTimer) {
      iVar8 = WorldTimer.GetPackedTime();
    }
    pCVar9->ApplyNoise(world_to_camera,(float)iVar8 * 0.00025,1.0);
  }
  return;
}


// TODO: DAT_80480a0c, DAT_80480a14 from PVehicle::UpdateListing

// void CameraMover::ChopperNoise
//                (bMatrix4 *world_to_camera,float f_scale,bool useWorldTimer){
//   float fVar1;
//   float fVar2;
//   float fVar3;
//   float fVar4;
//   float fVar5;
//   float fVar6;
//   float fVar7;
//   int iVar8;
//   int *piVar9;
//   float *pfVar10;
//   Camera *pCVar11;
//   IVehicle **iter;
//   IVehicle *vehicle;
//   bVector3 bpos;
//   bVector3 dir;
//   bVector4 v_frequency;
//   bVector4 v_magnitude;
//         iter = DAT_80480a0c;
//         if (f_scale > 0.0) {
//             for (; iter != DAT_80480a0c + DAT_80480a14; iter = iter + 1) {
//             vehicle = *iter;
//             iVar8 = (*(code *)(vehicle->__vtable->IsActive).__pfn)
//                                 (&vehicle->__base + (short)(vehicle->__vtable->IsActive).__delta);
//             if ((iVar8 != 0) &&
//                 (piVar9 = (int *)(*(code *)(vehicle->__vtable->GetVehicleClass).__pfn)
//                                             (&vehicle->__base +
//                                             (short)(vehicle->__vtable->GetVehicleClass).__delta),
//                 *piVar9 == UTL::Collections::Instanceable<>::_mList.__base.__base.mVectorSpace[0x19])) {
//                 pfVar10 = (float *)(*(code *)(vehicle->__vtable->GetPosition).__pfn)
//                                             (&vehicle->__base +
//                                             (short)(vehicle->__vtable->GetPosition).__delta);
//                 fVar7 = CameraNoiseChopperFrequency.z;
//                 fVar6 = CameraNoiseChopperFrequency.y;
//                 fVar5 = CameraNoiseChopperFrequency.x;
//                 fVar1 = 0.0;
//                 fVar2 = -*pfVar10 - (this->pCamera->CurrentKey).Position.y;
//                 fVar3 = pfVar10[2] - (this->pCamera->CurrentKey).Position.x;
//                 fVar2 = fVar3 * fVar3 + fVar2 * fVar2;
//                 if (fVar2 > 5e-11) {
//                 fVar1 = 1.0 / SQRT(fVar2);
//                 fVar1 = -(fVar2 * fVar1 * fVar1 - 1.0) * fVar1 * 0.5 + fVar1;
//                 fVar1 = (-(fVar2 * fVar1 * fVar1 - 1.0) * fVar1 * 0.5 + fVar1) * fVar2;
//                 }
//                 if (fVar1 < 40.0) {
//                 fVar1 = f_scale * (1.0 - fVar1 * 0.025);
//                 fVar2 = CameraNoiseChopperAmplitude.y * fVar1;
//                 fVar3 = CameraNoiseChopperAmplitude.z * fVar1;
//                 fVar4 = CameraNoiseChopperAmplitude.w * fVar1;
//                 fVar1 = CameraNoiseChopperAmplitude.x * fVar1;
//                 pCVar11 = this->pCamera;
//                 (pCVar11->CurrentKey).NoiseFrequency1.w = CameraNoiseChopperFrequency.w;
//                 (pCVar11->CurrentKey).NoiseFrequency1.x = fVar5;
//                 (pCVar11->CurrentKey).NoiseFrequency1.y = fVar6;
//                 (pCVar11->CurrentKey).NoiseFrequency1.z = fVar7;
//                 pCVar11 = this->pCamera;
//                 (pCVar11->CurrentKey).NoiseAmplitude1.x = fVar1;
//                 (pCVar11->CurrentKey).NoiseAmplitude1.w = fVar4;
//                 (pCVar11->CurrentKey).NoiseAmplitude1.y = fVar2;
//                 (pCVar11->CurrentKey).NoiseAmplitude1.z = fVar3;
//                 iVar8 = RealTimer.GetPackedTime();
//                 if (useWorldTimer) {
//                     iVar8 = WorldTimer.GetPackedTime();
//                 }
//                 Camera::ApplyNoise(this->pCamera,world_to_camera,(float)iVar8 * 0.00025,1.0);
//                 }
//             }
//             }
//         }
//   return;
// }







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


