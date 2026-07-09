#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"


Camera TheCamera;
// CameraMover CamMover;

static int g_DisableCommunication = 0;

unsigned short aBaselineFovNoise = 0x2aaa;

static CameraLink g_CameraLink;

Camera::Camera() {

    bMatrix4 defaultMatrix;

    bClearVelocity = false;
    Camera::StopUpdating = false;
    JR2ServerExists = 0;
    ElapsedTime = 0.0f;
    LastUpdateTime = 0;
    LastDisparateTime = 0;
    RenderDash = 0;

    defaultMatrix.v2.z = -1.0;
    defaultMatrix.v3.z = 1200.0;
    defaultMatrix.v2.w = 100.0;

    this->LastUpdateTime = -0x80000000;
    this->LastDisparateTime = RealTimeFrames;

    defaultMatrix.v1.x = 0.0;
    defaultMatrix.v2.x = 0.0;
    defaultMatrix.v3.x = 0.0;
    defaultMatrix.v0.y = 0.0;
    defaultMatrix.v1.y = -1.0;
    defaultMatrix.v2.y = 0.0;
    defaultMatrix.v3.y = 0.0;
    defaultMatrix.v0.z = 0.0;
    defaultMatrix.v1.z = 0.0;
    defaultMatrix.v0.w = 0.0;
    defaultMatrix.v1.w = 0.0;

    (this->CurrentKey).FocalDistance = 0.0;
    (this->CurrentKey).DepthOfField = 0.0;
    this->ElapsedTime = 1.0;

    defaultMatrix.v0.x = 1.0;
    defaultMatrix.v3.w = 1.0;

    (this->CurrentKey).FarZ = 10000.0;
    (this->CurrentKey).FieldOfView = 0x36fb;
    (this->CurrentKey).NoiseAmplitude2.w = 0.0;
    (this->CurrentKey).LB_height = 0.0;
    (this->CurrentKey).NoiseAmplitude1.x = 0.0;
    (this->CurrentKey).NoiseAmplitude1.y = 0.0;
    (this->CurrentKey).NoiseAmplitude1.z = 0.0;
    (this->CurrentKey).NoiseAmplitude1.w = 0.0;
    (this->CurrentKey).SimTimeMultiplier = 1.0;
    (this->CurrentKey).NoiseFrequency1.x = 1.0;
    (this->CurrentKey).NoiseFrequency1.y = 1.0;
    (this->CurrentKey).NoiseFrequency1.z = 1.0;
    (this->CurrentKey).NoiseFrequency1.w = 1.0;
    (this->CurrentKey).NoiseFrequency2.x = 1.0;
    (this->CurrentKey).NoiseFrequency2.y = 1.0;
    (this->CurrentKey).NoiseFrequency2.z = 1.0;
    (this->CurrentKey).NoiseFrequency2.w = 1.0;

    (this->CurrentKey).NoiseAmplitude2.x = 0.0;
    (this->CurrentKey).NoiseAmplitude2.y = 0.0;
    (this->CurrentKey).NoiseAmplitude2.z = 0.0;

    SetCameraMatrix(&defaultMatrix, 1.0);
    SetCameraMatrix(&defaultMatrix, 1.0);
}



