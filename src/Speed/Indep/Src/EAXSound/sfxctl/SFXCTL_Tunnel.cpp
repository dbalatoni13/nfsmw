#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

void VU0_v4sub(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);

extern Slope g_WooshVol_vs_Vel;
extern float GetFloatFromHundredthsdB__11NFSMixShapei(int ndB);

extern ParameterAccessor ReverbAccessor;
extern int ReverbZoneCrossMap[];
extern float TimeBetweenOcclusionTests;
extern float MaxDistanceToOccludeTest;
extern unsigned int TickerTimeStart;
extern unsigned int TickerTimeAccum;
extern unsigned int bGetTicker();

extern stREVERB_PARAMS g_REVERBFXMODULES[];

eTrackPathZoneType SFXCTL_Tunnel::m_PlayerZoneType = TRACK_PATH_ZONE_RESET;

SndBase::TypeInfo *SFXCTL_Tunnel::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_Tunnel::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Tunnel::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Tunnel::GetStaticTypeInfo()->typeName, false) SFXCTL_Tunnel();
    }
    return new (SFXCTL_Tunnel::GetStaticTypeInfo()->typeName, true) SFXCTL_Tunnel();
}

SFXCTL_Tunnel::SFXCTL_Tunnel() {
    this->m_ReverbType = static_cast<eREVERBFX>(5);
    this->m_TargetType = static_cast<eREVERBFX>(5);
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

    if (this->GetPhysCar()->GetContext() == Sound::kRaceContext_QuickRace && this->m_pStateBase->m_InstNum == 0) {
        this->m_IsLeadCar = true;
    } else {
        this->m_IsLeadCar = false;
    }
    this->IsOccluded = false;
}

int SFXCTL_Tunnel::GetController(int Index) { return -1; }

