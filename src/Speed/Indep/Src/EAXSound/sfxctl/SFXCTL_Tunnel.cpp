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
    m_ReverbType = static_cast<eREVERBFX>(5);
    m_TargetType = static_cast<eREVERBFX>(5);
    m_GinsuDryVol = 0x7F;
    m_CurWetGinsu = 0.0f;
    m_CurWetAems = 0.0f;
    m_CurWetGinsuTarget = 0.0f;
    m_CurWetAemsTarget = 0.0f;
    m_fIntensity = 0.0f;
    m_CurReverbZone = -1;
    vDriveByLoc = bVector3(0.0f, 0.0f, 0.0f);
    tTimeToWaitBeforeAnotherDriveBy = 0.0f;
    tTimeToWaitBeforeAnotherExitDriveBy = 0.0f;
    m_GinsuWetVol = 0;
    m_AEMSWetVol = 0;
    m_AEMSDryVol = 0;
    m_CurDryGinsu = 1.0f;
    m_CurDryAems = 1.0f;
    m_CurDryGinsuTarget = 1.0f;
    m_CurDryAemsTarget = 1.0f;
    m_PrevReverbZone = 0;
    bPlayDriveBy = false;
    pLastZoneWePlayedWooshFor = nullptr;
    pLastZoneWePlayedExitWooshFor = nullptr;
    bFadingOut = false;
    bFadingIn = false;
    bIsReadyForSwitch = false;
    ReflRamp.Initialize(0.0f, 0.0f, 1, LINEAR);
    bToggleOffset = false;
    m_IsLeadCar = false;
    m_LastOcclusionTest = g_pEAXSound->Random(1.0f);
    TickerTimeStart = bGetTicker();
    TickerTimeAccum = 0;
}

SFXCTL_Tunnel::~SFXCTL_Tunnel() {}

void SFXCTL_Tunnel::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXCTL_Tunnel::InitSFX() {
    SFXCTL::InitSFX();
    m_bIsInTunnel = false;
    m_bWasInTunnel = false;

    if (GetPhysCar()->GetContext() == Sound::kRaceContext_QuickRace && m_pStateBase->m_InstNum == 0) {
        m_IsLeadCar = true;
    } else {
        m_IsLeadCar = false;
    }
    IsOccluded = false;
}

int SFXCTL_Tunnel::GetController(int Index) { return -1; }

