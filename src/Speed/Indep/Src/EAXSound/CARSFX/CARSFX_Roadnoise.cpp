#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_RoadNoise.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface_hash.h"

static const int MEM_LEAK_DISABLE_ROADNOISE = 0; // size: 0x4, Decl: 34
static const int SPEW_CURB_ON_INFO = 0;          // size: 0x4, Decl: 35

static const int VOL_SCALE_FOR_INCAR_ROADNOISE = 8000; // size: 0x4, Decl: 39

Slope RoadNoiseTransitionVolSlope(20000.0f, 32767.0f, 12.0f, 60.0f); // size: 0x1C, address: 0x8045E330, Decl: 41

Slope RoadNoiseTransitionPitchSlope(3600.0f, 4400.0f, 12.0f, 60.0f); // size: 0x1C, address: 0x8045E34C, Decl: 47

// size: 0x28, address: 0x8045E368, Decl: 54
bVector2 RoadNoiseVolumeCurve[5] = {
    bVector2(0.0f, 0.0f), bVector2(60.0f, 28000.0f), bVector2(100.0f, 32500.0f), bVector2(150.0f, 24500.0f), bVector2(175.0f, 18000.0f),
};

Graph RoadNoiseVolGraph(RoadNoiseVolumeCurve, 5); // size: 0x8, address: 0x8045E390, Decl: 64

static const float RoadNoiseMinPitch = 1500.0f; // size: 0x4, Decl: 66
static const float RoadNoiseMaxPitch = 4500.0f; // size: 0x4, Decl: 67

Slope RoadNoiseSpeedToPitch(1500.0f, 4500.0f, 0.0f, 100.0f); // size: 0x1C, address: 0x8045E398, Decl: 69

static const float gfTireNoiseVolumeScale = 0.047f; // size: 0x4, Decl: 74
static const float gfTireNoisePitchScale = 0.0065f; // size: 0x4, Decl: 75

static const float gfMaxTireNoiseVolume = 2.5f; // size: 0x4, Decl: 77

static const int gnMaxRoadNoisePitch = 6000; // size: 0x4, Decl: 80

static const float gfRoadNoiseWheelSlipVolScale = 0.01f; // size: 0x4, Decl: 83
static const float gfRoadNoiseTractionVolScale = 1.0f;   // size: 0x4, Decl: 84

static const float gfMaxRoadNoiseWheelSlipVolume = 0.15f; // size: 0x4, Decl: 86
static const float gfMaxRoadNoiseTractionVolume = 0.1f;   // size: 0x4, Decl: 87

static const float gfRoadNoiseSlipPitchScale = 0.01f; // size: 0x4, Decl: 89
static const float gfRoadNoiseTrackPitchScale = 1.0f; // size: 0x4, Decl: 90
static const float gfMaxRoadNoiseSlipPitch = 0.2f;    // size: 0x4, Decl: 91
static const float gfMaxRoadNoiseTracPitch = 0.15f;   // size: 0x4, Decl: 92

static const int ROADNOISE_VOL = 32767; // size: 0x4, Decl: 94

DEFINE_CREATABLE(0x20080, CARSFX_RoadNoise, SndBase);

CARSFX_RoadNoise::CARSFX_RoadNoise() {
    for (int n = 0; n < 2; n++) {
        this->m_pWetRoad[n] = nullptr;
        this->m_pRoadNoiseControl[n] = nullptr;
        this->m_pStitchLoopControl[n] = nullptr;
        this->m_pTransition[n] = nullptr;
        this->m_pStitchTransition[n] = nullptr;
        this->LoopID[n] = FXROADNOISE_LOOP_NONE;
    }
    this->m_pWheelCtl = nullptr;
}

CARSFX_RoadNoise::~CARSFX_RoadNoise() {
    this->Destroy();
}