void SFXCTL_Tunnel::AttachController(SFXCTL *psfxctl) {}

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

    (void)PrevFutureZoneType;
    this->FutureZoneType = TRACK_PATH_ZONE_RESET;

    const bVector2 *CurCarPos = this->m_pEAXCar->GetPhysCar()->GetPosition2D();
    bVector2 UnNormalCurCarDir(
        this->m_pEAXCar->GetPhysCar()->GetForwardVector()->x,
        this->m_pEAXCar->GetPhysCar()->GetForwardVector()->y);
    bVector2 CurCarDir = bNormalize(UnNormalCurCarDir);
    bVector2 FutureCarDir = bScale(CurCarDir, this->m_pEAXCar->GetPhysCar()->GetForwardSpeed() * 0.4f);
    bVector2 FutureCar2dPos = bAdd(*CurCarPos, FutureCarDir);
    bVector3 FutureCarPos(
        FutureCar2dPos.x,
        FutureCar2dPos.y,
        this->GetPhysCar()->GetPosition()->z);

    this->FutureZoneType = TRACK_PATH_ZONE_TUNNEL;
    zone = this->GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_TUNNEL);
    if (!zone) {
        this->FutureZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = this->GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_OVERPASS);
        if (!zone) {
            this->FutureZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = this->GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (!zone) {
                this->FutureZoneType = TRACK_PATH_ZONE_RESET;
            }
        }
    }

    if (!this->m_bIsInTunnel) {
        if (this->FutureZoneType != TRACK_PATH_ZONE_RESET &&
            ((this->tTimeToWaitBeforeAnotherDriveBy < 0.01f && this->pLastZoneWePlayedWooshFor == zone) ||
             this->pLastZoneWePlayedWooshFor != zone)) {
            if (g_WooshVol_vs_Vel.GetValue(this->m_pEAXCar->GetPhysCar()->GetVelocityMagnitude()) > 0.01f) {
                this->bPlayDriveBy = true;
                this->tTimeToWaitBeforeAnotherDriveBy = 3.0f;
                this->pLastZoneWePlayedWooshFor = zone;
                this->vDriveByLoc = bVector3(FutureCarPos.x, FutureCarPos.y, this->m_pEAXCar->GetPhysCar()->GetPosition()->z + 10.0f);
                this->m_fIntensity = g_WooshVol_vs_Vel.GetValue(this->m_pEAXCar->GetPhysCar()->GetVelocityMagnitude());
                stDriveByInfo tmpdrivebypackage;
                tmpdrivebypackage.eDriveByType = DRIVE_BY_TUNNEL_IN;
                tmpdrivebypackage.pEAXCar = this->m_pEAXCar;
                tmpdrivebypackage.ClosingVelocity = this->GetPhysCar()->GetVelocityMagnitude();
                tmpdrivebypackage.UniqueID = reinterpret_cast<unsigned int>(this->pLastZoneWePlayedWooshFor);
                tmpdrivebypackage.vLocation = this->vDriveByLoc;
                CSTATE_Base *ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
                if (ReturnedObj) {
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

    if (this->FutureZoneType == TRACK_PATH_ZONE_RESET &&
        this->CurZoneType == TRACK_PATH_ZONE_TUNNEL &&
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
            if (ReturnedObj) {
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
    while ((zone = TheTrackPathManager.FindZone(static_cast<bVector2 *>(static_cast<void *>(&pos)), zonetype, zone))) {
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
    int ninst;
    int nother;
    EAXCar *peaxcar_a;

    SFXCTL::UpdateParams(t);

    ninst = this->GetStateBase()->m_InstNum;
    nother = static_cast<int>(this->GetPhysCar()->GetContext());
    peaxcar_a = g_pEAXSound->GetPlayerTunerCar(ninst);

    this->UpdateIsInTunnel(t);
    this->UpdateDriveBySnds(t);
    this->UpdateCityVerb(t);
    this->UpdateReflectionParams(t);

    nother = static_cast<int>(this->GetPhysCar()->GetContext());
    if (nother != kRaceContext_Online) {
        if (nother != kRaceContext_Career) {
            return;
        }
    }
    this->UpdateOcclusion(t);
}

void SFXCTL_Tunnel::UpdateIsInTunnel(float t) {
    (void)t;

    TrackPathZone *zone;
    bool InTunnel;

    this->CurZoneType = TRACK_PATH_ZONE_TUNNEL;
    zone = this->GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&this->m_pStateBase->GetPhysCar()->mMatrix.v3)),
                         TRACK_PATH_ZONE_TUNNEL);
    if (!zone) {
        this->CurZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = this->GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&this->m_pStateBase->GetPhysCar()->mMatrix.v3)),
                             TRACK_PATH_ZONE_OVERPASS);
        if (!zone) {
            this->CurZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = this->GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&this->m_pStateBase->GetPhysCar()->mMatrix.v3)),
                                 TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (!zone) {
                this->CurZoneType = TRACK_PATH_ZONE_GARAGE;
                zone = this->GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&this->m_pStateBase->GetPhysCar()->mMatrix.v3)),
                                     TRACK_PATH_ZONE_GARAGE);
                if (!zone) {
                    this->CurZoneType = TRACK_PATH_ZONE_DYNAMIC;
                    zone = this->GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&this->m_pStateBase->GetPhysCar()->mMatrix.v3)),
                                         TRACK_PATH_ZONE_DYNAMIC);
                    if (zone) {
                        if (zone->VisitInfo == 1) {
                            zone = nullptr;
                        }
                        if (zone) {
                            goto LAB_IN_TUNNEL;
                        }
                    }
                    InTunnel = false;
                    this->CurZoneType = TRACK_PATH_ZONE_RESET;
                    goto LAB_UPDATE_END;
                }
            }
        }
    }

LAB_IN_TUNNEL:
    InTunnel = true;
    if (!this->m_bIsInTunnel) {
        this->m_bIsInTunnel = true;
        if (static_cast<int>(this->m_pStateBase->GetPhysCar()->mContext) == 0) {
            MMiscSound(1).Send(UCrc32("TunnelUpdate"));

            eREVERBFX NewVerbType;
            if (this->CurZoneType > TRACK_PATH_ZONE_OVERPASS_SMALL) {
LAB_DEFAULT_VERB:
                NewVerbType = static_cast<eREVERBFX>(3);
            } else {
                if (this->CurZoneType >= TRACK_PATH_ZONE_OVERPASS) {
                    NewVerbType = static_cast<eREVERBFX>(6);
                } else if (this->CurZoneType == TRACK_PATH_ZONE_TUNNEL) {
                    NewVerbType = static_cast<eREVERBFX>(5);
                } else {
                    goto LAB_DEFAULT_VERB;
                }
            }
            this->SetCurrentReverbType(NewVerbType, 0);
        }
    }

