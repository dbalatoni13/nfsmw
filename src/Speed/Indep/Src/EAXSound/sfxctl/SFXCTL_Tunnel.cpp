#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/Data/SND_REVERBFXPARAMS.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

static const int DBGPRNT_REVERBOUT = 0; // size: 0x4, Decl: 31

int ToggleReflection = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 34

int DEBUG_PRINT_TUNNEL = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 37

int DEBUG_REVERB_CHANGES = 0;          // size: 0x4, address: 0xFFFFFFFF, Decl: 40
static const int SPEW_REVERB_INFO = 0; // size: 0x4, Decl: 41

extern ParameterAccessor ReverbAccessor; // size: 0x1C, Decl: 45
eREVERBFX ReverbZoneCrossMap[12];        // size: 0x30, Decl: 46

eTrackPathZoneType SFXCTL_Tunnel::m_PlayerZoneType = TRACK_PATH_ZONE_RESET; // size: 0x4, Decl: 49

bool DebugOcclusionProfile = false; // size: 0x1, address: 0xFFFFFFFF, Decl: 52
uint32 TickerTimeStart = 0;         // size: 0x4, address: 0x80417A10, Decl: 53
uint32 TickerTimeAccum = 0;         // size: 0x4, address: 0x80417A14, Decl: 54

DEFINE_CREATABLE(0x20060, SFXCTL_Tunnel, SFXCTL);

SFXCTL_Tunnel::SFXCTL_Tunnel() {
    this->m_ReverbType = RVRB_SIMPLE_TUNNEL;
    this->m_TargetType = RVRB_SIMPLE_TUNNEL;
    this->m_GinsuDryVol = 0x7F;
    this->m_CurWetGinsu = 0.0f;
    this->m_CurWetAems = 0.0f;
    this->m_CurWetGinsuTarget = 0.0f;
    this->m_CurWetAemsTarget = 0.0f;
    this->m_fIntensity = 0.0f;
    this->m_CurReverbZone = -1;
    this->vDriveByLoc = bVector3(0.0f, 0.0f, 0.0f);
    this->tTimeToWaitBeforeAnotherDriveBy = 0.0f;
    this->tTimeToWaitBeforeAnotherExitDriveBy = 0.0f;
    this->m_GinsuWetVol = 0;
    this->m_AEMSWetVol = 0;
    this->m_AEMSDryVol = 0;
    this->m_CurDryGinsu = 1.0f;
    this->m_CurDryAems = 1.0f;
    this->m_CurDryGinsuTarget = 1.0f;
    this->m_CurDryAemsTarget = 1.0f;
    this->m_PrevReverbZone = 0;
    this->bPlayDriveBy = false;
    this->pLastZoneWePlayedWooshFor = nullptr;
    this->pLastZoneWePlayedExitWooshFor = nullptr;
    this->bFadingOut = false;
    this->bFadingIn = false;
    this->bIsReadyForSwitch = false;
    this->ReflRamp.Initialize(0.0f, 0.0f, 1, LINEAR);
    this->bToggleOffset = false;
    this->m_IsLeadCar = false;
    this->m_LastOcclusionTest = g_pEAXSound->Random(1.0f);
    TickerTimeStart = bGetTicker();
    TickerTimeAccum = 0;
}

SFXCTL_Tunnel::~SFXCTL_Tunnel() {}

void SFXCTL_Tunnel::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXCTL_Tunnel::InitSFX() {
    SFXCTL::InitSFX();
    this->m_bIsInTunnel = false;
    this->m_bWasInTunnel = false;

    if (this->GetPhysCar()->GetContext() == Sound::CONTEXT_PLAYER && this->m_pStateBase->m_InstNum == 0) {
        this->m_IsLeadCar = true;
    } else {
        this->m_IsLeadCar = false;
    }
    this->IsOccluded = false;
}

int SFXCTL_Tunnel::GetController(int Index) {
    return -1;
}

void SFXCTL_Tunnel::AttachController(SFXCTL *psfxctl) {}

static const float TIME_BEFORE_ENTRANCE_FOR_WOOSH = 0.4f; // size: 0x4, Decl: 185
static const float TIME_TO_WAIT_BETWEEN_WOOSH = 3.0f;     // size: 0x4, Decl: 186
extern Slope g_WooshVol_vs_Vel;                           // size: 0x1C, Decl: 187

