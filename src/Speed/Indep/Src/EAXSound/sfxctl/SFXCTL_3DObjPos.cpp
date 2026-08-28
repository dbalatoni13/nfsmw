#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"

DEFINE_CREATABLE(0x20090, SFXCTL_3DObjPos, SFXCTL);

static const float fTWO_PLYR_XFADE_AZIM_DIST = 20.0f;                  // size: 0x4, Decl: 12
static const unsigned int TWO_PLAYER_DISTANT_TARGET_AZIM_RIGHT = 5461; // size: 0x4, Decl: 13
static const unsigned int TWO_PLAYER_DISTANT_TARGET_AZIM_LEFT = 60074; // size: 0x4, Decl: 14

#define NUM_DOPPLER_AVG_POINTS 4 // Decl: 16

bVector2 *SFXCTL_3DObjPos::m_pv2AzimRefDir = nullptr;                                                                       // size: 0x4, Decl: 17
bVector2 *SFXCTL_3DObjPos::m_pv2AzimRefPos = nullptr;                                                                       // size: 0x4, Decl: 18
bAngle SFXCTL_3DObjPos::m_CameraAngle = 0;                                                                                  // size: 0x2, Decl: 19
bAngle SFXCTL_3DObjPos::m_PlyrCarAzimToCam[2];                                                                              // size: 0x4, Decl: 20
bAngle SFXCTL_3DObjPos::m_PlyrCarLockAzim[2] = {TWO_PLAYER_DISTANT_TARGET_AZIM_RIGHT, TWO_PLAYER_DISTANT_TARGET_AZIM_LEFT}; // size: 0x4, Decl: 21
float SFXCTL_3DObjPos::m_fDistBetweenPlyrCars = 0.0f;                                                                       // size: 0x4, Decl: 22
int SFXCTL_3DObjPos::m_CurLeadCar = 0;                                                                                      // size: 0x4, Decl: 23
int SFXCTL_3DObjPos::m_OutTempRefCar[16];                                                                                   // size: 0x40, Decl: 24
int SFXCTL_3DObjPos::m_OutTempOtherCar[16];                                                                                 // size: 0x40, Decl: 25
float SFXCTL_3DObjPos::fratio = 1.0f;                                                                                       // size: 0x4, Decl: 26
float SFXCTL_3DObjPos::fratio_2 = 1.0f;                                                                                     // size: 0x4, Decl: 27
bVector2 SFXCTL_3DObjPos::m_v2ObjPosCopy;                                                                                   // size: 0x8, Decl: 28

bVector2 SFXCTL_3DObjPos::m_v2ObjDirCopy; // size: 0x8, Decl: 30

SFXCTL_3DObjPos::SFXCTL_3DObjPos()
    : m_pV3ObjPos(nullptr),                  //
      m_pV3ObjDir(nullptr),                  //
      m_pV3ObjVel(nullptr),                  //
      m_bIsInTwoPlayerTransitionZone(false), //
      m_bDirectionClockwise(false),          //
      m_bIsOtherCamRightSide(false) {
    this->SetPlayerRef(0);
    this->m_fDistToRef[0][0] = 0.0f;
    this->m_fDistToRef[0][1] = 0.0f;
    this->m_fDistToRef[1][0] = 0.0f;
    this->m_fDistToRef[1][1] = 0.0f;
}

SFXCTL_3DObjPos::~SFXCTL_3DObjPos() {}

void SFXCTL_3DObjPos::SetPlayerRef(int i) {
    this->m_PlayerRef = i;
}

void SFXCTL_3DObjPos::SetCameraAngle() {
    bVector2 v2Pos(0.0f, 0.0f);
    bSub(&v2Pos, &m_v2ObjPosCopy, m_pv2AzimRefPos);

    bVector2 v2NormPos(0.0f, 0.0f);
    bNormalize(&v2NormPos, &v2Pos);

    float fdotobj = bDot(&v2NormPos, m_pv2AzimRefDir);
    m_CameraAngle = bACos(fdotobj);

    if (-v2NormPos.y * m_pv2AzimRefDir->x + v2NormPos.x * m_pv2AzimRefDir->y < 0.0f) {
        m_CameraAngle = static_cast<unsigned short>(~m_CameraAngle);
    }
}

ePOSMIXTYPE POSMIXTYPE = SINGLE_PLAYER; // size: 0x4, address: 0x80417AB8, Decl: 126

// STRIPPED
void SFXCTL_3DObjPos::GenerateTwoPlayerMix() {}

// STRIPPED
void SFXCTL_3DObjPos::GenerateTwoPlayerAzim(eTP3DPosInputs azimtype) {}

