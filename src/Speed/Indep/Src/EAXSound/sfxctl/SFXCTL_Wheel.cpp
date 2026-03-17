#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace {
extern Slope g_WheelLoadSlope;
extern float gWheelSlipSensitivity[2];
extern int PRINT_SKID_FX_DEBUG;

struct EAXCarStateWheelEntryView {
    bVector2 mWheelSlip;
    float _pad8;
    float mWheelTractionMag;
    int mWheelOnGround;
    char _pad14[0x4];
    const Attrib::Collection *mTerrainCollection;
    char _pad1C[0x20];
    float mLoad;
    unsigned char mBlownState;
    char _pad41[0x3];
};

struct EAXCarStateWheelView {
    char _pad0[0xA8];
    EAXCarStateWheelEntryView mWheel[4];
    char _pad1B8[0x58];
    int mContext;
};

struct CSTATE_WheelView {
    char _pad0[0x34];
    EAXCarStateWheelView *mCarState;
};

struct SFXCTL_WheelPosView {
    char _pad0[0x28];
    bVector3 mWheelPos[2];
};

struct SFXCTL_WheelRuntimeView {
    char _pad0[0x28];
    bVector2 mNormWheelSlip[4];
    bVector2 mTotalRight;
    bVector2 mTotalLeft;
    float mWheelTractionMag[4];
    float mWheelLoad[4];
    char _pad78[0x70];
    int mLeftSideTouchingGround;
    int mRightSideTouchingGround;
};

#define WHEEL_STATE_VIEW(ptr) (*static_cast<CSTATE_WheelView *>(static_cast<void *>(ptr)))
#define WHEEL_POS_VIEW(ptr) (*static_cast<SFXCTL_WheelPosView *>(static_cast<void *>(ptr)))
#define WHEEL_RUNTIME_VIEW(ptr) (*static_cast<SFXCTL_WheelRuntimeView *>(static_cast<void *>(ptr)))
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
            return &WHEEL_POS_VIEW(this).mWheelPos[0];
        }
        return &WHEEL_POS_VIEW(this).mWheelPos[1];
    }
    if (numtires == 1) {
        return &WHEEL_POS_VIEW(this).mWheelPos[0];
    }
    return &WHEEL_POS_VIEW(this).mWheelPos[0];
}

void SFXCTL_Wheel::GenerateWheelPosition() {}

void SFXCTL_Wheel::GenerateTerrainTypes() {
    EAXCarStateWheelView *car = m_pStateBase != nullptr ? WHEEL_STATE_VIEW(m_pStateBase).mCarState : nullptr;

    const Attrib::Collection *col0 = car->mWheel[0].mTerrainCollection;
    const Attrib::Collection *col1 = car->mWheel[1].mTerrainCollection;
    const Attrib::Collection *col2 = car->mWheel[2].mTerrainCollection;
    const Attrib::Collection *col3 = car->mWheel[3].mTerrainCollection;

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

    if (car->mWheel[1].mBlownState == 2 || car->mWheel[2].mBlownState == 2) {
        const Attrib::Collection *blownTerrain =
            Attrib::FindCollection(Attrib::Gen::simsurface::ClassKey(), 0x8EE645B3);
        Attrib::Instance blown(blownTerrain, 0, nullptr);
        blown.SetDefaultLayout(0xFC);
        RightSideTerrain.Attrib::Instance::operator=(blown);
    } else {
        RightSideTerrain.Attrib::Instance::operator=(CurRight);
    }

    if (car->mWheel[0].mBlownState == 2 || car->mWheel[3].mBlownState == 2) {
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

    EAX_CarState *car =
        *static_cast<EAX_CarState **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34));

    int *leftSideTouchingGround = static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + 0xe8));
    int *rightSideTouchingGround = static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + 0xec));

    *leftSideTouchingGround = 1;
    if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xb8)) == 0 &&
        *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xb8 + 3 * 0x44)) == 0) {
        *leftSideTouchingGround = 0;
    }

    *rightSideTouchingGround = 1;
    if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xb8 + 0x44)) == 0 &&
        *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xb8 + 2 * 0x44)) == 0) {
        *rightSideTouchingGround = 0;
    }

    bVector2 *normWheelSlip = static_cast<bVector2 *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + 0x28));
    bVector2 *totalRight = static_cast<bVector2 *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + 0x48));
    bVector2 *totalLeft = static_cast<bVector2 *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + 0x50));
    float *wheelTractionMag = static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + 0x58));
    float *wheelLoad = static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + 0x68));
    bVector2 wheelslip[4];

    totalRight->x = 0.0f;
    totalRight->y = 0.0f;
    totalLeft->x = 0.0f;
    totalLeft->y = 0.0f;

    for (int i = 0; i <= 3; i++) {
        const int wheelOffset = i * 0x44;
        wheelTractionMag[i] = bAbs(*static_cast<float *>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xb4 + wheelOffset)));

        wheelslip[i].x = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xa8 + wheelOffset));
        wheelslip[i].y = *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xac + wheelOffset));
        wheelLoad[i] = g_WheelLoadSlope.GetValue(
            *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xe4 + wheelOffset)));

        if (static_cast<unsigned int>(i - 1) < 2U) {
            if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xb8 + wheelOffset)) != 0) {
                totalRight->x += wheelslip[i].x;
                totalRight->y += wheelslip[i].y;
            }
        } else if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0xb8 + wheelOffset)) != 0) {
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

        if (PRINT_SKID_FX_DEBUG != 0 &&
            *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(car)) + 0x210)) == 0) {
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
