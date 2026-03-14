#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

extern Slope g_WooshVol_vs_Vel;
extern float GetFloatFromHundredthsdB__11NFSMixShapei(int ndB);

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

static EAX_CarState *GetCarStateFromEAXCar(EAXCar *car) {
    return car != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(car) + 0x34) : nullptr;
}

static float GetCarStateFloat(EAX_CarState *car, int offset) {
    return *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + offset);
}

static float GetCarSpeedMagnitude(EAX_CarState *car) {
    float vx = GetCarStateFloat(car, 0x54);
    float vy = GetCarStateFloat(car, 0x58);
    float vz = GetCarStateFloat(car, 0x5C);
    return bSqrt(vx * vx + vy * vy + vz * vz);
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

SFXCTL_Tunnel::SFXCTL_Tunnel() {}

SFXCTL_Tunnel::~SFXCTL_Tunnel() {}

SndBase *SFXCTL_Tunnel::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Tunnel();
}

SndBase::TypeInfo *SFXCTL_Tunnel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Tunnel::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Tunnel::InitSFX() {
    SFXCTL::InitSFX();
}

void SFXCTL_Tunnel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    UpdateIsInTunnel(t);
    UpdateDriveBySnds(t);
    UpdateCityVerb(t);
    UpdateReflectionParams(t);
}

void SFXCTL_Tunnel::UpdateMixerOutputs() {
    SetDMIX_Input(0, 0);
    SetDMIX_Input(1, 0);
    SetDMIX_Input(2, 0);
    SetDMIX_Input(3, 0);
    SetDMIX_Input(4, 0);
    SetDMIX_Input(5, 0);
    SetDMIX_Input(6, 0);
}

int SFXCTL_Tunnel::GetController(int Index) { return -1; }

void SFXCTL_Tunnel::AttachController(SFXCTL *psfxctl) {}

void SFXCTL_Tunnel::Destroy() {}

void SFXCTL_Tunnel::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

TrackPathZone *SFXCTL_Tunnel::GetTunnelType(bVector3 &pos, eTrackPathZoneType zonetype) {
    TrackPathZone *zone = nullptr;
    while (true) {
        zone = TheTrackPathManager.FindZone(reinterpret_cast< bVector2 * >(&pos), zonetype, zone);
        if (zone == nullptr) {
            return nullptr;
        }
        if (zone->Elevation == 0.0f) {
            break;
        }
        return zone;
    }
    return zone;
}

void SFXCTL_Tunnel::UpdateIsInTunnel(float t) {
    (void)t;
}

void SFXCTL_Tunnel::SetCurrentReverbType(eREVERBFX type, int reverboffset) {
    (void)type;
    (void)reverboffset;
}

void SFXCTL_Tunnel::UpdateCityVerb(float t) {
    (void)t;
}

void SFXCTL_Tunnel::AdjustReverbOffset(int reverboffset) {
    (void)reverboffset;
}