void SFXCTL_Tunnel::UpdateDriveBySnds(float t) {
    bool InTunnel;
    eTrackPathZoneType PrevFutureZoneType = this->FutureZoneType;
    TrackPathZone *zone;

    this->bPlayDriveBy = false;
    this->bPlayTunnelExit = false;

    this->tTimeToWaitBeforeAnotherDriveBy -= t;
    if (this->tTimeToWaitBeforeAnotherDriveBy < 0.0f) {
        this->tTimeToWaitBeforeAnotherDriveBy = 0.0f;
    }
    this->tTimeToWaitBeforeAnotherExitDriveBy -= t;
    if (this->tTimeToWaitBeforeAnotherExitDriveBy < 0.0f) {
        this->tTimeToWaitBeforeAnotherExitDriveBy = 0.0f;
    }

    this->FutureZoneType = TRACK_PATH_ZONE_RESET;

    const bVector2 *CurCarPos = this->m_pEAXCar->GetPhysCar()->GetPosition2D();
    bVector2 UnNormalCurCarDir(this->m_pEAXCar->GetPhysCar()->GetForwardVector()->x, this->m_pEAXCar->GetPhysCar()->GetForwardVector()->y);
    bVector2 CurCarDir = bNormalize(UnNormalCurCarDir);
    bVector2 FutureCarDir = bScale(CurCarDir, this->m_pEAXCar->GetPhysCar()->GetForwardSpeed() * TIME_BEFORE_ENTRANCE_FOR_WOOSH);
    bVector2 FutureCar2dPos = bAdd(*CurCarPos, FutureCarDir);
    bVector3 FutureCarPos(FutureCar2dPos.x, FutureCar2dPos.y, this->GetPhysCar()->GetPosition()->z);

    this->FutureZoneType = TRACK_PATH_ZONE_TUNNEL;
    zone = this->GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_TUNNEL);
    if (zone == nullptr) {
        this->FutureZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = this->GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_OVERPASS);
        if (zone == nullptr) {
            this->FutureZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = this->GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (zone == nullptr) {
                this->FutureZoneType = TRACK_PATH_ZONE_RESET;
            }
        }
    }

    if (!this->m_bIsInTunnel) {
        if (this->FutureZoneType != TRACK_PATH_ZONE_RESET &&
            ((this->tTimeToWaitBeforeAnotherDriveBy < 0.01f && this->pLastZoneWePlayedWooshFor == zone) || this->pLastZoneWePlayedWooshFor != zone)) {
            if (g_WooshVol_vs_Vel.GetValue(this->m_pEAXCar->GetPhysCar()->GetVelocityMagnitude()) > 0.01f) {
                this->bPlayDriveBy = true;
                this->tTimeToWaitBeforeAnotherDriveBy = TIME_TO_WAIT_BETWEEN_WOOSH;
                this->pLastZoneWePlayedWooshFor = zone;
                this->vDriveByLoc = bVector3(FutureCarPos.x, FutureCarPos.y, this->m_pEAXCar->GetPhysCar()->GetPosition()->z + 10.0f);
                this->m_fIntensity = g_WooshVol_vs_Vel.GetValue(this->m_pEAXCar->GetPhysCar()->GetVelocityMagnitude());

                stDriveByInfo tmpdrivebypackage;
                tmpdrivebypackage.eDriveByType = DRIVE_BY_TUNNEL_IN;
                tmpdrivebypackage.pEAXCar = this->m_pEAXCar;
                tmpdrivebypackage.ClosingVelocity = this->GetPhysCar()->GetVelocityMagnitude();
                tmpdrivebypackage.UniqueID = reinterpret_cast<uintptr_t>(this->pLastZoneWePlayedWooshFor);
                tmpdrivebypackage.vLocation = this->vDriveByLoc;
                CSTATE_Base *ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
                if (ReturnedObj != nullptr) {
                    ReturnedObj->Attach(&tmpdrivebypackage);
                }
                {
                    int break_here;
                }
            }
        }

        if (!this->m_bIsInTunnel) {
            return;
        }
    }

    if (this->FutureZoneType == TRACK_PATH_ZONE_RESET && this->CurZoneType == TRACK_PATH_ZONE_TUNNEL &&
        ((this->tTimeToWaitBeforeAnotherExitDriveBy < 0.01f && this->pLastZoneWePlayedExitWooshFor == zone) ||
         this->pLastZoneWePlayedExitWooshFor != zone)) {
        if (g_WooshVol_vs_Vel.GetValue(this->m_pEAXCar->GetPhysCar()->GetVelocityMagnitude()) > 0.01f) {
            this->bPlayTunnelExit = true;
            this->tTimeToWaitBeforeAnotherExitDriveBy = 3.0f;
            this->pLastZoneWePlayedExitWooshFor = zone;
            this->vDriveByLoc = bVector3(FutureCarPos.x, FutureCarPos.y, this->m_pEAXCar->GetPhysCar()->GetPosition()->z + 10.0f);
            this->m_fExitIntensity = g_WooshVol_vs_Vel.GetValue(this->m_pEAXCar->GetPhysCar()->GetVelocityMagnitude());

            stDriveByInfo tmpdrivebypackage;
            tmpdrivebypackage.eDriveByType = DRIVE_BY_TUNNEL_OUT;
            tmpdrivebypackage.pEAXCar = this->m_pEAXCar;
            tmpdrivebypackage.ClosingVelocity = this->GetPhysCar()->GetVelocityMagnitude();
            tmpdrivebypackage.UniqueID = 0;
            tmpdrivebypackage.vLocation = this->vDriveByLoc;
            CSTATE_Base *ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
            if (ReturnedObj != nullptr) {
                ReturnedObj->Attach(&tmpdrivebypackage);
            }
            {
                int break_here;
            }
        }
    }
}

