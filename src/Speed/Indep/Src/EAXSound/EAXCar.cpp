#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include <new>
#include <types.h>

Attrib::Gen::shiftpattern *EAXCar::g_ShiftInfo = nullptr;
Attrib::Gen::turbosfx *EAXCar::g_TurboInfo = nullptr;

CSTATE_Base::StateInfo EAXCar::s_StateInfo = {
    0x00020000,
    "EAXCar",
    nullptr,
    &EAXCar::CreateState,
};

CSTATE_Base::StateInfo *EAXCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

const char *EAXCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXCar::CreateState(unsigned int allocator) {
    return new (AudioMemBase::operator new(
        sizeof(EAXCar), GetStaticStateInfo()->stateName)) EAXCar;
}

EAXCar::EAXCar()
    : m_nHornState(0), //
      m_FEEngineAttribs(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr), //
      m_pPhysicsCTL(nullptr), //
      t_CurTime(0.0f), //
      t_DeltaTime(0.0f), //
      m_bIsInSoundSphere(false), //
      m_v3CurSpherePos(), //
      m_IsDriveCamera(0), //
      mEngineInfo(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr), //
      mShiftInfo(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr), //
      mTurboInfo(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr), //
      mAccelInfo(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr) {
    m_fAudioRPM = 0.0f;
    m_TransmissionUGL = AEMS_LEVEL0;
    m_nTrueEngineUpgradeLevel = AEMS_LEVEL2;
    m_nHornState = 0;
    t_CurTime = 0.0f;
    t_DeltaTime = 0.0f;
    m_IsDriveCamera = 0;
    m_pPhysicsCTL = nullptr;
    m_EngUGL = AEMS_LEVEL2;
    m_TurboUGL = AEMS_LEVEL0;
    m_NOSUGL = AEMS_LEVEL0;
    m_TireUGL = AEMS_LEVEL0;
}

EAXCar::~EAXCar() {
    CSTATE_Base::Destroy();
}

Attrib::Gen::engineaudio &EAXCar::GetEngineAttributes() {
    if (GetPhysCar()) {
        return GetAttributes();
    }
    return m_FEEngineAttribs;
}

int UpgradeIntervals(int phys_cur_upgrade, int phys_num_upgrades, int aud_num_upgrades) {
    int base_upgrade;
    int curupgade_offset;
    int aud_engine_upgrade;
    int NumEngine;

    base_upgrade = bClamp(4 - phys_num_upgrades, 0, 4);
    curupgade_offset = bClamp(phys_cur_upgrade, 0, 4);
    aud_engine_upgrade = 0;

    if (base_upgrade < 3) {
        if (base_upgrade < 1) {
            NumEngine = base_upgrade + curupgade_offset;
            if (NumEngine < 3) {
                if (NumEngine > 0) {
                    aud_engine_upgrade = 1;
                }
            } else {
                aud_engine_upgrade = 2;
            }
        } else {
            aud_engine_upgrade = base_upgrade + curupgade_offset > 2;
        }
    }

    return bClamp(aud_engine_upgrade, 0, aud_num_upgrades - 1);
}

