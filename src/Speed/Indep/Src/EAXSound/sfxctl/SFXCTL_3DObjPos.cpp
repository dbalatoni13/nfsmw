#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern bool g_EAXIsPaused(void);

enum ePOSMIXTYPE {
    SINGLE_PLAYER = 0,
    TPMIX_AVE_CAM = 1,
    TPMIX_AVE_CAR = 2,
    TPMIX_AVE_CAM_CAR = 3,
};

extern int POSMIXTYPE;

inline float bDistBetween(const bVector2 *v1, const bVector2 *v2) {
    float y;
    float x;

    y = v2->y - v1->y;
    x = v2->x - v1->x;
    return bSqrt(x * x + y * y);
}

bVector2 SFXCTL_3DObjPos::m_v2ObjPosCopy;
bVector2 *SFXCTL_3DObjPos::m_pv2AzimRefDir = nullptr;
bVector2 *SFXCTL_3DObjPos::m_pv2AzimRefPos = nullptr;
unsigned short SFXCTL_3DObjPos::m_CameraAngle = 0;

SndBase::TypeInfo *SFXCTL_3DObjPos::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_3DObjPos::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_3DObjPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_3DObjPos::GetStaticTypeInfo()->typeName, false) SFXCTL_3DObjPos();
    }
    return new (SFXCTL_3DObjPos::GetStaticTypeInfo()->typeName, true) SFXCTL_3DObjPos();
}

SFXCTL_3DObjPos::SFXCTL_3DObjPos()
    : m_pV3ObjPos(nullptr) //
    , m_pV3ObjDir(nullptr) //
    , m_pV3ObjVel(nullptr) //
    , m_bIsInTwoPlayerTransitionZone(false) //
    , m_bDirectionClockwise(false) //
    , m_bIsOtherCamRightSide(false) {
    this->SetPlayerRef(0);
    this->m_fDistToRef[0][0] = 0.0f;
    this->m_fDistToRef[0][1] = 0.0f;
    this->m_fDistToRef[1][0] = 0.0f;
    this->m_fDistToRef[1][1] = 0.0f;
}

SFXCTL_3DObjPos::~SFXCTL_3DObjPos() {}

void SFXCTL_3DObjPos::SetPlayerRef(int i) { this->m_PlayerRef = i; }

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