TrackPathZone *SFXCTL_Tunnel::GetTunnelType(bVector3 &pos, eTrackPathZoneType zonetype) {
    TrackPathZone *zone = nullptr;
    while ((zone = TheTrackPathManager.FindZone(reinterpret_cast<bVector2 *>(&pos), zonetype, zone)) != nullptr) {
        if (zone->GetElevation() == 0.0f) {
            return zone;
        }
        if (zone->GetElevation() >= this->GetPhysCar()->GetPosition()->z) {
            return zone;
        }
    }
    return nullptr;
}

void SFXCTL_Tunnel::UpdateParams(float t) {

    SFXCTL::UpdateParams(t);

    int ninst = this->GetStateBase()->m_InstNum;
    int nother = static_cast<int>(this->GetPhysCar()->GetContext());
    EAXCar *peaxcar_a = g_pEAXSound->GetPlayerTunerCar(ninst);

    this->UpdateIsInTunnel(t);
    this->UpdateDriveBySnds(t);
    this->UpdateCityVerb(t);
    this->UpdateReflectionParams(t);

    // TODO switch?
    nother = static_cast<int>(this->GetPhysCar()->GetContext());
    if (nother != Sound::CONTEXT_AIRACER) {
        if (nother != Sound::CONTEXT_COP) {
            return;
        }
    }
    this->UpdateOcclusion(t);
}