void CARSFX_RoadNoise::Detach() {
    this->Destroy();
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
            this->m_pWheelCtl = static_cast<SFXCTL_Wheel *>(psfxctl);
            break;
        case 0xB:
            this->m_pRightWheelPos = (SFXCTL_3DRightWheelPos *)psfxctl;
            break;
        case 0xC:
            this->m_pLeftWheelPos = (SFXCTL_3DLeftWheelPos *)psfxctl;
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
    if (this->m_pWheelCtl == nullptr) {
        this->Disable();
    } else {
        if (this->m_pLeftWheelPos != nullptr) {
            this->m_pLeftWheelPos->AssignPositionVector(this->m_pWheelCtl->GetWheelPos(0, 2));
            this->m_pLeftWheelPos->AssignVelocityVector(nullptr);
        }

        if (this->m_pRightWheelPos != nullptr) {
            this->m_pRightWheelPos->AssignPositionVector(this->m_pWheelCtl->GetWheelPos(1, 2));
            this->m_pRightWheelPos->AssignVelocityVector(nullptr);
        }

        for (int n = 0; n < 2; n++) {
            g_pEAXSound->SetCsisName(this);
            m_pWetRoad[n] = new Csis::FX_ROADNOISE(FXROADNOISE_LOOP_WETROAD, 0, 0, 0, Csis::FXROADNOISETYPETYPE_LOOP, 0, 0, 25000, 0, 0x7FFF, 0);
        }
    }
}

void CARSFX_RoadNoise::Destroy() {
    for (int n = 0; n < 2; n++) {
        delete this->m_pWetRoad[n];
        this->m_pWetRoad[n] = nullptr;

        delete this->m_pRoadNoiseControl[n];
        this->m_pRoadNoiseControl[n] = nullptr;

        delete this->m_pStitchLoopControl[n];
        this->m_pStitchLoopControl[n] = nullptr;

        delete this->m_pTransition[n];
        this->m_pTransition[n] = nullptr;

        delete this->m_pStitchTransition[n];
        this->m_pStitchTransition[n] = nullptr;
    }
}

int RoadNoiseVolumes[9]; // size: 0x24, Decl: 211

void CARSFX_RoadNoise::UpdateParams(float t) {
    SndBase::UpdateParams(t);
    this->GenerateRoadNoise();
}

