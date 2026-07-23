#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"


// NOISE 

float NoiseBase(int x)
{
    unsigned int hash = (x << 13) ^ x;
    
    hash = hash * (hash * hash * 0x3D73 + 0xC0AE5) + 0x5208DD0D;
    
    hash = (hash & 0x7FFFFFFF) ^ 0x8000;

    float normalized = (float)hash * (1.0f / 1073741824.0f); 
    
    return 1.0f - normalized;
}

float NoiseInterpolated(float x)
{
  int left_index;
  float x_floor;
  float right_noise;
  
  x_floor = (float)(int)x;
  if (x < x_floor) {
    x_floor = x_floor - 1.0f;
  }
  left_index = (int)x_floor;
  x_floor = NoiseBase(left_index);
  right_noise = NoiseBase(left_index + 1);
  return (x - (float)left_index) * right_noise + (1.0f - (x - (float)left_index)) * x_floor;
}

float Noise(float x) {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    
    for (int i = 0; i < 6; ++i) {
        total += NoiseInterpolated(x * frequency) * amplitude;
        
        frequency *= 2.0f; 
        amplitude *= 0.5f; 
    }
    
    return total;
}

Camera::Camera() {
    int RealTime;
    // bMatrix4 m; // 0x4
    bMatrix4 defaultMatrix; // 0x68


    RealTime = RealTimeFrames;
    LastUpdateTime = -0x80000000;
    LastDisparateTime = RealTime;
    RenderDash = 0;

    defaultMatrix.v2.z = -1.0;
    defaultMatrix.v3.z = 1200.0;
    defaultMatrix.v2.w = 100.0;

    CurrentKey.TargetDistance = 10.0;
    CurrentKey.NearZ = 0.5;
    bClearVelocity = false;

    
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

    CurrentKey.FocalDistance = 0.0;
    CurrentKey.DepthOfField = 0.0;
    ElapsedTime = 1.0;

    defaultMatrix.v0.x = 1.0;
    defaultMatrix.v3.w = 1.0;

    CurrentKey.FarZ = 10000.0;
    CurrentKey.FieldOfView = 0x36fb;


    CurrentKey.NoiseAmplitude2.w = 0.0f;
    CurrentKey.SimTimeMultiplier = 1.0;
    CurrentKey.LB_height = 0.0;
    CurrentKey.NoiseFrequency1.x = 1.0;
    CurrentKey.NoiseAmplitude1.x = 0.0;
    CurrentKey.NoiseAmplitude1.y = 0.0;
    CurrentKey.NoiseAmplitude1.z = 0.0;
    CurrentKey.NoiseAmplitude1.w = 0.0;

    
    
    CurrentKey.NoiseFrequency1.y = 1.0;
    CurrentKey.NoiseFrequency1.z = 1.0;
    CurrentKey.NoiseFrequency1.w = 1.0;
    CurrentKey.NoiseFrequency2.x = 1.0;
    CurrentKey.NoiseFrequency2.y = 1.0;
    CurrentKey.NoiseFrequency2.z = 1.0;
    CurrentKey.NoiseFrequency2.w = 1.0;



    CurrentKey.NoiseAmplitude2.x = 0.0;
    CurrentKey.NoiseAmplitude2.y = 0.0;
    CurrentKey.NoiseAmplitude2.z = 0.0;



    SetCameraMatrix(&defaultMatrix, 1.0);
    SetCameraMatrix(&defaultMatrix, 1.0);
}