void SFXCTL_Tunnel::UpdateIsInTunnel(float t) {
    this->CurZoneType = TRACK_PATH_ZONE_TUNNEL;
    TrackPathZone *zone = this->GetTunnelType(*this->GetPhysCar()->GetPosition(), TRACK_PATH_ZONE_TUNNEL);
    if (zone == nullptr) {
        this->CurZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = this->GetTunnelType(*this->GetPhysCar()->GetPosition(), TRACK_PATH_ZONE_OVERPASS);
        if (zone == nullptr) {
            this->CurZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = this->GetTunnelType(*this->GetPhysCar()->GetPosition(), TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (zone == nullptr) {
                this->CurZoneType = TRACK_PATH_ZONE_GARAGE;
                zone = this->GetTunnelType(*this->GetPhysCar()->GetPosition(), TRACK_PATH_ZONE_GARAGE);
                if (zone == nullptr) {
                    this->CurZoneType = TRACK_PATH_ZONE_DYNAMIC;
                    zone = this->GetTunnelType(*this->GetPhysCar()->GetPosition(), TRACK_PATH_ZONE_DYNAMIC);
                    if (zone != nullptr) {
                        if (zone->GetVisitInfo() == 1) {
                            zone = nullptr;
                        }
                    }
                }
            }
        }
    }

    bool InTunnel;
    if (zone == nullptr) {
        InTunnel = false;
        this->CurZoneType = TRACK_PATH_ZONE_RESET;
    } else {
        InTunnel = true;
        if (!this->m_bIsInTunnel) {
            this->m_bIsInTunnel = true;
            if (this->GetPhysCar()->GetContext() == Sound::CONTEXT_PLAYER) {
                eREVERBFX NewVerbType;
                MMiscSound(1).Send(UCrc32("TunnelUpdate"));

                switch (this->CurZoneType) {
                    case TRACK_PATH_ZONE_OVERPASS:
                    case TRACK_PATH_ZONE_OVERPASS_SMALL:
                        NewVerbType = RVRB_SIMPLE_TUNNEL_SML;
                        break;
                    case TRACK_PATH_ZONE_TUNNEL:
                        NewVerbType = RVRB_SIMPLE_TUNNEL;
                        break;
                    case TRACK_PATH_ZONE_GARAGE:
                    case TRACK_PATH_ZONE_DYNAMIC:
                    default:
                        NewVerbType = RVRB_GARAGE;
                        break;
                }

                this->SetCurrentReverbType(NewVerbType, 0);
            }
        }
    }

    if (!InTunnel && this->m_bIsInTunnel) {
        this->m_bIsInTunnel = false;
        if (this->GetPhysCar()->GetContext() == Sound::CONTEXT_PLAYER) {
            MMiscSound(0).Send(UCrc32("TunnelUpdate"));
            this->EndTunnelVerb();
        }
    }

    this->m_bWasInTunnel = this->m_bIsInTunnel;
    if (this->GetPhysCar()->IsLocalPlayerCar()) {
        this->m_PlayerZoneType = this->CurZoneType;
    }
}

float TimeBetweenOcclusionTests = 0.5f;  // size: 0x4, address: 0x80417A28, Decl: 503
float MaxDistanceToOccludeTest = 100.0f; // size: 0x4, address: 0x80417A2C, Decl: 504

void SFXCTL_Tunnel::UpdateOcclusion(float t) {
    if (SndCamera::NumPlayers > 1) {
        return;
    }

    this->m_LastOcclusionTest -= t;
    if (this->m_LastOcclusionTest > 0.0f) {
        return;
    }

    this->m_LastOcclusionTest = TimeBetweenOcclusionTests;
    if (m_PlayerZoneType != this->CurZoneType) {
        if (m_PlayerZoneType == TRACK_PATH_ZONE_TUNNEL || this->CurZoneType == TRACK_PATH_ZONE_TUNNEL) {
            UMath::Vector4 originToBarrier[2];
            UMath::Vector4 DirectionVec;

            originToBarrier[0].x = -SndCamera::GetWorldCarPos3(0)->y;
            originToBarrier[0].y = SndCamera::GetWorldCarPos3(0)->z;
            originToBarrier[0].z = SndCamera::GetWorldCarPos3(0)->x;

            originToBarrier[1].z = this->GetPhysCar()->GetPosition()->x;
            originToBarrier[1].x = -this->GetPhysCar()->GetPosition()->y;
            originToBarrier[1].y = this->GetPhysCar()->GetPosition()->z;

            float fTestDist = UMath::Distancexyz(originToBarrier[0], originToBarrier[1]);
            if (fTestDist > MaxDistanceToOccludeTest) {
                fTestDist = MaxDistanceToOccludeTest;
            }

            originToBarrier[0].y = originToBarrier[0].y + 2.0f;
            UMath::Sub(originToBarrier[0], originToBarrier[1], DirectionVec);
            UMath::Unit(DirectionVec, DirectionVec);
            VU0_v4scaleadd(originToBarrier[1], fTestDist, originToBarrier[0], DirectionVec);

            WCollisionMgr::WorldCollisionInfo cInfo;
            if (WCollisionMgr(0, 3).CheckHitWorld(originToBarrier, cInfo, 2) != 0) {
                if (UMath::DistanceSquarexyz(originToBarrier[0], cInfo.fCollidePt) < fTestDist * fTestDist - 9.0f) {
                    this->IsOccluded = true;
                    return;
                }
                this->IsOccluded = false;
                return;
            }
            this->IsOccluded = false;
            return;
        }
    }

    this->IsOccluded = false;
}

void SFXCTL_Tunnel::UpdateMixerOutputs() {
    this->SetDMIX_Input(0, 0);
    this->SetDMIX_Input(1, 0);
    this->SetDMIX_Input(2, 0);
    this->SetDMIX_Input(3, 0);

    this->SetDMIX_Input(4, static_cast<int>(this->IsOccluded) != 0 ? 0x7FFF : 0);

    if (this->GetPhysCar()->GetContext() == Sound::CONTEXT_PLAYER) {
        this->SetDMIX_Input(5, this->m_AEMSDryVol);
        this->SetDMIX_Input(6, this->m_AEMSWetVol);
    }

    if (!this->m_bIsInTunnel) {
        return;
    }

    switch (this->CurZoneType) {
        case TRACK_PATH_ZONE_GARAGE:
            this->SetDMIX_Input(2, 0x7FFF);
            break;

        case TRACK_PATH_ZONE_OVERPASS:
        case TRACK_PATH_ZONE_OVERPASS_SMALL:
            this->SetDMIX_Input(1, 0x7FFF);
            break;

        case TRACK_PATH_ZONE_RESET:
        default:
            this->SetDMIX_Input(0, 0x7FFF);
            break;
    }
    this->SetDMIX_Input(3, 0x7FFF);
}

void SFXCTL_Tunnel::UpdateCityVerb(float t) {
    int ncurrentoffset = 0;
    this->m_PrevReverbZone = this->m_CurReverbZone;

    if (ReverbAccessor.IsValid()) {
        int nFXOffset;

        if ((this->m_pEAXCar != nullptr) && (this->GetGroupID() == 2)) {
            EAX_CarState *pcar = this->m_pEAXCar->GetPhysCar();
            if (pcar != nullptr) {
                const bVector2 *ppos = pcar->GetPosition2D();

                ReverbAccessor.CaptureData(ppos->x, ppos->y);
                this->m_CurReverbZone = ReverbAccessor.GetDataInt(0);
                nFXOffset = ReverbAccessor.GetDataInt(2);
                ncurrentoffset = nFXOffset;
            }
        }
    }

    if (static_cast<unsigned int>(this->m_CurReverbZone) >= RVRB_MAX_TYPES) {
        if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
            this->m_CurReverbZone = RVRB_CARSHOW_SML;
        } else {
            this->m_CurReverbZone = RVRB_CITYOPEN;
        }
    }

    eREVERBFX currentverb = static_cast<eREVERBFX>(ReverbZoneCrossMap[this->m_CurReverbZone]);
    if (currentverb >= RVRB_MAX_TYPES) {
        currentverb = RVRB_CITYOPEN;
    }

    if ((this->m_CurReverbZone != this->m_PrevReverbZone) && !this->IsInTunnel()) {
        this->SetCurrentReverbType(currentverb, ncurrentoffset);
    } else {
        this->AdjustReverbOffset(ncurrentoffset);
    }
}