void Camera::SetCameraMatrix(bMatrix4 *m, float fTime) {
    bMatrix4 bStack_a8;

    if (Camera::StopUpdating == 0) {
        bMemCpy(&this->PreviousKey, this, 0xd4);
        this->ElapsedTime = fTime;

        if (Camera::JollyRancherResponse.UseMatrix == 0 || DisableCommunication != 0) {
            if (cameralink.field_3469 != 0) {
                cameralink.field_3469 = 0;
            }
            PSMTX44Copy((const float (*)[4])m, (float (*)[4])this);
        } else {
            if (cameralink.field_3469 == 0) {
                cameralink.field_3469 = 1;
            }
            bMemCpy(&bStack_a8, &Camera::JollyRancherResponse.CamMatrix, 0x40);
            bStack_a8.v3.x *= 0.01f;
            bStack_a8.v3.y *= 0.01f;
            bStack_a8.v3.z *= 0.01f;
            bStack_a8.v3.w = 1.0f;
            PSMTX44Copy((const float (*)[4]) & bStack_a8, (float (*)[4])this);
        }

        bTransposeMatrix(&bStack_a8, (bMatrix4 *)this);

        bStack_a8.v0.w = 0.0f;
        bStack_a8.v1.w = 0.0f;
        bStack_a8.v2.w = 0.0f;
        eMulVector(&(this->CurrentKey).Position, &bStack_a8, (bVector3 *)&(this->CurrentKey).Matrix.v3);

        (this->CurrentKey).Position.x = -(this->CurrentKey).Position.x;
        (this->CurrentKey).Position.y = -(this->CurrentKey).Position.y;
        (this->CurrentKey).Position.z = -(this->CurrentKey).Position.z;

        bNormalize(&(this->CurrentKey).Direction, (bVector3 *)&bStack_a8.v2);

        float targetDist = (this->CurrentKey).TargetDistance;
        (this->CurrentKey).Target.x = (this->CurrentKey).Direction.x * targetDist + (this->CurrentKey).Position.x;
        (this->CurrentKey).Target.y = (this->CurrentKey).Direction.y * targetDist + (this->CurrentKey).Position.y;
        (this->CurrentKey).Target.z = (this->CurrentKey).Direction.z * targetDist + (this->CurrentKey).Position.z;

        if (this->bClearVelocity) {
            this->bClearVelocity = false;
            bMemCpy(&this->PreviousKey, this, 0xd4);
            this->ElapsedTime = 1.0f;
        }

        if (0.0 < (double)this->ElapsedTime) {
            float invTime = 1.0f / this->ElapsedTime;

            (this->VelocityKey).Position.x = ((this->CurrentKey).Position.x - (this->PreviousKey).Position.x) * invTime;
            (this->VelocityKey).Position.y = ((this->CurrentKey).Position.y - (this->PreviousKey).Position.y) * invTime;
            (this->VelocityKey).Position.z = ((this->CurrentKey).Position.z - (this->PreviousKey).Position.z) * invTime;

            (this->VelocityKey).Direction.x = ((this->CurrentKey).Direction.x - (this->PreviousKey).Direction.x) * invTime;
            (this->VelocityKey).Direction.y = ((this->CurrentKey).Direction.y - (this->PreviousKey).Direction.y) * invTime;
            (this->VelocityKey).Direction.z = ((this->CurrentKey).Direction.z - (this->PreviousKey).Direction.z) * invTime;

            (this->VelocityKey).Target.x = ((this->CurrentKey).Target.x - (this->PreviousKey).Target.x) * invTime;
            (this->VelocityKey).Target.y = ((this->CurrentKey).Target.y - (this->PreviousKey).Target.y) * invTime;
            (this->VelocityKey).Target.z = ((this->CurrentKey).Target.z - (this->PreviousKey).Target.z) * invTime;

            (this->VelocityKey).TargetDistance = ((this->CurrentKey).TargetDistance - (this->PreviousKey).TargetDistance) * invTime;
            (this->VelocityKey).FocalDistance = ((this->CurrentKey).FocalDistance - (this->PreviousKey).FocalDistance) * invTime;
            (this->VelocityKey).DepthOfField = ((this->CurrentKey).DepthOfField - (this->PreviousKey).DepthOfField) * invTime;
            (this->VelocityKey).NearZ = ((this->CurrentKey).NearZ - (this->PreviousKey).NearZ) * invTime;
            (this->VelocityKey).FarZ = ((this->CurrentKey).FarZ - (this->PreviousKey).FarZ) * invTime;
            (this->VelocityKey).LB_height = ((this->CurrentKey).LB_height - (this->PreviousKey).LB_height) * invTime;
            (this->VelocityKey).SimTimeMultiplier = ((this->CurrentKey).SimTimeMultiplier - (this->PreviousKey).SimTimeMultiplier) * invTime;

            (this->VelocityKey).FieldOfView =
                (unsigned short)(int)(invTime * (float)((short)((this->CurrentKey).FieldOfView - (this->PreviousKey).FieldOfView)));

            (this->VelocityKey).NoiseFrequency1.x = ((this->CurrentKey).NoiseFrequency1.x - (this->PreviousKey).NoiseFrequency1.x) * invTime;
            (this->VelocityKey).NoiseFrequency1.y = ((this->CurrentKey).NoiseFrequency1.y - (this->PreviousKey).NoiseFrequency1.y) * invTime;
            (this->VelocityKey).NoiseFrequency1.z = ((this->CurrentKey).NoiseFrequency1.z - (this->PreviousKey).NoiseFrequency1.z) * invTime;
            (this->VelocityKey).NoiseFrequency1.w = ((this->CurrentKey).NoiseFrequency1.w - (this->PreviousKey).NoiseFrequency1.w) * invTime;

            (this->VelocityKey).NoiseFrequency2.x = ((this->CurrentKey).NoiseFrequency2.x - (this->PreviousKey).NoiseFrequency2.x) * invTime;
            (this->VelocityKey).NoiseFrequency2.y = ((this->CurrentKey).NoiseFrequency2.y - (this->PreviousKey).NoiseFrequency2.y) * invTime;
            (this->VelocityKey).NoiseFrequency2.z = ((this->CurrentKey).NoiseFrequency2.z - (this->PreviousKey).NoiseFrequency2.z) * invTime;
            (this->VelocityKey).NoiseFrequency2.w = ((this->CurrentKey).NoiseFrequency2.w - (this->PreviousKey).NoiseFrequency2.w) * invTime;

            (this->VelocityKey).NoiseAmplitude1.x = ((this->CurrentKey).NoiseAmplitude1.x - (this->PreviousKey).NoiseAmplitude1.x) * invTime;
            (this->VelocityKey).NoiseAmplitude1.y = ((this->CurrentKey).NoiseAmplitude1.y - (this->PreviousKey).NoiseAmplitude1.y) * invTime;
            (this->VelocityKey).NoiseAmplitude1.z = ((this->CurrentKey).NoiseAmplitude1.z - (this->PreviousKey).NoiseAmplitude1.z) * invTime;
            (this->VelocityKey).NoiseAmplitude1.w = ((this->CurrentKey).NoiseAmplitude1.w - (this->PreviousKey).NoiseAmplitude1.w) * invTime;

            (this->VelocityKey).NoiseAmplitude2.x = ((this->CurrentKey).NoiseAmplitude2.x - (this->PreviousKey).NoiseAmplitude2.x) * invTime;
            (this->VelocityKey).NoiseAmplitude2.y = ((this->CurrentKey).NoiseAmplitude2.y - (this->PreviousKey).NoiseAmplitude2.y) * invTime;
            (this->VelocityKey).NoiseAmplitude2.z = ((this->CurrentKey).NoiseAmplitude2.z - (this->PreviousKey).NoiseAmplitude2.z) * invTime;
            (this->VelocityKey).NoiseAmplitude2.w = ((this->CurrentKey).NoiseAmplitude2.w - (this->PreviousKey).NoiseAmplitude2.w) * invTime;
        }
    }
}

