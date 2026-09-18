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

extern int RoadNoiseVolumes[9]; // lo define EAXVolumes.cpp (zEAXSound), como el original

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
#ifdef EA_BUILD_A124
        case FXROADNOISE_LOOP_METAL:
        case FXROADNOISE_LOOP_STITCH_LOOP:
#endif
        default:
            return eVOL_ROADNOISE_ASHPHALT;
#ifndef EA_BUILD_A124
        case FXROADNOISE_LOOP_METAL:
            return eVOL_ROADNOISE_METAL;
        case FXROADNOISE_LOOP_STITCH_LOOP:
            return eVOL_ROADNOISE_STITCH_LOOP;
#endif
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

// UNSOLVED (r47, 1240 B al 97,11 %, ~20 filas): orden del planificador +
// rotacion FPR. Las dos acumulaciones estan INTERCAMBIADAS (objetivo A@102 y
// B@114, nosotros A@114 y B'@134, con f10/f7 cruzados) y los pools en distinto
// orden; lmap da las mismas sentencias (lineas 526-608: mismos clamps,
// fmadds, frsqrte, fnmsubs, bso). Sin sentencia ausente (triage MURO). Eje:
// orden de sentencias/cargas del pool en la fuente (sin probar).
// r60 -- EL EJE ESTA ACOTADO Y ES UN ADELANTO ESPECULATIVO, no una rotacion.
// Con `function_reloc_diffs=none` quedan 16 filas y TODAS cuelgan de lo mismo:
// nosotros subimos la carga del 0,1f (slipBoost) POR ENCIMA del `bso` del sqrt
// del segundo bLength (filas 91 y 94) y ademas el `lwz r11,0x28(r31)` (fila 99);
// el objetivo las tiene DETRAS, agrupadas en 101-105 con las otras del pool.
// r61 -- DOS COSAS RESUELTAS Y UNA ACOTADA. Base de la r61: fncmp `20 insn,
// 73 reub`, 1240/1240 B, .o sha1 689cec08e85957023035739a2b66cdc0a5d8235f.
//
// (1) EL POOL: RESUELTO. La SEMILLA DE POOL de la r59 -- la linea
//     `if (0) { ftemp = 0.15f; }` de mas arriba -- pone el 0,15f DELANTE del
//     0,1f y deja la `.rodata` EXACTAMENTE como la del objetivo:
//       antes  08f0 ... 3c23d70a 3dcccccd / 0900 3e19999a ...   (0,01 0,1 0,15)
//       ahora  08f0 ... 3c23d70a 3e19999a / 0900 3dcccccd ...   (0,01 0,15 0,1)
//       objetivo 0a00 ... 3c23d70a 3e19999a / 0a10 3dcccccd ...
//     `diff` de los dos volcados completos de `.rodata` (1.125 lineas): SOLO
//     esas dos lineas, cero desplazamiento del resto. Coste 0 B de .text.
//     .o sha1 con la semilla sola: fb570764b73d0aa112b2fe22fa4f94ca7cb45b15.
//     CADUCA la linea de la r60 que decia que $LC566/$LC567 estaban
//     intercambiados: con la semilla el emparejamiento es una biyeccion.
//
// (2) EL BLOQUE DE LA CONSTANTE DE slipBoost: RESUELTO. La declaracion tiene
//     que ir DETRAS del `if (ftemp > 0.15f)` (no delante) Y con pin `fr10`.
//     Las dos mitades son necesarias, medido con la metrica POSICIONAL (sin
//     resincronizar; el objeto mide 1240 B en los cuatro casos):
//       base (decl delante del if) ............................. 20 posiciones
//       decl detras del if, SIN pin ............................ 51
//       decl detras del if, `register float slipBoost asm("fr10")` 17
//       igual pero con pin fr8 ... 18   con pin fr9 ... 20
//     Con el pin, el bloque anterior al `bso` casa ENTERO con el objetivo (la
//     primera fila marcada pasa de la 91 a la 95) y fncmp baja de `20 insn` a
//     `17 insn`. .o sha1 del arbol tal y como queda: 551fd1614be66e8b35a91b604ef7a95934572403.
//     OJO: `fndiff` SUBE de 16 a 20 filas marcadas y el fuzzy BAJA de 97,70968
//     a 96,80322 -- es la trampa de `nfsmw-medidas-que-enganan`: su alineador
//     resincroniza y cuenta el mismo defecto dos veces. La medida honrada aqui
//     es `fncmp` (20 -> 17 insn) y el conteo posicional (20 -> 17).
//
// (3) LO QUE QUEDA son 15 instrucciones del MISMO bloque basico (el de union
//     tras el clamp derecho) en OTRO ORDEN, y de ahi cuelga la unica pareja de
//     registros mala (el 3,0f del rsqrt: objetivo f8, nosotros f10, porque
//     nuestro `fmadds` de slipBoost sale ANTES y libera f10).
//       objetivo: lis 0,5 | lis 0,1 | lwz 0x28 | fmaddsA | lfs 0,5 | lfs 0,1 |
//                 lfs 0x54 | lis eps | lfs 0x50 | lis 3,0 | fmuls | lfs eps |
//                 fmadds | lfs 3,0 | fmaddsB
//       nuestro:  lwz 0x28 | lis 0,1 | lfs 0,1 | lis eps | lfs 0x54 | fmaddsA |
//                 lfs 0x50 | lis 0,5 | fmuls | lfs eps | fmadds | lis 3,0 |
//                 fmaddsB | lfs 0,5 | lfs 3,0
//     Con `schedtrace` sobre la mini-TU, el bloque es el `basic block 8` y la
//     lista de listos de sched2 sale `420 384 416 377 393` (mejor el ultimo):
//     gana 393 (`lwz`) y el objetivo empieza por 420 (`high` del 0,5f), o sea
//     que al objetivo le sobra PRIORIDAD en la pareja del 0,5f. Diagnostico:
//     no es el reparto (con el pin el reparto ya casa salvo esa pareja), es
//     `INSN_PRIORITY` en sched. `-fno-schedule-insns` da 281 posiciones y
//     1228 B, `-fno-schedule-insns2` da 69: las dos pasadas hacen falta.
//
// NEGATIVOS de la r61, todos con la metrica POSICIONAL y todos a 1240/1240 B
// (base con semilla = 20, mejor = 17):
//   decl detras del if SIN pin ................................ 51
//   0,1f como literal, sin variable (con la semilla puesta) .... 51
//   decl delante de ACC1 / delante de ACC2, con pin fr10 ....... 19 y 19
//   ACC2 (`fRightVol += fRightVol*slipBoost`) detras del bLength izq . 33
//     -- y el `fmadds` se va al bloque SIGUIENTE: es peor de lo que dice el numero
//   ACC2 detras del `ftemp = ftemp * 0.01f;` ................... 33
//   `asm("" : "+f"(fRightVol))` entre ACC1 y ACC2 .............. 26
//   el mismo x2 ............................................... 29
//   el mismo con clobber "r0" (barrera de ranura) ............. 26
//   `asm("" : "+f"(slipBoost))` delante de ACC2 ............... 18
//   `asm("" : "+f"(fLeftVol))` / `("+f"(ftemp))` delante de ACC1 19 y 19
//   `asm("" : : "f"(slipBoost))` delante del bLength izq ...... 16 EN EL NUMERO
//     pero ADELANTA el fmaddsB a la ranura 100: es la trampa del alineador,
//     estructuralmente PEOR. Detras del bLength: 210 posiciones y 1244 B.
//   `ftemp = ftemp*0.01f` fundido en una sola sentencia ........ 17 (identico)
//   variable propia (`rslip`) para el clamp derecho ........... 21
//   pin `fr11` sobre `ftemp` (con y sin mover la decl) ........ 162
// Banco de la r61: `scratchpad/eax61/h.py` (mini-TU de este .cpp con los cflags
// reales de zEAXSound2, compara CODIFICACIONES contra build/GOWE69/asm, 7 s por
// variante) y `vp.py` (aplica una edicion, mide y REVIERTE).
void CARSFX_RoadNoise::GenerateRoadNoise() {
    float fRightVol = 0.0f;
    float fLeftVol;
    float fRightPitch = 0.0f;
    float fLeftPitch;
    float speed;
    float ftemp;

    speed = this->GetPhysCar()->GetVelocityMagnitudeMPH();
    fLeftVol = static_cast<float>(static_cast<int>(RoadNoiseVolGraph.GetValue(speed)) * 0x7FFF >> 15);

    ftemp = bLength(this->m_pWheelCtl->m_bvTotalRightWheelSlip) * 0.01f;
    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }

    fRightVol = fLeftVol + fLeftVol * ftemp;
    fRightVol = fRightVol + fRightVol * 0.1f;
    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip);
    ftemp = ftemp * 0.01f;
    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }

    fLeftVol = fLeftVol + fLeftVol * ftemp;

    ftemp = (this->m_pWheelCtl->m_fWheelTractionMag[0] + this->m_pWheelCtl->m_fWheelTractionMag[3]) * 0.5f;
    ftemp = ftemp * 0.1f;
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

    fRightPitch = fLeftPitch = RoadNoiseSpeedToPitch.GetValue(speed);

    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip) * 0.01f;
    if (ftemp > 0.2f) {
        ftemp = 0.2f;
    }

    fLeftPitch = fLeftPitch + fLeftPitch * ftemp;

    ftemp = (this->m_pWheelCtl->m_fWheelTractionMag[0] + this->m_pWheelCtl->m_fWheelTractionMag[3]) * 0.5f;
    ftemp = ftemp * 0.15f;
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

    ftemp = (this->m_pWheelCtl->m_fWheelTractionMag[1] + this->m_pWheelCtl->m_fWheelTractionMag[2]) * 0.5f;
    ftemp = ftemp * 0.15f;
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
        {
            m_pRoadNoiseControl[side] = new Csis::FX_ROADNOISE(ID, 0, 0x1000, 0, Csis::FXROADNOISETYPETYPE_LOOP, 0, 0, 25000, 0, 0x7FFF, -1);
            int refcnt = m_pRoadNoiseControl[side]->GetRefCount();
        }
    }
}