void SFXCTL_Tunnel::Destroy() {}

void SFXCTL_Tunnel::EndTunnelVerb() {
    eREVERBFX currentverb = static_cast<eREVERBFX>(ReverbZoneCrossMap[this->m_CurReverbZone]);
    if (currentverb >= RVRB_MAX_TYPES) {
        currentverb = RVRB_CITYOPEN;
    }
    this->SetCurrentReverbType(currentverb, 0);
}

void SFXCTL_Tunnel::AdjustReverbOffset(int reverboffset) {
    if (this->bFadingOut || this->bFadingIn) {
        return;
    }
    this->m_ReverbOffset = static_cast<float>(reverboffset);

    int nQGinWetTarget = bClamp(g_REVERBFXMODULES[this->m_ReverbType].GinsuWet + reverboffset, -10000, 0);
    int nQAemsWetTarget = bClamp(g_REVERBFXMODULES[this->m_ReverbType].AemsWet + reverboffset, -10000, 0);

    this->m_CurWetGinsuTarget = NFSMixShape::GetFloatFromHundredthsdB(nQGinWetTarget);
    this->m_CurWetAemsTarget = NFSMixShape::GetFloatFromHundredthsdB(nQAemsWetTarget);
}

void SFXCTL_Tunnel::SetCurrentReverbType(eREVERBFX ereverbtype, int reverboffset) {
    this->bFadingOut = true;
    this->m_ReverbOffset = static_cast<float>(reverboffset);
    this->m_TargetType = ereverbtype;
    this->ReflRamp.Initialize(0.0f, 1.0f, g_REVERBFXMODULES[this->m_ReverbType].FadeOut, LINEAR);
    this->m_CurWetGinsuTarget = 0.0f;
    this->m_CurDryGinsuTarget = 1.0f;
    this->m_CurWetAemsTarget = 0.0f;
    this->m_CurDryAemsTarget = 1.0f;
}

