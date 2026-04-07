#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Woosh.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"

SFXObj_Woosh::TypeInfo SFXObj_Woosh::s_TypeInfo = {0x00080000, "SFXObj_Woosh", &SndBase::s_TypeInfo, SFXObj_Woosh::CreateObject};
SFXCTL_3DWooshPos::TypeInfo SFXCTL_3DWooshPos::s_TypeInfo = {
    0x00080000, "SFXCTL_3DWooshPos", &SFXCTL_3DObjPos::s_TypeInfo, SFXCTL_3DWooshPos::CreateObject};

extern Slope g_WooshVol_vs_Vel;

void GetWooshBlockSizeParams(eDRIVE_BY_TYPE type, STICH_WHOOSH_TYPE &base, int &numblocks, int &sizeperblock) {
    switch (type) {
    case DRIVE_BY_TREE:
        base = WHSH_Tree_Slow_01;
        numblocks = 3;
        sizeperblock = 6;
        return;
    case DRIVE_BY_LAMPPOST:
        base = WHSH_Post_Slow_01;
        numblocks = 3;
        sizeperblock = 6;
        return;
    case DRIVE_BY_SMOKABLE:
        base = WHSH_Smack_Med_01;
        numblocks = 2;
        sizeperblock = 6;
        return;
    case DRIVE_BY_TUNNEL_IN:
        base = WHSH_Tunnel_Slow_01;
        numblocks = 3;
        sizeperblock = 3;
        return;
    case DRIVE_BY_TUNNEL_OUT:
    case DRIVE_BY_OVERPASS_OUT:
        base = WHSH_TunOut_Med_01;
        numblocks = 2;
        sizeperblock = 3;
        return;
    case DRIVE_BY_AI_CAR:
    case DRIVE_BY_TRAFFIC:
        base = WHSH_Traf_Med_01;
        numblocks = 2;
        sizeperblock = 6;
        return;
    case DRIVE_BY_PRE_COL:
        base = WHSH_Pre_Med_01;
        numblocks = 2;
        sizeperblock = 3;
        return;
    case DRIVE_BY_CAMERA_BY:
        base = WHSH_Post_Fast_01;
        numblocks = 1;
        sizeperblock = 6;
        return;
    case DRIVE_BY_UNKNOWN:
    case DRIVE_BY_OVERPASS_IN:
    case DRIVE_BY_BRIDGE:
    default:
        base = WHSH_Bridge_Slow_01;
        numblocks = 3;
        sizeperblock = 3;
        return;
    }
}

SFXObj_Woosh::TypeInfo *SFXObj_Woosh::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_Woosh::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_Woosh::CreateObject(unsigned int allocator) {
    if (!allocator) {
        return new (SFXObj_Woosh::GetStaticTypeInfo()->typeName, false) SFXObj_Woosh();
    }

    return new (SFXObj_Woosh::GetStaticTypeInfo()->typeName, true) SFXObj_Woosh();
}

SFXObj_Woosh::SFXObj_Woosh()
    : CARSFX() {
    m_SndParams.Vol = 0x7FFF;
    m_SndParams.Pitch = 0x1000;
    m_pWooshStich = nullptr;
    m_SndParams.ID = 0;
    m_SndParams.Az = 0;
    m_SndParams.Mag = 0;
    m_SndParams.RVerb = 0;
    m_pDriveByState = nullptr;
    m_pStitchData = nullptr;
}

SFXObj_Woosh::~SFXObj_Woosh() {
    Detach();
}

void SFXObj_Woosh::Detach() {
    if (m_pWooshStich) {
        delete m_pWooshStich;
    }

    m_pWooshStich = nullptr;
}

void SFXObj_Woosh::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pDriveByState = static_cast<CSTATE_DriveBy *>(m_pStateBase);
}

