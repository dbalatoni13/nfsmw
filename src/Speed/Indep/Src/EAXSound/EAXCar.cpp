#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

Attrib::Gen::shiftpattern *EAXCar::g_ShiftInfo = nullptr;
Attrib::Gen::turbosfx *EAXCar::g_TurboInfo = nullptr;

ROOTSTATETYPE_IMPLEMENT(0x20000, EAXCar);

EAXCar::EAXCar()
    : m_nHornState(0),                                                                 //
      m_FEEngineAttribs(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr), //
      m_pPhysicsCTL(nullptr),                                                          //
      t_CurTime(0.0f),                                                                 //
      t_DeltaTime(0.0f),                                                               //
      m_bIsInSoundSphere(false),                                                       //
      m_v3CurSpherePos(),                                                              //
      m_IsDriveCamera(0),                                                              //
      mEngineInfo(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),             //
      mShiftInfo(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),              //
      mTurboInfo(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),              //
      mAccelInfo(static_cast<Attrib::Collection *>(nullptr), 0, nullptr) {
    this->m_nHornState = 0;
    this->t_CurTime = 0.0f;
    this->t_DeltaTime = 0.0f;
    this->m_IsDriveCamera = 0;
    this->m_pPhysicsCTL = nullptr;
    this->m_EngUGL = AEMS_LEVEL2;
    this->m_fAudioRPM = 0.0f;
    this->m_TurboUGL = AEMS_LEVEL0;
    this->m_NOSUGL = AEMS_LEVEL0;
    this->m_TireUGL = AEMS_LEVEL0;
    this->m_TransmissionUGL = AEMS_LEVEL0;
    this->m_nTrueEngineUpgradeLevel = AEMS_LEVEL2;
}

EAXCar::~EAXCar() {
    CSTATE_Base::Destroy();
}

Attrib::Gen::engineaudio &EAXCar::GetEngineAttributes() {
    if (this->GetPhysCar() != nullptr) {
        return this->GetAttributes();
    }
    return this->m_FEEngineAttribs;
}

int UpgradeIntervals(int phys_cur_upgrade, int phys_num_upgrades, int aud_num_upgrades) {
    int base_upgrade = bClamp(4 - phys_num_upgrades, 0, 4);
    int curupgade_offset = bClamp(phys_cur_upgrade, 0, 4);
    int aud_engine_upgrade = 0;
    int NumEngine;

    if (base_upgrade < 3) {
        if (base_upgrade > 0) {
            if (base_upgrade + curupgade_offset >= 3) {
                aud_engine_upgrade = 1;
            } else {
                aud_engine_upgrade = 0;
            }
        } else {
            NumEngine = base_upgrade + curupgade_offset;
            if (NumEngine > 2) {
                aud_engine_upgrade = 2;
            } else if (NumEngine > 0) {
                aud_engine_upgrade = 1;
            }
        }
    }

    return bClamp(aud_engine_upgrade, 0, aud_num_upgrades - 1);
}