void SFXCTL_3DObjPos::GenerateSinglePlayerMix() {
    if ((SndCamera::GetCam(this->m_PlayerRef) == nullptr) || (this->m_pV3ObjPos == nullptr)) {
        this->SetDMIX_Input(3, 0);
        this->SetDMIX_Input(1, -1);
        this->SetDMIX_Input(2, 0);
        this->SetDMIX_Input(0, -1);
        return;
    }

    this->m_v2ObjPosCopy.x = this->m_pV3ObjPos->x;
    this->m_v2ObjPosCopy.y = this->m_pV3ObjPos->y;

    switch (POSMIXTYPE) {
        case SINGLE_PLAYER: {

            this->m_pv2AzimRefPos = SndCamera::GetWorldCamPos(this->m_PlayerRef);
            this->m_pv2AzimRefDir = SndCamera::GetAvgCamDir(this->m_PlayerRef);
            this->SetCameraAngle();
            this->SetDMIX_Input(3, static_cast<unsigned int>(this->m_CameraAngle));

            float fDistToObj = bDistBetween(&this->m_v2ObjPosCopy, this->m_pv2AzimRefPos);
            fDistToObj *= 100.0f;
            this->SetDMIX_Input(1, static_cast<int>(fDistToObj));

            this->m_pv2AzimRefPos = SndCamera::GetWorldCarPos(this->m_PlayerRef);
            this->m_pv2AzimRefDir = SndCamera::GetNormCarDir(this->m_PlayerRef);
            this->SetCameraAngle();
            this->SetDMIX_Input(2, static_cast<unsigned int>(this->m_CameraAngle));

            fDistToObj = bDistBetween(&this->m_v2ObjPosCopy, this->m_pv2AzimRefPos);
            fDistToObj *= 100.0f;
            this->SetDMIX_Input(0, static_cast<int>(fDistToObj));
            break;
        }

        case TPMIX_AVE_CAM: {
            this->m_pv2AzimRefPos = SndCamera::GetAveragedCamPos(this->m_PlayerRef);
            this->m_pv2AzimRefDir = SndCamera::GetNormCamDir(this->m_PlayerRef);
            this->SetCameraAngle();
            this->SetDMIX_Input(3, static_cast<unsigned int>(this->m_CameraAngle));

            float fDistToObj = bDistBetween(&this->m_v2ObjPosCopy, this->m_pv2AzimRefPos);
            fDistToObj *= 100.0f;
            this->SetDMIX_Input(1, static_cast<int>(fDistToObj));

            this->m_pv2AzimRefPos = SndCamera::GetWorldCarPos(this->m_PlayerRef);
            this->m_pv2AzimRefDir = SndCamera::GetNormCarDir(this->m_PlayerRef);
            this->SetCameraAngle();
            this->SetDMIX_Input(2, static_cast<unsigned int>(this->m_CameraAngle));

            fDistToObj = bDistBetween(&this->m_v2ObjPosCopy, this->m_pv2AzimRefPos);
            fDistToObj *= 100.0f;
            this->SetDMIX_Input(0, static_cast<int>(fDistToObj));
            break;
        }

        case TPMIX_AVE_CAR: {
            this->m_pv2AzimRefPos = SndCamera::GetCenteredCarPos(this->m_PlayerRef);
            this->m_pv2AzimRefDir = SndCamera::GetNormCamDir(this->m_PlayerRef);
            this->SetCameraAngle();
            this->SetDMIX_Input(2, static_cast<unsigned int>(this->m_CameraAngle));

            float fDistToObj = bDistBetween(&this->m_v2ObjPosCopy, this->m_pv2AzimRefPos);
            fDistToObj *= 100.0f;
            this->SetDMIX_Input(0, static_cast<int>(fDistToObj));
            break;
        }

        case TPMIX_AVE_CAM_CAR: {
            this->m_pv2AzimRefPos = SndCamera::GetCenteredCarPos(this->m_PlayerRef);
            this->m_pv2AzimRefDir = SndCamera::GetNormCamDir(this->m_PlayerRef);
            this->SetCameraAngle();
            this->SetDMIX_Input(2, static_cast<unsigned int>(this->m_CameraAngle));

            float fDistToObj = bDistBetween(&this->m_v2ObjPosCopy, this->m_pv2AzimRefPos);
            fDistToObj *= 100.0f;
            this->SetDMIX_Input(0, static_cast<int>(fDistToObj));
            break;
        }
    }

    if (this->m_pV3ObjDir == nullptr) {
        this->SetDMIX_Input(5, 0);
        this->SetDMIX_Input(6, 0);
        this->SetDMIX_Input(10, 0);
    }
}

void SFXCTL_3DObjPos::Generate3DParams(int nplayer) {
    if (this->m_pV3ObjPos != nullptr) {
        this->m_PlayerRef = nplayer;
        if (g_pEAXSound->GetPlayerMixMode() == EAXS3D_SINGLE_PLAYER_MIX) {
            POSMIXTYPE = SINGLE_PLAYER;
            this->m_PlayerRef = 0;
            this->GenerateSinglePlayerMix();
        } else {
            bVector2 v2test;
            v2test.x = this->m_pV3ObjPos->x;
            v2test.y = this->m_pV3ObjPos->y;

            POSMIXTYPE = TPMIX_AVE_CAM;
            float fdisttoCar0 = bDistBetween(&v2test, SndCamera::GetWorldCarPos(0));
            float fdisttoCar1 = bDistBetween(&v2test, SndCamera::GetWorldCarPos(1));
            if (fdisttoCar0 < fdisttoCar1) {
                this->m_PlayerRef = 0;
            } else {
                this->m_PlayerRef = 1;
            }
            this->GenerateSinglePlayerMix();
        }
    }
}

