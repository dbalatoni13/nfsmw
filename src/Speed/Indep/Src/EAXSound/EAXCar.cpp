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

CSTATE_Base *EAXCar::CreateState(unsigned int) {
    return new (gAudioMemoryManager.AllocateMemory(
        sizeof(EAXCar), s_StateInfo.stateName, false)) EAXCar;
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
}

Attrib::Gen::engineaudio &EAXCar::GetEngineAttributes() {
    if (m_pCar) {
        return mEngineInfo;
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

    mEngineInfo.ChangeWithDefault(pCar->GetEngineInfo()->GetCollection());
    if (mEngineInfo.IsValid()) {
        mAccelInfo.ChangeWithDefault(mEngineInfo.acceltrans());
    }

    {
        int cur_upgrade;
        int aud_shift_upgrade;

        cur_upgrade = pCar->GetAttributes()->transmission_current();
        aud_shift_upgrade = cur_upgrade + 4 - pCar->GetAttributes()->transmission_upgrades();

        {
            int n;

            for (n = 0; n < static_cast<int>(pCar->GetAttributes()->Num_ShiftSND()); n++) {
                if (aud_shift_upgrade < pCar->GetAttributes()->ShiftSND(n).Level) {
                    break;
                }
            }

            if (n != 0) {
                n--;
            }

            mShiftInfo.ChangeWithDefault(pCar->GetAttributes()->ShiftSND(n).Item);
        }
    }

    if (!g_ShiftInfo) {
        g_ShiftInfo = &mShiftInfo;
    }

    if (!g_TurboInfo) {
        int cur_upgrade;
        int aud_turbo_upgrade;

        cur_upgrade = pCar->GetAttributes()->induction_current();
        aud_turbo_upgrade = cur_upgrade + 4 - pCar->GetAttributes()->induction_upgrades();

        {
            int n;

            for (n = 0; n < static_cast<int>(pCar->GetAttributes()->Num_TurboSND()); n++) {
                if (aud_turbo_upgrade < pCar->GetAttributes()->TurboSND(n).Level) {
                    break;
                }
            }

            if (n != 0) {
                n--;
            }

            mTurboInfo.ChangeWithDefault(pCar->GetAttributes()->TurboSND(n).Item);
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

    if (pCar->GetContext() == Sound::CONTEXT_ONLINE) {
        int base_upgrade;

        base_upgrade = bClamp(4 - phys_num_upgrades, 0, 4);
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
    return m_IsDriveCamera != 0 && m_PovType == 1;
}

bool EAXCar::IsBumperCameraOn() {
    return m_IsDriveCamera != 0 && m_PovType == 0;
}

void EAXCar::UpdatePov() {
    m_IsDriveCamera = 0;
}

int EAXCar::SFXMessage(eSFXMessageType, unsigned int, unsigned int) {
    return 0;
}

void EAXCar::PreLoadAssets() {
}

void EAXCar::ProcessSoundSphere(unsigned int, int, bVector3 *, float) {
}

void EAXCar::UpdateCarPhysics() {
}

void EAXCar::StartHonkHorn() {
}

void EAXCar::StopHonkHorn() {
}

bool EAXCar::IsHonking() {
    return false;
}

eAemsUpgradeLevel EAXCar::GetEngineUpgradeLevel() {
    return m_EngUGL;
}