void SFXCTL_Tunnel::UpdateReflectionParams(float t) {
    this->bIsReadyForSwitch = false;
    this->ReflRamp.Update(t);

    if (this->bFadingOut) {
        if (this->ReflRamp.IsFinished() && !this->bIsTunnelRamping) {
            if (this->m_IsLeadCar) {
                this->bIsReadyForSwitch = true;
            }
            this->bFadingIn = true;
            this->m_ReverbType = this->m_TargetType;
            this->bFadingOut = false;
            this->ReflRamp.Initialize(0.0f, 1.0f, g_REVERBFXMODULES[this->m_TargetType].FadeIn, LINEAR);

            this->m_CurWetGinsu = this->m_CurWetGinsuTarget;
            this->m_CurDryGinsu = this->m_CurDryGinsuTarget;
            this->m_CurWetAems = this->m_CurWetAemsTarget;
            this->m_CurDryAems = this->m_CurDryAemsTarget;

            int nQGinWetTarget = bClamp(g_REVERBFXMODULES[this->m_ReverbType].GinsuWet + static_cast<int>(this->m_ReverbOffset), -10000, 0);
            int nQAemsWetTarget = bClamp(g_REVERBFXMODULES[this->m_ReverbType].AemsWet + static_cast<int>(this->m_ReverbOffset), -10000, 0);

            this->m_CurWetGinsuTarget = NFSMixShape::GetFloatFromHundredthsdB(nQGinWetTarget);
            this->m_CurWetAemsTarget = NFSMixShape::GetFloatFromHundredthsdB(nQAemsWetTarget);
            this->m_CurDryGinsuTarget = NFSMixShape::GetFloatFromHundredthsdB(g_REVERBFXMODULES[this->m_ReverbType].GinsuDry);
            this->m_CurDryAemsTarget = NFSMixShape::GetFloatFromHundredthsdB(g_REVERBFXMODULES[this->m_ReverbType].AemsDry);
        }
    } else if (this->bFadingIn && this->ReflRamp.IsFinished()) {
        this->bFadingIn = false;
        this->m_CurWetGinsu = this->m_CurWetGinsuTarget;
        this->m_CurDryGinsu = this->m_CurDryGinsuTarget;
        this->m_CurWetAems = this->m_CurWetAemsTarget;
        this->m_CurDryAems = this->m_CurDryAemsTarget;
    }

    if (this->bFadingOut || this->bFadingIn) {
        this->m_GinsuWetVol =
            static_cast<int>(((this->m_CurWetGinsuTarget - this->m_CurWetGinsu) * this->ReflRamp.GetValue() + this->m_CurWetGinsu) * 32767.0f);
        this->m_GinsuDryVol =
            static_cast<int>(((this->m_CurDryGinsuTarget - this->m_CurDryGinsu) * this->ReflRamp.GetValue() + this->m_CurDryGinsu) * 32767.0f);
        this->m_AEMSWetVol =
            static_cast<int>(((this->m_CurWetAemsTarget - this->m_CurWetAems) * this->ReflRamp.GetValue() + this->m_CurWetAems) * 32767.0f);
        this->m_AEMSDryVol =
            static_cast<int>(((this->m_CurDryAemsTarget - this->m_CurDryAems) * this->ReflRamp.GetValue() + this->m_CurDryAems) * 32767.0f);
    } else {
        this->m_CurWetGinsu = smooth(this->m_CurWetGinsu, this->m_CurWetGinsuTarget, 0.25f);
        this->m_CurDryGinsu = smooth(this->m_CurDryGinsu, this->m_CurDryGinsuTarget, 0.25f);
        this->m_CurWetAems = smooth(this->m_CurWetAems, this->m_CurWetAemsTarget, 0.25f);
        this->m_CurDryAems = smooth(this->m_CurDryAems, this->m_CurDryAemsTarget, 0.25f);

        this->m_GinsuWetVol = static_cast<int>(this->m_CurWetGinsu * 32767.0f);
        this->m_GinsuDryVol = static_cast<int>(this->m_CurDryGinsu * 32767.0f);
        this->m_AEMSWetVol = static_cast<int>(this->m_CurWetAems * 32767.0f);
        this->m_AEMSDryVol = static_cast<int>(this->m_CurDryAems * 32767.0f);
    }

    if (this->bToggleOffset) {
        this->m_AEMSWetVol = bClamp(++this->m_AEMSWetVol, 0, 0x7FFF);
        this->m_GinsuWetVol = bClamp(++this->m_GinsuWetVol, 0, 0x7FFF);
        this->bToggleOffset = false;
    } else {
        this->bToggleOffset = true;
    }

    this->bIsTunnelRamping = !this->ReflRamp.IsFinished();
}