void SFXCTL_3DObjPos::AssignPositionVector(bVector3 *pV3ObjPos) {
    this->m_pV3ObjPos = pV3ObjPos;
}

void SFXCTL_3DObjPos::AssignDirectionVector(const bVector3 *pV3ObjDir) {
    this->m_pV3ObjDir = const_cast<bVector3 *>(pV3ObjDir);
}

void SFXCTL_3DObjPos::AssignVelocityVector(const bVector3 *pV3ObjVel) {
    this->m_pV3ObjVel = const_cast<bVector3 *>(pV3ObjVel);
}

void SFXCTL_3DObjPos::Detach() {
    this->m_pV3ObjPos = nullptr;
    this->m_pV3ObjDir = nullptr;
}

void SFXCTL_3DObjPos::UpdateParams(float t) {
    if (this->m_pV3ObjPos == nullptr) {
        this->SetDMIX_Input(DMX_AZIM, 0);
        this->SetDMIX_Input(DMX_PITCH, -1);
        this->SetDMIX_Input(DMX_FREQ, 0);
        this->SetDMIX_Input(DMX_VOL, -1);
        int nvar = this->GetDMIX_InputValue(15) & ~1;
        this->SetDMIX_Input(15, nvar);
        return;
    }
    int nvar = this->GetDMIX_InputValue(15) | 1;
    this->SetDMIX_Input(15, nvar);
    this->SetDMIX_Input(11, this->m_PlayerRef);
    SFXCTL::UpdateParams(t);
    this->Generate3DParams(0);
    this->UpdateDoppler(this->m_PlayerRef, t);
}

static const float SPEED_OF_SOUND = 343.0f; // size: 0x4, Decl: 1073

void SFXCTL_3DObjPos::UpdateDoppler(int PlayerNum, float t) {
    if (g_EAXIsPaused() || (this->m_pV3ObjVel == nullptr)) {
        return;
    }

    bVector3 vel_to_car = *this->m_pV3ObjVel - *SndCamera::GetV3WorldCarVel(this->m_PlayerRef);
    bVector3 vel_to_cam = *this->m_pV3ObjVel - *SndCamera::GetWorldCamVel(this->m_PlayerRef);

    this->m_fDistToRef[0][1] = this->m_fDistToRef[0][0];
    t = bDistBetween(SndCamera::GetV3WorldCarPos(this->m_PlayerRef), this->m_pV3ObjPos);
    this->m_fDistToRef[1][1] = this->m_fDistToRef[1][0];
    this->m_fDistToRef[0][0] = t;
    this->m_fDistToRef[1][0] = bDistBetween(SndCamera::GetCamPos(this->m_PlayerRef), this->m_pV3ObjPos);

    this->m_fdvelmag_car[1] = this->m_fdvelmag_car[0];
    this->m_fdvelmag_car[0] = bLength(&vel_to_car);
    this->m_fdvelmag_cam[1] = this->m_fdvelmag_cam[0];
    this->m_fdvelmag_cam[0] = bLength(&vel_to_cam);

    if (this->m_fDistToRef[0][1] > this->m_fDistToRef[0][0]) {
        this->m_fdvelmag_car[0] = -this->m_fdvelmag_car[0];
    }

    if (this->m_fDistToRef[1][1] > this->m_fDistToRef[1][0]) {
        this->m_fdvelmag_cam[0] = -this->m_fdvelmag_cam[0];
    }

    if (((this->m_fdvelmag_car[1] < 0.0f) && (0.0f < this->m_fdvelmag_car[0])) ||
        ((0.0f < this->m_fdvelmag_car[1]) && (this->m_fdvelmag_car[0] < 0.0f))) {
        int nvar = this->GetDMIX_InputValue(15) | 0x80000000;
        this->SetDMIX_Input(15, nvar);
    }

    if (((this->m_fdvelmag_cam[1] < 0.0f) && (0.0f < this->m_fdvelmag_cam[0])) ||
        ((0.0f < this->m_fdvelmag_cam[1]) && (this->m_fdvelmag_cam[0] < 0.0f))) {
        int nvar = this->GetDMIX_InputValue(15) | 0x40000000;
        this->SetDMIX_Input(15, nvar);
    }

    this->SetDMIX_Input(13, static_cast<int>(this->m_fdvelmag_car[0] * 100.0f));
    this->SetDMIX_Input(14, static_cast<int>(this->m_fdvelmag_cam[0] * 100.0f));
}