void CARSFX_RoadNoise::ProcessUpdate() {
    this->SetDMIX_Input(0, 0);

    for (int n = 0; n < 2; n++) {
        bool wheelstouchingground = true;
        const Attrib::Gen::simsurface &currentterrain = n == 0 ? this->m_pWheelCtl->LeftSideTerrain : this->m_pWheelCtl->RightSideTerrain;
        const Attrib::Gen::simsurface &prevterrain = n == 0 ? this->m_pWheelCtl->PrevLeftSideTerrain : this->m_pWheelCtl->PrevRightSideTerrain;
        eVOL_ROADNOISE AzSlot;
        int GeneratedVolume = 0;
        int GeneratedPitch = 0;
        bool bPuncturedTire = false;
        bool bBlownTire = false;
        int TireTransition = -1;

        if (n == 0) {
            wheelstouchingground = this->m_pWheelCtl->LeftSideTouchingGround;
            AzSlot = eAZI_ROADNOISE_LEFT_AZ;
            GeneratedVolume = this->m_nLTRoadNoiseVol;
            GeneratedPitch = this->m_nLTRoadNoisePitch;
            bPuncturedTire = false;

            if (this->GetPhysCar()->DidTireJustPucture(0) || this->GetPhysCar()->DidTireJustPucture(3)) {
                bPuncturedTire = true;
            }

            bBlownTire = false;
            if (this->GetPhysCar()->DidTireJustBlow(0) || this->GetPhysCar()->DidTireJustBlow(3)) {
                bBlownTire = true;
            }

            for (int tire_num = 0; tire_num <= 3; tire_num += 3) {
                if (this->GetPhysCar()->GetWheelTerrain(tire_num).GetCollection() !=
                    this->GetPhysCar()->GetPrevWheelTerrain(tire_num).GetCollection()) {
                    if (this->GetPhysCar()->GetWheelTerrain(tire_num).Aud_Roadnoise_LOOP() !=
                        this->GetPhysCar()->GetPrevWheelTerrain(tire_num).Aud_Roadnoise_LOOP()) {
                        TireTransition = tire_num;
                    }
                }
            }
        } else if (n == 1) {
            wheelstouchingground = this->m_pWheelCtl->RightSideTouchingGround;
            AzSlot = eAZI_ROADNOISE_RIGHT_AZ;
            GeneratedVolume = this->m_nRTRoadNoiseVol;
            GeneratedPitch = this->m_nRTRoadNoisePitch;

            if (this->GetPhysCar()->DidTireJustPucture(1) || this->GetPhysCar()->DidTireJustPucture(2)) {
                bPuncturedTire = true;
            }

            bBlownTire = false;
            if (this->GetPhysCar()->DidTireJustBlow(1) || this->GetPhysCar()->DidTireJustBlow(2)) {
                bBlownTire = true;
            }

            for (int tire_num = 1; tire_num <= 2; tire_num++) {
                if (this->GetPhysCar()->GetWheelTerrain(tire_num).GetCollection() !=
                    this->GetPhysCar()->GetPrevWheelTerrain(tire_num).GetCollection()) {
                    if (this->GetPhysCar()->GetWheelTerrain(tire_num).Aud_Roadnoise_LOOP() !=
                        this->GetPhysCar()->GetPrevWheelTerrain(tire_num).Aud_Roadnoise_LOOP()) {
                        TireTransition = tire_num;
                    }
                }
            }
        }

        if (prevterrain.GetCollection() != currentterrain.GetCollection()) {
            if (this->LoopID[n] == currentterrain.Aud_Roadnoise_LOOP()) {
                return;
            }

            this->LoopID[n] = currentterrain.Aud_Roadnoise_LOOP();
            this->SetDMIX_Input(0, 0x7FFF);
            if (this->LoopID[n] != FXROADNOISE_LOOP_NONE) {
                this->Play(this->LoopID[n], n);
            } else {
                delete this->m_pRoadNoiseControl[n];
                this->m_pRoadNoiseControl[n] = nullptr;
            }
        }

        if (wheelstouchingground && TireTransition != -1) {
            FXROADNOISE_TRANSITION OntoTransitionID = this->GetPhysCar()->GetWheelTerrain(TireTransition).Aud_RoadNoise_TransON();
            FXROADNOISE_TRANSITION OffTransitionID = this->GetPhysCar()->GetPrevWheelTerrain(TireTransition).Aud_RoadNoise_TransOFF();

            if (OffTransitionID != FXROADNOISE_TRANSITION_DONTPLAY && OntoTransitionID != FXROADNOISE_TRANSITION_DONTPLAY) {
                if (OntoTransitionID != FXROADNOISE_TRANSITION_NONE) {
                    this->PlayTransition(OntoTransitionID, n);
                } else if (OffTransitionID != FXROADNOISE_TRANSITION_NONE) {
                    this->PlayTransition(OffTransitionID, n);
                }
            }
        }

        if (bPuncturedTire) {
            this->PlayTransition(FXROADNOISE_TRANSITION_SPIKESTRIP, n);
        }
        if (bBlownTire) {
            this->PlayTransition(FXROADNOISE_TRANSITION_BLOWN, n);
        }

        int tempVol = GeneratedVolume * this->GetDMixOutput(this->MapLoopToVolume(this->LoopID[n]), DMX_VOL) >> 15;
        if (!wheelstouchingground) {
            tempVol = 0;
        }

        if (this->m_pRoadNoiseControl[n] != nullptr) {
            this->m_pRoadNoiseControl[n]->SetVolume(tempVol);
            this->m_pRoadNoiseControl[n]->SetPitch(GeneratedPitch);
            this->m_pRoadNoiseControl[n]->SetAzimuth(this->GetDMixOutput(AzSlot, DMX_AZIM));
            this->m_pRoadNoiseControl[n]->SetFilter_Effects_Wet_FX(this->GetDMixOutput(eVRB_ROADNOISE_VERB, DMX_VOL));
            if (currentterrain.GetCollection() == Attrib::Hash::simsurface::key_dirt ||
                currentterrain.GetCollection() == Attrib::Hash::simsurface::key_gravel) {
                if (this->m_pRoadNoiseControl[n] != nullptr) {
                    this->m_pRoadNoiseControl[n]->SetSecondaryNoise(1);
                }
            }
            this->m_pRoadNoiseControl[n]->CommitMemberData();
        }

        if (this->m_pStitchLoopControl[n] != nullptr) {
            SND_Params TmpParams(0, tempVol, 0, this->GetDMixOutput(AzSlot, DMX_AZIM), 0, this->GetDMixOutput(eVRB_ROADNOISE_VERB, DMX_VOL));
            this->m_pStitchLoopControl[n]->Update(&TmpParams, SndBase::m_fDeltaTime);
        }

        if (this->m_pWetRoad[n] != nullptr) {
            tempVol = GeneratedVolume * RoadNoiseVolumes[4] >> 15;
            tempVol = tempVol * this->GetDMixOutput(eVOL_ROADNOISE_WET_ROAD, DMX_VOL) >> 15;
            this->m_pWetRoad[n]->SetVolume(tempVol);
            this->m_pWetRoad[n]->SetPitch(GeneratedPitch);
            this->m_pWetRoad[n]->SetAzimuth(this->GetDMixOutput(AzSlot, DMX_AZIM));
            this->m_pWetRoad[n]->CommitMemberData();
        }

        int tempPitch;
        if (this->m_pTransition[n] != nullptr) {
            tempPitch = this->m_pTransition[n]->GetRefCount();
            if (tempPitch > 1 && g_EAXIsPaused()) {
                int Pitch = 0x1000;
                this->m_pTransition[n]->SetVolume(0);
                this->m_pTransition[n]->SetPitch(Pitch);
                this->m_pTransition[n]->CommitMemberData();
            }
        }

        if (this->m_pStitchTransition[n] != nullptr) {
            if (!this->m_pStitchTransition[n]->IsPlaying()) {
                delete this->m_pStitchTransition[n];
                this->m_pStitchTransition[n] = nullptr;
            } else {
                SND_Params TmpParams(0, this->GetDMixOutput(eVOL_ROADNOISE_STITCH_TRANS, DMX_VOL), 0, this->GetDMixOutput(AzSlot, DMX_AZIM), 0,
                                     this->GetDMixOutput(eVRB_ROADNOISE_VERB, DMX_VOL));
                this->m_pStitchTransition[n]->Update(&TmpParams);
            }
        }
    }
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
        case FXROADNOISE_LOOP_NONE:
        case FXROADNOISE_LOOP_ASHPHALT00:
        case FXROADNOISE_LOOP_ASHPHALT01:
        default:
            return eVOL_ROADNOISE_ASHPHALT;
        case FXROADNOISE_LOOP_METAL:
            return eVOL_ROADNOISE_METAL;
        case FXROADNOISE_LOOP_STITCH_LOOP:
            return eVOL_ROADNOISE_STITCH_LOOP;
    }
}