void SFXCTL_Tunnel::UpdateReflectionParams(float t) {
    *reinterpret_cast<int *>(&bIsReadyForSwitch) = 0;
    ReflRamp.Update(t);

    eREVERBFX targetType = m_TargetType;
    int isFadingOut = *reinterpret_cast<int *>(&bFadingOut);
    if (isFadingOut == 0) {
        if (*reinterpret_cast<int *>(&bFadingIn) != 0 && *reinterpret_cast<int *>(&ReflRamp.bComplete) != 0) {
            m_CurWetGinsu = m_CurWetGinsuTarget;
            m_CurDryGinsu = m_CurDryGinsuTarget;
            m_CurWetAems = m_CurWetAemsTarget;
            m_CurDryAems = m_CurDryAemsTarget;
            *reinterpret_cast<int *>(&bFadingIn) = 0;
        }
    } else if (*reinterpret_cast<int *>(&ReflRamp.bComplete) != 0 && *reinterpret_cast<int *>(&bIsTunnelRamping) == 0) {
        *reinterpret_cast<int *>(&bFadingOut) = 0;
        *reinterpret_cast<int *>(&bFadingIn) = 1;
        int fadeIn = g_REVERBFXMODULES[targetType].FadeIn;
        *reinterpret_cast<int *>(&bIsReadyForSwitch) = 1;
        m_ReverbType = targetType;
        ReflRamp.Initialize(0.0f, 1.0f, fadeIn, LINEAR);
        m_CurWetAems = m_CurWetAemsTarget;

        int aemsWetDb = g_REVERBFXMODULES[m_ReverbType].AemsWet + static_cast<int>(m_ReverbOffset);
        int ginsuWetDb = g_REVERBFXMODULES[m_ReverbType].GinsuWet + static_cast<int>(m_ReverbOffset);
        if (aemsWetDb < -10000) {
            aemsWetDb = -10000;
        } else if (aemsWetDb > 0) {
            aemsWetDb = 0;
        }
        if (ginsuWetDb < -10000) {
            ginsuWetDb = -10000;
        } else if (ginsuWetDb > 0) {
            ginsuWetDb = 0;
        }

        m_CurWetGinsu = m_CurWetGinsuTarget;
        m_CurDryGinsu = m_CurDryGinsuTarget;
        m_CurDryAems = m_CurDryAemsTarget;
        m_CurWetGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(ginsuWetDb);
        m_CurWetAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(aemsWetDb);
        m_CurDryGinsuTarget = GetFloatFromHundredthsdB__11NFSMixShapei(g_REVERBFXMODULES[m_ReverbType].GinsuDry);
        m_CurDryAemsTarget = GetFloatFromHundredthsdB__11NFSMixShapei(g_REVERBFXMODULES[m_ReverbType].AemsDry);
        isFadingOut = *reinterpret_cast<int *>(&bFadingOut);
    }

    float wetAems;
    float wetGinsu;
    float dryAems;
    float dryGinsu;
    if (isFadingOut == 0 && *reinterpret_cast<int *>(&bFadingIn) == 0) {
        m_CurWetGinsu = smooth(m_CurWetGinsu, m_CurWetGinsuTarget, 0.25f);
        m_CurDryGinsu = smooth(m_CurDryGinsu, m_CurDryGinsuTarget, 0.25f);
        m_CurWetAems = smooth(m_CurWetAems, m_CurWetAemsTarget, 0.25f);
        m_CurDryAems = smooth(m_CurDryAems, m_CurDryAemsTarget, 0.25f);
        wetAems = m_CurWetAems;
        wetGinsu = m_CurWetGinsu;
        dryAems = m_CurDryAems;
        dryGinsu = m_CurDryGinsu;
    } else {
        float ramp = ReflRamp.CurValue;
        wetAems = (m_CurWetAemsTarget - m_CurWetAems) * ramp + m_CurWetAems;
        wetGinsu = (m_CurWetGinsuTarget - m_CurWetGinsu) * ramp + m_CurWetGinsu;
        dryAems = (m_CurDryAemsTarget - m_CurDryAems) * ramp + m_CurDryAems;
        dryGinsu = (m_CurDryGinsuTarget - m_CurDryGinsu) * ramp + m_CurDryGinsu;
    }

    m_GinsuDryVol = static_cast<int>(dryGinsu * 32767.0f);
    m_AEMSDryVol = static_cast<int>(dryAems * 32767.0f);
    m_GinsuWetVol = static_cast<int>(wetGinsu * 32767.0f);
    m_AEMSWetVol = static_cast<int>(wetAems * 32767.0f);

    if (*reinterpret_cast<int *>(&bToggleOffset) != 0) {
        int aemsWet = m_AEMSWetVol + 1;
        int ginsuWet = m_GinsuWetVol + 1;
        if (aemsWet < 0) {
            aemsWet = 0;
        } else if (aemsWet > 0x7FFF) {
            aemsWet = 0x7FFF;
        }
        if (ginsuWet < 0) {
            ginsuWet = 0;
        } else if (ginsuWet > 0x7FFF) {
            ginsuWet = 0x7FFF;
        }
        m_AEMSWetVol = aemsWet;
        m_GinsuWetVol = ginsuWet;
    }

    *reinterpret_cast<int *>(&bToggleOffset) = (*reinterpret_cast<int *>(&bToggleOffset) == 0);
    *reinterpret_cast<int *>(&bIsTunnelRamping) = *reinterpret_cast<int *>(&ReflRamp.bComplete) ^ 1;
}

