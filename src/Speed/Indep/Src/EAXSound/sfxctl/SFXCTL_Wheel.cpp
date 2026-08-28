#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface_hash.h"

int PRINT_SKID_FX_DEBUG = 0; // size: 0x4, address: 0x8041792C, Decl: 9

DEFINE_CREATABLE(0x20010, SFXCTL_Wheel, SFXCTL);

SFXCTL_Wheel::SFXCTL_Wheel()
    : LeftSideTerrain(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),     //
      RightSideTerrain(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),    //
      PrevLeftSideTerrain(static_cast<Attrib::Collection *>(nullptr), 0, nullptr), //
      PrevRightSideTerrain(static_cast<Attrib::Collection *>(nullptr), 0, nullptr) {
    this->RightSideTouchingGround = true;
    this->LeftSideTouchingGround = true;

    this->LeftSideTerrain = Attrib::Gen::simsurface(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);
    this->RightSideTerrain = Attrib::Gen::simsurface(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);
    this->PrevLeftSideTerrain = Attrib::Gen::simsurface(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);
    this->PrevRightSideTerrain = Attrib::Gen::simsurface(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);
}

SFXCTL_Wheel::~SFXCTL_Wheel() {}

void SFXCTL_Wheel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    this->UpdateTireParams();
}

float gfTireOffsetDist = 5.0f;    // size: 0x4, address: 0x80417940, Decl: 49
float gfTireFwdOffsetDist = 3.0f; // size: 0x4, address: 0x80417944, Decl: 50

float gWheelSlipSensitivity[2] = {200.0f, 81.0f};   // size: 0x8, address: 0x80417948, Decl: 54
float gWheelLoadThreshold[2] = {4000.0f, 10000.0f}; // size: 0x8, address: 0x80417950, Decl: 55

// size: 0x1C, address: 0x8045CF80, Decl: 59
Slope g_WheelLoadSlope(0.0f, 1023.0f, gWheelLoadThreshold[0], gWheelLoadThreshold[1]);

void SFXCTL_Wheel::InitSFX() {
    SFXCTL::InitSFX();

    this->LeftSideTerrain = Attrib::Gen::simsurface(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);
    this->RightSideTerrain = Attrib::Gen::simsurface(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);
    this->PrevLeftSideTerrain = Attrib::Gen::simsurface(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);
    this->PrevRightSideTerrain = Attrib::Gen::simsurface(static_cast<Attrib::Collection *>(nullptr), 0, nullptr);
}

void SFXCTL_Wheel::GenerateWheelPosition() {
    if (this->GetPhysCar()->GetContext() == Sound::CONTEXT_TRAFFIC) {
        return;
    }

    const bVector3 *pv3CarPos = this->GetPhysCar()->GetPosition();
    bVector3 pv3LeftDir = bScale(*this->GetPhysCar()->GetLeftVector(), gfTireOffsetDist);
    bVector3 FwdOffsetVec = bScale(*this->GetPhysCar()->GetForwardVector(), gfTireFwdOffsetDist);
    this->v3NewPosRight = bSub(*pv3CarPos, pv3LeftDir);
    this->v3NewPosRight += FwdOffsetVec;
    this->v3NewPosLeft = bAdd(*pv3CarPos, pv3LeftDir);
    this->v3NewPosLeft += FwdOffsetVec;
}

