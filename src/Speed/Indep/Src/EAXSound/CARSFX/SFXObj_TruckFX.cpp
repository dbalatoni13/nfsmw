#include "./SFXObj_TruckFX.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace Csis {
extern ClassHandle gFX_TRUCK_FXHandle;
extern InterfaceId FX_TRUCK_FXId;
} // namespace Csis

extern int IsSoundEnabled;

struct FX_TRUCK_FXStruct {
    int id;           // offset 0x0, size 0x4
    int volume;       // offset 0x4, size 0x4
    int pitch_OFFSET; // offset 0x8, size 0x4
    int azimuth;      // offset 0xC, size 0x4
    int loPass;       // offset 0x10, size 0x4
    int hiPass;       // offset 0x14, size 0x4
    int fX_Dry;       // offset 0x18, size 0x4
    int fX_Wet;       // offset 0x1C, size 0x4
};

struct FX_TRUCK_FX {
    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    FX_TRUCK_FX(int id, int volume, int pitch_OFFSET, int azimuth, int loPass, int hiPass, int fX_Dry, int fX_Wet)
        : mpClass(nullptr) {
        Csis::System::Result result;

        SetId(id);
        SetVolume(volume);
        SetPitch_OFFSET(pitch_OFFSET);
        SetAzimuth(azimuth);
        SetLoPass(loPass);
        SetHiPass(hiPass);
        SetFX_Dry(fX_Dry);
        SetFX_Wet(fX_Wet);

        result = static_cast<Csis::System::Result>(Csis::Class::CreateInstance(&Csis::gFX_TRUCK_FXHandle, &mData, &mpClass));
        if (result < Csis::System::kResult_Ok) {
            Csis::gFX_TRUCK_FXHandle.Set(&Csis::FX_TRUCK_FXId);
            Csis::Class::CreateInstance(&Csis::gFX_TRUCK_FXHandle, &mData, &mpClass);
        }
    }

    ~FX_TRUCK_FX() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }

    int GetRefCount() {
        int refCount;

        refCount = 0;
        if (mpClass) {
            mpClass->GetRefCount(&refCount);
        }
        return refCount;
    }

    void SetId(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xF) {
            x = 0xF;
        }
        mData.id = x;
    }

    void SetVolume(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.volume = x;
    }

    void SetPitch_OFFSET(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        mData.pitch_OFFSET = x;
    }

    void SetAzimuth(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0xFFFF) {
            x = 0xFFFF;
        }
        mData.azimuth = x;
    }

    void SetLoPass(int x) {
        mData.loPass = x;
    }

    void SetHiPass(int x) {
        mData.hiPass = x;
    }

    void SetFX_Dry(int x) {
        mData.fX_Dry = x;
    }

    void SetFX_Wet(int x) {
        mData.fX_Wet = x;
    }

    Csis::Class *mpClass;    // offset 0x0, size 0x4
    FX_TRUCK_FXStruct mData; // offset 0x4, size 0x20
};

SndBase::TypeInfo SFXCTL_3DTrailerPos::s_TypeInfo = {
    0x000C00F0, "SFXCTL_3DTrailerPos", &SFXCTL_3DCarPos::s_TypeInfo, SFXCTL_3DTrailerPos::CreateObject};
SndBase::TypeInfo CARSFX_TruckWoosh::s_TypeInfo = {
    0x000C00E0, "CARSFX_TruckWoosh", &CARSFX_TrafficWoosh::s_TypeInfo, CARSFX_TruckWoosh::CreateObject};
SndBase::TypeInfo SFXObj_TruckFX::s_TypeInfo = { 0, "SFXObj_TruckFX", nullptr, SFXObj_TruckFX::CreateObject };

SFXCTL_3DTrailerPos::~SFXCTL_3DTrailerPos() {}

SndBase::TypeInfo *SFXCTL_3DTrailerPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DTrailerPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DTrailerPos::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) SFXCTL_3DTrailerPos();
    }
    return new (s_TypeInfo.typeName, true) SFXCTL_3DTrailerPos();
}

SndBase::TypeInfo *CARSFX_TruckWoosh::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_TruckWoosh::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_TruckWoosh::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (CARSFX_TruckWoosh::GetStaticTypeInfo()->typeName, false) CARSFX_TruckWoosh();
    }
    return new (CARSFX_TruckWoosh::GetStaticTypeInfo()->typeName, true) CARSFX_TruckWoosh();
}

CARSFX_TruckWoosh::CARSFX_TruckWoosh()
    : CARSFX_TrafficWoosh()
    , m_TrailerRef(0) {
    m_p3DTrailerPos = nullptr;
    m_vTrailerPos = bVector3(0.0f, 0.0f, 0.0f);
    m_vTrailerVel = bVector3(0.0f, 0.0f, 0.0f);
}

CARSFX_TruckWoosh::~CARSFX_TruckWoosh() {}

int CARSFX_TruckWoosh::GetController(int Index) {
    int iVar1;

    iVar1 = 0xF;
    if (Index != 0) {
        iVar1 = -1;
    }
    return iVar1;
}

void CARSFX_TruckWoosh::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0xF) {
        m_p3DTrailerPos = static_cast<SFXCTL_3DCarPos *>(psfxctl);
    }
}

void CARSFX_TruckWoosh::InitSFX() {
    SndBase::InitSFX();
    m_p3DTrailerPos->AssignPositionVector(&m_vTrailerPos);
    m_p3DTrailerPos->AssignVelocityVector(&m_vTrailerVel);
}

bool CARSFX_TruckWoosh::IsPlayerCarInRadius() {
    return GetPlayerCarInRadius(m_vTrailerPos, 10.0f) != nullptr;
}