void SFXObj_Woosh::InitSFX() {
    float fVelRatio;
    float fVelInensity;
    int StitchID;
    int numblocks;
    int sizeperblock;
    STICH_WHOOSH_TYPE base;

    SndBase::InitSFX();
    m_p3DPos->AssignPositionVector(&m_pDriveByState->m_DriveByInfo.vLocation);
    m_p3DPos->AssignVelocityVector(0);
    fVelInensity = bClamp(g_WooshVol_vs_Vel.GetValue(m_pDriveByState->m_DriveByInfo.ClosingVelocity), 0.0f, 0.99f);
    fVelInensity = bClamp(fVelInensity * 127.0f, 0.0f, 127.0f);
    if (g_pEAXSound->GetSndGameMode() == SND_PURSUITBREAKER) {
        fVelInensity = 0.0f;
    }

    GetWooshBlockSizeParams(m_pDriveByState->m_DriveByInfo.eDriveByType, base, numblocks, sizeperblock);
    switch (m_pDriveByState->m_DriveByInfo.eDriveByType) {
    case DRIVE_BY_TUNNEL_IN:
    case DRIVE_BY_OVERPASS_IN:
        {
            static int LastRandom;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
    case DRIVE_BY_TUNNEL_OUT:
    case DRIVE_BY_OVERPASS_OUT:
        {
            static int LastRandom;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
    case DRIVE_BY_LAMPPOST:
        {
            static int LastRandom;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
    case DRIVE_BY_AI_CAR:
        {
            static int LastRandom;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
    case DRIVE_BY_SMOKABLE:
        {
            static int LastRandom;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
    case DRIVE_BY_TRAFFIC:
        {
            static int LastRandom;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
        break;
    case DRIVE_BY_CAMERA_BY:
        SetDMIX_Input(13, 0x7FFF);
        {
            static int LastRandom;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
    default:
        break;
    }

    fVelRatio = static_cast<float>(numblocks) * 0.0078125f;
    StitchID = static_cast<int>(fVelInensity * fVelRatio);
    {
        static int LastRandom;

        StitchID = base + StitchID * sizeperblock + (LastRandom - (LastRandom / sizeperblock) * sizeperblock);
        LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
    }

    m_pStitchData = &g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_WOOSH, StitchID);
    m_SndParams.Vol = 0;
    m_SndParams.Az = 0;
    m_SndParams.Pitch = 0;
    m_pWooshStich = new cStichWrapper(*m_pStitchData);
    m_pWooshStich->Play(&m_SndParams);
    SetDMIX_Input(11, 0x7FFF);
    SetDMIX_Input(12, bClamp(static_cast<int>(fVelInensity) << 8, 0, 0x7FFF) >> 15);
}

void SFXObj_Woosh::UpdateParams(float t) {
    if (m_pDriveByState->IsAttached() && m_pWooshStich && !m_pWooshStich->IsPlaying()) {
        delete m_pWooshStich;
        m_pWooshStich = nullptr;
        m_pDriveByState->Detach();
    }
}

void SFXObj_Woosh::ProcessUpdate() {
    int DMixSlot;
    int pitch;

    GetOutputBlockPtr()[11] = 0;
    GetOutputBlockPtr()[13] = 0;
    if (m_pDriveByState->IsAttached() && m_pWooshStich) {
        pitch = GetDMixOutput(14, DMX_PITCH);
        switch (m_pDriveByState->m_DriveByInfo.eDriveByType) {
        case DRIVE_BY_LAMPPOST:
            DMixSlot = 2;
            break;
        case DRIVE_BY_SMOKABLE:
            DMixSlot = 8;
            break;
        case DRIVE_BY_TUNNEL_IN:
            DMixSlot = 4;
            break;
        case DRIVE_BY_TUNNEL_OUT:
            DMixSlot = 5;
            break;
        case DRIVE_BY_OVERPASS_IN:
            DMixSlot = 6;
            break;
        case DRIVE_BY_OVERPASS_OUT:
            DMixSlot = 7;
            break;
        case DRIVE_BY_AI_CAR:
            DMixSlot = 3;
            break;
        case DRIVE_BY_BRIDGE:
            DMixSlot = 9;
            break;
        case DRIVE_BY_CAMERA_BY:
            DMixSlot = 10;
            break;
        default:
            DMixSlot = 1;
            break;
        }

        m_SndParams.Az = GetDMixOutput(0, DMX_AZIM);
        m_SndParams.Vol = GetDMixOutput(DMixSlot, DMX_VOL);
        m_SndParams.Pitch = pitch;
        m_SndParams.RVerb = GetDMixOutput(15, DMX_VOL);
        m_pWooshStich->Update(&m_SndParams);
    }
}

int SFXObj_Woosh::GetController(int Index) {
    return Index == 0 ? 0 : -1;
}

void SFXObj_Woosh::AttachController(SFXCTL *psfxctl) {
    if ((psfxctl->GetTypeInfo()->ObjectID & 0xFFF0) == 0) {
        m_p3DPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
    }
}

void SFXObj_Woosh::Destroy() {
    if (m_pWooshStich) {
        delete m_pWooshStich;
    }

    m_pWooshStich = nullptr;
}

SFXCTL_3DWooshPos::TypeInfo *SFXCTL_3DWooshPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DWooshPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DWooshPos::CreateObject(unsigned int allocator) {
    if (!allocator) {
        return new (SFXCTL_3DWooshPos::GetStaticTypeInfo()->typeName, false) SFXCTL_3DWooshPos();
    }

    return new (SFXCTL_3DWooshPos::GetStaticTypeInfo()->typeName, true) SFXCTL_3DWooshPos();
}