void SFXCTL_Tunnel::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Tunnel::UpdateDriveBySnds(float t) {
    bool InTunnel;
    eTrackPathZoneType PrevFutureZoneType = FutureZoneType;
    TrackPathZone *zone;

    bPlayDriveBy = false;
    bPlayTunnelExit = false;

    tTimeToWaitBeforeAnotherDriveBy -= t;
    if (tTimeToWaitBeforeAnotherDriveBy < 0.0f) {
        tTimeToWaitBeforeAnotherDriveBy = 0.0f;
    }
    tTimeToWaitBeforeAnotherExitDriveBy -= t;
    if (tTimeToWaitBeforeAnotherExitDriveBy < 0.0f) {
        tTimeToWaitBeforeAnotherExitDriveBy = 0.0f;
    }

    (void)PrevFutureZoneType;
    FutureZoneType = TRACK_PATH_ZONE_RESET;

    const bVector2 *CurCarPos = m_pEAXCar->GetPhysCar()->GetPosition2D();
    bVector2 UnNormalCurCarDir(
        m_pEAXCar->GetPhysCar()->GetForwardVector()->x,
        m_pEAXCar->GetPhysCar()->GetForwardVector()->y);
    bVector2 CurCarDir = bNormalize(UnNormalCurCarDir);
    bVector2 FutureCarDir = bScale(CurCarDir, m_pEAXCar->GetPhysCar()->GetForwardSpeed() * 0.4f);
    bVector2 FutureCar2dPos = bAdd(*CurCarPos, FutureCarDir);
    bVector3 FutureCarPos(
        FutureCar2dPos.x,
        FutureCar2dPos.y,
        GetPhysCar()->GetPosition()->z);

    FutureZoneType = TRACK_PATH_ZONE_TUNNEL;
    zone = GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_TUNNEL);
    if (!zone) {
        FutureZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_OVERPASS);
        if (!zone) {
            FutureZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (!zone) {
                FutureZoneType = TRACK_PATH_ZONE_RESET;
            }
        }
    }

    if (!m_bIsInTunnel) {
        if (FutureZoneType != TRACK_PATH_ZONE_RESET &&
            ((tTimeToWaitBeforeAnotherDriveBy < 0.01f && pLastZoneWePlayedWooshFor == zone) ||
             pLastZoneWePlayedWooshFor != zone)) {
            if (g_WooshVol_vs_Vel.GetValue(m_pEAXCar->GetPhysCar()->GetVelocityMagnitude()) > 0.01f) {
                bPlayDriveBy = true;
                tTimeToWaitBeforeAnotherDriveBy = 3.0f;
                pLastZoneWePlayedWooshFor = zone;
                vDriveByLoc = bVector3(FutureCarPos.x, FutureCarPos.y, m_pEAXCar->GetPhysCar()->GetPosition()->z + 10.0f);
                m_fIntensity = g_WooshVol_vs_Vel.GetValue(m_pEAXCar->GetPhysCar()->GetVelocityMagnitude());
                stDriveByInfo tmpdrivebypackage;
                tmpdrivebypackage.eDriveByType = DRIVE_BY_TUNNEL_IN;
                tmpdrivebypackage.pEAXCar = m_pEAXCar;
                tmpdrivebypackage.ClosingVelocity = GetPhysCar()->GetVelocityMagnitude();
                tmpdrivebypackage.UniqueID = reinterpret_cast<unsigned int>(pLastZoneWePlayedWooshFor);
                tmpdrivebypackage.vLocation = vDriveByLoc;
                CSTATE_Base *ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
                if (ReturnedObj) {
                    ReturnedObj->Attach(&tmpdrivebypackage);
                }
                {
                    int break_here;
                }
            }
        }

        if (!m_bIsInTunnel) {
            return;
        }
    }

    if (FutureZoneType == TRACK_PATH_ZONE_RESET &&
        CurZoneType == TRACK_PATH_ZONE_TUNNEL &&
        ((tTimeToWaitBeforeAnotherExitDriveBy < 0.01f && pLastZoneWePlayedExitWooshFor == zone) ||
         pLastZoneWePlayedExitWooshFor != zone)) {
        if (g_WooshVol_vs_Vel.GetValue(m_pEAXCar->GetPhysCar()->GetVelocityMagnitude()) > 0.01f) {
            bPlayTunnelExit = true;
            tTimeToWaitBeforeAnotherExitDriveBy = 3.0f;
            pLastZoneWePlayedExitWooshFor = zone;
            vDriveByLoc = bVector3(FutureCarPos.x, FutureCarPos.y, m_pEAXCar->GetPhysCar()->GetPosition()->z + 10.0f);
            m_fExitIntensity = g_WooshVol_vs_Vel.GetValue(m_pEAXCar->GetPhysCar()->GetVelocityMagnitude());
            stDriveByInfo tmpdrivebypackage;
            tmpdrivebypackage.eDriveByType = DRIVE_BY_TUNNEL_OUT;
            tmpdrivebypackage.pEAXCar = m_pEAXCar;
            tmpdrivebypackage.ClosingVelocity = GetPhysCar()->GetVelocityMagnitude();
            tmpdrivebypackage.UniqueID = 0;
            tmpdrivebypackage.vLocation = vDriveByLoc;
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
        if (zone->GetElevation() >= GetPhysCar()->GetPosition()->z) {
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

    ninst = GetStateBase()->m_InstNum;
    nother = static_cast<int>(GetPhysCar()->GetContext());
    peaxcar_a = g_pEAXSound->GetPlayerTunerCar(ninst);

    UpdateIsInTunnel(t);
    UpdateDriveBySnds(t);
    UpdateCityVerb(t);
    UpdateReflectionParams(t);

    nother = static_cast<int>(GetPhysCar()->GetContext());
    if (nother != kRaceContext_Online) {
        if (nother != kRaceContext_Career) {
            return;
        }
    }
    UpdateOcclusion(t);
}

void SFXCTL_Tunnel::UpdateIsInTunnel(float t) {
    (void)t;

    TrackPathZone *zone;
    bool InTunnel;

    CurZoneType = TRACK_PATH_ZONE_TUNNEL;
    zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                         TRACK_PATH_ZONE_TUNNEL);
    if (!zone) {
        CurZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                             TRACK_PATH_ZONE_OVERPASS);
        if (!zone) {
            CurZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                                 TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (!zone) {
                CurZoneType = TRACK_PATH_ZONE_GARAGE;
                zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                                     TRACK_PATH_ZONE_GARAGE);
                if (!zone) {
                    CurZoneType = TRACK_PATH_ZONE_DYNAMIC;
                    zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
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
                    CurZoneType = TRACK_PATH_ZONE_RESET;
                    goto LAB_UPDATE_END;
                }
            }
        }
    }