// TODO move?
int GetRoadNoiseTransitionVol(FXROADNOISE_TRANSITION ID);

void CARSFX_RoadNoise::PlayTransition(FXROADNOISE_TRANSITION ID, int side) {
    float Speed = this->GetPhysCar()->GetVelocityMagnitudeMPH();
    int Vol = GetRoadNoiseTransitionVol(ID);
    int Pitch = 0x7FFF;

    Vol = Vol * Pitch >> 15;

    if (ID == FXROADNOISE_TRANSITION_SPIKESTRIP) {
        Vol = Vol * this->GetDMixOutput(eVOL_ROADNOISE_SPIKE, DMX_VOL) >> 15;
    } else if (ID == FXROADNOISE_TRANSITION_BLOWN) {
        Vol = Vol * this->GetDMixOutput(eVOL_ROADNOISE_BLOWN, DMX_VOL) >> 15;
    } else {
        Vol = Vol * this->GetDMixOutput(eVOL_ROADNOISE_TRANSITION, DMX_VOL) >> 15;
    }

    this->TransitionVol[side] = Vol;
    Pitch = static_cast<int>(RoadNoiseTransitionPitchSlope.GetValue(Speed));

    g_pEAXSound->SetCsisName(this);

    delete this->m_pTransition[side];
    this->m_pTransition[side] = new Csis::FX_ROADNOISE_TRANS(ID, Vol, Pitch, this->GetDMixOutput(static_cast<int>(side != 0), DMX_AZIM),
                                                             Csis::FXROADNOISETRANSTYPETYPE_TRANSITION_, 0, 0, 25000, 0, 0x7FFF,
                                                             this->GetDMixOutput(eVRB_ROADNOISE_VERB, DMX_VOL));

    if (false) {
        int StitchID;
        GEN_RND_OFFSET(StitchID, Speed, 1, 1, 1);

        SND_Stich *m_pStitchData = &g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_STATIC, StitchID);
        SND_Params m_SndParams;
    }
}

