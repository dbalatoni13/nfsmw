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

inline e3DPlayerMix EAXSound::GetPlayerMixMode() {
    return m_ePlayerMixMode;
}

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

SndBase *SFXCTL_3DObjPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_3DObjPos::s_TypeInfo.typeName, false) SFXCTL_3DObjPos();
    }
    return new (SFXCTL_3DObjPos::s_TypeInfo.typeName, true) SFXCTL_3DObjPos();
}

SFXCTL_3DObjPos::~SFXCTL_3DObjPos() {}

SFXCTL_3DObjPos::SFXCTL_3DObjPos()
    : m_pV3ObjPos(nullptr) //
    , m_pV3ObjDir(nullptr) //
    , m_pV3ObjVel(nullptr) //
    , m_bIsInTwoPlayerTransitionZone(false) //
    , m_bDirectionClockwise(false) //
    , m_bIsOtherCamRightSide(false) {
    SetPlayerRef(0);
    m_fDistToRef[0][0] = 0.0f;
    m_fDistToRef[0][1] = 0.0f;
    m_fDistToRef[1][0] = 0.0f;
    m_fDistToRef[1][1] = 0.0f;
}

SndBase::TypeInfo *SFXCTL_3DObjPos::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_3DObjPos::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_3DObjPos::SetPlayerRef(int i) { m_PlayerRef = i; }

void SFXCTL_3DObjPos::AssignPositionVector(bVector3 *pV3ObjPos) { m_pV3ObjPos = pV3ObjPos; }

void SFXCTL_3DObjPos::AssignDirectionVector(const bVector3 *pV3ObjDir) {
    m_pV3ObjDir = const_cast<bVector3 *>(pV3ObjDir);
}

void SFXCTL_3DObjPos::AssignVelocityVector(const bVector3 *pV3ObjVel) {
    m_pV3ObjVel = const_cast<bVector3 *>(pV3ObjVel);
}

void SFXCTL_3DObjPos::Detach() {
    m_pV3ObjPos = nullptr;
    m_pV3ObjDir = nullptr;
}

void SFXCTL_3DObjPos::SetCameraAngle() {
    bVector2 v2Pos;
    bVector2 v2NormPos;
    float fdotobj;

    v2Pos.x = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
    v2Pos.y = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
    v2NormPos.x = 0.0f;
    v2NormPos.y = 0.0f;
    bNormalize(&v2NormPos, &v2Pos);

    fdotobj = bDot(&v2NormPos, m_pv2AzimRefDir);
    m_CameraAngle = bACos(fdotobj);

    if (-v2NormPos.y * m_pv2AzimRefDir->x + v2NormPos.x * m_pv2AzimRefDir->y < 0.0f) {
        m_CameraAngle = static_cast<unsigned short>(~m_CameraAngle);
    }
}

void SFXCTL_3DObjPos::Generate3DParams(int nplayer) {
    if (m_pV3ObjPos != nullptr) {
        m_PlayerRef = nplayer;
        if (g_pEAXSound->GetPlayerMixMode() == EAXS3D_SINGLE_PLAYER_MIX) {
            POSMIXTYPE = SINGLE_PLAYER;
            m_PlayerRef = 0;
            GenerateSinglePlayerMix();
        } else {
            bVector2 v2test;
            float fdisttoCar0;
            float fdisttoCar1;

            v2test.x = m_pV3ObjPos->x;
            v2test.y = m_pV3ObjPos->y;

            POSMIXTYPE = TPMIX_AVE_CAM;
            fdisttoCar0 = bDistBetween(SndCamera::GetWorldCarPos(0), &v2test);
            fdisttoCar1 = bDistBetween(SndCamera::GetWorldCarPos(1), &v2test);
            if (fdisttoCar0 < fdisttoCar1) {
                m_PlayerRef = 0;
            } else {
                m_PlayerRef = 1;
            }
            GenerateSinglePlayerMix();
        }
    }
}

