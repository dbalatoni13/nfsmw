#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace {
extern Slope g_WheelLoadSlope;
extern float gWheelSlipSensitivity[2];
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
            return reinterpret_cast< bVector3 * >(reinterpret_cast< char * >(this) + 0x28);
        }
        return reinterpret_cast< bVector3 * >(reinterpret_cast< char * >(this) + 0x34);
    }
    if (numtires == 1) {
        return reinterpret_cast< bVector3 * >(reinterpret_cast< char * >(this) + 0x28);
    }
    return reinterpret_cast< bVector3 * >(reinterpret_cast< char * >(this) + 0x28);
}

void SFXCTL_Wheel::GenerateWheelPosition() {}

void SFXCTL_Wheel::GenerateTerrainTypes() {
    EAX_CarState *car =
        m_pStateBase != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pStateBase) + 0x34) : nullptr;

    const Attrib::Collection *col0 = *reinterpret_cast<Attrib::Collection **>(reinterpret_cast<char *>(car) + 0xC0);
    const Attrib::Collection *col1 = *reinterpret_cast<Attrib::Collection **>(reinterpret_cast<char *>(car) + 0x104);
    const Attrib::Collection *col2 = *reinterpret_cast<Attrib::Collection **>(reinterpret_cast<char *>(car) + 0x148);
    const Attrib::Collection *col3 = *reinterpret_cast<Attrib::Collection **>(reinterpret_cast<char *>(car) + 0x18C);

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

    if (*reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(car) + 0x12C) == 2 ||
        *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(car) + 0x170) == 2) {
        const Attrib::Collection *blownTerrain =
            Attrib::FindCollection(Attrib::Gen::simsurface::ClassKey(), 0x8EE645B3);
        Attrib::Instance blown(blownTerrain, 0, nullptr);
        blown.SetDefaultLayout(0xFC);
        RightSideTerrain.Attrib::Instance::operator=(blown);
    } else {
        RightSideTerrain.Attrib::Instance::operator=(CurRight);
    }

    if (*reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(car) + 0xE8) == 2 ||
        *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(car) + 0x1B4) == 2) {
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

    EAX_CarState *car = *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pStateBase) + 0x34);

    int *leftSideTouchingGround = reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0xe8);
    int *rightSideTouchingGround = reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0xec);

    *leftSideTouchingGround = 1;
    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(car) + 0xb8) == 0 &&
        *reinterpret_cast<int *>(reinterpret_cast<char *>(car) + 0xb8 + 3 * 0x44) == 0) {
        *leftSideTouchingGround = 0;
    }

    *rightSideTouchingGround = 1;
    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(car) + 0xb8 + 0x44) == 0 &&
        *reinterpret_cast<int *>(reinterpret_cast<char *>(car) + 0xb8 + 2 * 0x44) == 0) {
        *rightSideTouchingGround = 0;
    }

    bVector2 *normWheelSlip = reinterpret_cast<bVector2 *>(reinterpret_cast<char *>(this) + 0x28);
    bVector2 *totalRight = reinterpret_cast<bVector2 *>(reinterpret_cast<char *>(this) + 0x48);
    bVector2 *totalLeft = reinterpret_cast<bVector2 *>(reinterpret_cast<char *>(this) + 0x50);
    float *wheelTractionMag = reinterpret_cast<float *>(reinterpret_cast<char *>(this) + 0x58);
    float *wheelLoad = reinterpret_cast<float *>(reinterpret_cast<char *>(this) + 0x68);
    bVector2 wheelslip[4];

    totalRight->x = 0.0f;
    totalRight->y = 0.0f;
    totalLeft->x = 0.0f;
    totalLeft->y = 0.0f;

    for (int i = 0; i <= 3; i++) {
        const int wheelOffset = i * 0x44;
        wheelTractionMag[i] = bAbs(*reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0xb4 + wheelOffset));

        wheelslip[i].x = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0xa8 + wheelOffset);
        wheelslip[i].y = *reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0xac + wheelOffset);
        wheelLoad[i] = g_WheelLoadSlope.GetValue(*reinterpret_cast<float *>(reinterpret_cast<char *>(car) + 0xe4 + wheelOffset));

        if (static_cast<unsigned int>(i - 1) < 2U) {
            if (*reinterpret_cast<int *>(reinterpret_cast<char *>(car) + 0xb8 + wheelOffset) != 0) {
                totalRight->x += wheelslip[i].x;
                totalRight->y += wheelslip[i].y;
            }
        } else if (*reinterpret_cast<int *>(reinterpret_cast<char *>(car) + 0xb8 + wheelOffset) != 0) {
            totalLeft->x += wheelslip[i].x;
            totalLeft->y += wheelslip[i].y;
        }

        float slipX = wheelslip[i].x * gWheelSlipSensitivity[0];
        if (slipX < -1023.0f) {
            slipX = -1023.0f;
        }
        if (slipX > 1023.0f) {
            slipX = 1023.0f;
        }
        normWheelSlip[i].x = slipX;

        float slipY = wheelslip[i].y * gWheelSlipSensitivity[1];
        if (slipY < -1023.0f) {
            slipY = -1023.0f;
        }
        if (slipY > 1023.0f) {
            slipY = 1023.0f;
        }
        normWheelSlip[i].y = slipY;

        if (PRINT_SKID_FX_DEBUG != 0 && *reinterpret_cast<int *>(reinterpret_cast<char *>(car) + 0x210) == 0) {
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

            int value = static_cast<int>(normWheelSlip[i].x);
            DebugPrintSkidBar(x, y, "X", (value + 0x3ff) / 2);
            value = static_cast<int>(normWheelSlip[i].y);
            DebugPrintSkidBar(x, y + 0x14, "Y", (value + 0x3ff) / 2);
            DebugPrintSkidBar(x, y + 0x28, "LD", static_cast<int>(wheelLoad[i]));
        }
    }
}
