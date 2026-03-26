#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"

void VU0_v4sub(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);

extern Slope g_WooshVol_vs_Vel;
extern float GetFloatFromHundredthsdB__11NFSMixShapei(int ndB);

class ParameterAccessor {
  public:
    void *mNext;
    void *mPrev;
    void *Layer;

    void CaptureData(float x, float y);
    int GetDataInt(int field_index);
};

extern ParameterAccessor ReverbAccessor;
extern int ReverbZoneCrossMap[];
extern float TimeBetweenOcclusionTests;
extern float MaxDistanceToOccludeTest;
extern unsigned int TickerTimeStart;
extern unsigned int TickerTimeAccum;
extern unsigned int bGetTicker();

struct stREVERB_PARAMS {
    int GinsuWet;
    int GinsuDry;
    int AemsWet;
    int AemsDry;
    int FadeOut;
    int FadeIn;
};

extern stREVERB_PARAMS g_REVERBFXMODULES[];

namespace {
enum eDRIVE_BY_TYPE {
    DRIVE_BY_UNKNOWN = 0,
    DRIVE_BY_TREE = 1,
    DRIVE_BY_LAMPPOST = 2,
    DRIVE_BY_SMOKABLE = 3,
    DRIVE_BY_TUNNEL_IN = 4,
    DRIVE_BY_TUNNEL_OUT = 5,
    DRIVE_BY_OVERPASS_IN = 6,
    DRIVE_BY_OVERPASS_OUT = 7,
    DRIVE_BY_AI_CAR = 8,
    DRIVE_BY_TRAFFIC = 9,
    DRIVE_BY_BRIDGE = 10,
    DRIVE_BY_PRE_COL = 11,
    DRIVE_BY_CAMERA_BY = 12,
    MAX_DRIVE_BY_TYPES = 13,
};

struct stDriveByInfo {
    eDRIVE_BY_TYPE eDriveByType;
    EAXCar *pEAXCar;
    float ClosingVelocity;
    bVector3 vLocation;
    unsigned int UniqueID;
};

static inline float Distancexyz(const UMath::Vector4 &a, const UMath::Vector4 &b) {
    UMath::Vector4 temp;
    VU0_v4subxyz(a, b, temp);
    return VU0_sqrt(VU0_v4lengthsquarexyz(temp));
}

static void DispatchDriveByEvent(stDriveByInfo &info) {
    CSTATEMGR_Base *driveByMgr = EAXSound::m_pStateMgr[eMM_DRIVEBY];
    if (driveByMgr == nullptr) {
        return;
    }

    CSTATE_Base *state = driveByMgr->GetFreeState(&info);
    if (state != nullptr) {
        state->Attach(&info);
    }
}
} // namespace

eTrackPathZoneType SFXCTL_Tunnel::m_PlayerZoneType = TRACK_PATH_ZONE_RESET;

SFXCTL_Tunnel::SFXCTL_Tunnel() {
    m_ReverbType = static_cast< eREVERBFX >(5);
    m_TargetType = static_cast< eREVERBFX >(5);
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

SndBase *SFXCTL_Tunnel::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Tunnel::s_TypeInfo.typeName, false) SFXCTL_Tunnel();
    }
    return new (SFXCTL_Tunnel::s_TypeInfo.typeName, true) SFXCTL_Tunnel();
}

SndBase::TypeInfo *SFXCTL_Tunnel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Tunnel::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Tunnel::InitSFX() {
    SFXCTL::InitSFX();
    m_bWasInTunnel = false;
    m_bIsInTunnel = false;

    EAX_CarState *car = m_pStateBase->GetPhysCar();
    if (static_cast<int>(car->mContext) != 0) {
        m_IsLeadCar = 0;
    } else if (m_pStateBase->m_InstNum != 0) {
        m_IsLeadCar = 0;
    } else {
        m_IsLeadCar = 1;
    }
    IsOccluded = 0;
}