void Camera::CommunicateWithJollyRancher(char *cameraname) {

    char data[0x60];
    Mtx44 scaledmatrix;

    JollyRancherResponsePacket *local_10;
    int local_c;

    if (DisableCommunication == 0) {

        local_10 = &Camera::JollyRancherResponse;
        local_c = DisableCommunication;

        bMemCpy(data, &local_10, 4);
        bMemCpy(data + 4, &local_c, 4);
        {
            PSMTX44Copy((const float (*)[4])this, (float (*)[4]) & scaledmatrix);
        }
        scaledmatrix[3][0] *= 100.0f;
        scaledmatrix[3][1] *= 100.0f;
        scaledmatrix[3][2] *= 100.0f;
        scaledmatrix[3][3] = 1.0f;
        {
            bMemCpy(data + 8, (void *)(&scaledmatrix), 0x40);
        }
        bStrCpy(data + 0x48, cameraname);
        bFunkCallASync("JR2Server", 1, data, 0x60);
    }
    return;
}

unsigned short Camera::FovRelativeAngle(unsigned short a) {
    float sin_a = bSin(a);
    float sin_fov = bSin((unsigned short)(this->CurrentKey.FieldOfView >> 1));
    float sin_baseline = bSin((unsigned short)(aBaselineFovNoise >> 1));

    return bASin((float)(sin_a * sin_fov) / sin_baseline);
}

void Camera::UpdateAll(float dT){
    UpdateCameraMovers(dT);
    //UpdateCameraShakers((float)dT); //TODO
}

