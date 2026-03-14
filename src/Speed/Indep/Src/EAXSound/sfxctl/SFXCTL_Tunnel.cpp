#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

extern Slope g_WooshVol_vs_Vel;

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
    ReflRamp.Update(t);
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

    EAX_CarState *car = GetCarStateFromEAXCar(m_pEAXCar);
    if (car == nullptr) {
        return;
    }

    FutureZoneType = TRACK_PATH_ZONE_RESET;

    bVector2 unNormalizedCurCarDir;
    unNormalizedCurCarDir.x = GetCarStateFloat(car, 0x14);
    unNormalizedCurCarDir.y = GetCarStateFloat(car, 0x18);

    bVector2 curCarDir;
    bNormalize(&curCarDir, &unNormalizedCurCarDir);

    float futureDistance = GetCarStateFloat(car, 0x90) * 0.4f;
    bVector2 futureCarDir;
    futureCarDir.x = curCarDir.x * futureDistance;
    futureCarDir.y = curCarDir.y * futureDistance;

    bVector3 futureCarPos;
    futureCarPos.x = GetCarStateFloat(car, 0x44) + futureCarDir.x;
    futureCarPos.y = GetCarStateFloat(car, 0x48) + futureCarDir.y;

    EAX_CarState *stateCar =
        m_pStateBase != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pStateBase) + 0x34) : nullptr;
    futureCarPos.z = stateCar != nullptr ? GetCarStateFloat(stateCar, 0x4C) : GetCarStateFloat(car, 0x4C);

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
            float wooshIntensity = g_WooshVol_vs_Vel.GetValue(GetCarSpeedMagnitude(car));
            if (wooshIntensity > 0.01f) {
                bPlayDriveBy = true;
                tTimeToWaitBeforeAnotherDriveBy = 3.0f;
                pLastZoneWePlayedWooshFor = futureZone;

                vDriveByLoc.x = futureCarPos.x;
                vDriveByLoc.y = futureCarPos.y;
                vDriveByLoc.z = GetCarStateFloat(car, 0x4C) + 10.0f;
                m_fIntensity = g_WooshVol_vs_Vel.GetValue(GetCarSpeedMagnitude(car));

                stDriveByInfo driveByInfo;
                driveByInfo.eDriveByType = DRIVE_BY_TUNNEL_IN;
                driveByInfo.pEAXCar = m_pEAXCar;
                driveByInfo.ClosingVelocity = stateCar != nullptr ? GetCarSpeedMagnitude(stateCar) : 0.0f;
                driveByInfo.vLocation = vDriveByLoc;
                driveByInfo.UniqueID = 0;
                DispatchDriveByEvent(driveByInfo);
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
            float exitIntensity = g_WooshVol_vs_Vel.GetValue(GetCarSpeedMagnitude(car));
            if (exitIntensity > 0.01f) {
                bPlayTunnelExit = true;
                tTimeToWaitBeforeAnotherExitDriveBy = 3.0f;
                pLastZoneWePlayedExitWooshFor = futureZone;

                vDriveByLoc.x = futureCarPos.x;
                vDriveByLoc.y = futureCarPos.y;
                vDriveByLoc.z = GetCarStateFloat(car, 0x4C) + 10.0f;
                m_fExitIntensity = g_WooshVol_vs_Vel.GetValue(GetCarSpeedMagnitude(car));

                stDriveByInfo driveByInfo;
                driveByInfo.eDriveByType = DRIVE_BY_TUNNEL_OUT;
                driveByInfo.pEAXCar = m_pEAXCar;
                driveByInfo.ClosingVelocity = stateCar != nullptr ? GetCarSpeedMagnitude(stateCar) : 0.0f;
                driveByInfo.vLocation = vDriveByLoc;
                driveByInfo.UniqueID = 0;
                DispatchDriveByEvent(driveByInfo);
            }
        }
    }
}
