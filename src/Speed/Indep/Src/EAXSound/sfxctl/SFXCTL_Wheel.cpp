#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace {
extern Slope g_WheelLoadSlope;
extern float gWheelSlipSensitivity[2];
extern float gfTireOffsetDist;
extern float gfTireFwdOffsetDist;
extern int PRINT_SKID_FX_DEBUG;
} // namespace

void DebugPrintSkidBar(int Horz, int Vert, char *Str, int Value);

SFXCTL_Wheel::SFXCTL_Wheel()
    : LeftSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , RightSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , PrevLeftSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) //
    , PrevRightSideTerrain(static_cast< const Attrib::Collection * >(nullptr), 0, nullptr) {}

SFXCTL_Wheel::~SFXCTL_Wheel() {}

SndBase *SFXCTL_Wheel::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Wheel();
}

SndBase::TypeInfo *SFXCTL_Wheel::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Wheel::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Wheel::InitSFX() {
    SFXCTL::InitSFX();
}

void SFXCTL_Wheel::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    UpdateTireParams();
}

bVector3 *SFXCTL_Wheel::GetWheelPos(int wheelID, int numtires) {
    if (numtires == 2) {
        if (wheelID == 0) {
            return &v3NewPosLeft;
        }
        return &v3NewPosRight;
    }
    if (numtires == 1) {
        return &v3NewPosLeft;
    }
    return &v3NewPosLeft;
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

void SFXCTL_Wheel::GenerateTerrainTypes() {
    EAX_CarState *car = m_pStateBase != nullptr ? m_pStateBase->GetPhysCar() : nullptr;
    Sound::Wheel *wheels = car->mWheel;

    const Attrib::Collection *col0 = wheels[0].mTerrainType.GetConstCollection();
    const Attrib::Collection *col1 = wheels[1].mTerrainType.GetConstCollection();
    const Attrib::Collection *col2 = wheels[2].mTerrainType.GetConstCollection();
    const Attrib::Collection *col3 = wheels[3].mTerrainType.GetConstCollection();

    Attrib::Instance FLTerrainType(col0, 0, nullptr);
    FLTerrainType.SetDefaultLayout(0xFC);
    Attrib::Instance FRTerrainType(FLTerrainType);
    FRTerrainType.SetDefaultLayout(0xFC);

    Attrib::Instance RRTerrainType(col1, 0, nullptr);
    RRTerrainType.SetDefaultLayout(0xFC);
    Attrib::Instance FRTemp(RRTerrainType);
    FRTemp.SetDefaultLayout(0xFC);

    Attrib::Instance RLTerrainType(col2, 0, nullptr);
    RLTerrainType.SetDefaultLayout(0xFC);
    Attrib::Instance RRTemp(RLTerrainType);
    RRTemp.SetDefaultLayout(0xFC);

    Attrib::Instance CurRightSrc(col3, 0, nullptr);
    CurRightSrc.SetDefaultLayout(0xFC);
    Attrib::Instance RLTemp(CurRightSrc);
    RLTemp.SetDefaultLayout(0xFC);

    Attrib::Instance CurRight(FRTemp.GetCollection() == RRTemp.GetCollection() ? FRTemp : RightSideTerrain);
    CurRight.SetDefaultLayout(0xFC);
    Attrib::Instance CurLeft(FLTerrainType.GetCollection() == RLTemp.GetCollection() ? FLTerrainType : LeftSideTerrain);
    CurLeft.SetDefaultLayout(0xFC);

    PrevRightSideTerrain.Attrib::Instance::operator=(RightSideTerrain);
    PrevLeftSideTerrain.Attrib::Instance::operator=(LeftSideTerrain);

    if (wheels[1].mBlownState == 2 || wheels[2].mBlownState == 2) {
        const Attrib::Collection *blownTerrain =
            Attrib::FindCollection(Attrib::Gen::simsurface::ClassKey(), 0x8EE645B3);
        Attrib::Instance blown(blownTerrain, 0, nullptr);
        blown.SetDefaultLayout(0xFC);
        RightSideTerrain.Attrib::Instance::operator=(blown);
    } else {
        RightSideTerrain.Attrib::Instance::operator=(CurRight);
    }

    if (wheels[0].mBlownState == 2 || wheels[3].mBlownState == 2) {
        const Attrib::Collection *blownTerrain =
            Attrib::FindCollection(Attrib::Gen::simsurface::ClassKey(), 0x8EE645B3);
        Attrib::Instance blown(blownTerrain, 0, nullptr);
        blown.SetDefaultLayout(0xFC);
        LeftSideTerrain.Attrib::Instance::operator=(blown);
    } else {
        LeftSideTerrain.Attrib::Instance::operator=(CurLeft);
    }
}

void SFXCTL_Wheel::UpdateTireParams() {
    GenerateWheelPosition();
    GenerateTerrainTypes();

    EAX_CarState *car = m_pStateBase->GetPhysCar();
    bVector2 wheelslip[4];

    LeftSideTouchingGround = car->mWheel[0].mWheelOnGround || car->mWheel[3].mWheelOnGround;
    RightSideTouchingGround = car->mWheel[1].mWheelOnGround || car->mWheel[2].mWheelOnGround;

    m_bvTotalRightWheelSlip.x = 0.0f;
    m_bvTotalRightWheelSlip.y = 0.0f;
    m_bvTotalLeftWheelSlip.x = 0.0f;
    m_bvTotalLeftWheelSlip.y = 0.0f;

    for (int i = 0; i <= 3; i++) {
        m_fWheelTractionMag[i] = bAbs(car->mWheel[i].mPercentFsFkTransfer);
        wheelslip[i] = car->mWheel[i].mWheelSlip;
        m_fLoad[i] = g_WheelLoadSlope.GetValue(car->mWheel[i].mLoad);

        if (static_cast<unsigned int>(i - 1) < 2U) {
            if (car->mWheel[i].mWheelOnGround) {
                m_bvTotalRightWheelSlip.x += wheelslip[i].x;
                m_bvTotalRightWheelSlip.y += wheelslip[i].y;
            }
        } else if (car->mWheel[i].mWheelOnGround) {
            m_bvTotalLeftWheelSlip.x += wheelslip[i].x;
            m_bvTotalLeftWheelSlip.y += wheelslip[i].y;
        }

        float slipX = wheelslip[i].x * gWheelSlipSensitivity[0];
        if (slipX < -1023.0f) {
            slipX = -1023.0f;
        }
        if (slipX > 1023.0f) {
            slipX = 1023.0f;
        }
        m_NormWheelSlip[i].x = slipX;

        float slipY = wheelslip[i].y * gWheelSlipSensitivity[1];
        if (slipY < -1023.0f) {
            slipY = -1023.0f;
        }
        if (slipY > 1023.0f) {
            slipY = 1023.0f;
        }
        m_NormWheelSlip[i].y = slipY;

        if (PRINT_SKID_FX_DEBUG != 0 && car->mContext == Sound::CONTEXT_PLAYER) {
            int x = 0;
            int y = 0;

            if (i == 0) {
                x = -0x122;
                y = -0xaa;
            } else if (i == 1) {
                x = 0xe6;
                y = -0xaa;
            } else if (i == 2) {
                x = 0xe6;
                y = -100;
            } else if (i == 3) {
                x = -0x122;
                y = -100;
            }

            int value = static_cast<int>(m_NormWheelSlip[i].x);
            DebugPrintSkidBar(x, y, "X", (value + 0x3ff) / 2);
            value = static_cast<int>(m_NormWheelSlip[i].y);
            DebugPrintSkidBar(x, y + 0x14, "Y", (value + 0x3ff) / 2);
            DebugPrintSkidBar(x, y + 0x28, "LD", static_cast<int>(m_fLoad[i]));
        }
    }
}
