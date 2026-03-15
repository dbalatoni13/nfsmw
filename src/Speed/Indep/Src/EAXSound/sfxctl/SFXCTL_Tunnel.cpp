#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
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

namespace SndCamera {
extern int NumPlayers;
extern bVector3 m_v3WorldCarPos[2];
}

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
    return car != nullptr
               ? *static_cast<EAX_CarState **>(
                     static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x34))
               : nullptr;
}

static inline float GetCarStateFloat(EAX_CarState *car, int offset) {
    return *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + offset));
}

static inline float GetCarSpeedMagnitude(EAX_CarState *car) {
    float vx = GetCarStateFloat(car, 0x54);
    float vy = GetCarStateFloat(car, 0x58);
    float vz = GetCarStateFloat(car, 0x5C);
    return bSqrt(vx * vx + vy * vy + vz * vz);
}

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
    float kZero = 0.0f;
    float kOne = 1.0f;

    m_TargetType = static_cast< eREVERBFX >(5);
    m_CurReverbZone = -1;
    vDriveByLoc.x = kZero;
    vDriveByLoc.y = kZero;
    m_ReverbType = static_cast< eREVERBFX >(5);
    m_CurWetGinsu = kZero;
    m_CurWetAems = kZero;
    m_CurWetGinsuTarget = kZero;
    m_CurWetAemsTarget = kZero;
    m_fIntensity = kZero;
    m_GinsuDryVol = 0x7F;
    vDriveByLoc.z = kZero;
    tTimeToWaitBeforeAnotherDriveBy = kZero;
    tTimeToWaitBeforeAnotherExitDriveBy = kZero;
    m_GinsuWetVol = 0;
    m_AEMSWetVol = 0;
    m_AEMSDryVol = 0;
    m_CurDryGinsu = kOne;
    m_CurDryAems = kOne;
    m_CurDryGinsuTarget = kOne;
    m_CurDryAemsTarget = kOne;
    m_PrevReverbZone = 0;
    bPlayDriveBy = false;
    pLastZoneWePlayedWooshFor = nullptr;
    pLastZoneWePlayedExitWooshFor = nullptr;
    bFadingOut = false;
    bFadingIn = false;
    bIsReadyForSwitch = false;
    ReflRamp.Initialize(kZero, kZero, 1, LINEAR);
    bToggleOffset = false;
    m_IsLeadCar = false;
    m_LastOcclusionTest = g_pEAXSound->Random(kOne);
    TickerTimeStart = bGetTicker();
    TickerTimeAccum = 0;
}

SFXCTL_Tunnel::~SFXCTL_Tunnel() {}

SndBase *SFXCTL_Tunnel::CreateObject(unsigned int allocator) {
    SFXCTL_Tunnel *object;
    if (allocator != 0) {
        object = static_cast<SFXCTL_Tunnel *>(
            gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_Tunnel), SFXCTL_Tunnel::s_TypeInfo.typeName, true));
    } else {
        object = static_cast<SFXCTL_Tunnel *>(
            gAudioMemoryManager.AllocateMemory(sizeof(SFXCTL_Tunnel), SFXCTL_Tunnel::s_TypeInfo.typeName, false));
    }
    if (object != nullptr) {
        object = new (object) SFXCTL_Tunnel();
    }
    return object;
}

SndBase::TypeInfo *SFXCTL_Tunnel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Tunnel::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Tunnel::InitSFX() {
    SFXCTL::InitSFX();
    m_bIsInTunnel = false;
    m_bWasInTunnel = false;

    int context = *static_cast<int *>(static_cast<void *>(
        static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
        0x210));
    int isLeadCar = 0;
    if (context == 0 &&
        *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x10)) == 0) {
        isLeadCar = 1;
    }

    m_IsLeadCar = isLeadCar;
    IsOccluded = 0;
}