void DebugPrintSkidBar(int Horz, int Vert, char *Str, int Value);

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

    this->m_bvTotalRightWheelSlip = bVector2(0.0f, 0.0f);
    this->m_bvTotalLeftWheelSlip = bVector2(0.0f, 0.0f);

    bVector2 wheelslip[4];
    float totalwheelload = 0.0f;
    int nloop;
    for (nloop = 0; nloop < 4; nloop++) {
        float vehicle_mass = this->GetPhysCar()->GetAttributes()->MASS();

        this->m_fWheelTractionMag[nloop] = bAbs(this->GetPhysCar()->GetWheelTractionUsage(nloop));
        wheelslip[nloop] = this->GetPhysCar()->GetWheelSlip(nloop);
        this->m_fLoad[nloop] = g_WheelLoadSlope.GetValue(this->GetPhysCar()->GetWheelLoad(nloop));

        if (nloop - 1 < 2u) {
            if (this->GetPhysCar()->IsWheelTouchingGround(nloop)) {
                this->m_bvTotalRightWheelSlip += wheelslip[nloop];
            }
        } else if (this->GetPhysCar()->IsWheelTouchingGround(nloop)) {
            this->m_bvTotalLeftWheelSlip += wheelslip[nloop];
        }

        this->m_NormWheelSlip[nloop].x = bClamp(wheelslip[nloop].x * gWheelSlipSensitivity[0], -1023.0f, 1023.0f);
        this->m_NormWheelSlip[nloop].y = bClamp(wheelslip[nloop].y * gWheelSlipSensitivity[1], -1023.0f, 1023.0f);

        if (PRINT_SKID_FX_DEBUG != 0 && this->GetPhysCar()->IsLocalPlayerCar()) {
            int x = 0;
            int y = 0;
            const float scale = 1.0f;

            switch (nloop) {
                case 0:
                    x = -290;
                    y = -170;
                    break;
                case 1:
                    x = 230;
                    y = -170;
                    break;
                case 3:
                    x = -290;
                    y = -100;
                    break;
                case 2:
                    x = 230;
                    y = -100;
                    break;
            }

            DebugPrintSkidBar(x, y, "X", (static_cast<int>(this->m_NormWheelSlip[nloop].x * scale) + 1023) / 2);
            DebugPrintSkidBar(x, y + 20, "Y", (static_cast<int>(this->m_NormWheelSlip[nloop].y * scale) + 1023) / 2);
            DebugPrintSkidBar(x, y + 40, "LD", static_cast<int>(this->m_fLoad[nloop] * scale));
        }
    }
}

bVector3 *SFXCTL_Wheel::GetWheelPos(int wheelID, int numtires) {
    switch (numtires) {
        case 3:
        default:
            return this->GetPhysCar()->GetPosition();

        case 2:
            if (wheelID == 0) {
                return &this->v3NewPosLeft;
            } else {
                return &this->v3NewPosRight;
            }

        case 1:
            return this->GetPhysCar()->GetPosition();
    }
}

void SFXCTL_Wheel::GenerateTerrainTypes() {
    Attrib::Gen::simsurface FLTerrainType(this->GetPhysCar()->GetWheelTerrain(0));
    Attrib::Gen::simsurface FRTerrainType(this->GetPhysCar()->GetWheelTerrain(1));
    Attrib::Gen::simsurface RRTerrainType(this->GetPhysCar()->GetWheelTerrain(2));
    Attrib::Gen::simsurface RLTerrainType(this->GetPhysCar()->GetWheelTerrain(3));
    Attrib::Gen::simsurface CurRight(FRTerrainType.GetCollection() != RRTerrainType.GetCollection() ? this->RightSideTerrain : FRTerrainType);
    Attrib::Gen::simsurface CurLeft(FLTerrainType.GetCollection() != RLTerrainType.GetCollection() ? this->LeftSideTerrain : FLTerrainType);

    this->PrevRightSideTerrain = this->RightSideTerrain;
    this->PrevLeftSideTerrain = this->LeftSideTerrain;

    if (this->GetPhysCar()->TireState(1) == TIRE_DAMAGE_BLOWN || this->GetPhysCar()->TireState(2) == TIRE_DAMAGE_BLOWN) {
        Attrib::Gen::simsurface blown(Attrib::Hash::simsurface::key_blown_tire, 0, nullptr);
        this->RightSideTerrain = blown;
    } else {
        this->RightSideTerrain = CurRight;
    }

    if (this->GetPhysCar()->TireState(0) == TIRE_DAMAGE_BLOWN || this->GetPhysCar()->TireState(3) == TIRE_DAMAGE_BLOWN) {
        Attrib::Gen::simsurface blown(Attrib::Hash::simsurface::key_blown_tire, 0, nullptr);
        this->LeftSideTerrain = blown;
    } else {
        this->LeftSideTerrain = CurLeft;
    }
}