void CARSFX_TruckWoosh::UpdateParams(float t) {
    if (GetPhysCar()->GetContext() == Sound::CONTEXT_TRACTOR) {
        unsigned int worldid = GetPhysCar()->mTrailerID;

        if (worldid != 0) {
            m_TrailerRef.Set(worldid);
            if (m_TrailerRef.IsValid()) {
                m_vTrailerPos = *static_cast<const bVector3 *>(static_cast<const void *>(&m_TrailerRef.GetMatrix()->v3));
                m_vTrailerVel = *GetPhysCar()->GetVelocity();
            }
        }
    }
    CARSFX_TrafficWoosh::UpdateParams(t);
}

eDRIVE_BY_TYPE CARSFX_TruckWoosh::GetWooshSample() {
    EAX_CarState *pcar;
    bVector3 normalvel;
    float dotprod;
    const eDRIVE_BY_TYPE *resultptr;

    pcar = GetClosestPlayerCar(GetPhysCar()->GetPosition());
    if (!pcar) {
        return DRIVE_BY_TREE;
    }

    normalvel = bNormalize(m_vTrailerVel);
    dotprod = bAbs(bDot(*pcar->GetForwardVector(), normalvel));
    if (0.2f <= dotprod) {
        SetDMIX_Input(6, 0);
        return DRIVE_BY_AI_CAR;
    }

    SetDMIX_Input(6, 0x7FFF);
    resultptr = reinterpret_cast<const eDRIVE_BY_TYPE *>(GetPhysCar()->GetAttributes()->GetAttributePointer(0x7e744600, 0));
    if (!resultptr) {
        resultptr = reinterpret_cast<const eDRIVE_BY_TYPE *>(Attrib::DefaultDataArea(sizeof(eDRIVE_BY_TYPE)));
    }
    return *resultptr;
}

SndBase::TypeInfo *SFXObj_TruckFX::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXObj_TruckFX::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXObj_TruckFX::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXObj_TruckFX::GetStaticTypeInfo()->typeName, false) SFXObj_TruckFX();
    }
    return new (SFXObj_TruckFX::GetStaticTypeInfo()->typeName, true) SFXObj_TruckFX();
}

SFXObj_TruckFX::SFXObj_TruckFX()
    : CARSFX() {
    m_pTruckFX = nullptr;
    m_fSpeed = 0.0f;
    m_bStopped = true;
}

SFXObj_TruckFX::~SFXObj_TruckFX() {
    Destroy();
}

void SFXObj_TruckFX::Destroy() {
    if (m_pTruckFX) {
        delete m_pTruckFX;
    }
    m_pTruckFX = nullptr;
}

void SFXObj_TruckFX::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXObj_TruckFX::UpdateParams(float t) {
    if (!m_pTruckFX) {
        if (GetPhysCar()->IsShifting()) {
            int rand_puff_ID;

            rand_puff_ID = bRandom(4) + 2;
            g_pEAXSound->SetCsisName("Csis::Tractor air puff SFX");
            m_pTruckFX = new FX_TRUCK_FX(rand_puff_ID, 0, 0, GetDMixOutput(0, DMX_AZIM), 25000, 0, 0x7FFF, 0);
            if (m_pTruckFX && m_pTruckFX->GetRefCount() < 3) {
                delete m_pTruckFX;
                m_pTruckFX = nullptr;
                return;
            }
        } else if ((GetPhysCar()->GetBrake() > 0.0f || GetPhysCar()->GetEBrake() > 0.0f) &&
                   GetPhysCar()->GetVelocityMagnitudeMPH() < 1.0f &&
                   m_fSpeed >= 1.0f) {
            if (!m_bStopped) {
                int rand_brake_ID;

                rand_brake_ID = bRandom(2);
                g_pEAXSound->SetCsisName("Csis::Tractor brake SFX");
                m_pTruckFX = new FX_TRUCK_FX(rand_brake_ID, 0, 0, GetDMixOutput(0, DMX_AZIM), 25000, 0, 0x7FFF, 0);
                if (m_pTruckFX && m_pTruckFX->GetRefCount() < 3) {
                    delete m_pTruckFX;
                    m_pTruckFX = nullptr;
                    return;
                }
                m_bStopped = true;
            }
        }
    }

    if (m_bStopped) {
        if (m_fSpeed < GetPhysCar()->GetVelocityMagnitudeMPH()) {
            m_bStopped = false;
        }
    }

    m_fSpeed = GetPhysCar()->GetVelocityMagnitudeMPH();
}

void SFXObj_TruckFX::ProcessUpdate() {
    if (m_pTruckFX) {
        if (m_pTruckFX->GetRefCount() == 1) {
            delete m_pTruckFX;
            m_pTruckFX = nullptr;
        } else {
            m_pTruckFX->SetVolume(GetDMixOutput(1, DMX_VOL));
            m_pTruckFX->SetAzimuth(GetDMixOutput(0, DMX_AZIM));
            m_pTruckFX->SetPitch_OFFSET(GetDMixOutput(2, DMX_PITCH) - 0x1000);
            m_pTruckFX->CommitMemberData();
        }
    }
}

int SFXObj_TruckFX::GetController(int Index) {
    return -1;
}

void SFXObj_TruckFX::AttachController(SFXCTL *psfxctl) {}

void SFXObj_TruckFX::InitSFX() {
    if (IsSoundEnabled != 0 && GetInputBlockPtr()) {
        GetInputBlockPtr()[15] = 1;
    }
}

void SFXObj_TruckFX::Detach() {
    Destroy();
}