void SFXCTL_Tunnel::UpdateOcclusion(float t) {
    (void)t;
}

void SFXCTL_Tunnel::EndTunnelVerb() {}

void SFXCTL_Tunnel::UpdateDriveBySnds(float t) {
    *reinterpret_cast<int *>(&bPlayDriveBy) = 0;
    *reinterpret_cast<int *>(&bPlayTunnelExit) = 0;

    tTimeToWaitBeforeAnotherDriveBy -= t;
    if (tTimeToWaitBeforeAnotherDriveBy < 0.0f) {
        tTimeToWaitBeforeAnotherDriveBy = 0.0f;
    }
    tTimeToWaitBeforeAnotherExitDriveBy -= t;
    if (tTimeToWaitBeforeAnotherExitDriveBy < 0.0f) {
        tTimeToWaitBeforeAnotherExitDriveBy = 0.0f;
    }

    EAX_CarState *car =
        m_pEAXCar != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pEAXCar) + 0x34) : nullptr;
    if (car == nullptr) {
        return;
    }

    FutureZoneType = TRACK_PATH_ZONE_RESET;

    bVector2 unNormalizedCurCarDir;
    unNormalizedCurCarDir.x = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x14);
    unNormalizedCurCarDir.y = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x18);

    bVector2 curCarDir;
    bNormalize(&curCarDir, &unNormalizedCurCarDir);

    float futureDistance = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x90) * 0.4f;
    bVector2 futureCarDir;
    futureCarDir.x = curCarDir.x * futureDistance;
    futureCarDir.y = curCarDir.y * futureDistance;

    bVector3 futureCarPos;
    futureCarPos.x = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x44) + futureCarDir.x;
    futureCarPos.y = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x48) + futureCarDir.y;

    EAX_CarState *stateCar =
        m_pStateBase != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pStateBase) + 0x34) : nullptr;
    futureCarPos.z = stateCar != nullptr
                         ? *reinterpret_cast<float *>(reinterpret_cast<char *>(stateCar) + 0x4C)
                         : *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x4C);

    FutureZoneType = TRACK_PATH_ZONE_TUNNEL;
    TrackPathZone *futureZone = GetTunnelType(futureCarPos, TRACK_PATH_ZONE_TUNNEL);
    if (futureZone == nullptr) {
        FutureZoneType = TRACK_PATH_ZONE_OVERPASS;
        futureZone = GetTunnelType(futureCarPos, TRACK_PATH_ZONE_OVERPASS);
        if (futureZone == nullptr) {
            FutureZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            futureZone = GetTunnelType(futureCarPos, TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (futureZone == nullptr) {
                FutureZoneType = TRACK_PATH_ZONE_RESET;
            }
        }
    }

    if (!m_bIsInTunnel && FutureZoneType != TRACK_PATH_ZONE_RESET) {
        bool shouldCheckDriveBy = false;
        if ((tTimeToWaitBeforeAnotherDriveBy < 0.01f && pLastZoneWePlayedWooshFor == futureZone) ||
            pLastZoneWePlayedWooshFor != futureZone) {
            shouldCheckDriveBy = true;
        }

        if (shouldCheckDriveBy) {
            float vx = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x54);
            float vy = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x58);
            float vz = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x5C);
            float wooshIntensity = g_WooshVol_vs_Vel.GetValue(bSqrt(vx * vx + vy * vy + vz * vz));
            if (wooshIntensity > 0.01f) {
                bPlayDriveBy = true;
                tTimeToWaitBeforeAnotherDriveBy = 3.0f;
                pLastZoneWePlayedWooshFor = futureZone;

                vDriveByLoc.x = futureCarPos.x;
                vDriveByLoc.y = futureCarPos.y;
                vDriveByLoc.z = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x4C) + 10.0f;
                m_fIntensity = g_WooshVol_vs_Vel.GetValue(bSqrt(vx * vx + vy * vy + vz * vz));

                stDriveByInfo driveByInfo;
                driveByInfo.eDriveByType = DRIVE_BY_TUNNEL_IN;
                driveByInfo.pEAXCar = m_pEAXCar;
                if (stateCar != nullptr) {
                    float stateVx = *reinterpret_cast<float *>(reinterpret_cast<char *>(stateCar) + 0x54);
                    float stateVy = *reinterpret_cast<float *>(reinterpret_cast<char *>(stateCar) + 0x58);
                    float stateVz = *reinterpret_cast<float *>(reinterpret_cast<char *>(stateCar) + 0x5C);
                    driveByInfo.ClosingVelocity = bSqrt(stateVx * stateVx + stateVy * stateVy + stateVz * stateVz);
                } else {
                    driveByInfo.ClosingVelocity = 0.0f;
                }
                driveByInfo.vLocation = vDriveByLoc;
                driveByInfo.UniqueID = 0;
                CSTATEMGR_Base *driveByMgr = EAXSound::m_pStateMgr[eMM_DRIVEBY];
                if (driveByMgr != nullptr) {
                    CSTATE_Base *state = driveByMgr->GetFreeState(&driveByInfo);
                    if (state != nullptr) {
                        state->Attach(&driveByInfo);
                    }
                }
            }
        }
    }

    if (m_bIsInTunnel && FutureZoneType == TRACK_PATH_ZONE_RESET && CurZoneType == TRACK_PATH_ZONE_TUNNEL) {
        bool shouldCheckExitDriveBy = false;
        if ((tTimeToWaitBeforeAnotherExitDriveBy < 0.01f && pLastZoneWePlayedExitWooshFor == futureZone) ||
            pLastZoneWePlayedExitWooshFor != futureZone) {
            shouldCheckExitDriveBy = true;
        }

        if (shouldCheckExitDriveBy) {
            float vx = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x54);
            float vy = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x58);
            float vz = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x5C);
            float exitIntensity = g_WooshVol_vs_Vel.GetValue(bSqrt(vx * vx + vy * vy + vz * vz));
            if (exitIntensity > 0.01f) {
                bPlayTunnelExit = true;
                tTimeToWaitBeforeAnotherExitDriveBy = 3.0f;
                pLastZoneWePlayedExitWooshFor = futureZone;

                vDriveByLoc.x = futureCarPos.x;
                vDriveByLoc.y = futureCarPos.y;
                vDriveByLoc.z = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0x4C) + 10.0f;
                m_fExitIntensity = g_WooshVol_vs_Vel.GetValue(bSqrt(vx * vx + vy * vy + vz * vz));

                stDriveByInfo driveByInfo;
                driveByInfo.eDriveByType = DRIVE_BY_TUNNEL_OUT;
                driveByInfo.pEAXCar = m_pEAXCar;
                if (stateCar != nullptr) {
                    float stateVx = *reinterpret_cast<float *>(reinterpret_cast<char *>(stateCar) + 0x54);
                    float stateVy = *reinterpret_cast<float *>(reinterpret_cast<char *>(stateCar) + 0x58);
                    float stateVz = *reinterpret_cast<float *>(reinterpret_cast<char *>(stateCar) + 0x5C);
                    driveByInfo.ClosingVelocity = bSqrt(stateVx * stateVx + stateVy * stateVy + stateVz * stateVz);
                } else {
                    driveByInfo.ClosingVelocity = 0.0f;
                }
                driveByInfo.vLocation = vDriveByLoc;
                driveByInfo.UniqueID = 0;
                CSTATEMGR_Base *driveByMgr = EAXSound::m_pStateMgr[eMM_DRIVEBY];
                if (driveByMgr != nullptr) {
                    CSTATE_Base *state = driveByMgr->GetFreeState(&driveByInfo);
                    if (state != nullptr) {
                        state->Attach(&driveByInfo);
                    }
                }
            }
        }
    }
}
