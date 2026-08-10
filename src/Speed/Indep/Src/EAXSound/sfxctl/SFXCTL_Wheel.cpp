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
    , LeftSideTerrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr) //
    , RightSideTerrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr) //
    , PrevLeftSideTerrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr) //
    , PrevRightSideTerrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr) {
    this->RightSideTouchingGround = true;
    this->LeftSideTouchingGround = true;

    {
        Attrib::Gen::simsurface terrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        this->LeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        this->RightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        this->PrevLeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        this->PrevRightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(terrain));
    }
}

SFXCTL_Wheel::~SFXCTL_Wheel() {}

void SFXCTL_Wheel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    this->UpdateTireParams();
}

void SFXCTL_Wheel::InitSFX() {
    SFXCTL::InitSFX();

    {
        Attrib::Gen::simsurface terrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        this->LeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        this->RightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        this->PrevLeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(terrain));
    }

    {
        Attrib::Gen::simsurface terrain(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
        this->PrevRightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(terrain));
    }
}

void SFXCTL_Wheel::GenerateWheelPosition() {
    const bVector3 *pv3CarPos;

    if (this->GetPhysCar()->GetContext() == Sound::kRaceContext_Count) {
        return;
    }

    pv3CarPos = this->GetPhysCar()->GetPosition();
    bVector3 pv3LeftDir = bScale(*this->GetPhysCar()->GetLeftVector(), gfTireOffsetDist);
    bVector3 FwdOffsetVec = bScale(*this->GetPhysCar()->GetForwardVector(), gfTireFwdOffsetDist);
    this->v3NewPosRight = bSub(*pv3CarPos, pv3LeftDir);
    this->v3NewPosRight += FwdOffsetVec;
    this->v3NewPosLeft = bAdd(*pv3CarPos, pv3LeftDir);
    this->v3NewPosLeft += FwdOffsetVec;
}

void SFXCTL_Wheel::UpdateTireParams() {
    this->GenerateWheelPosition();
    this->GenerateTerrainTypes();

    this->LeftSideTouchingGround = true;
    if (!this->GetPhysCar()->IsWheelTouchingGround(0) && !this->GetPhysCar()->IsWheelTouchingGround(3)) {
        this->LeftSideTouchingGround = false;
    }

    this->RightSideTouchingGround = true;
    if (!this->GetPhysCar()->IsWheelTouchingGround(1) && !this->GetPhysCar()->IsWheelTouchingGround(2)) {
        this->RightSideTouchingGround = false;
    }

    {
        float totalwheelload;
        int nloop;

        this->m_bvTotalLeftWheelSlip.y = 0.0f;
        this->m_bvTotalRightWheelSlip.x = 0.0f;
        this->m_bvTotalLeftWheelSlip.x = 0.0f;
        this->m_bvTotalRightWheelSlip.y = 0.0f;
        totalwheelload = 0.0f;
        bVector2 wheelslip[4];
        for (nloop = 0; nloop <= 3; nloop++) {
            this->m_fWheelTractionMag[nloop] = bAbs(this->GetPhysCar()->GetWheelTractionUsage(nloop));
            wheelslip[nloop] = this->GetPhysCar()->GetWheelSlip(nloop);
            this->m_fLoad[nloop] = g_WheelLoadSlope.GetValue(this->GetPhysCar()->GetWheelLoad(nloop));

            if (static_cast<unsigned int>(nloop - 1) < 2U) {
                if (this->GetPhysCar()->IsWheelTouchingGround(nloop)) {
                    bAdd(&this->m_bvTotalRightWheelSlip, &this->m_bvTotalRightWheelSlip, &wheelslip[nloop]);
                }
            } else if (this->GetPhysCar()->IsWheelTouchingGround(nloop)) {
                bAdd(&this->m_bvTotalLeftWheelSlip, &this->m_bvTotalLeftWheelSlip, &wheelslip[nloop]);
            }

            this->m_NormWheelSlip[nloop].x =
                bClamp(wheelslip[nloop].x * gWheelSlipSensitivity[0], -1023.0f, 1023.0f);
            this->m_NormWheelSlip[nloop].y =
                bClamp(wheelslip[nloop].y * gWheelSlipSensitivity[1], -1023.0f, 1023.0f);

            if (PRINT_SKID_FX_DEBUG != 0 && this->GetPhysCar()->IsLocalPlayerCar()) {
                int x;
                int y;

                x = 0;
                y = 0;

                switch (nloop) {
                case 0:
                    x = -0x122;
                    y = -0xaa;
                    break;
                case 1:
                    x = 0xe6;
                    y = -0xaa;
                    break;
                case 3:
                    x = -0x122;
                    y = -100;
                    break;
                case 2:
                    x = 0xe6;
                    y = -100;
                    break;
                }

                DebugPrintSkidBar(x, y, "X", (static_cast<int>(this->m_NormWheelSlip[nloop].x) + 0x3ff) / 2);
                DebugPrintSkidBar(x, y + 0x14, "Y", (static_cast<int>(this->m_NormWheelSlip[nloop].y) + 0x3ff) / 2);
                DebugPrintSkidBar(x, y + 0x28, "LD", static_cast<int>(this->m_fLoad[nloop]));
            }
        }
    }
}