LAB_IN_TUNNEL:
    InTunnel = true;
    if (!m_bIsInTunnel) {
        m_bIsInTunnel = true;
        if (static_cast<int>(m_pStateBase->GetPhysCar()->mContext) == 0) {
            MMiscSound(1).Send(UCrc32("TunnelUpdate"));

            eREVERBFX NewVerbType;
            if (CurZoneType > TRACK_PATH_ZONE_OVERPASS_SMALL) {
LAB_DEFAULT_VERB:
                NewVerbType = static_cast<eREVERBFX>(3);
            } else {
                if (CurZoneType >= TRACK_PATH_ZONE_OVERPASS) {
                    NewVerbType = static_cast<eREVERBFX>(6);
                } else if (CurZoneType == TRACK_PATH_ZONE_TUNNEL) {
                    NewVerbType = static_cast<eREVERBFX>(5);
                } else {
                    goto LAB_DEFAULT_VERB;
                }
            }
            SetCurrentReverbType(NewVerbType, 0);
        }
    }

LAB_UPDATE_END:
    if (!InTunnel && m_bIsInTunnel) {
        m_bIsInTunnel = false;
        if (static_cast<int>(m_pStateBase->GetPhysCar()->mContext) == 0) {
            MMiscSound(0).Send(UCrc32("TunnelUpdate"));
            EndTunnelVerb();
        }
    }

    m_bWasInTunnel = m_bIsInTunnel;
    if (static_cast<int>(m_pStateBase->GetPhysCar()->mContext) == 0) {
        m_PlayerZoneType = CurZoneType;
    }
}

void SFXCTL_Tunnel::UpdateOcclusion(float t) {
    if (SndCamera::NumPlayers > 1) {
        return;
    }

    m_LastOcclusionTest -= t;
    if (m_LastOcclusionTest > 0.0f) {
        return;
    }

    m_LastOcclusionTest = TimeBetweenOcclusionTests;
    if (m_PlayerZoneType == CurZoneType) {
        IsOccluded = false;
        return;
    }

    if (m_PlayerZoneType != TRACK_PATH_ZONE_TUNNEL && CurZoneType != TRACK_PATH_ZONE_TUNNEL) {
        IsOccluded = false;
        return;
    }

    {
        UMath::Vector4 originToBarrier[2];
        UMath::Vector4 DirectionVec;

        originToBarrier[0].x = -SndCamera::GetWorldCarPos3(0)->y;
        originToBarrier[0].y = SndCamera::GetWorldCarPos3(0)->z;
        originToBarrier[0].z = SndCamera::GetWorldCarPos3(0)->x;

        originToBarrier[1].z = GetPhysCar()->GetPosition()->x;
        originToBarrier[1].x = -GetPhysCar()->GetPosition()->y;
        originToBarrier[1].y = GetPhysCar()->GetPosition()->z;

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
                IsOccluded = true;
                return;
            }
        }

        IsOccluded = false;
    }
}