void Camera::SetCameraMatrix(bMatrix4 *m, float fTime) {
    bMatrix4 matrixBuffer;

    if (StopUpdating == 0) {
        bMemCpy(&PreviousKey, this, 0xd4);
        ElapsedTime = fTime;

        if (Camera::JollyRancherResponse.UseMatrix == 0 || DisableCommunication != 0) {
            if (g_cameralink.linked != 0) {
                g_cameralink.linked = 0;
            }
            PSMTX44Copy((const float (*)[4])m, (float (*)[4])this);
        } else {
            if (g_cameralink.linked == 0) {
                g_cameralink.linked = 1;
            }
            bMemCpy(&matrixBuffer, &Camera::JollyRancherResponse.CamMatrix, 0x40);
            matrixBuffer.v3.x *= 0.01f;
            matrixBuffer.v3.y *= 0.01f;
            matrixBuffer.v3.z *= 0.01f;
            matrixBuffer.v3.w = 1.0f;
            PSMTX44Copy((const float (*)[4]) & matrixBuffer, (float (*)[4])this);
        }

        bTransposeMatrix(&matrixBuffer, (bMatrix4 *)this);

        matrixBuffer.v0.w = 0.0f;
        matrixBuffer.v1.w = 0.0f;
        matrixBuffer.v2.w = 0.0f;
        eMulVector(&CurrentKey.Position, &matrixBuffer, (bVector3 *)&CurrentKey.Matrix.v3);

        CurrentKey.Position.x = -CurrentKey.Position.x;
        CurrentKey.Position.y = -CurrentKey.Position.y;
        CurrentKey.Position.z = -CurrentKey.Position.z;

        bNormalize(&CurrentKey.Direction, (bVector3 *)&matrixBuffer.v2);

        float targetDist = CurrentKey.TargetDistance;
        CurrentKey.Target.x = CurrentKey.Direction.x * targetDist + CurrentKey.Position.x;
        CurrentKey.Target.y = CurrentKey.Direction.y * targetDist + CurrentKey.Position.y;
        CurrentKey.Target.z = CurrentKey.Direction.z * targetDist + CurrentKey.Position.z;

        if (bClearVelocity) {
            bClearVelocity = false;
            bMemCpy(&PreviousKey, this, 0xd4);
            ElapsedTime = 1.0f;
        }

        if (0.0 < (double)ElapsedTime) {
            float invTime = 1.0f / ElapsedTime;

            VelocityKey.Position.x = (CurrentKey.Position.x - PreviousKey.Position.x) * invTime;
            VelocityKey.Position.y = (CurrentKey.Position.y - PreviousKey.Position.y) * invTime;
            VelocityKey.Position.z = (CurrentKey.Position.z - PreviousKey.Position.z) * invTime;

            VelocityKey.Direction.x = (CurrentKey.Direction.x - PreviousKey.Direction.x) * invTime;
            VelocityKey.Direction.y = (CurrentKey.Direction.y - PreviousKey.Direction.y) * invTime;
            VelocityKey.Direction.z = (CurrentKey.Direction.z - PreviousKey.Direction.z) * invTime;

            VelocityKey.Target.x = (CurrentKey.Target.x - PreviousKey.Target.x) * invTime;
            VelocityKey.Target.y = (CurrentKey.Target.y - PreviousKey.Target.y) * invTime;
            VelocityKey.Target.z = (CurrentKey.Target.z - PreviousKey.Target.z) * invTime;

            VelocityKey.TargetDistance = (CurrentKey.TargetDistance - PreviousKey.TargetDistance) * invTime;
            VelocityKey.FocalDistance = (CurrentKey.FocalDistance - PreviousKey.FocalDistance) * invTime;
            VelocityKey.DepthOfField = (CurrentKey.DepthOfField - PreviousKey.DepthOfField) * invTime;
            VelocityKey.NearZ = (CurrentKey.NearZ - PreviousKey.NearZ) * invTime;
            VelocityKey.FarZ = (CurrentKey.FarZ - PreviousKey.FarZ) * invTime;
            VelocityKey.LB_height = (CurrentKey.LB_height - PreviousKey.LB_height) * invTime;
            VelocityKey.SimTimeMultiplier = (CurrentKey.SimTimeMultiplier - PreviousKey.SimTimeMultiplier) * invTime;

            VelocityKey.FieldOfView =
                (unsigned short)(int)(invTime * (float)((short)(CurrentKey.FieldOfView - PreviousKey.FieldOfView)));

            VelocityKey.NoiseFrequency1.x = (CurrentKey.NoiseFrequency1.x - PreviousKey.NoiseFrequency1.x) * invTime;
            VelocityKey.NoiseFrequency1.y = (CurrentKey.NoiseFrequency1.y - PreviousKey.NoiseFrequency1.y) * invTime;
            VelocityKey.NoiseFrequency1.z = (CurrentKey.NoiseFrequency1.z - PreviousKey.NoiseFrequency1.z) * invTime;
            VelocityKey.NoiseFrequency1.w = (CurrentKey.NoiseFrequency1.w - PreviousKey.NoiseFrequency1.w) * invTime;

            VelocityKey.NoiseFrequency2.x = (CurrentKey.NoiseFrequency2.x - PreviousKey.NoiseFrequency2.x) * invTime;
            VelocityKey.NoiseFrequency2.y = (CurrentKey.NoiseFrequency2.y - PreviousKey.NoiseFrequency2.y) * invTime;
            VelocityKey.NoiseFrequency2.z = (CurrentKey.NoiseFrequency2.z - PreviousKey.NoiseFrequency2.z) * invTime;
            VelocityKey.NoiseFrequency2.w = (CurrentKey.NoiseFrequency2.w - PreviousKey.NoiseFrequency2.w) * invTime;

            VelocityKey.NoiseAmplitude1.x = (CurrentKey.NoiseAmplitude1.x - PreviousKey.NoiseAmplitude1.x) * invTime;
            VelocityKey.NoiseAmplitude1.y = (CurrentKey.NoiseAmplitude1.y - PreviousKey.NoiseAmplitude1.y) * invTime;
            VelocityKey.NoiseAmplitude1.z = (CurrentKey.NoiseAmplitude1.z - PreviousKey.NoiseAmplitude1.z) * invTime;
            VelocityKey.NoiseAmplitude1.w = (CurrentKey.NoiseAmplitude1.w - PreviousKey.NoiseAmplitude1.w) * invTime;

            VelocityKey.NoiseAmplitude2.x = (CurrentKey.NoiseAmplitude2.x - PreviousKey.NoiseAmplitude2.x) * invTime;
            VelocityKey.NoiseAmplitude2.y = (CurrentKey.NoiseAmplitude2.y - PreviousKey.NoiseAmplitude2.y) * invTime;
            VelocityKey.NoiseAmplitude2.z = (CurrentKey.NoiseAmplitude2.z - PreviousKey.NoiseAmplitude2.z) * invTime;
            VelocityKey.NoiseAmplitude2.w = (CurrentKey.NoiseAmplitude2.w - PreviousKey.NoiseAmplitude2.w) * invTime;
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
        bFunkCallASync("JR2Server", 1, data, 0x60); //do nothing
    }
    return;
}

unsigned short Camera::FovRelativeAngle(unsigned short a) {

    float sin_a = bSin(a);
    float sin_fov = bSin((unsigned short)(CurrentKey.FieldOfView >> 1));
    float sin_baseline = bSin((unsigned short)(0x2aaa >> 1));

    return bASin((float)(sin_a * sin_fov) / sin_baseline);
}

void Camera::UpdateAll(float dT){
    UpdateCameraMovers(dT);
    //UpdateCameraShakers((float)dT); 
    // TODO
}

void Camera::ApplyNoise(bMatrix4 *p_matrix,float time,float intensity){
  int a;

  unsigned short angle;
  float x1;
  float y1;
  float z1;
  float w1;

  float x2;
  float y2;
  float z2;
  float w2;

  bMatrix4 bmatrix4;
  long long local_48;
  
  x1 = Noise(CurrentKey.NoiseFrequency1.x * time);
  y1 = Noise(CurrentKey.NoiseFrequency1.y * time);
  z1 = Noise(CurrentKey.NoiseFrequency1.z * time);
  w1 = Noise(CurrentKey.NoiseFrequency1.w * time);

  x2 = Noise(CurrentKey.NoiseFrequency2.x * time);
  y2 = Noise(CurrentKey.NoiseFrequency2.y * time);
  z2 = Noise(CurrentKey.NoiseFrequency2.z * time);
  w2 = Noise(CurrentKey.NoiseFrequency2.w * time);

  PSMTX44Identity( (float (*)[4])&bmatrix4);

  a = (int)((z1 * CurrentKey.NoiseFrequency1.w + z2 * CurrentKey.NoiseFrequency2.w) * intensity * 65536.0);

  bmatrix4.v3.x = (x1 * CurrentKey.NoiseAmplitude1.x + x2 * CurrentKey.NoiseAmplitude2.x) * intensity;
  bmatrix4.v3.y = (y1 * CurrentKey.NoiseAmplitude1.y + y2 * CurrentKey.NoiseAmplitude2.y) * intensity;

  angle = Camera::FovRelativeAngle((unsigned short)(a / 0x168));
  eRotateX(&bmatrix4,&bmatrix4,angle);

  a = (int)((w1 * CurrentKey.NoiseAmplitude1.w + w2 * CurrentKey.NoiseAmplitude2.w) * intensity * 65536.0);
  angle = Camera::FovRelativeAngle((unsigned short)(a / 0x168));
  eRotateY(&bmatrix4,&bmatrix4,angle);
  
  eMulMatrix(p_matrix,p_matrix,&bmatrix4);
  return;
}


void UpdateCameraMovers(float deltaTime)
{
    for (int viewIndex = 0; viewIndex < 22; ++viewIndex) // 22 is NUM_VIEWS
    {
        eView& view = eViews[viewIndex];

        bNode* head = eViews[viewIndex].CameraMoverList.GetHead();
        bNode* first = head->Next;

        if (first != head)
        {
            CameraMover* mover = (CameraMover*)first;
            mover->Update(deltaTime);
        }
    }

    if (!WeHaveCheckedIfJR2ServerExists)
    {
        JR2ServerExists = bFunkDoesServerExist("JR2Server");
        WeHaveCheckedIfJR2ServerExists = 1;
    }

    if (JR2ServerExists && eViews[1].pCamera)
    {
        int elapsed = abs(RealTime - LastUpdateTimeJR2);

        if (elapsed > 16)
        {
            LastUpdateTimeJR2 = RealTime;
            eViews[1].pCamera->CommunicateWithJollyRancher("SpeedCam");
        }
    }

    const bool warping =
        GManager::Get().mObj &&
        GManager::mObj->mWarping;

    const bool waitingForLoad =
        GRaceStatus::fObj &&
        GRaceStatus::fObj->mScriptWaitingForLoad;

    if (warping || waitingForLoad)
        return;

    bool streamerCleared = false;

    for (int viewIndex = 1; viewIndex < 3; ++viewIndex)
    {
        eView& view = eViews[viewIndex];

        if (!view.Active)
            continue;

        if (view.CameraMoverList.IsEmpty())
            continue;

        if (!streamerCleared)
        {
            TheTrackStreamer.ClearStreamingPositions();
            streamerCleared = true;
        }

        Camera* camera = view.pCamera;

        bVector3 position  = camera->CurrentKey.Position;
        bVector3 velocity  = camera->VelocityKey.Position;
        bVector3 direction = camera->CurrentKey.Direction;

        if (bStreamingPositionFromICE)
        {
            if (INIS* inis = UTL::Collections::Singleton<INIS>::Get())
            {
                const Vector3* editorPos = inis->GetStartCameraLocation();

                position.x = editorPos->z;
                position.y = -editorPos->x;
                position.z = editorPos->y;
            }

            velocity = bVector3(0.0f, 0.0f, 0.0f);
            direction = bVector3(0.0f, 0.0f, 0.0f);
        }

        const bool rearView = (viewIndex == 2);

        const bool freezePrediction =
            view.CameraMoverList.GetHead()->Next->Prev ==
            reinterpret_cast<bNode*>(1);

        TheTrackStreamer.PredictStreamingPosition(
            rearView,
            &position,
            &velocity,
            &direction,
            freezePrediction);
    }
}






