#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Woosh.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"

SFXObj_Woosh::TypeInfo SFXObj_Woosh::s_TypeInfo = {0x00080000, "SFXObj_Woosh", &SndBase::s_TypeInfo, SFXObj_Woosh::CreateObject};
SFXCTL_3DWooshPos::TypeInfo SFXCTL_3DWooshPos::s_TypeInfo = {
    0x00080000, "SFXCTL_3DWooshPos", &SFXCTL_3DObjPos::s_TypeInfo, SFXCTL_3DWooshPos::CreateObject};

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
    return new (GetStaticTypeInfo()->typeName, allocator != 0) SFXObj_Woosh();
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
    return new (GetStaticTypeInfo()->typeName, allocator != 0) SFXCTL_3DWooshPos();
}
