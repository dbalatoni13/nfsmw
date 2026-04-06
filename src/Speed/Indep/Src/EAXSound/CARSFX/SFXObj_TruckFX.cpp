#include "./SFXObj_TruckFX.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace Csis {
extern ClassHandle gFX_TRUCK_FXHandle;
extern InterfaceId FX_TRUCK_FXId;
} // namespace Csis

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

SndBase::TypeInfo SFXObj_TruckFX::s_TypeInfo = { 0, "SFXObj_TruckFX", nullptr, SFXObj_TruckFX::CreateObject };

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
    SndBase::InitSFX();
}

void SFXObj_TruckFX::Detach() {
    Destroy();
}
