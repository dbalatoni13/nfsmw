#include "./CARSFX_RoadNoise.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/snd_gen/ENVIRO_AEMS.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"

struct cStitchLoop {
    cStitchLoop(unsigned int nameHash);
    ~cStitchLoop();
    void Update(const SND_Params *Params, float dt);
};

extern int RoadNoiseVolumes[9];
extern Graph RoadNoiseVolGraph;
extern Slope RoadNoiseSpeedToPitch;
extern Slope RoadNoiseTransitionPitchSlope;
extern int GetRoadNoiseTransitionVol(FXROADNOISE_TRANSITION ID);

SndBase::TypeInfo CARSFX_RoadNoise::s_TypeInfo = {
    0x00020080, "CARSFX_RoadNoise", &SndBase::s_TypeInfo, CARSFX_RoadNoise::CreateObject};

SndBase::TypeInfo *CARSFX_RoadNoise::GetTypeInfo() const { return &s_TypeInfo; }

const char *CARSFX_RoadNoise::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *CARSFX_RoadNoise::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (CARSFX_RoadNoise::GetStaticTypeInfo()->typeName, false) CARSFX_RoadNoise();
    }
    return new (CARSFX_RoadNoise::GetStaticTypeInfo()->typeName, true) CARSFX_RoadNoise();
}

CARSFX_RoadNoise::CARSFX_RoadNoise() {
    for (int n = 0; n < 2; n++) {
        m_pWetRoad[n] = nullptr;
        m_pRoadNoiseControl[n] = nullptr;
        m_pStitchLoopControl[n] = nullptr;
        m_pTransition[n] = nullptr;
        m_pStitchTransition[n] = nullptr;
        LoopID[n] = FXROADNOISE_LOOP_NONE;
    }
    m_pWheelCtl = nullptr;
}

CARSFX_RoadNoise::~CARSFX_RoadNoise() {
    Destroy();
}

int CARSFX_RoadNoise::GetController(int Index) {
    switch (Index) {
    case 0:
        return 1;
    case 1:
        return 0xB;
    case 2:
        return 0xC;
    default:
        return -1;
    }
}

void CARSFX_RoadNoise::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
    case 1:
        m_pWheelCtl = static_cast<SFXCTL_Wheel *>(psfxctl);
        break;
    case 0xB:
        m_pRightWheelPos = (SFXCTL_3DRightWheelPos *)psfxctl;
        break;
    case 0xC:
        m_pLeftWheelPos = (SFXCTL_3DLeftWheelPos *)psfxctl;
        break;
    default:
        break;
    }
}

void CARSFX_RoadNoise::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void CARSFX_RoadNoise::InitSFX() {
    SndBase::InitSFX();
    if (GetInputBlockPtr()) {
        GetInputBlockPtr()[eVRB_ROADNOISE_VERB] = 1;
    }

    if (!m_pWheelCtl) {
        Disable();
    } else {
        for (int n = 0; n < 2; n++) {
            g_pEAXSound->SetCsisName(this);
            m_pWetRoad[n] =
                new FX_ROADNOISE(FXROADNOISE_LOOP_WETROAD, 0, 0, 0, FXROADNOISETYPETYPE_LOOP, 0, 0, 25000, 0, 0x7FFF, 0);
        }
    }
}

void CARSFX_RoadNoise::UpdateParams(float t) {
    GenerateRoadNoise();
}

void CARSFX_RoadNoise::Destroy() {
    for (int n = 0; n < 2; n++) {
        delete m_pWetRoad[n];
        m_pWetRoad[n] = nullptr;
        delete m_pRoadNoiseControl[n];
        m_pRoadNoiseControl[n] = nullptr;
        delete m_pStitchLoopControl[n];
        m_pStitchLoopControl[n] = nullptr;
        delete m_pTransition[n];
        m_pTransition[n] = nullptr;
        delete m_pStitchTransition[n];
        m_pStitchTransition[n] = nullptr;
    }
}

void CARSFX_RoadNoise::Detach() {
    Destroy();
}

