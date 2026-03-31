#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern float gWheelLoadThreshold[];
Slope g_WheelLoadSlope(0.0f, 1023.0f, gWheelLoadThreshold[0], gWheelLoadThreshold[1]);

namespace {
extern float gWheelSlipSensitivity[2];
extern float gfTireOffsetDist;
extern float gfTireFwdOffsetDist;
extern int PRINT_SKID_FX_DEBUG;
} // namespace

void DebugPrintSkidBar(int Horz, int Vert, char *Str, int Value);

SndBase::TypeInfo *SFXCTL_Wheel::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_Wheel::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Wheel::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Wheel::GetStaticTypeInfo()->typeName, false) SFXCTL_Wheel();
    }
    return new (SFXCTL_Wheel::GetStaticTypeInfo()->typeName, true) SFXCTL_Wheel();
}

SFXCTL_Wheel::SFXCTL_Wheel()
    : SFXCTL() //
    , LeftSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , RightSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , PrevLeftSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , PrevRightSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) {
    RightSideTouchingGround = true;
    LeftSideTouchingGround = true;

    {
        Attrib::Gen::simsurface terrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr);
        LeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr);
        RightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr);
        PrevLeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr);
        PrevRightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(terrain));
    }
}

SFXCTL_Wheel::~SFXCTL_Wheel() {}

void SFXCTL_Wheel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    UpdateTireParams();
}

void SFXCTL_Wheel::InitSFX() {
    SFXCTL::InitSFX();

    {
        Attrib::Gen::simsurface terrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr);
        LeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr);
        RightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr);
        PrevLeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr);
        PrevRightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(terrain));
    }
}

void SFXCTL_Wheel::GenerateWheelPosition() {
    const bVector3 *pv3CarPos;
    bVector3 pv3LeftDir;
    bVector3 FwdOffsetVec;

    if (GetPhysCar()->GetContext() == Sound::kRaceContext_Count) {
        return;
    }

    pv3CarPos = GetPhysCar()->GetPosition();
    pv3LeftDir = bScale(*GetPhysCar()->GetLeftVector(), gfTireOffsetDist);
    FwdOffsetVec = bScale(*GetPhysCar()->GetForwardVector(), gfTireFwdOffsetDist);
    v3NewPosRight = bSub(*pv3CarPos, pv3LeftDir);
    v3NewPosRight += FwdOffsetVec;
    v3NewPosLeft = bAdd(*pv3CarPos, pv3LeftDir);
    v3NewPosLeft += FwdOffsetVec;
}

