#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct bVector3;

extern eView eViews[];

CSTATE_Base::StateInfo EAXTunerCar::s_StateInfo = {
    0x00020000,
    "EAXTunerCar",
    &EAXCar::s_StateInfo,
    reinterpret_cast<CSTATE_Base *(*)(void)>(&EAXTunerCar::CreateState),
};

CSTATE_Base::StateInfo *EAXTunerCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

const char *EAXTunerCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXTunerCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTunerCar), s_StateInfo.stateName, false)) EAXTunerCar;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTunerCar), s_StateInfo.stateName, true)) EAXTunerCar;
    }
}

EAXTunerCar::EAXTunerCar()
    : EAXCar() //
{
    bFirstUpdate = true;
    TrunkBounceInstensity = 0.0f;
    BottomOutPlay = false;
    TrunkBouncePlay = false;
    PlayBackFire = false;
}

EAXTunerCar::~EAXTunerCar() {}

void EAXTunerCar::PreLoadAssets() {}

void EAXTunerCar::ProcessSoundSphere(unsigned int unamehash, int nparamid, bVector3 *pv3pos, float fradius) {}

int EAXTunerCar::SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) {
    switch (SFXMessageType) {
    case SFX_NONE:
        break;
    case SFX_SHIFT_UP:
    case SFX_SHIFT_DOWN:
    case SFX_NITROUS:
        break;
    case SFX_BOTTOMOUT:
        BottomOutPlay = true;
        BottomOutIntensity = param1 >> 8;
        break;
    case SFX_TRUNKBOUNCE:
        TrunkBouncePlay = true;
        *reinterpret_cast<int *>(&TrunkBounceInstensity) = param1;
        break;
    case SFX_CHANGEGEAR:
        return 0;
    default:
        break;
    }
    return EAXCar::SFXMessage(SFXMessageType, param1, param2);
}

void DebugPrintSkidBar(int Horz, int Vert, char *Str, int Value) {}

int EAXTunerCar::UpdateRotation() {
    int val = 0;
    m_Rotation = 0;
    val = bMax(val, 0);
    asm volatile("" : "+r"(val));
    val = bMin(val, 0x400);
    m_Rotation = val;
    return val;
}

void EAXTunerCar::UpdatePov() {
    char *views = (char *)eViews;
    void **pNext = (void **)(views + 0xA4);
    void *sentinel = (void *)(views + 0xA4);
    char *cm = 0;
    if (*pNext != sentinel) {
        cm = (char *)*pNext;
    }
    if (cm != 0) {
        int *vtable2 = *(int **)(cm + 0x8);
        short vthis_off = *(short *)((char *)vtable2 + 0x28);
        int (*vfunc)(char *) = (int (*)(char *))(*(int *)((char *)vtable2 + 0x2C));
        char *anchor = (char *)vfunc(cm + vthis_off);

        m_IsDriveCamera = (*(int *)(cm + 0xC) == 1);

        if (anchor != 0) {
            m_PovType = static_cast<int>(*(short *)(anchor + 0xD8));
        } else {
            m_PovType = 7;
        }
    } else {
        m_IsDriveCamera = 0;
    }
}

void EAXTunerCar::FirstUpdate(float t) {
    bFirstUpdate = false;
}

void EAXTunerCar::UpdateParams(float t) {
    EAXCar::UpdateParams(t);
    if (m_pCar != 0) {
        if (bFirstUpdate) {
            FirstUpdate(t);
        }
        UpdatePov();
        UpdateRotation();
    }
}