LAB_UPDATE_END:
    if (!InTunnel && this->m_bIsInTunnel) {
        this->m_bIsInTunnel = false;
        if (static_cast<int>(this->m_pStateBase->GetPhysCar()->mContext) == 0) {
            MMiscSound(0).Send(UCrc32("TunnelUpdate"));
            this->EndTunnelVerb();
        }
    }

    this->m_bWasInTunnel = this->m_bIsInTunnel;
    if (static_cast<int>(this->m_pStateBase->GetPhysCar()->mContext) == 0) {
        m_PlayerZoneType = this->CurZoneType;
    }
}

void SFXCTL_Tunnel::UpdateOcclusion(float t) {
    if (SndCamera::NumPlayers > 1) {
        return;
    }

    this->m_LastOcclusionTest -= t;
    if (this->m_LastOcclusionTest > 0.0f) {
        return;
    }

    this->m_LastOcclusionTest = TimeBetweenOcclusionTests;
    if (m_PlayerZoneType == this->CurZoneType) {
        this->IsOccluded = false;
        return;
    }

    if (m_PlayerZoneType != TRACK_PATH_ZONE_TUNNEL && this->CurZoneType != TRACK_PATH_ZONE_TUNNEL) {
        this->IsOccluded = false;
        return;
    }

    {
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
        if (WCollisionMgr(0, 3).CheckHitWorld(originToBarrier, cInfo, 2)) {
            if (!(UMath::DistanceSquarexyz(originToBarrier[0], cInfo.fCollidePt) >= fTestDist * fTestDist - 9.0f)) {
                this->IsOccluded = true;
                return;
            }
        }

        this->IsOccluded = false;
    }
}

void SFXCTL_Tunnel::UpdateMixerOutputs() {
    this->SetDMIX_Input(0, 0);
    this->SetDMIX_Input(1, 0);
    this->SetDMIX_Input(2, 0);
    this->SetDMIX_Input(3, 0);

    this->SetDMIX_Input(4, this->IsOccluded != 0 ? 0x7FFF : 0);

    if (static_cast<int>(this->m_pStateBase->GetPhysCar()->mContext) == 0) {
        this->SetDMIX_Input(5, this->m_AEMSDryVol);
        this->SetDMIX_Input(6, this->m_AEMSWetVol);
    }

    if (!this->m_bIsInTunnel) {
        return;
    }

    if (this->CurZoneType < TRACK_PATH_ZONE_STREAMER_PREDICTION) {
        if (this->CurZoneType >= TRACK_PATH_ZONE_OVERPASS) {
            this->SetDMIX_Input(1, 0x7FFF);
        } else {
            this->SetDMIX_Input(0, 0x7FFF);
        }
    } else {
        if (this->CurZoneType == TRACK_PATH_ZONE_GARAGE) {
            this->SetDMIX_Input(2, 0x7FFF);
        } else {
            this->SetDMIX_Input(0, 0x7FFF);
        }
    }

    this->SetDMIX_Input(3, 0x7FFF);
}

void SFXCTL_Tunnel::UpdateCityVerb(float t) {
    (void)t;

    int ncurrentoffset = 0;
    int isValid = 1;
    this->m_PrevReverbZone = this->m_CurReverbZone;

    if (!ReverbAccessor.Layer) {
        isValid = 0;
    }

    if ((isValid != 0) &&
        (this->m_pEAXCar) &&
        (((this->GetUniqueID() >> 16) & 0xFF) == 2)) {
        EAX_CarState *pcar = this->m_pStateBase->GetPhysCar();
        if (pcar) {
            ReverbAccessor.CaptureData(pcar->mMatrix.v3.x, pcar->mMatrix.v3.y);
            this->m_CurReverbZone = ReverbAccessor.GetDataInt(0);
            ncurrentoffset = ReverbAccessor.GetDataInt(2);
        }
    }

    if (static_cast<unsigned int>(this->m_CurReverbZone) > 0xB) {
        int zone;
        if (g_pEAXSound->GetSoundGameMode() == SND_FRONTEND) {
            zone = 0;
        } else {
            zone = 9;
        }
        this->m_CurReverbZone = zone;
    }

    eREVERBFX currentverb = static_cast<eREVERBFX>(ReverbZoneCrossMap[this->m_CurReverbZone]);
    if (static_cast<int>(currentverb) > 0xB) {
        currentverb = static_cast<eREVERBFX>(9);
    }

    if ((this->m_CurReverbZone != this->m_PrevReverbZone) && !this->m_bIsInTunnel) {
        this->SetCurrentReverbType(currentverb, ncurrentoffset);
    } else {
        this->AdjustReverbOffset(ncurrentoffset);
    }
}