void SFXCTL_Tunnel::UpdateMixerOutputs() {
    SetDMIX_Input(0, 0);
    SetDMIX_Input(1, 0);
    SetDMIX_Input(2, 0);
    SetDMIX_Input(3, 0);

    SetDMIX_Input(4, IsOccluded != 0 ? 0x7FFF : 0);

    if (static_cast<int>(m_pStateBase->GetPhysCar()->mContext) == 0) {
        SetDMIX_Input(5, m_AEMSDryVol);
        SetDMIX_Input(6, m_AEMSWetVol);
    }

    if (!m_bIsInTunnel) {
        return;
    }

    if (CurZoneType < TRACK_PATH_ZONE_STREAMER_PREDICTION) {
        if (CurZoneType >= TRACK_PATH_ZONE_OVERPASS) {
            SetDMIX_Input(1, 0x7FFF);
        } else {
            SetDMIX_Input(0, 0x7FFF);
        }
    } else {
        if (CurZoneType == TRACK_PATH_ZONE_GARAGE) {
            SetDMIX_Input(2, 0x7FFF);
        } else {
            SetDMIX_Input(0, 0x7FFF);
        }
    }

    SetDMIX_Input(3, 0x7FFF);
}

void SFXCTL_Tunnel::UpdateCityVerb(float t) {
    (void)t;

    int ncurrentoffset = 0;
    int isValid = 1;
    m_PrevReverbZone = m_CurReverbZone;

    if (!ReverbAccessor.Layer) {
        isValid = 0;
    }

    if ((isValid != 0) &&
        (m_pEAXCar) &&
        (((GetUniqueID() >> 16) & 0xFF) == 2)) {
        EAX_CarState *pcar = m_pStateBase->GetPhysCar();
        if (pcar) {
            ReverbAccessor.CaptureData(pcar->mMatrix.v3.x, pcar->mMatrix.v3.y);
            m_CurReverbZone = ReverbAccessor.GetDataInt(0);
            ncurrentoffset = ReverbAccessor.GetDataInt(2);
        }
    }

    if (static_cast<unsigned int>(m_CurReverbZone) > 0xB) {
        int zone;
        if (g_pEAXSound->GetSoundGameMode() == SND_FRONTEND) {
            zone = 0;
        } else {
            zone = 9;
        }
        m_CurReverbZone = zone;
    }

    eREVERBFX currentverb = static_cast<eREVERBFX>(ReverbZoneCrossMap[m_CurReverbZone]);
    if (static_cast<int>(currentverb) > 0xB) {
        currentverb = static_cast<eREVERBFX>(9);
    }

    if ((m_CurReverbZone != m_PrevReverbZone) && !m_bIsInTunnel) {
        SetCurrentReverbType(currentverb, ncurrentoffset);
    } else {
        AdjustReverbOffset(ncurrentoffset);
    }
}

void SFXCTL_Tunnel::Destroy() {}

void SFXCTL_Tunnel::EndTunnelVerb() {
    eREVERBFX currentverb = static_cast<eREVERBFX>(ReverbZoneCrossMap[m_CurReverbZone]);
    if (static_cast<int>(currentverb) > 0xB) {
        currentverb = static_cast<eREVERBFX>(9);
    }
    SetCurrentReverbType(currentverb, 0);
}

void SFXCTL_Tunnel::AdjustReverbOffset(int reverboffset) {
    if (!bFadingOut && !bFadingIn) {
        m_ReverbOffset = static_cast<float>(reverboffset);

        int ndBGinsu = -10000;
        int ginsuWet = g_REVERBFXMODULES[m_ReverbType].GinsuWet + reverboffset;
        if (ginsuWet > -10000) {
            ndBGinsu = ginsuWet;
        }
        if (ndBGinsu > 0) {
            ndBGinsu = 0;
        }

        int ndBAems = -10000;
        int aemsWet = g_REVERBFXMODULES[m_ReverbType].AemsWet + reverboffset;
        if (aemsWet > -10000) {
            ndBAems = aemsWet;
        }
        if (ndBAems > 0) {
            ndBAems = 0;
        }

        m_CurWetGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(ndBGinsu);
        m_CurWetAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(ndBAems);
    }
}

void SFXCTL_Tunnel::SetCurrentReverbType(eREVERBFX ereverbtype, int reverboffset) {
    bFadingOut = true;
    m_ReverbOffset = static_cast<float>(reverboffset);
    m_TargetType = ereverbtype;
    ReflRamp.Initialize(0.0f, 1.0f, g_REVERBFXMODULES[m_ReverbType].FadeOut, LINEAR);
    m_CurWetGinsuTarget = 0.0f;
    m_CurDryGinsuTarget = 1.0f;
    m_CurWetAemsTarget = 0.0f;
    m_CurDryAemsTarget = 1.0f;
}