void SFXCTL_Tunnel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);

    g_pEAXSound->GetPlayerTunerCar(m_pStateBase->m_InstNum);

    UpdateIsInTunnel(t);
    UpdateDriveBySnds(t);
    UpdateCityVerb(t);
    UpdateReflectionParams(t);

    int context = static_cast<int>(m_pStateBase->GetPhysCar()->mContext);
    if (context == 1 || context == 2) {
        UpdateOcclusion(t);
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

int SFXCTL_Tunnel::GetController(int Index) { return -1; }

void SFXCTL_Tunnel::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Tunnel::Destroy() {}

void SFXCTL_Tunnel::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

TrackPathZone *SFXCTL_Tunnel::GetTunnelType(bVector3 &pos, eTrackPathZoneType zonetype) {
    TrackPathZone *zone = nullptr;
    while ((zone = TheTrackPathManager.FindZone(static_cast<bVector2 *>(static_cast<void *>(&pos)), zonetype, zone)) != nullptr) {
        if (zone->GetElevation() == 0.0f) {
            return zone;
        }
        if (zone->GetElevation() >= GetPhysCar()->GetPosition()->z) {
            return zone;
        }
    }
    return nullptr;
}

void SFXCTL_Tunnel::UpdateIsInTunnel(float t) {
    (void)t;

    TrackPathZone *zone;
    bool InTunnel;

    CurZoneType = TRACK_PATH_ZONE_TUNNEL;
    zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                         TRACK_PATH_ZONE_TUNNEL);
    if (zone == nullptr) {
        CurZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                             TRACK_PATH_ZONE_OVERPASS);
        if (zone == nullptr) {
            CurZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                                 TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (zone == nullptr) {
                CurZoneType = TRACK_PATH_ZONE_GARAGE;
                zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                                     TRACK_PATH_ZONE_GARAGE);
                if (zone == nullptr) {
                    CurZoneType = TRACK_PATH_ZONE_DYNAMIC;
                    zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(&m_pStateBase->GetPhysCar()->mMatrix.v3)),
                                         TRACK_PATH_ZONE_DYNAMIC);
                    if (zone != nullptr) {
                        if (zone->VisitInfo == 1) {
                            zone = nullptr;
                        }
                        if (zone != nullptr) {
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
    if (m_bIsInTunnel == 0) {
        m_bIsInTunnel = true;
        if (static_cast<int>(m_pStateBase->GetPhysCar()->mContext) == 0) {
            MMiscSound(1).Send(UCrc32("TunnelUpdate"));

            eREVERBFX NewVerbType;
            if (CurZoneType > TRACK_PATH_ZONE_OVERPASS_SMALL) {
LAB_DEFAULT_VERB:
                NewVerbType = static_cast< eREVERBFX >(3);
            } else {
                if (CurZoneType >= TRACK_PATH_ZONE_OVERPASS) {
                    NewVerbType = static_cast< eREVERBFX >(6);
                } else if (CurZoneType == TRACK_PATH_ZONE_TUNNEL) {
                    NewVerbType = static_cast< eREVERBFX >(5);
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

void SFXCTL_Tunnel::SetCurrentReverbType(eREVERBFX type, int reverboffset) {
    bFadingOut = true;
    m_ReverbOffset = static_cast< float >(reverboffset);
    m_TargetType = type;
    ReflRamp.Initialize(0.0f, 1.0f, g_REVERBFXMODULES[m_ReverbType].FadeOut, LINEAR);
    m_CurWetGinsuTarget = 0.0f;
    m_CurDryGinsuTarget = 1.0f;
    m_CurWetAemsTarget = 0.0f;
    m_CurDryAemsTarget = 1.0f;
}

void SFXCTL_Tunnel::UpdateCityVerb(float t) {
    (void)t;

    int ncurrentoffset = 0;
    int isValid = 1;
    m_PrevReverbZone = m_CurReverbZone;

    if (ReverbAccessor.Layer == nullptr) {
        isValid = 0;
    }

    if ((isValid != 0) &&
        (m_pEAXCar != nullptr) &&
        (((GetUniqueID() >> 16) & 0xFF) == 2)) {
        EAX_CarState *pcar = m_pStateBase->GetPhysCar();
        if (pcar != nullptr) {
            ReverbAccessor.CaptureData(pcar->mMatrix.v3.x, pcar->mMatrix.v3.y);
            m_CurReverbZone = ReverbAccessor.GetDataInt(0);
            ncurrentoffset = ReverbAccessor.GetDataInt(2);
        }
    }

    if (static_cast< unsigned int >(m_CurReverbZone) > 0xB) {
        register int zone asm("r0");
        if (g_pEAXSound->GetSoundGameMode() == SND_FRONTEND) {
            zone = 0;
        } else {
            zone = 9;
        }
        m_CurReverbZone = zone;
    }

    eREVERBFX currentverb = static_cast< eREVERBFX >(ReverbZoneCrossMap[m_CurReverbZone]);
    if (static_cast< int >(currentverb) > 0xB) {
        currentverb = static_cast< eREVERBFX >(9);
    }

    if ((m_CurReverbZone != m_PrevReverbZone) && !m_bIsInTunnel) {
        SetCurrentReverbType(currentverb, ncurrentoffset);
    } else {
        AdjustReverbOffset(ncurrentoffset);
    }
}

void SFXCTL_Tunnel::AdjustReverbOffset(int reverboffset) {
    if (!bFadingOut && !bFadingIn) {
        m_ReverbOffset = static_cast< float >(reverboffset);

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

void SFXCTL_Tunnel::UpdateReflectionParams(float t) {
    bIsReadyForSwitch = false;
    ReflRamp.Update(t);

    if (bFadingOut) {
        if (*static_cast<int *>(static_cast<void *>(&ReflRamp.bComplete)) != 0 &&
            bIsTunnelRamping == 0) {
            if (m_IsLeadCar != 0) {
                bIsReadyForSwitch = true;
            }
            bFadingIn = true;
            m_ReverbType = m_TargetType;
            bFadingOut = bIsTunnelRamping;
            ReflRamp.Initialize(0.0f, 1.0f, g_REVERBFXMODULES[m_TargetType].FadeIn, LINEAR);

            m_CurWetGinsu = m_CurWetGinsuTarget;
            m_CurDryGinsu = m_CurDryGinsuTarget;
            m_CurWetAems = m_CurWetAemsTarget;
            m_CurDryAems = m_CurDryAemsTarget;

            int nQGinWetTarget = g_REVERBFXMODULES[m_ReverbType].GinsuWet + static_cast<int>(m_ReverbOffset);
            nQGinWetTarget = bClamp(nQGinWetTarget, -10000, 0);

            int nQAemsWetTarget = g_REVERBFXMODULES[m_ReverbType].AemsWet + static_cast<int>(m_ReverbOffset);
            nQAemsWetTarget = bClamp(nQAemsWetTarget, -10000, 0);

            m_CurWetGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(nQGinWetTarget);
            m_CurWetAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(nQAemsWetTarget);
            m_CurDryGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(g_REVERBFXMODULES[m_ReverbType].GinsuDry);
            m_CurDryAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(g_REVERBFXMODULES[m_ReverbType].AemsDry);
        }
    } else if (bFadingIn &&
               *static_cast<int *>(static_cast<void *>(&ReflRamp.bComplete)) != 0) {
        m_CurWetGinsu = m_CurWetGinsuTarget;
        m_CurDryGinsu = m_CurDryGinsuTarget;
        m_CurWetAems = m_CurWetAemsTarget;
        m_CurDryAems = m_CurDryAemsTarget;
        bFadingIn = false;
    }

    if (bFadingOut || bFadingIn) {
        float ramp = ReflRamp.CurValue;
        m_GinsuWetVol = static_cast<int>(((m_CurWetGinsuTarget - m_CurWetGinsu) * ramp + m_CurWetGinsu) * 32767.0f);
        m_GinsuDryVol = static_cast<int>(((m_CurDryGinsuTarget - m_CurDryGinsu) * ramp + m_CurDryGinsu) * 32767.0f);
        m_AEMSWetVol = static_cast<int>(((m_CurWetAemsTarget - m_CurWetAems) * ramp + m_CurWetAems) * 32767.0f);
        m_AEMSDryVol = static_cast<int>(((m_CurDryAemsTarget - m_CurDryAems) * ramp + m_CurDryAems) * 32767.0f);
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

    int toggleValue;
    if (!bToggleOffset) {
        toggleValue = 1;
    } else {
        int aemsWet = m_AEMSWetVol + 1;
        m_AEMSWetVol = aemsWet;
        int clampedAemsWet = 0;
        if (aemsWet > 0) {
            clampedAemsWet = aemsWet;
        }
        if (clampedAemsWet > 0x7FFF) {
            clampedAemsWet = 0x7FFF;
        }
        m_AEMSWetVol = clampedAemsWet;

        int ginsuWet = m_GinsuWetVol + 1;
        m_GinsuWetVol = ginsuWet;
        int clampedGinsuWet = 0;
        if (ginsuWet > 0) {
            clampedGinsuWet = ginsuWet;
        }
        if (clampedGinsuWet > 0x7FFF) {
            clampedGinsuWet = 0x7FFF;
        }
        m_GinsuWetVol = clampedGinsuWet;
        toggleValue = 0;
    }

    bToggleOffset = toggleValue;
    bIsTunnelRamping = ReflRamp.bComplete ^ 1;
}

void SFXCTL_Tunnel::UpdateOcclusion(float t) {
    if (SndCamera::NumPlayers > 1) {
        return;
    }

    float kZero = 0.0f;
    float lastOcclusionTest = m_LastOcclusionTest - t;
    m_LastOcclusionTest = lastOcclusionTest;
    if (lastOcclusionTest > kZero) {
        return;
    }

    m_LastOcclusionTest = TimeBetweenOcclusionTests;
    if (m_PlayerZoneType == CurZoneType) {
        IsOccluded = 0;
        return;
    }
    if (m_PlayerZoneType != TRACK_PATH_ZONE_TUNNEL) {
        if (CurZoneType != TRACK_PATH_ZONE_TUNNEL) {
            IsOccluded = 0;
            return;
        }
    }

    UMath::Vector4 originToBarrier[2];
    UMath::Vector4 directionVec;

    originToBarrier[0].x = -SndCamera::GetV3WorldCarPos(0)->y;
    originToBarrier[0].y = SndCamera::GetV3WorldCarPos(0)->z;
    originToBarrier[0].z = SndCamera::GetV3WorldCarPos(0)->x;

    originToBarrier[1].z = m_pStateBase->GetPhysCar()->mMatrix.v3.x;
    originToBarrier[1].x = -m_pStateBase->GetPhysCar()->mMatrix.v3.y;
    originToBarrier[1].y = m_pStateBase->GetPhysCar()->mMatrix.v3.z;

    float testDist = Distancexyz(originToBarrier[0], originToBarrier[1]);
    if (MaxDistanceToOccludeTest < testDist) {
        testDist = MaxDistanceToOccludeTest;
    }

    originToBarrier[0].y = originToBarrier[0].y + 2.0f;
    VU0_v4sub(originToBarrier[0], originToBarrier[1], directionVec);
    UMath::Unit(directionVec, directionVec);
    VU0_v4scaleadd(originToBarrier[1], testDist, originToBarrier[0], directionVec);

    WCollisionMgr::WorldCollisionInfo cInfo;
    memset(&cInfo.fBle, 0, 0x20);

    WCollisionMgr collisionMgr(0, 3);
    if (!collisionMgr.CheckHitWorld(originToBarrier, cInfo, 2)) {
        IsOccluded = 0;
        return;
    }

    if (VU0_v4distancesquarexyz(originToBarrier[0], cInfo.fCollidePt) < testDist * testDist - 9.0f) {
        IsOccluded = 1;
        return;
    }

    IsOccluded = 0;
}

void SFXCTL_Tunnel::EndTunnelVerb() {
    eREVERBFX reverbType = static_cast<eREVERBFX>(ReverbZoneCrossMap[m_CurReverbZone]);
    if (static_cast<int>(reverbType) > 0xB) {
        reverbType = static_cast<eREVERBFX>(9);
    }
    SetCurrentReverbType(reverbType, 0);
}

void SFXCTL_Tunnel::UpdateDriveBySnds(float t) {
    eTrackPathZoneType PrevFutureZoneType = FutureZoneType;
    TrackPathZone *zone;
    const bVector2 *CurCarPos;
    bVector3 FutureCarPos;
    bVector2 UnNormalCurCarDir;
    bVector2 CurCarDir;
    bVector2 FutureCarDir;
    bVector2 FutureCar2dPos;

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

    CurCarPos = m_pEAXCar->GetPhysCar()->GetPosition2D();
    bFill(&UnNormalCurCarDir,
        m_pEAXCar->GetPhysCar()->GetForwardVector()->x,
        m_pEAXCar->GetPhysCar()->GetForwardVector()->y);
    bNormalize(&CurCarDir, &UnNormalCurCarDir);
    bScale(&FutureCarDir, &CurCarDir, m_pEAXCar->GetPhysCar()->GetForwardSpeed() * 0.4f);
    bAdd(&FutureCar2dPos, CurCarPos, &FutureCarDir);
    bFill(&FutureCarPos,
        FutureCar2dPos.x,
        FutureCar2dPos.y,
        m_pEAXCar->GetPhysCar()->GetPosition()->z);

    FutureZoneType = TRACK_PATH_ZONE_TUNNEL;
    zone = GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_TUNNEL);
    if (zone == nullptr) {
        FutureZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_OVERPASS);
        if (zone == nullptr) {
            FutureZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = GetTunnelType(FutureCarPos, TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (zone == nullptr) {
                FutureZoneType = TRACK_PATH_ZONE_RESET;
            }
        }
    }

    bool InTunnel = m_bIsInTunnel;
    if (!InTunnel) {
        if (FutureZoneType != TRACK_PATH_ZONE_RESET &&
            ((tTimeToWaitBeforeAnotherDriveBy < 0.01f && pLastZoneWePlayedWooshFor == zone) ||
             pLastZoneWePlayedWooshFor != zone)) {
            if (g_WooshVol_vs_Vel.GetValue(m_pEAXCar->GetPhysCar()->GetVelocityMagnitude()) > 0.01f) {
                stDriveByInfo tmpdrivebypackage;
                CSTATE_Base *ReturnedObj;

                bPlayDriveBy = true;
                tTimeToWaitBeforeAnotherDriveBy = 3.0f;
                pLastZoneWePlayedWooshFor = zone;
                vDriveByLoc = bVector3(FutureCarPos.x, FutureCarPos.y, m_pEAXCar->GetPhysCar()->GetPosition()->z + 10.0f);
                m_fIntensity = g_WooshVol_vs_Vel.GetValue(m_pEAXCar->GetPhysCar()->GetVelocityMagnitude());
                tmpdrivebypackage.eDriveByType = DRIVE_BY_TUNNEL_IN;
                tmpdrivebypackage.pEAXCar = m_pEAXCar;
                tmpdrivebypackage.ClosingVelocity = m_pEAXCar->GetPhysCar()->GetVelocityMagnitude();
                tmpdrivebypackage.vLocation = vDriveByLoc;
                tmpdrivebypackage.UniqueID = 0;
                ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
                if (ReturnedObj != nullptr) {
                    ReturnedObj->Attach(&tmpdrivebypackage);
                }
            }
        }

        if (!InTunnel) {
            return;
        }
    }

    if (FutureZoneType == TRACK_PATH_ZONE_RESET &&
        CurZoneType == TRACK_PATH_ZONE_TUNNEL &&
        ((tTimeToWaitBeforeAnotherExitDriveBy < 0.01f && pLastZoneWePlayedExitWooshFor == zone) ||
         pLastZoneWePlayedExitWooshFor != zone)) {
        if (g_WooshVol_vs_Vel.GetValue(m_pEAXCar->GetPhysCar()->GetVelocityMagnitude()) > 0.01f) {
            stDriveByInfo tmpdrivebypackage;
            CSTATE_Base *ReturnedObj;

            bPlayTunnelExit = true;
            tTimeToWaitBeforeAnotherExitDriveBy = 3.0f;
            pLastZoneWePlayedExitWooshFor = zone;
            vDriveByLoc = bVector3(FutureCarPos.x, FutureCarPos.y, m_pEAXCar->GetPhysCar()->GetPosition()->z + 10.0f);
            m_fExitIntensity = g_WooshVol_vs_Vel.GetValue(m_pEAXCar->GetPhysCar()->GetVelocityMagnitude());
            tmpdrivebypackage.eDriveByType = DRIVE_BY_TUNNEL_OUT;
            tmpdrivebypackage.pEAXCar = m_pEAXCar;
            tmpdrivebypackage.ClosingVelocity = m_pEAXCar->GetPhysCar()->GetVelocityMagnitude();
            tmpdrivebypackage.vLocation = vDriveByLoc;
            tmpdrivebypackage.UniqueID = 0;
            ReturnedObj = EAXSound::GetStateMgr(eMM_DRIVEBY)->GetFreeState(&tmpdrivebypackage);
            if (ReturnedObj != nullptr) {
                ReturnedObj->Attach(&tmpdrivebypackage);
            }
        }
    }
}