void UpdateCameraMovers(float dT) {

  bool bVar1;
  int iVar2;
  Vector3 *sl;
  bNode *pbVar3;
  Camera *pCVar4;
  bNode *pbVar5;
  bNode *pbVar6;
  Camera *camera;
  int uVar7;
  unsigned long long in_f31;
  double dVar8;
  bVector3 pos;
  bVector3 vel;
  bVector3 dir;
  //LongVector fix_look;
  bVector3 local_78;
  bVector3 local_68;
  bVector3 local_58 [2];
  char local_8 [8];
  
  dVar8 = (double)dT;
  uVar7 = 0;

  camera = (Camera *)0x0;
  do {
    if ((int)camera * 0x68 != 0x7fb9e0a4) {
      bNode* head = (bNode*)&eViews[(int)camera].CameraMoverList;
      bNode* first = head->Next;
    //   if (pbVar6 != (bNode *)((int)camera * 0x68 + -0x7fb9e068)) { 
    //     pbVar5 = pbVar6;
    //   }
      if (first != head) { 
        
        CameraMover* mover = (CameraMover*)first; 
        mover->Update(dT);

      }
    }
    camera = (Camera *)((int)&(camera->CurrentKey).Matrix.v0.x + 1);
  } while ((int)camera < 0x16);
  if (WeHaveCheckedIfJR2ServerExists == 0) { // who is owner of this variable? Camera class? World?
    JR2ServerExists = bFunkDoesServerExist("JR2Server");
    WeHaveCheckedIfJR2ServerExists = 1;
  }
  if ((JR2ServerExists != 0) && (eViews[1].pCamera != 0x0)) {
    iVar2 = RealTime - LastUpdateTimeJR2; // who is owner of the JR?
    if (iVar2 < 0) {
      iVar2 = -iVar2;
    }
    if (0x10 < iVar2) {
      LastUpdateTimeJR2 = RealTime;
      eViews[1].pCamera->CommunicateWithJollyRancher("SpeedCam");
    }
  }
  if (((GManager::Get().mObj == (GManager *)0x0) || (*(int *)&GManager::mObj->mWarping == 0)) && 
     ((GRaceStatus::fObj == (GRaceStatus *)0x0 ||
      (*(int *)&GRaceStatus::fObj->mScriptWaitingForLoad == 0)))) {
    dVar8 = 0.0;
    bVar1 = false;
    iVar2 = 1;
    do {
      if (eViews[iVar2].Active != '\0') {
        //pbVar3 = eViews[iVar2].CameraMoverList.__base.HeadNode.Next; 
        bNode* firstNode = eViews[iVar2].CameraMoverList.GetHead()->Next;
        pbVar6 = (bNode *)(iVar2 * 0x68 + -0x7fb9e068);
        pbVar5 = (bNode *)0x0;
        if (pbVar3 != pbVar6) {
          pbVar5 = pbVar3;
        }
        if (pbVar5 != (bNode *)0x0) {
          if (!bVar1) {
            TheTrackStreamer.ClearStreamingPositions();
            bVar1 = true;
          }
          pCVar4 = eViews[iVar2].pCamera;
          local_78.x = pCVar4->CurrentKey.Position.x;
          local_78.y = (pCVar4->CurrentKey).Position.y;
          local_78.z = (pCVar4->CurrentKey).Position.z;
          local_68.z = (pCVar4->VelocityKey).Position.z;
          local_68.x = (pCVar4->VelocityKey).Position.x;
          local_68.y = (pCVar4->VelocityKey).Position.y;
          pCVar4 = eViews[iVar2].pCamera;
          local_58[0].z = (pCVar4->CurrentKey).Direction.z;
          local_58[0].x = (pCVar4->CurrentKey).Direction.x;
          local_58[0].y = (pCVar4->CurrentKey).Direction.y;
          if (bStreamingPositionFromICE != 0) {
            if (UTL::Collections::Singleton<INIS>::Get() != (INIS *)0x0) {
              sl = (Vector3 *)((UTL::Collections::Singleton<INIS>::Get()->GetStartCameraLocation()));
              local_78.x = sl->z;
              local_78.z = sl->y;
              local_78.y = -sl->x;
            }
            local_68.x = (float)dVar8;
            local_58[0].z = local_68.z;
            local_68.y = (float)dVar8;
            local_58[0].x = (float)dVar8;
            local_58[0].y = (float)dVar8;
          }
          pbVar3 = (bNode*)&eViews[iVar2].CameraMoverList;
          pbVar5 = (bNode *)0x0;
          if (pbVar3 != pbVar6) {
            pbVar5 = pbVar3;
          }
          TheTrackStreamer.PredictStreamingPosition(
            (unsigned int)(iVar2 == 2),
            &local_78,
            &local_68,
            local_58,
            pbVar5[1].Prev == (bNode *)0x1
        );
        }
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < 3);
  }
  return;

}