#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

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
    int *out = GetOutputBlockPtr();
    if (out == nullptr || m_pV3ObjPos == nullptr) {
        return;
    }

    float x = m_pV3ObjPos->x;
    float y = m_pV3ObjPos->y;
    int angle = static_cast<int>(bATan(x, y));
    if (angle < 0) {
        angle += 65536;
    }
    SetDMIX_Input(DMX_AZIM, angle);
}

void SFXCTL_3DObjPos::GenerateSinglePlayerMix() {
    int *out = GetOutputBlockPtr();
    if (out == nullptr || m_pV3ObjPos == nullptr) {
        SetDMIX_Input(DMX_VOL, -1);
        SetDMIX_Input(DMX_PITCH, -1);
        SetDMIX_Input(DMX_FREQ, 0);
        SetDMIX_Input(DMX_AZIM, 0);
        return;
    }

    float dx = m_pV3ObjPos->x;
    float dy = m_pV3ObjPos->y;
    float distSq = dx * dx + dy * dy;
    float distanceScale = 1.0f;
    if (distSq > 0.00001f) {
        distanceScale = bSqrt(distSq);
    }

    int volume = static_cast<int>((1.0f / distanceScale) * 32767.0f);
    if (volume < 0) {
        volume = 0;
    } else if (volume > 32767) {
        volume = 32767;
    }

    SetDMIX_Input(DMX_VOL, volume);
    SetDMIX_Input(DMX_PITCH, static_cast<int>(dx * 4096.0f));
    SetDMIX_Input(DMX_FREQ, static_cast<int>(dy * 4096.0f));
    SetDMIX_Input(DMX_AZIM, m_PlayerRef);

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