eVOL_ROADNOISE CARSFX_RoadNoise::MapLoopToVolume(FXROADNOISE_LOOP ID) {
    switch (ID) {
    case FXROADNOISE_LOOP_GRAVEL00:
        return eVOL_ROADNOISE_GRAVEL;
    case FXROADNOISE_LOOP_SIDEWALK:
        return eVOL_ROADNOISE_SIDEWALK;
    case FXROADNOISE_LOOP_COBBLESTONE00:
        return eVOL_ROADNOISE_COBBLESTONE;
    case FXROADNOISE_LOOP_DEEPWATER:
        return eVOL_ROADNOISE_DEEPWATER;
    case FXROADNOISE_LOOP_WETROAD:
        return eVOL_ROADNOISE_WETROAD;
    case FXROADNOISE_LOOP_METAL:
        return eVOL_ROADNOISE_METAL;
    case FXROADNOISE_LOOP_STITCH_LOOP:
        return eVOL_ROADNOISE_STITCH_LOOP;
    default:
        return eVOL_ROADNOISE_ASHPHALT;
    }
}

void CARSFX_RoadNoise::PlayTransition(FXROADNOISE_TRANSITION ID, int side) {
    float Speed = GetPhysCar()->GetVelocityMagnitudeMPH();
    int Vol = GetRoadNoiseTransitionVol(ID);
    int Pitch;

    if (ID == FXROADNOISE_TRANSITION_SPIKESTRIP) {
        Vol = (Vol * 0x7FFF >> 15) * GetDMixOutput(eVOL_ROADNOISE_SPIKE, DMX_VOL) >> 15;
    } else if (ID == FXROADNOISE_TRANSITION_BLOWN) {
        Vol = (Vol * 0x7FFF >> 15) * GetDMixOutput(eVOL_ROADNOISE_BLOWN, DMX_VOL) >> 15;
    } else {
        Vol = (Vol * 0x7FFF >> 15) * GetDMixOutput(eVOL_ROADNOISE_TRANSITION, DMX_VOL) >> 15;
    }

    TransitionVol[side] = Vol;
    Pitch = static_cast<int>(RoadNoiseTransitionPitchSlope.GetValue(Speed));
    g_pEAXSound->SetCsisName(this);
    delete m_pTransition[side];
    m_pTransition[side] = new FX_ROADNOISE_TRANS(ID, Vol, Pitch, GetDMixOutput(side != 0, DMX_AZIM),
                                                 FXROADNOISETRANSTYPETYPE_TRANSITION_, 0, 0, 25000, 0, 0x7FFF,
                                                 GetDMixOutput(eVRB_ROADNOISE_VERB, DMX_VOL));
}

void CARSFX_RoadNoise::GenerateRoadNoise() {
    float fRightVol;
    float fLeftVol;
    float fRightPitch;
    float fLeftPitch;
    float speed;
    float ftemp;

    speed = GetPhysCar()->GetVelocityMagnitudeMPH();
    ftemp = static_cast<float>(static_cast<int>(RoadNoiseVolGraph.GetValue(speed)) * 0x7FFF >> 15);

    fRightVol = bLength(&m_pWheelCtl->m_bvTotalRightWheelSlip) * 0.01f;
    if (fRightVol > 0.15f) {
        fRightVol = 0.15f;
    }
    fRightVol = ftemp + ftemp * fRightVol;
    fRightVol = fRightVol + fRightVol * 0.1f;

    fLeftVol = bLength(&m_pWheelCtl->m_bvTotalLeftWheelSlip) * 0.01f;
    if (fLeftVol > 0.15f) {
        fLeftVol = 0.15f;
    }
    fLeftVol = ftemp + ftemp * fLeftVol;
    ftemp = (m_pWheelCtl->m_fWheelTractionMag[0] + m_pWheelCtl->m_fWheelTractionMag[3]) * 0.5f * 0.1f;
    if (ftemp > 0.1f) {
        ftemp = 0.1f;
    }
    fLeftVol = fLeftVol + fLeftVol * ftemp;
    if (fLeftVol > 32000.0f) {
        fLeftVol = 32000.0f;
    }
    if (fRightVol > 32000.0f) {
        fRightVol = 32000.0f;
    }

    fRightPitch = RoadNoiseSpeedToPitch.GetValue(speed);
    fLeftPitch = bLength(&m_pWheelCtl->m_bvTotalLeftWheelSlip) * 0.01f;
    if (fLeftPitch > 0.2f) {
        fLeftPitch = 0.2f;
    }
    ftemp = (m_pWheelCtl->m_fWheelTractionMag[0] + m_pWheelCtl->m_fWheelTractionMag[3]) * 0.5f * 0.15f;
    fLeftPitch = fRightPitch + fRightPitch * fLeftPitch;
    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }
    fLeftPitch = fLeftPitch + fLeftPitch * ftemp;
    if (fLeftPitch > 6000.0f) {
        fLeftPitch = 6000.0f;
    }

    ftemp = bLength(&m_pWheelCtl->m_bvTotalRightWheelSlip) * 0.01f;
    if (ftemp > 0.2f) {
        ftemp = 0.2f;
    }
    fRightPitch = fRightPitch + fRightPitch * ftemp;
    ftemp = (m_pWheelCtl->m_fWheelTractionMag[1] + m_pWheelCtl->m_fWheelTractionMag[2]) * 0.5f * 0.15f;
    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }
    fRightPitch = fRightPitch + fRightPitch * ftemp;
    if (fRightPitch > 6000.0f) {
        fRightPitch = 6000.0f;
    }

    m_nLTRoadNoiseVol = static_cast<int>(fLeftVol);
    m_nRTRoadNoiseVol = static_cast<int>(fRightVol);
    m_nRTRoadNoisePitch = static_cast<int>(fRightPitch);
    m_nLTRoadNoisePitch = static_cast<int>(fLeftPitch);
}