// UNSOLVED
void CARSFX_RoadNoise::GenerateRoadNoise() {
    float speed = this->GetPhysCar()->GetVelocityMagnitudeMPH();
    float fLeftVol = static_cast<float>(static_cast<int>(RoadNoiseVolGraph.GetValue(speed)) * 0x7FFF >> 15);

    float ftemp = bLength(this->m_pWheelCtl->m_bvTotalRightWheelSlip) * 0.01f;
    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }

    float fRightVol = fLeftVol + fLeftVol * ftemp;
    fRightVol = fRightVol + fRightVol * 0.1f;

    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip) * 0.01f;
    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }

    fLeftVol = fLeftVol + fLeftVol * ftemp;

    ftemp = ((this->m_pWheelCtl->m_fWheelTractionMag[0] + this->m_pWheelCtl->m_fWheelTractionMag[3]) / 2.0f) * 0.1f;
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

    float fRightPitch = RoadNoiseSpeedToPitch.GetValue(speed);
    float fLeftPitch = fRightPitch;

    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip) * 0.01f;
    if (ftemp > 0.2f) {
        ftemp = 0.2f;
    }

    fLeftPitch = fLeftPitch + fLeftPitch * ftemp;

    ftemp = ((this->m_pWheelCtl->m_fWheelTractionMag[0] + this->m_pWheelCtl->m_fWheelTractionMag[3]) / 2.0f) * 0.15f;
    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }

    fLeftPitch = fLeftPitch + fLeftPitch * ftemp;
    if (fLeftPitch > 6000.0f) {
        fLeftPitch = 6000.0f;
    }

    ftemp = bLength(this->m_pWheelCtl->m_bvTotalRightWheelSlip) * 0.01f;
    if (ftemp > 0.2f) {
        ftemp = 0.2f;
    }

    fRightPitch = fRightPitch + fRightPitch * ftemp;

    ftemp = ((this->m_pWheelCtl->m_fWheelTractionMag[1] + this->m_pWheelCtl->m_fWheelTractionMag[2]) / 2.0f) * 0.15f;
    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }

    fRightPitch = fRightPitch + fRightPitch * ftemp;
    if (fRightPitch > 6000.0f) {
        fRightPitch = 6000.0f;
    }

    this->m_nLTRoadNoiseVol = static_cast<int>(fLeftVol);
    this->m_nRTRoadNoiseVol = static_cast<int>(fRightVol);
    this->m_nLTRoadNoisePitch = static_cast<int>(fLeftPitch);
    this->m_nRTRoadNoisePitch = static_cast<int>(fRightPitch);
}

// UNSOLVED
void CARSFX_RoadNoise::Play(FXROADNOISE_LOOP ID, int side) {
    delete this->m_pRoadNoiseControl[side];
    delete this->m_pStitchLoopControl[side];

    if (ID > FXROADNOISE_LOOP_METAL) {
        unsigned int attribID = 0x4B41DEC8;
        m_pStitchLoopControl[side] = new ("Stitch Loop", 0) cStitchLoop(attribID);
    } else {
        g_pEAXSound->SetCsisName(this);
        m_pRoadNoiseControl[side] = new Csis::FX_ROADNOISE(ID, 0, 0x1000, 0, Csis::FXROADNOISETYPETYPE_LOOP, 0, 0, 25000, 0, 0x7FFF, 0);
        {
            int refcnt = m_pRoadNoiseControl[side]->GetRefCount();
        }
    }
}