void SFXCTL_Tunnel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);

    g_pEAXSound->GetPlayerTunerCar(
        *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x10)));

    UpdateIsInTunnel(t);
    UpdateDriveBySnds(t);
    UpdateCityVerb(t);
    UpdateReflectionParams(t);

    int context = *static_cast<int *>(static_cast<void *>(
        static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
        0x210));
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

    if (*static_cast<int *>(static_cast<void *>(
            static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
            0x210)) == 0) {
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
    while (true) {
        zone = TheTrackPathManager.FindZone(static_cast<bVector2 *>(static_cast<void *>(&pos)), zonetype, zone);
        if (zone == nullptr) {
            return nullptr;
        }
        if (zone->Elevation == 0.0f) {
            break;
        }
        if (*static_cast<float *>(static_cast<void *>(
                static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                    static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
                0x4C)) <=
            zone->Elevation) {
            return zone;
        }
    }
    return zone;
}

void SFXCTL_Tunnel::UpdateIsInTunnel(float t) {
    (void)t;

    TrackPathZone *zone;
    bool InTunnel;

    CurZoneType = TRACK_PATH_ZONE_TUNNEL;
    zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(
                             static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                                 static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
                             0x44)),
                         TRACK_PATH_ZONE_TUNNEL);
    if (zone == nullptr) {
        CurZoneType = TRACK_PATH_ZONE_OVERPASS;
        zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(
                                 static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                                     static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
                                 0x44)),
                             TRACK_PATH_ZONE_OVERPASS);
        if (zone == nullptr) {
            CurZoneType = TRACK_PATH_ZONE_OVERPASS_SMALL;
            zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(
                                     static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                                         static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
                                     0x44)),
                                 TRACK_PATH_ZONE_OVERPASS_SMALL);
            if (zone == nullptr) {
                CurZoneType = TRACK_PATH_ZONE_GARAGE;
                zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(
                                         static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                                             static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
                                         0x44)),
                                     TRACK_PATH_ZONE_GARAGE);
                if (zone == nullptr) {
                    CurZoneType = TRACK_PATH_ZONE_DYNAMIC;
                    zone = GetTunnelType(*static_cast<bVector3 *>(static_cast<void *>(
                                             static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                                                 static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
                                             0x44)),
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
        int carContext = *static_cast<int *>(static_cast<void *>(
            static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
            0x210));
        if (carContext == 0) {
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
        int carContext = *static_cast<int *>(static_cast<void *>(
            static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
            0x210));
        if (carContext == 0) {
            MMiscSound(0).Send(UCrc32("TunnelUpdate"));
            EndTunnelVerb();
        }
    }

    m_bWasInTunnel = m_bIsInTunnel;
    if (*static_cast<int *>(static_cast<void *>(
            static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
                static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
            0x210)) == 0) {
        m_PlayerZoneType = CurZoneType;
    }
}

void SFXCTL_Tunnel::SetCurrentReverbType(eREVERBFX type, int reverboffset) {
    *static_cast<int *>(static_cast<void *>(&bFadingOut)) = 1;
    m_ReverbOffset = static_cast< float >(reverboffset);
    m_TargetType = type;
    ReflRamp.Initialize(0.0f, 1.0f, g_REVERBFXMODULES[m_ReverbType].FadeOut, LINEAR);
    m_CurWetAemsTarget = 0.0f;
    m_CurDryAemsTarget = 1.0f;
    m_CurWetGinsuTarget = 0.0f;
    m_CurDryGinsuTarget = 1.0f;
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
        (*static_cast<unsigned char *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + 0x21)) == 2)) {
        EAX_CarState *pcar = *static_cast<EAX_CarState **>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0x34));
        if (pcar != nullptr) {
            ReverbAccessor.CaptureData(
                *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(pcar)) + 0x44)),
                *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(pcar)) + 0x48)));
            m_CurReverbZone = ReverbAccessor.GetDataInt(0);
            ncurrentoffset = ReverbAccessor.GetDataInt(2);
        }
    }

    if (static_cast< unsigned int >(m_CurReverbZone) > 0xB) {
        register int zone asm("r0");
        if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(g_pEAXSound)) + 0x84)) == SND_FRONTEND) {
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
    if (*static_cast<int *>(static_cast<void *>(&bFadingOut)) == 0 &&
        *static_cast<int *>(static_cast<void *>(&bFadingIn)) == 0) {
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
    *static_cast<int *>(static_cast<void *>(&bIsReadyForSwitch)) = 0;
    ReflRamp.Update(t);

    if (*static_cast<int *>(static_cast<void *>(&bFadingOut)) != 0) {
        if (*static_cast<int *>(static_cast<void *>(&ReflRamp.bComplete)) != 0 &&
            *static_cast<int *>(static_cast<void *>(&bIsTunnelRamping)) == 0) {
            if (m_IsLeadCar != 0) {
                *static_cast<int *>(static_cast<void *>(&bIsReadyForSwitch)) = 1;
            }
            *static_cast<int *>(static_cast<void *>(&bFadingIn)) = 1;
            m_ReverbType = m_TargetType;
            *static_cast<int *>(static_cast<void *>(&bFadingOut)) = *static_cast<int *>(static_cast<void *>(&bIsTunnelRamping));
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
    } else if (*static_cast<int *>(static_cast<void *>(&bFadingIn)) != 0 &&
               *static_cast<int *>(static_cast<void *>(&ReflRamp.bComplete)) != 0) {
        m_CurWetGinsu = m_CurWetGinsuTarget;
        m_CurDryGinsu = m_CurDryGinsuTarget;
        m_CurWetAems = m_CurWetAemsTarget;
        m_CurDryAems = m_CurDryAemsTarget;
        *static_cast<int *>(static_cast<void *>(&bFadingIn)) = 0;
    }

    if (*static_cast<int *>(static_cast<void *>(&bFadingOut)) != 0 ||
        *static_cast<int *>(static_cast<void *>(&bFadingIn)) != 0) {
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
    if (*static_cast<int *>(static_cast<void *>(&bToggleOffset)) == 0) {
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

    *static_cast<int *>(static_cast<void *>(&bToggleOffset)) = toggleValue;
    *static_cast<int *>(static_cast<void *>(&bIsTunnelRamping)) =
        *static_cast<int *>(static_cast<void *>(&ReflRamp.bComplete)) ^ 1;
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

    originToBarrier[0].x = -SndCamera::m_v3WorldCarPos[0].y;
    originToBarrier[0].y = SndCamera::m_v3WorldCarPos[0].z;
    originToBarrier[0].z = SndCamera::m_v3WorldCarPos[0].x;

    originToBarrier[1].z = *static_cast<float *>(static_cast<void *>(
        static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
        0x44));
    originToBarrier[1].x = -*static_cast<float *>(static_cast<void *>(
        static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
        0x48));
    originToBarrier[1].y = *static_cast<float *>(static_cast<void *>(
        static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
        0x4C));

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
    if (m_IsLeadCar != 0) {
        eREVERBFX reverbType = static_cast< eREVERBFX >(ReverbZoneCrossMap[m_CurReverbZone]);
        if (static_cast< int >(reverbType) > 0xB) {
            reverbType = static_cast< eREVERBFX >(9);
        }
        SetCurrentReverbType(reverbType, 0);
    }
}

void SFXCTL_Tunnel::UpdateDriveBySnds(float t) {
    *static_cast<int *>(static_cast<void *>(&bPlayDriveBy)) = 0;
    *static_cast<int *>(static_cast<void *>(&bPlayTunnelExit)) = 0;

    tTimeToWaitBeforeAnotherDriveBy -= t;
    if (tTimeToWaitBeforeAnotherDriveBy < 0.0f) {
        tTimeToWaitBeforeAnotherDriveBy = 0.0f;
    }
    tTimeToWaitBeforeAnotherExitDriveBy -= t;
    if (tTimeToWaitBeforeAnotherExitDriveBy < 0.0f) {
        tTimeToWaitBeforeAnotherExitDriveBy = 0.0f;
    }

    FutureZoneType = TRACK_PATH_ZONE_RESET;

    EAX_CarState *car =
        *static_cast<EAX_CarState **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0x34));
    bVector3 futureCarPos;
    bVector2 futureCarPos2D;
    bVector2 futureCarPos2DCopy;
    bVector2 scaledFutureCarDir;
    bVector2 scaledFutureCarDirCopy;
    bVector2 futureCarDir;
    bVector2 unNormalizedCurCarDirCopy;
    bVector2 unNormalizedCurCarDir;
    bVector2 curCarDir;

    unNormalizedCurCarDir.x = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x14));
    unNormalizedCurCarDir.y = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x18));
    unNormalizedCurCarDirCopy = unNormalizedCurCarDir;
    bNormalize(&curCarDir, &unNormalizedCurCarDirCopy);

    float futureDistance = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x90)) * 0.4f;
    futureCarDir = curCarDir;
    scaledFutureCarDir.x = futureCarDir.x * futureDistance;
    scaledFutureCarDir.y = futureCarDir.y * futureDistance;
    scaledFutureCarDirCopy = scaledFutureCarDir;
    futureCarPos2D.x = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x44)) + scaledFutureCarDirCopy.x;
    futureCarPos2D.y = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x48)) + scaledFutureCarDirCopy.y;
    futureCarPos2DCopy = futureCarPos2D;
    futureCarPos.x = futureCarPos2DCopy.x;
    futureCarPos.y = futureCarPos2DCopy.y;
    futureCarPos.z = *static_cast<float *>(static_cast<void *>(
        static_cast<char *>(static_cast<void *>(*static_cast<EAX_CarState **>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34)))) +
        0x4C));

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
            if (g_WooshVol_vs_Vel.GetValue(
                    bSqrt(*static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x54)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x54)) +
                          *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x58)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x58)) +
                          *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x5C)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x5C)))) > 0.01f) {
                bPlayDriveBy = true;
                tTimeToWaitBeforeAnotherDriveBy = 3.0f;
                pLastZoneWePlayedWooshFor = futureZone;

                vDriveByLoc.x = futureCarPos.x;
                vDriveByLoc.y = futureCarPos.y;
                vDriveByLoc.z = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x4C)) + 10.0f;
                m_fIntensity = g_WooshVol_vs_Vel.GetValue(
                    bSqrt(*static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x54)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x54)) +
                          *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x58)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x58)) +
                          *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x5C)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x5C))));

                stDriveByInfo driveByInfo;
                driveByInfo.eDriveByType = DRIVE_BY_TUNNEL_IN;
                driveByInfo.pEAXCar = m_pEAXCar;
                EAX_CarState *stateCar = *static_cast<EAX_CarState **>(
                    static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34));
                driveByInfo.ClosingVelocity = bSqrt(
                    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x54)) *
                        *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x54)) +
                    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x58)) *
                        *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x58)) +
                    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x5C)) *
                        *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x5C)));
                driveByInfo.vLocation = vDriveByLoc;
                driveByInfo.UniqueID = 0;
                CSTATE_Base *state = EAXSound::m_pStateMgr[eMM_DRIVEBY]->GetFreeState(&driveByInfo);
                if (state != nullptr) {
                    state->Attach(&driveByInfo);
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
            if (g_WooshVol_vs_Vel.GetValue(
                    bSqrt(*static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x54)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x54)) +
                          *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x58)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x58)) +
                          *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x5C)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x5C)))) > 0.01f) {
                bPlayTunnelExit = true;
                tTimeToWaitBeforeAnotherExitDriveBy = 3.0f;
                pLastZoneWePlayedExitWooshFor = futureZone;

                vDriveByLoc.x = futureCarPos.x;
                vDriveByLoc.y = futureCarPos.y;
                vDriveByLoc.z = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x4C)) + 10.0f;
                m_fExitIntensity = g_WooshVol_vs_Vel.GetValue(
                    bSqrt(*static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x54)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x54)) +
                          *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x58)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x58)) +
                          *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x5C)) *
                              *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x5C))));

                stDriveByInfo driveByInfo;
                driveByInfo.eDriveByType = DRIVE_BY_TUNNEL_OUT;
                driveByInfo.pEAXCar = m_pEAXCar;
                EAX_CarState *stateCar = *static_cast<EAX_CarState **>(
                    static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34));
                driveByInfo.ClosingVelocity = bSqrt(
                    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x54)) *
                        *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x54)) +
                    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x58)) *
                        *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x58)) +
                    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x5C)) *
                        *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x5C)));
                driveByInfo.vLocation = vDriveByLoc;
                driveByInfo.UniqueID = 0;
                CSTATE_Base *state = EAXSound::m_pStateMgr[eMM_DRIVEBY]->GetFreeState(&driveByInfo);
                if (state != nullptr) {
                    state->Attach(&driveByInfo);
                }
            }
        }
    }
}