void EAXCar::Attach(void *pAttachment) {
    EAX_CarState *pCar;

    pCar = static_cast<EAX_CarState *>(pAttachment);
    m_pCar = pCar;

    if (GetSFXObject(2)) {
        m_EngineType = eGINSU_ENG_DUAL;
    } else if (GetSFXObject(1)) {
        m_EngineType = eGINSU_ENG_SINGLE;
    } else {
        m_EngineType = eGINSU_ENG_AEMS;
    }

    mEngineInfo.ChangeWithDefault(m_pCar->GetEngineInfo()->GetCollection());
    if (mEngineInfo.IsValid()) {
        mAccelInfo.ChangeWithDefault(mEngineInfo.acceltrans());
    }

    {
        int cur_upgrade;
        int aud_shift_upgrade;

        cur_upgrade = m_pCar->GetAttributes()->transmission_current() + 4 -
                      m_pCar->GetAttributes()->transmission_upgrades();
        aud_shift_upgrade = 0;

        {
            int n;

            for (n = 0; n < static_cast<int>(m_pCar->GetAttributes()->Num_ShiftSND()); n++) {
                if (m_pCar->GetAttributes()->ShiftSND(n).Level > cur_upgrade) {
                    break;
                }
                aud_shift_upgrade = n;
            }

            mShiftInfo.ChangeWithDefault(m_pCar->GetAttributes()->ShiftSND(aud_shift_upgrade).Item);
        }
    }

    if (!g_ShiftInfo) {
        g_ShiftInfo = &mShiftInfo;
    }

    if (!g_TurboInfo) {
        int cur_upgrade;
        int aud_turbo_upgrade;

        cur_upgrade = m_pCar->GetAttributes()->induction_current() + 4 -
                      m_pCar->GetAttributes()->induction_upgrades();
        aud_turbo_upgrade = 0;

        {
            int n;

            for (n = 0; n < static_cast<int>(m_pCar->GetAttributes()->Num_TurboSND()); n++) {
                if (m_pCar->GetAttributes()->TurboSND(n).Level > cur_upgrade) {
                    break;
                }
                aud_turbo_upgrade = n;
            }

            mTurboInfo.ChangeWithDefault(m_pCar->GetAttributes()->TurboSND(aud_turbo_upgrade).Item);
        }

        if (!g_TurboInfo) {
            g_TurboInfo = &mTurboInfo;
        }
    } else {
        mTurboInfo.ChangeWithDefault(g_TurboInfo->GetCollection());
    }

    CSTATE_Base::Attach(pAttachment);
}

unsigned int GenerateUpgradedEngine(EAX_CarState *pCar, int playerUpgrade) {
    Attrib::Gen::pvehicle curpvehicle(*pCar->GetAttributes());
    int phys_num_upgrades;
    int curupgade_offset;
    int aud_engine_upgrade;

    phys_num_upgrades = curpvehicle.engine_upgrades();
    curupgade_offset = curpvehicle.engine_current();

    if (pCar->GetContext() == Sound::CONTEXT_AIRACER) {
        int base_upgrade;

        base_upgrade = 4 - phys_num_upgrades;
        if (base_upgrade < 0) {
            base_upgrade = 0;
        }
        if (base_upgrade > 4) {
            base_upgrade = 4;
        }
        curupgade_offset = playerUpgrade - base_upgrade;
        if (curupgade_offset < 0) {
            curupgade_offset = 0;
        }
    }

    aud_engine_upgrade = UpgradeIntervals(curupgade_offset, phys_num_upgrades,
                                          curpvehicle.Num_engineaudio());
    Attrib::Gen::engineaudio tempengine(curpvehicle.engineaudio(aud_engine_upgrade), 0, nullptr);
    return tempengine.GetCollection();
}

bool EAXCar::Detach() {
    m_pCar = nullptr;
    return CSTATE_Base::Detach();
}

void EAXCar::ProcessUpdate() {
    if (m_pCar) {
        CSTATE_Base::ProcessUpdate();
    }
}

void EAXCar::UpdateParams(float t) {
    if (m_pCar) {
        UpdateCarPhysics();
        CSTATE_Base::UpdateParams(t);
    }
}

int EAXCar::UpdateRotation() {
    m_Rotation = 0x200;
    return 0;
}

float EAXCar::GetVelocityMagnitudeMPH() {
    return m_pCar->GetVelocityMagnitudeMPH();
}

bool EAXCar::IsHoodCameraOn() {
    if (m_IsDriveCamera != 0) {
        if (m_PovType == 1) {
            return true;
        }
    }
    return false;
}

bool EAXCar::IsBumperCameraOn() {
    if (m_IsDriveCamera != 0) {
        if (m_PovType == 0) {
            return true;
        }
    }
    return false;
}

void EAXCar::UpdatePov() {
    m_IsDriveCamera = 0;
}

int EAXCar::SFXMessage(eSFXMessageType SFXMessageType, unsigned int, unsigned int) {
    switch (SFXMessageType) {
    case SFX_CHANGEGEAR:
        return 0;
    default:
        break;
    }
    return 0;
}

void EAXCar::PreLoadAssets() {
}