void SFXCTL_Tunnel::UpdateReflectionParams(float t) {
    bIsReadyForSwitch = false;
    ReflRamp.Update(t);

    if (bFadingOut) {
        if (ReflRamp.IsFinished() &&
            bIsTunnelRamping == 0) {
            if (m_IsLeadCar != 0) {
                bIsReadyForSwitch = true;
            }
            bFadingIn = true;
            m_ReverbType = m_TargetType;
            bFadingOut = false;
            ReflRamp.Initialize(0.0f, 1.0f, g_REVERBFXMODULES[m_TargetType].FadeIn, LINEAR);

            m_CurWetGinsu = m_CurWetGinsuTarget;
            m_CurDryGinsu = m_CurDryGinsuTarget;
            m_CurWetAems = m_CurWetAemsTarget;
            m_CurDryAems = m_CurDryAemsTarget;

            int nQGinWetTarget = bClamp(
                g_REVERBFXMODULES[m_ReverbType].GinsuWet + static_cast<int>(m_ReverbOffset), -10000, 0);
            int nQAemsWetTarget = bClamp(
                g_REVERBFXMODULES[m_ReverbType].AemsWet + static_cast<int>(m_ReverbOffset), -10000, 0);

            m_CurWetGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(nQGinWetTarget);
            m_CurWetAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(nQAemsWetTarget);
            m_CurDryGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(g_REVERBFXMODULES[m_ReverbType].GinsuDry);
            m_CurDryAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(g_REVERBFXMODULES[m_ReverbType].AemsDry);
        }
    } else if (bFadingIn &&
               ReflRamp.IsFinished()) {
        bFadingIn = false;
        m_CurWetGinsu = m_CurWetGinsuTarget;
        m_CurDryGinsu = m_CurDryGinsuTarget;
        m_CurWetAems = m_CurWetAemsTarget;
        m_CurDryAems = m_CurDryAemsTarget;
    }

    if (bFadingOut || bFadingIn) {
        m_GinsuWetVol = static_cast<int>(((m_CurWetGinsuTarget - m_CurWetGinsu) * ReflRamp.GetValue() + m_CurWetGinsu) * 32767.0f);
        m_GinsuDryVol = static_cast<int>(((m_CurDryGinsuTarget - m_CurDryGinsu) * ReflRamp.GetValue() + m_CurDryGinsu) * 32767.0f);
        m_AEMSWetVol = static_cast<int>(((m_CurWetAemsTarget - m_CurWetAems) * ReflRamp.GetValue() + m_CurWetAems) * 32767.0f);
        m_AEMSDryVol = static_cast<int>(((m_CurDryAemsTarget - m_CurDryAems) * ReflRamp.GetValue() + m_CurDryAems) * 32767.0f);
    } else {
        m_CurWetGinsu = smooth(m_CurWetGinsu, m_CurWetGinsuTarget, 0.25f);
        m_CurDryGinsu = smooth(m_CurDryGinsu, m_CurDryGinsuTarget, 0.25f);
        m_CurWetAems = smooth(m_CurWetAems, m_CurWetAemsTarget, 0.25f);
        m_CurDryAems = smooth(m_CurDryAems, m_CurDryAemsTarget, 0.25f);

        m_GinsuWetVol = static_cast<int>(m_CurWetGinsu * 32767.0f);
        m_GinsuDryVol = static_cast<int>(m_CurDryGinsu * 32767.0f);
        m_AEMSWetVol = static_cast<int>(m_CurWetAems * 32767.0f);
        m_AEMSDryVol = static_cast<int>(m_CurDryAems * 32767.0f);
    }

    if (bToggleOffset) {
        m_AEMSWetVol = bClamp(m_AEMSWetVol + 1, 0, 0x7FFF);
        m_GinsuWetVol = bClamp(m_GinsuWetVol + 1, 0, 0x7FFF);
        bToggleOffset = false;
    } else {
        bToggleOffset = true;
    }

    bIsTunnelRamping = ReflRamp.IsFinished() ^ 1;
}