void SFXCTL_3DObjPos::GenerateSinglePlayerMix() {
    if (!SndCamera::GetCam(this->m_PlayerRef) || !this->m_pV3ObjPos) {
        this->SetDMIX_Input(3, 0);
        this->SetDMIX_Input(1, -1);
        this->SetDMIX_Input(2, 0);
        this->SetDMIX_Input(0, -1);
        return;
    }

    bVector2 *objPosCopy = &m_v2ObjPosCopy;
    objPosCopy->x = this->m_pV3ObjPos->x;
    objPosCopy->y = this->m_pV3ObjPos->y;

    float outVol;

    if (POSMIXTYPE == TPMIX_AVE_CAM) {
        goto ave_cam_mix;
    }

    if (POSMIXTYPE <= TPMIX_AVE_CAM) {
        if (POSMIXTYPE == SINGLE_PLAYER) {
            goto single_player_mix;
        }
        goto done_mix;
    }

    if (POSMIXTYPE == TPMIX_AVE_CAR) {
        goto centered_car_mix;
    }
    if (POSMIXTYPE == TPMIX_AVE_CAM_CAR) {
        goto centered_car_cam_mix;
    }
    goto done_mix;

single_player_mix:
    {
        m_pv2AzimRefPos = SndCamera::GetWorldCamPos(this->m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetAvgCamDir(this->m_PlayerRef);
        this->SetCameraAngle();
        this->SetDMIX_Input(3, static_cast<unsigned int>(m_CameraAngle));

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        float distSqr = dx * dx + dy * dy;
        float fDistToObj = bSqrt(distSqr);
        this->SetDMIX_Input(1, static_cast<int>(fDistToObj * 100.0f));

        m_pv2AzimRefPos = SndCamera::GetWorldCarPos(this->m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCarDir(this->m_PlayerRef);
        this->SetCameraAngle();
        this->SetDMIX_Input(2, static_cast<unsigned int>(m_CameraAngle));

        dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        distSqr = dx * dx + dy * dy;
        fDistToObj = bSqrt(distSqr);
        outVol = fDistToObj * 100.0f;
        goto store_mix;
    }

ave_cam_mix:
    {
        m_pv2AzimRefPos = SndCamera::GetAveragedCamPos(this->m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCamDir(this->m_PlayerRef);
        this->SetCameraAngle();
        this->SetDMIX_Input(3, static_cast<unsigned int>(m_CameraAngle));

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        float distSqr = dx * dx + dy * dy;
        float fDistToObj = bSqrt(distSqr);
        this->SetDMIX_Input(1, static_cast<int>(fDistToObj * 100.0f));

        m_pv2AzimRefPos = SndCamera::GetWorldCarPos(this->m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCarDir(this->m_PlayerRef);
        this->SetCameraAngle();
        this->SetDMIX_Input(2, static_cast<unsigned int>(m_CameraAngle));

        dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        distSqr = dx * dx + dy * dy;
        fDistToObj = bSqrt(distSqr);
        outVol = fDistToObj * 100.0f;
        goto store_mix;
    }

centered_car_mix:
    {
        m_pv2AzimRefPos = SndCamera::GetCenteredCarPos(this->m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCamDir(this->m_PlayerRef);
        this->SetCameraAngle();
        this->SetDMIX_Input(2, static_cast<unsigned int>(m_CameraAngle));

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        float distSqr = dx * dx + dy * dy;
        float fDistToObj = bSqrt(distSqr);
        outVol = fDistToObj * 100.0f;
        goto store_mix;
    }

centered_car_cam_mix:
    {
        m_pv2AzimRefPos = SndCamera::GetCenteredCarPos(this->m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCamDir(this->m_PlayerRef);
        this->SetCameraAngle();
        this->SetDMIX_Input(2, static_cast<unsigned int>(m_CameraAngle));

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        float distSqr = dx * dx + dy * dy;
        float fDistToObj = bSqrt(distSqr);
        outVol = fDistToObj * 100.0f;
        goto store_mix;
    }

store_mix:
    this->SetDMIX_Input(0, static_cast<int>(outVol));

done_mix:
    if (!this->m_pV3ObjDir) {
        this->SetDMIX_Input(5, 0);
        this->SetDMIX_Input(6, 0);
        this->SetDMIX_Input(10, 0);
    }
}

void SFXCTL_3DObjPos::Generate3DParams(int nplayer) {
    if (this->m_pV3ObjPos) {
        this->m_PlayerRef = nplayer;
        if (g_pEAXSound->GetPlayerMixMode() == EAXS3D_SINGLE_PLAYER_MIX) {
            POSMIXTYPE = SINGLE_PLAYER;
            this->m_PlayerRef = 0;
            this->GenerateSinglePlayerMix();
        } else {
            bVector2 v2test;
            float fdisttoCar0;
            float fdisttoCar1;

            v2test.x = this->m_pV3ObjPos->x;
            v2test.y = this->m_pV3ObjPos->y;

            POSMIXTYPE = TPMIX_AVE_CAM;
            fdisttoCar0 = bDistBetween(SndCamera::GetWorldCarPos(0), &v2test);
            fdisttoCar1 = bDistBetween(SndCamera::GetWorldCarPos(1), &v2test);
            if (fdisttoCar0 < fdisttoCar1) {
                this->m_PlayerRef = 0;
            } else {
                this->m_PlayerRef = 1;
            }
            this->GenerateSinglePlayerMix();
        }
    }
}

void SFXCTL_3DObjPos::AssignPositionVector(bVector3 *pV3ObjPos) { this->m_pV3ObjPos = pV3ObjPos; }

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
    if (!this->m_pV3ObjPos) {
        this->SetDMIX_Input(DMX_AZIM, 0);
        this->SetDMIX_Input(DMX_PITCH, -1);
        this->SetDMIX_Input(DMX_FREQ, 0);
        this->SetDMIX_Input(DMX_VOL, -1);
        int nvar = this->GetDMIX_InputValue(15);
        this->SetDMIX_Input(15, nvar & ~1);
        return;
    }
    int nvar = this->GetDMIX_InputValue(15);
    this->SetDMIX_Input(15, nvar | 1);
    int playerRef = this->m_PlayerRef;
    this->SetDMIX_Input(11, playerRef);
    SFXCTL::UpdateParams(t);
    this->Generate3DParams(0);
    this->UpdateDoppler(this->m_PlayerRef, t);
}

void SFXCTL_3DObjPos::UpdateDoppler(int PlayerNum, float t) {
    if (g_EAXIsPaused() || !this->m_pV3ObjVel) {
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
        register int nvar = this->GetDMIX_InputValue(15);
        this->SetDMIX_Input(15, nvar | 0x80000000);
    }

    if (((this->m_fdvelmag_cam[1] < 0.0f) && (0.0f < this->m_fdvelmag_cam[0])) ||
        ((0.0f < this->m_fdvelmag_cam[1]) && (this->m_fdvelmag_cam[0] < 0.0f))) {
        int nvar = this->GetDMIX_InputValue(15);
        this->SetDMIX_Input(15, nvar | 0x40000000);
    }

    this->SetDMIX_Input(13, static_cast<int>(this->m_fdvelmag_car[0] * 100.0f));
    this->SetDMIX_Input(14, static_cast<int>(this->m_fdvelmag_cam[0] * 100.0f));
}