void EAXCar::Attach(void *pAttachment) {
    EAX_CarState *pCar = static_cast<EAX_CarState *>(pAttachment);
    this->m_pCar = pCar;

    if (this->GetSFXObject(2) != nullptr) {
        this->m_EngineType = eGINSU_ENG_DUAL;
    } else if (this->GetSFXObject(1) != nullptr) {
        this->m_EngineType = eGINSU_ENG_SINGLE;
    } else {
        this->m_EngineType = eGINSU_ENG_AEMS;
    }

    this->mEngineInfo.ChangeWithDefault(this->m_pCar->GetEngineInfo()->GetCollection());
    if (this->mEngineInfo.IsValid()) {
        this->mAccelInfo.ChangeWithDefault(this->mEngineInfo.acceltrans());
    }

    {
        int cur_upgrade = 4 - this->m_pCar->GetAttributes()->transmission_upgrades() + this->m_pCar->GetAttributes()->transmission_current();
        int aud_shift_upgrade = 0;

        for (int n = 0; n < static_cast<int>(this->m_pCar->GetAttributes()->Num_ShiftSND()); n++) {
            if (this->m_pCar->GetAttributes()->ShiftSND(n).Level > cur_upgrade) {
                break;
            }

            aud_shift_upgrade = n;
        }

        this->mShiftInfo.ChangeWithDefault(this->m_pCar->GetAttributes()->ShiftSND(aud_shift_upgrade).Item);
    }

    if (g_ShiftInfo == nullptr) {
        g_ShiftInfo = &this->mShiftInfo;
    }

    if (g_TurboInfo == nullptr) {
        int cur_upgrade = 4 - this->m_pCar->GetAttributes()->induction_upgrades() + this->m_pCar->GetAttributes()->induction_current();
        int aud_turbo_upgrade = 0;

        for (int n = 0; n < static_cast<int>(this->m_pCar->GetAttributes()->Num_TurboSND()); n++) {
            if (this->m_pCar->GetAttributes()->TurboSND(n).Level > cur_upgrade) {
                break;
            }

            aud_turbo_upgrade = n;
        }

        this->mTurboInfo.ChangeWithDefault(this->m_pCar->GetAttributes()->TurboSND(aud_turbo_upgrade).Item);

        if (g_TurboInfo == nullptr) {
            g_TurboInfo = &this->mTurboInfo;
        }
    } else {
        this->mTurboInfo.ChangeWithDefault(g_TurboInfo->GetCollection());
    }

    CSTATE_Base::Attach(pAttachment);
}

const unsigned int GenerateUpgradedEngine(EAX_CarState *pCar, int playerUpgrade) {
    Attrib::Gen::pvehicle curpvehicle(pCar->mAttributes);
    int phys_num_upgrades = curpvehicle.engine_upgrades();
    int curupgade_offset = curpvehicle.engine_current();

    if (pCar->GetContext() == Sound::CONTEXT_AIRACER) {
        int base_upgrade = bClamp(4 - phys_num_upgrades, 0, 4);

        curupgade_offset = playerUpgrade - base_upgrade;
        if (curupgade_offset < 0) {
            curupgade_offset = 0;
        }
    }

    int aud_engine_upgrade = UpgradeIntervals(curupgade_offset, curpvehicle.engine_upgrades(), curpvehicle.Num_engineaudio());

    Attrib::Gen::engineaudio tempengine(curpvehicle.engineaudio(aud_engine_upgrade), 0, nullptr);
    return tempengine.GetCollection();
}

bool EAXCar::Detach() {
    this->m_pCar = nullptr;
    return CSTATE_Base::Detach();
}

void EAXCar::ProcessUpdate() {
    if (this->GetPhysCar() != nullptr) {
        CSTATE_Base::ProcessUpdate();
    }
}

int PRINT_PERFORMANCE_LVL; // size: 0x4, address: 0xFFFFFFFF, Decl: 337

void EAXCar::UpdateParams(float t) {
    if (this->GetPhysCar() != nullptr) {
        this->UpdateCarPhysics();
        CSTATE_Base::UpdateParams(t);
    }
}

int EAXCar::UpdateRotation() {
    this->m_Rotation = 0x200;
    return 0;
}

float EAXCar::GetVelocityMagnitudeMPH() {
    return this->m_pCar->GetVelocityMagnitudeMPH();
}

bool EAXCar::IsHoodCameraOn() {
    if (this->m_IsDriveCamera != 0) {
        if (this->m_PovType == 1) {
            return true;
        }
    }
    return false;
}

bool EAXCar::IsBumperCameraOn() {
    if (this->m_IsDriveCamera != 0) {
        if (this->m_PovType == 0) {
            return true;
        }
    }
    return false;
}

// STRIPPED
bool EAXCar::IsRearCameraOn() {
    return false;
}

void EAXCar::UpdatePov() {
    this->m_IsDriveCamera = 0;
}

int EAXCar::SFXMessage(eSFXMessageType SFXMessageType, unsigned int, unsigned int) {
    switch (SFXMessageType) {
        case SFX_CHANGEGEAR:
            return 0;
        case SFX_SHIFT_UP:
            break;
        default:
            break;
    }

    return 0;
}