void SFXCTL_3DObjPos::GenerateSinglePlayerMix() {
    if (SndCamera::GetCam(m_PlayerRef) == nullptr || m_pV3ObjPos == nullptr) {
        SetDMIX_Input(3, 0);
        SetDMIX_Input(1, -1);
        SetDMIX_Input(2, 0);
        SetDMIX_Input(0, -1);
        return;
    }

    bVector2 *objPosCopy = &m_v2ObjPosCopy;
    objPosCopy->x = m_pV3ObjPos->x;
    objPosCopy->y = m_pV3ObjPos->y;

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
        m_pv2AzimRefPos = SndCamera::GetWorldCamPos(m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetAvgCamDir(m_PlayerRef);
        SetCameraAngle();
        SetDMIX_Input(3, static_cast<unsigned int>(m_CameraAngle));

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        float distSqr = dx * dx + dy * dy;
        float fDistToObj = bSqrt(distSqr);
        SetDMIX_Input(1, static_cast<int>(fDistToObj * 100.0f));

        m_pv2AzimRefPos = SndCamera::GetWorldCarPos(m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCarDir(m_PlayerRef);
        SetCameraAngle();
        SetDMIX_Input(2, static_cast<unsigned int>(m_CameraAngle));

        dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        distSqr = dx * dx + dy * dy;
        fDistToObj = bSqrt(distSqr);
        outVol = fDistToObj * 100.0f;
        goto store_mix;
    }

ave_cam_mix:
    {
        m_pv2AzimRefPos = SndCamera::GetAveragedCamPos(m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCamDir(m_PlayerRef);
        SetCameraAngle();
        SetDMIX_Input(3, static_cast<unsigned int>(m_CameraAngle));

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        float distSqr = dx * dx + dy * dy;
        float fDistToObj = bSqrt(distSqr);
        SetDMIX_Input(1, static_cast<int>(fDistToObj * 100.0f));

        m_pv2AzimRefPos = SndCamera::GetWorldCarPos(m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCarDir(m_PlayerRef);
        SetCameraAngle();
        SetDMIX_Input(2, static_cast<unsigned int>(m_CameraAngle));

        dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        distSqr = dx * dx + dy * dy;
        fDistToObj = bSqrt(distSqr);
        outVol = fDistToObj * 100.0f;
        goto store_mix;
    }

centered_car_mix:
    {
        m_pv2AzimRefPos = SndCamera::GetCenteredCarPos(m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCamDir(m_PlayerRef);
        SetCameraAngle();
        SetDMIX_Input(2, static_cast<unsigned int>(m_CameraAngle));

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        float distSqr = dx * dx + dy * dy;
        float fDistToObj = bSqrt(distSqr);
        outVol = fDistToObj * 100.0f;
        goto store_mix;
    }

centered_car_cam_mix:
    {
        m_pv2AzimRefPos = SndCamera::GetCenteredCarPos(m_PlayerRef);
        m_pv2AzimRefDir = SndCamera::GetNormCamDir(m_PlayerRef);
        SetCameraAngle();
        SetDMIX_Input(2, static_cast<unsigned int>(m_CameraAngle));

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        float distSqr = dx * dx + dy * dy;
        float fDistToObj = bSqrt(distSqr);
        outVol = fDistToObj * 100.0f;
        goto store_mix;
    }

store_mix:
    SetDMIX_Input(0, static_cast<int>(outVol));

done_mix:
    if (m_pV3ObjDir == nullptr) {
        SetDMIX_Input(5, 0);
        SetDMIX_Input(6, 0);
        SetDMIX_Input(10, 0);
    }
}

void SFXCTL_3DObjPos::UpdateDoppler(int PlayerNum, float t) {
    if (g_EAXIsPaused() || m_pV3ObjVel == nullptr) {
        return;
    }

    bVector3 vel_to_car = *m_pV3ObjVel - *SndCamera::GetV3WorldCarVel(m_PlayerRef);
    bVector3 vel_to_cam = *m_pV3ObjVel - *SndCamera::GetWorldCamVel(m_PlayerRef);

    m_fDistToRef[0][1] = m_fDistToRef[0][0];
    t = bDistBetween(SndCamera::GetV3WorldCarPos(m_PlayerRef), m_pV3ObjPos);
    m_fDistToRef[1][1] = m_fDistToRef[1][0];
    m_fDistToRef[0][0] = t;
    m_fDistToRef[1][0] = bDistBetween(SndCamera::GetCamPos(m_PlayerRef), m_pV3ObjPos);

    m_fdvelmag_car[1] = m_fdvelmag_car[0];
    m_fdvelmag_car[0] = bLength(&vel_to_car);
    m_fdvelmag_cam[1] = m_fdvelmag_cam[0];
    m_fdvelmag_cam[0] = bLength(&vel_to_cam);

    if (m_fDistToRef[0][1] > m_fDistToRef[0][0]) {
        m_fdvelmag_car[0] = -m_fdvelmag_car[0];
    }

    if (m_fDistToRef[1][1] > m_fDistToRef[1][0]) {
        m_fdvelmag_cam[0] = -m_fdvelmag_cam[0];
    }

    if (((m_fdvelmag_car[1] < 0.0f) && (0.0f < m_fdvelmag_car[0])) ||
        ((0.0f < m_fdvelmag_car[1]) && (m_fdvelmag_car[0] < 0.0f))) {
        register int nvar asm("r0") = GetDMIX_InputValue(15);
        SetDMIX_Input(15, nvar | 0x80000000);
    }

    if (((m_fdvelmag_cam[1] < 0.0f) && (0.0f < m_fdvelmag_cam[0])) ||
        ((0.0f < m_fdvelmag_cam[1]) && (m_fdvelmag_cam[0] < 0.0f))) {
        register int nvar asm("r0") = GetDMIX_InputValue(15);
        SetDMIX_Input(15, nvar | 0x40000000);
    }

    SetDMIX_Input(13, static_cast<int>(m_fdvelmag_car[0] * 100.0f));
    SetDMIX_Input(14, static_cast<int>(m_fdvelmag_cam[0] * 100.0f));
}

void SFXCTL_3DObjPos::UpdateParams(float t) {
    if (m_pV3ObjPos == nullptr) {
        SetDMIX_Input(DMX_AZIM, 0);
        SetDMIX_Input(DMX_PITCH, -1);
        SetDMIX_Input(DMX_FREQ, 0);
        SetDMIX_Input(DMX_VOL, -1);
        int nvar = GetDMIX_InputValue(15);
        SetDMIX_Input(15, nvar & ~1);
        return;
    }
    int nvar = GetDMIX_InputValue(15);
    SetDMIX_Input(15, nvar | 1);
    SetDMIX_Input(11, m_PlayerRef);
    SFXCTL::UpdateParams(t);
    Generate3DParams(0);
    UpdateDoppler(m_PlayerRef, t);
}