void CARSFX_RoadNoise::Play(FXROADNOISE_LOOP ID, int side) {
    int refcnt;

    delete m_pRoadNoiseControl[side];
    if (m_pStitchLoopControl[side]) {
        delete m_pStitchLoopControl[side];
    }

    if (ID < FXROADNOISE_LOOP_STITCH_LOOP) {
        g_pEAXSound->SetCsisName(this);
        m_pRoadNoiseControl[side] =
            new FX_ROADNOISE(ID, 0, 0x1000, 0, FXROADNOISETYPETYPE_LOOP, 0, 0, 25000, 0, 0x7FFF, 0);
        refcnt = 0;
        if (m_pRoadNoiseControl[side]) {
            refcnt = m_pRoadNoiseControl[side]->GetRefCount();
        }
    } else {
        m_pStitchLoopControl[side] = new ("Stitch Loop", 0) cStitchLoop(0x4B41DEC8);
    }
}

void CARSFX_RoadNoise::ProcessUpdate() {
    SetDMIX_Input(0, 0);

    for (int n = 0; n < 2; n++) {
        bool wheelstouchingground;
        const Attrib::Gen::simsurface &currentterrain = n == 0 ? m_pWheelCtl->LeftSideTerrain : m_pWheelCtl->RightSideTerrain;
        const Attrib::Gen::simsurface &prevterrain = n == 0 ? m_pWheelCtl->PrevLeftSideTerrain : m_pWheelCtl->PrevRightSideTerrain;
        eVOL_ROADNOISE AzSlot;
        int GeneratedVolume;
        int GeneratedPitch;
        bool bPuncturedTire;
        bool bBlownTire;
        int TireTransition;
        int tempVol;
        int tempPitch;

        TireTransition = -1;
        if (n == 0) {
            wheelstouchingground = m_pWheelCtl->LeftSideTouchingGround;
            AzSlot = eAZI_ROADNOISE_LEFT_AZ;
            GeneratedVolume = m_nLTRoadNoiseVol;
            GeneratedPitch = m_nLTRoadNoisePitch;
            bPuncturedTire = GetPhysCar()->DidTireJustPucture(0) || GetPhysCar()->DidTireJustPucture(3);
            bBlownTire = GetPhysCar()->DidTireJustBlow(0) || GetPhysCar()->DidTireJustBlow(3);

            for (int tire_num = 0; tire_num <= 3; tire_num += 3) {
                if (GetPhysCar()->GetWheelTerrain(tire_num) != GetPhysCar()->GetPrevWheelTerrain(tire_num)) {
                    TireTransition = tire_num;
                }
            }
        } else {
            wheelstouchingground = m_pWheelCtl->RightSideTouchingGround;
            AzSlot = eAZI_ROADNOISE_RIGHT_AZ;
            GeneratedVolume = m_nRTRoadNoiseVol;
            GeneratedPitch = m_nRTRoadNoisePitch;
            bPuncturedTire = GetPhysCar()->DidTireJustPucture(1) || GetPhysCar()->DidTireJustPucture(2);
            bBlownTire = GetPhysCar()->DidTireJustBlow(1) || GetPhysCar()->DidTireJustBlow(2);

            for (int tire_num = 1; tire_num <= 2; tire_num++) {
                if (GetPhysCar()->GetWheelTerrain(tire_num) != GetPhysCar()->GetPrevWheelTerrain(tire_num)) {
                    TireTransition = tire_num;
                }
            }
        }

        if (currentterrain.GetCollection() != prevterrain.GetCollection()) {
            LoopID[n] = currentterrain.Aud_Roadnoise_LOOP();
            SetDMIX_Input(0, 0x7FFF);
            if (LoopID[n] == FXROADNOISE_LOOP_NONE) {
                delete m_pRoadNoiseControl[n];
                m_pRoadNoiseControl[n] = nullptr;
            } else {
                Play(LoopID[n], n);
            }
        }

        if (wheelstouchingground && TireTransition != -1) {
            FXROADNOISE_TRANSITION OntoTransitionID = GetPhysCar()->GetWheelTerrain(TireTransition).Aud_RoadNoise_TransON();
            FXROADNOISE_TRANSITION OffTransitionID = GetPhysCar()->GetWheelTerrain(TireTransition).Aud_RoadNoise_TransOFF();

            if (OffTransitionID != FXROADNOISE_TRANSITION_DONTPLAY && OntoTransitionID != FXROADNOISE_TRANSITION_DONTPLAY) {
                if (OntoTransitionID == FXROADNOISE_TRANSITION_NONE) {
                    if (OffTransitionID != FXROADNOISE_TRANSITION_NONE) {
                        PlayTransition(OffTransitionID, n);
                    }
                } else {
                    PlayTransition(OntoTransitionID, n);
                }
            }
        }

        if (bPuncturedTire) {
            PlayTransition(FXROADNOISE_TRANSITION_SPIKESTRIP, n);
        }
        if (bBlownTire) {
            PlayTransition(FXROADNOISE_TRANSITION_BLOWN, n);
        }

        tempVol = GeneratedVolume * GetDMixOutput(MapLoopToVolume(LoopID[n]), DMX_VOL) >> 15;
        if (!wheelstouchingground) {
            tempVol = 0;
        }

        if (m_pRoadNoiseControl[n]) {
            m_pRoadNoiseControl[n]->SetVolume(tempVol);
            m_pRoadNoiseControl[n]->SetPitch(GeneratedPitch);
            m_pRoadNoiseControl[n]->SetAzimuth(GetDMixOutput(AzSlot, DMX_AZIM));
            m_pRoadNoiseControl[n]->SetFilter_Effects_Wet_FX(GetDMixOutput(eVRB_ROADNOISE_VERB, DMX_VOL));
            if (currentterrain.GetCollection() == 0xD929E923 || currentterrain.GetCollection() == 0xC1C577D6) {
                m_pRoadNoiseControl[n]->SetSecondaryNoise(1);
            }
            m_pRoadNoiseControl[n]->CommitMemberData();
        }

        if (m_pStitchLoopControl[n]) {
            SND_Params TmpParams(0, tempVol, 0, GetDMixOutput(AzSlot, DMX_AZIM), 0,
                                 GetDMixOutput(eVRB_ROADNOISE_VERB, DMX_VOL));
            m_pStitchLoopControl[n]->Update(&TmpParams, SndBase::m_fDeltaTime);
        }

        if (m_pWetRoad[n]) {
            tempVol = (GeneratedVolume * RoadNoiseVolumes[4] >> 15) * GetDMixOutput(eVOL_ROADNOISE_WET_ROAD, DMX_VOL) >> 15;
            m_pWetRoad[n]->SetVolume(tempVol);
            m_pWetRoad[n]->SetPitch(GeneratedPitch);
            m_pWetRoad[n]->SetAzimuth(GetDMixOutput(AzSlot, DMX_AZIM));
            m_pWetRoad[n]->CommitMemberData();
        }

        if (m_pTransition[n]) {
            if (m_pTransition[n]->GetRefCount() > 1 && g_EAXIsPaused()) {
                tempPitch = 0x1000;
                m_pTransition[n]->SetPitch(tempPitch);
                m_pTransition[n]->SetVolume(0);
                m_pTransition[n]->CommitMemberData();
            }
        }

        if (m_pStitchTransition[n]) {
            if (!m_pStitchTransition[n]->IsPlaying()) {
                delete m_pStitchTransition[n];
                m_pStitchTransition[n] = nullptr;
            } else {
                SND_Params TmpParams(0, GetDMixOutput(eVOL_ROADNOISE_STITCH_TRANS, DMX_VOL), 0, GetDMixOutput(AzSlot, DMX_AZIM),
                                     0, GetDMixOutput(eVRB_ROADNOISE_VERB, DMX_VOL));
                m_pStitchTransition[n]->Update(&TmpParams);
            }
        }
    }
}
