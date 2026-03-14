#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct Camera;

namespace SndCamera {
Camera *GetCam(int);
extern bVector2 m_AvergeCamDir[2];
extern bVector2 m_WorldCamPos[2];
extern bVector2 m_NormCamDir[2];
extern bVector2 m_AveragedCamPos[2];
extern bVector2 m_NormCarDir[2];
extern bVector2 m_WorldCarPos[2];
extern bVector2 m_CenteredCarPos[2];
} // namespace SndCamera

enum ePOSMIXTYPE {
    SINGLE_PLAYER = 0,
    TPMIX_AVE_CAM = 1,
    TPMIX_AVE_CAR = 2,
    TPMIX_AVE_CAM_CAR = 3,
};

extern int POSMIXTYPE;

namespace {
bVector2 m_v2ObjPosCopy;
bVector2 *m_pv2AzimRefPos;
bVector2 *m_pv2AzimRefDir;
unsigned short m_CameraAngle;
} // namespace

SndBase *SFXCTL_3DObjPos::CreateObject(unsigned int) { return new SFXCTL_3DObjPos(); }

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

    v2Pos.x = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
    v2Pos.y = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
    v2NormPos.x = 0.0f;
    v2NormPos.y = 0.0f;

    bNormalize(&v2NormPos, &v2Pos);

    const unsigned short angle =
        bASin(v2NormPos.x * m_pv2AzimRefDir->x + v2NormPos.y * m_pv2AzimRefDir->y);
    m_CameraAngle = 0x4000 - angle;

    if (-v2NormPos.y * m_pv2AzimRefDir->x + v2NormPos.x * m_pv2AzimRefDir->y < 0.0f) {
        m_CameraAngle = static_cast<unsigned short>(~m_CameraAngle);
    }
}

void SFXCTL_3DObjPos::GenerateSinglePlayerMix() {
    Camera *cam = SndCamera::GetCam(m_PlayerRef);
    int *out = GetOutputBlockPtr();

    if (cam == nullptr || m_pV3ObjPos == nullptr) {
        out[0] = -1;
        out[3] = 0;
        out[1] = -1;
        out[2] = 0;
        return;
    }

    m_v2ObjPosCopy.x = m_pV3ObjPos->x;
    m_v2ObjPosCopy.y = m_pV3ObjPos->y;

    float outVol = 0.0f;

    if (POSMIXTYPE == TPMIX_AVE_CAM) {
        m_pv2AzimRefDir = SndCamera::m_NormCamDir + m_PlayerRef;
        m_pv2AzimRefPos = SndCamera::m_AveragedCamPos + m_PlayerRef;
        SetCameraAngle();
        out[3] = static_cast<unsigned int>(m_CameraAngle);

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        out[1] = static_cast<int>(bSqrt(dx * dx + dy * dy) * 100.0f);

        m_pv2AzimRefDir = SndCamera::m_NormCarDir + m_PlayerRef;
        m_pv2AzimRefPos = SndCamera::m_WorldCarPos + m_PlayerRef;
        SetCameraAngle();
        out[2] = static_cast<unsigned int>(m_CameraAngle);

        dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        outVol = bSqrt(dx * dx + dy * dy) * 100.0f;
    } else if (POSMIXTYPE < 2) {
        if (POSMIXTYPE != SINGLE_PLAYER) {
            goto done_mix;
        }

        m_pv2AzimRefDir = SndCamera::m_AvergeCamDir + m_PlayerRef;
        m_pv2AzimRefPos = SndCamera::m_WorldCamPos + m_PlayerRef;
        SetCameraAngle();
        out[3] = static_cast<unsigned int>(m_CameraAngle);

        float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        out[1] = static_cast<int>(bSqrt(dx * dx + dy * dy) * 100.0f);

        m_pv2AzimRefDir = SndCamera::m_NormCarDir + m_PlayerRef;
        m_pv2AzimRefPos = SndCamera::m_WorldCarPos + m_PlayerRef;
        SetCameraAngle();
        out[2] = static_cast<unsigned int>(m_CameraAngle);

        dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        outVol = bSqrt(dx * dx + dy * dy) * 100.0f;
    } else {
        if (POSMIXTYPE != TPMIX_AVE_CAR && POSMIXTYPE != TPMIX_AVE_CAM_CAR) {
            goto done_mix;
        }

        m_pv2AzimRefDir = SndCamera::m_NormCamDir + m_PlayerRef;
        m_pv2AzimRefPos = SndCamera::m_CenteredCarPos + m_PlayerRef;
        SetCameraAngle();
        out[2] = static_cast<unsigned int>(m_CameraAngle);

        const float dx = m_v2ObjPosCopy.x - m_pv2AzimRefPos->x;
        const float dy = m_v2ObjPosCopy.y - m_pv2AzimRefPos->y;
        outVol = bSqrt(dx * dx + dy * dy) * 100.0f;
    }

    out[0] = static_cast<int>(outVol);

done_mix:
    if (m_pV3ObjDir == nullptr) {
        out[5] = 0;
        out[6] = 0;
        out[10] = 0;
    }
}

void SFXCTL_3DObjPos::UpdateDoppler(int outIndex, float t) {
    if (m_pV3ObjVel == nullptr) {
        SetDMIX_Input(outIndex, 0);
        return;
    }

    float velMag = bSqrt(m_pV3ObjVel->x * m_pV3ObjVel->x + m_pV3ObjVel->y * m_pV3ObjVel->y +
                         m_pV3ObjVel->z * m_pV3ObjVel->z);
    if (m_PlayerRef < 0) {
        m_PlayerRef = 0;
    } else if (m_PlayerRef > 1) {
        m_PlayerRef = 1;
    }

    float oldMag = m_fdvelmag_car[m_PlayerRef];
    m_fdvelmag_car[m_PlayerRef] = oldMag + (velMag - oldMag) * t;
    SetDMIX_Input(outIndex, static_cast<int>(m_fdvelmag_car[m_PlayerRef] * 4096.0f));
}

void SFXCTL_3DObjPos::UpdateParams(float t) {
    GenerateSinglePlayerMix();
    UpdateDoppler(DMX_DEPTH, t);
}
