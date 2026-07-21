#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Woosh.hpp"
#include "SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/STITCH_WSH.h"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"

DEFINE_CREATABLE(0x80000, SFXObj_Woosh, SndBase);

static const float gfLampPostMaxVelocity = 45.0f; // size: 0x4, Decl: 10
static const float gfLampPostMinVelocity = 18.0f; // size: 0x4, Decl: 11
static const int gnLampPostMaxVol = 32767;        // size: 0x4, Decl: 12

Slope g_WooshVol_vs_Vel(0.0f, 1.0f, gfLampPostMinVelocity, gfLampPostMaxVelocity); // Decl: 14

void GetWooshBlockSizeParams(eDRIVE_BY_TYPE type, STICH_WHOOSH_TYPE &base, int &numblocks, int &sizeperblock) {
    switch (type) {
        case DRIVE_BY_OVERPASS_IN:
        case DRIVE_BY_BRIDGE:
        default:
            base = WHSH_Bridge_Slow_01;
            numblocks = 3;
            sizeperblock = 3;
            return;

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

        case DRIVE_BY_CAMERA_BY:
            base = WHSH_Post_Fast_01;
            numblocks = 1;
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
    }
}

SFXObj_Woosh::SFXObj_Woosh() {
    this->m_SndParams.Vol = 0x7FFF;
    this->m_SndParams.Pitch = 0x1000;
    this->m_SndParams.ID = 0;
    this->m_SndParams.Az = 0;
    this->m_SndParams.Mag = 0;
    this->m_SndParams.RVerb = 0;
    this->m_pDriveByState = nullptr;
    this->m_pStitchData = nullptr;
    this->m_pWooshStich = nullptr;
}

SFXObj_Woosh::~SFXObj_Woosh() {
    Detach();
}

void SFXObj_Woosh::Detach() {
    delete this->m_pWooshStich;
    this->m_pWooshStich = nullptr;
}

void SFXObj_Woosh::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_pDriveByState = static_cast<CSTATE_DriveBy *>(this->m_pStateBase);
}