void SFXCTL_Wheel::UpdateTireParams() {
    GenerateWheelPosition();
    GenerateTerrainTypes();

    bVector2 wheelslip[4];
    float totalwheelload;
    int nloop;

    LeftSideTouchingGround = true;
    if (!GetPhysCar()->IsWheelTouchingGround(0) && !GetPhysCar()->IsWheelTouchingGround(3)) {
        LeftSideTouchingGround = false;
    }

    RightSideTouchingGround = true;
    if (!GetPhysCar()->IsWheelTouchingGround(1) && !GetPhysCar()->IsWheelTouchingGround(2)) {
        RightSideTouchingGround = false;
    }

    m_bvTotalRightWheelSlip = bVector2(0.0f, 0.0f);
    m_bvTotalLeftWheelSlip = bVector2(0.0f, 0.0f);
    totalwheelload = 0.0f;

    {
        float vehicle_mass;

        vehicle_mass = GetPhysCar()->GetAttributes()->MASS();
        for (nloop = 0; nloop <= 3; nloop++) {
            m_fWheelTractionMag[nloop] = bAbs(GetPhysCar()->GetWheelTractionUsage(nloop));
            wheelslip[nloop] = GetPhysCar()->GetWheelSlip(nloop);
            m_fLoad[nloop] = g_WheelLoadSlope.GetValue(GetPhysCar()->GetWheelLoad(nloop));

            if (static_cast<unsigned int>(nloop - 1) < 2U) {
                if (GetPhysCar()->IsWheelTouchingGround(nloop)) {
                    bAdd(&m_bvTotalRightWheelSlip, &m_bvTotalRightWheelSlip, &wheelslip[nloop]);
                }
            } else if (GetPhysCar()->IsWheelTouchingGround(nloop)) {
                bAdd(&m_bvTotalLeftWheelSlip, &m_bvTotalLeftWheelSlip, &wheelslip[nloop]);
            }

            m_NormWheelSlip[nloop].x =
                bClamp(wheelslip[nloop].x * gWheelSlipSensitivity[0], -1023.0f, 1023.0f);
            m_NormWheelSlip[nloop].y =
                bClamp(wheelslip[nloop].y * gWheelSlipSensitivity[1], -1023.0f, 1023.0f);

            if (PRINT_SKID_FX_DEBUG != 0 && GetPhysCar()->IsLocalPlayerCar()) {
                int x;
                int y;
                const float scale = 0.5f;

                x = 0;
                y = 0;

                if (nloop == 1) {
                    x = 0xe6;
                    y = -0xaa;
                } else if (nloop < 2) {
                    if (nloop == 0) {
                        x = -0x122;
                        y = -0xaa;
                    }
                } else {
                    if (nloop == 2) {
                        x = 0xe6;
                    } else {
                        if (nloop != 3) {
                            goto print_skid_bars;
                        }
                        x = -0x122;
                    }
                    y = -100;
                }

            print_skid_bars:
                (void)scale;
                DebugPrintSkidBar(x, y, "X", (static_cast<int>(m_NormWheelSlip[nloop].x) + 0x3ff) / 2);
                DebugPrintSkidBar(x, y + 0x14, "Y", (static_cast<int>(m_NormWheelSlip[nloop].y) + 0x3ff) / 2);
                DebugPrintSkidBar(x, y + 0x28, "LD", static_cast<int>(m_fLoad[nloop]));
            }
        }
    }
}

bVector3 *SFXCTL_Wheel::GetWheelPos(int wheelID, int numtires) {
    switch (numtires) {
    case 2:
        if (wheelID == 0) {
            return &v3NewPosLeft;
        }
        return &v3NewPosRight;
    case 1:
        return GetPhysCar()->GetPosition();
    default:
        return GetPhysCar()->GetPosition();
    }
}

void SFXCTL_Wheel::GenerateTerrainTypes() {
    Attrib::Gen::simsurface FLTerrainType(GetPhysCar()->GetWheelTerrain(0));
    Attrib::Gen::simsurface FRTerrainType(GetPhysCar()->GetWheelTerrain(1));
    Attrib::Gen::simsurface RRTerrainType(GetPhysCar()->GetWheelTerrain(2));
    Attrib::Gen::simsurface RLTerrainType(GetPhysCar()->GetWheelTerrain(3));
    Attrib::Gen::simsurface CurRight(FRTerrainType.GetCollection() != RRTerrainType.GetCollection() ? RightSideTerrain : FRTerrainType);
    Attrib::Gen::simsurface CurLeft(FLTerrainType.GetCollection() != RLTerrainType.GetCollection() ? LeftSideTerrain : FLTerrainType);

    PrevRightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(RightSideTerrain));
    PrevLeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(LeftSideTerrain));

    if (GetPhysCar()->TireState(1) == TIRE_DAMAGE_BLOWN || GetPhysCar()->TireState(2) == TIRE_DAMAGE_BLOWN) {
        Attrib::Gen::simsurface blown(0x8EE645B3, 0, nullptr);
        RightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(blown));
    } else {
        RightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(CurRight));
    }

    if (GetPhysCar()->TireState(0) == TIRE_DAMAGE_BLOWN || GetPhysCar()->TireState(3) == TIRE_DAMAGE_BLOWN) {
        Attrib::Gen::simsurface blown(0x8EE645B3, 0, nullptr);
        LeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(blown));
    } else {
        LeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast< const Attrib::Instance & >(CurLeft));
    }
}

const Attrib::Gen::simsurface &Attrib::Gen::simsurface::operator=(const Instance &rhs) {
    Instance::operator=(rhs);
    return *this;
}

Attrib::Key Attrib::Gen::simsurface::ClassKey() {
    return 0xfb111fef;
}