bVector3 *SFXCTL_Wheel::GetWheelPos(int wheelID, int numtires) {
    switch (numtires) {
    case 1:
        return this->GetPhysCar()->GetPosition();
    case 2:
        if (wheelID == 0) {
            return &this->v3NewPosLeft;
        }
        return &this->v3NewPosRight;
    case 3:
    default:
        return this->GetPhysCar()->GetPosition();
    }
    return this->GetPhysCar()->GetPosition();
}

void SFXCTL_Wheel::GenerateTerrainTypes() {
    Attrib::Gen::simsurface FLTerrainType(this->GetPhysCar()->GetWheelTerrain(0));
    Attrib::Gen::simsurface FRTerrainType(this->GetPhysCar()->GetWheelTerrain(1));
    Attrib::Gen::simsurface RRTerrainType(this->GetPhysCar()->GetWheelTerrain(2));
    Attrib::Gen::simsurface RLTerrainType(this->GetPhysCar()->GetWheelTerrain(3));
    Attrib::Gen::simsurface CurRight(FRTerrainType.GetCollection() != RRTerrainType.GetCollection() ? this->RightSideTerrain : FRTerrainType);
    Attrib::Gen::simsurface CurLeft(FLTerrainType.GetCollection() != RLTerrainType.GetCollection() ? this->LeftSideTerrain : FLTerrainType);

    this->PrevRightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(this->RightSideTerrain));
    this->PrevLeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(this->LeftSideTerrain));

    if (this->GetPhysCar()->TireState(1) == TIRE_DAMAGE_BLOWN || this->GetPhysCar()->TireState(2) == TIRE_DAMAGE_BLOWN) {
        Attrib::Gen::simsurface blown(0x8EE645B3, 0, nullptr);
        this->RightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(blown));
    } else {
        this->RightSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(CurRight));
    }

    if (this->GetPhysCar()->TireState(0) == TIRE_DAMAGE_BLOWN || this->GetPhysCar()->TireState(3) == TIRE_DAMAGE_BLOWN) {
        Attrib::Gen::simsurface blown(0x8EE645B3, 0, nullptr);
        this->LeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(blown));
    } else {
        this->LeftSideTerrain.Attrib::Gen::simsurface::operator=(static_cast<const Attrib::Instance &>(CurLeft));
    }
}

const Attrib::Gen::simsurface &Attrib::Gen::simsurface::operator=(const Instance &rhs) {
    Instance::operator=(rhs);
    return *this;
}

Attrib::Key Attrib::Gen::simsurface::ClassKey() {
    return 0xfb111fef;
}