// TODO dwarf variables are wrong
void SFXObj_Woosh::InitSFX() {
    this->SndBase::InitSFX();
    this->m_p3DPos->AssignPositionVector(&this->m_pDriveByState->m_DriveByInfo.vLocation);
    this->m_p3DPos->AssignVelocityVector(nullptr);
    float fVelInensity =
        bClamp(bClamp(g_WooshVol_vs_Vel.GetValue(this->m_pDriveByState->m_DriveByInfo.ClosingVelocity), 0.0f, 0.99f) * 127.0f, 0.0f, 127.0f);
    if (g_pEAXSound->GetSndGameMode() == SND_PURSUITBREAKER) {
        fVelInensity = 60.0f;
    }

    int numblocks;
    int sizeperblock;
    STICH_WHOOSH_TYPE base;
    GetWooshBlockSizeParams(this->m_pDriveByState->m_DriveByInfo.eDriveByType, base, numblocks, sizeperblock);
    switch (this->m_pDriveByState->m_DriveByInfo.eDriveByType) {
        case DRIVE_BY_TUNNEL_IN:
        case DRIVE_BY_OVERPASS_IN: {
            static int LastRandom = 0;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
        case DRIVE_BY_TUNNEL_OUT:
        case DRIVE_BY_OVERPASS_OUT: {
            static int LastRandom = 0;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
        case DRIVE_BY_LAMPPOST: {
            static int LastRandom = 0;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        } // fallthrough
        case DRIVE_BY_AI_CAR: {
            static int LastRandom = 0;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
        case DRIVE_BY_SMOKABLE: {
            static int LastRandom = 0;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        }
        case DRIVE_BY_TRAFFIC: {
            static int LastRandom = 0;

            LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
        } // fallthrough
        case DRIVE_BY_CAMERA_BY:
            this->SetDMIX_Input(13, 0x7FFF);
            {
                static int LastRandom = 0;

                LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
            }
        case DRIVE_BY_UNKNOWN:
        default:
            break;
    }

    float fVelRatio = static_cast<float>(numblocks) / 128.0f;
    int StitchID = static_cast<int>(fVelInensity * fVelRatio);
    {
        static int LastRandom = 0;

        StitchID = base + StitchID * sizeperblock + (LastRandom - (LastRandom / sizeperblock) * sizeperblock);
        LastRandom = (LastRandom - (LastRandom / sizeperblock) * sizeperblock) + 1;
    }

    this->m_pStitchData = &g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_WOOSH, StitchID);
    this->m_SndParams.Az = 0;
    this->m_SndParams.Pitch = 0;
    this->m_SndParams.Vol = 0;

    this->m_pWooshStich = new cStichWrapper(*this->m_pStitchData);
    this->m_pWooshStich->Play(&this->m_SndParams);
    this->SetDMIX_Input(11, 0x7FFF);
    this->SetDMIX_Input(12, bClamp(static_cast<int>(fVelInensity) << 8, 0, 0x7FFF) >> 15);
}

void SFXObj_Woosh::UpdateParams(float t) {
    if (this->m_pDriveByState->IsAttached()) {
        SndBase::UpdateParams(t);
        if ((this->m_pWooshStich != nullptr) && !this->m_pWooshStich->IsPlaying()) {
            delete this->m_pWooshStich;
            this->m_pWooshStich = nullptr;
            this->m_pDriveByState->Detach();
        }
    }
}

void SFXObj_Woosh::ProcessUpdate() {
    int Pitch;
    eVOL_WOOSH slottouse;

    SndBase::ProcessUpdate();

    this->SetDMIX_Input(11, 0);
    this->SetDMIX_Input(13, 0);
    if (this->m_pDriveByState->IsAttached() && (this->m_pWooshStich != nullptr)) {
        Pitch = this->GetDMixOutput(14, DMX_PITCH);
        switch (this->m_pDriveByState->m_DriveByInfo.eDriveByType) {
            case DRIVE_BY_AI_CAR:
                slottouse = eVOL_WOOSH_AICAR;
                break;
            case DRIVE_BY_TUNNEL_IN:
                slottouse = eVOL_WOOSH_TUNNEL_IN;
                break;
            case DRIVE_BY_TUNNEL_OUT:
                slottouse = eVOL_WOOSH_TUNNEL_OUT;
                break;
            case DRIVE_BY_OVERPASS_IN:
                slottouse = eVOL_WOOSH_OVERPASS_IN;
                break;
            case DRIVE_BY_OVERPASS_OUT:
                slottouse = eVOL_WOOSH_OVERPASS_OUT;
                break;
            case DRIVE_BY_SMOKABLE:
                slottouse = eVOL_WOOSH_SMOKABLE;
                break;
            case DRIVE_BY_BRIDGE:
                slottouse = eVOL_WOOSH_BRIDGE;
                break;
            case DRIVE_BY_LAMPPOST:
                slottouse = eVOL_WOOSH_LAMPPOST;
                break;
            case DRIVE_BY_CAMERA_BY:
                slottouse = eVOL_WOOSH_CAMERA_BY;
                break;
            case DRIVE_BY_TREE:
            default:
                slottouse = eVOL_WOOSH_TREE;
                break;
        }

        this->m_SndParams.Az = this->GetDMixOutput(0, DMX_AZIM);
        this->m_SndParams.Vol = this->GetDMixOutput(slottouse, DMX_VOL);
        this->m_SndParams.Pitch = Pitch;
        this->m_SndParams.RVerb = this->GetDMixOutput(15, DMX_VOL);
        this->m_pWooshStich->Update(&this->m_SndParams);
    }
}

int SFXObj_Woosh::GetController(int Index) {
    return Index == 0 ? 0 : -1;
}

void SFXObj_Woosh::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0) {
        this->m_p3DPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
    }
}

void SFXObj_Woosh::Destroy() {
    delete this->m_pWooshStich;
    this->m_pWooshStich = nullptr;
}

DEFINE_CREATABLE(0x80000, SFXCTL_3DWooshPos, SndBase);