void SFXCTL_Tunnel::Destroy() {}

void SFXCTL_Tunnel::EndTunnelVerb() {
    eREVERBFX currentverb = static_cast<eREVERBFX>(ReverbZoneCrossMap[this->m_CurReverbZone]);
    if (static_cast<int>(currentverb) > 0xB) {
        currentverb = static_cast<eREVERBFX>(9);
    }
    this->SetCurrentReverbType(currentverb, 0);
}

void SFXCTL_Tunnel::AdjustReverbOffset(int reverboffset) {
    if (!this->bFadingOut && !this->bFadingIn) {
        this->m_ReverbOffset = static_cast<float>(reverboffset);

        int ndBGinsu = -10000;
        int ginsuWet = g_REVERBFXMODULES[this->m_ReverbType].GinsuWet + reverboffset;
        if (ginsuWet > -10000) {
            ndBGinsu = ginsuWet;
        }
        if (ndBGinsu > 0) {
            ndBGinsu = 0;
        }

        int ndBAems = -10000;
        int aemsWet = g_REVERBFXMODULES[this->m_ReverbType].AemsWet + reverboffset;
        if (aemsWet > -10000) {
            ndBAems = aemsWet;
        }
        if (ndBAems > 0) {
            ndBAems = 0;
        }

        this->m_CurWetGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(ndBGinsu);
        this->m_CurWetAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(ndBAems);
    }
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
        if (this->ReflRamp.IsFinished() &&
            this->bIsTunnelRamping == 0) {
            if (this->m_IsLeadCar != 0) {
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

            int nQGinWetTarget = bClamp(
                g_REVERBFXMODULES[this->m_ReverbType].GinsuWet + static_cast<int>(this->m_ReverbOffset), -10000, 0);
            int nQAemsWetTarget = bClamp(
                g_REVERBFXMODULES[this->m_ReverbType].AemsWet + static_cast<int>(this->m_ReverbOffset), -10000, 0);

            this->m_CurWetGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(nQGinWetTarget);
            this->m_CurWetAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(nQAemsWetTarget);
            this->m_CurDryGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(g_REVERBFXMODULES[this->m_ReverbType].GinsuDry);
            this->m_CurDryAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(g_REVERBFXMODULES[this->m_ReverbType].AemsDry);
        }
    } else if (this->bFadingIn &&
               this->ReflRamp.IsFinished()) {
        this->bFadingIn = false;
        this->m_CurWetGinsu = this->m_CurWetGinsuTarget;
        this->m_CurDryGinsu = this->m_CurDryGinsuTarget;
        this->m_CurWetAems = this->m_CurWetAemsTarget;
        this->m_CurDryAems = this->m_CurDryAemsTarget;
    }

    if (this->bFadingOut || this->bFadingIn) {
        this->m_GinsuWetVol = static_cast<int>(((this->m_CurWetGinsuTarget - this->m_CurWetGinsu) * this->ReflRamp.GetValue() + this->m_CurWetGinsu) * 32767.0f);
        this->m_GinsuDryVol = static_cast<int>(((this->m_CurDryGinsuTarget - this->m_CurDryGinsu) * this->ReflRamp.GetValue() + this->m_CurDryGinsu) * 32767.0f);
        this->m_AEMSWetVol = static_cast<int>(((this->m_CurWetAemsTarget - this->m_CurWetAems) * this->ReflRamp.GetValue() + this->m_CurWetAems) * 32767.0f);
        this->m_AEMSDryVol = static_cast<int>(((this->m_CurDryAemsTarget - this->m_CurDryAems) * this->ReflRamp.GetValue() + this->m_CurDryAems) * 32767.0f);
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
        this->m_AEMSWetVol = bClamp(this->m_AEMSWetVol + 1, 0, 0x7FFF);
        this->m_GinsuWetVol = bClamp(this->m_GinsuWetVol + 1, 0, 0x7FFF);
        this->bToggleOffset = false;
    } else {
        this->bToggleOffset = true;
    }

    this->bIsTunnelRamping = this->ReflRamp.IsFinished() ^ 1;
}
