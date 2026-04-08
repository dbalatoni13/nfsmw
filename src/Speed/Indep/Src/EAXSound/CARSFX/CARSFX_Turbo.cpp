#include "./CARSFX_Turbo.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

namespace Csis {
InterfaceId FX_TURBO_01Id = {"FX_TURBO_01", 0x5EF2, 0x5EF2};
ClassHandle gFX_TURBO_01Handle;
} // namespace Csis

int gnMemLeakTurboBLOWOFFCountTest = 0;
int gnMemLeakTurboSPOOLCountTest = 0;
float MIN_TORQUE_FOR_BLOWOFF = 20.0f;
extern int IsSoundEnabled;
extern int g_nArrayCosTable[0x201];

CARSFX_Turbo::TypeInfo CARSFX_Turbo::s_TypeInfo = {
    0x00020040,
    "CARSFX_Turbo",
    &SndBase::s_TypeInfo,
    CARSFX_Turbo::CreateObject,
};

CARSFX_Turbo::TypeInfo *CARSFX_Turbo::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_Turbo::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_Turbo::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_Turbo();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_Turbo();
}

CARSFX_Turbo::CARSFX_Turbo()
    : CARSFX()
    , m_BlowoffRampDown() //
    , m_SpoolDuck() //
    , m_fDeltaTurbo(3) {
    tLastBlowoffTime = 0.0f;
    m_pTurboBlowoffControl = nullptr;
    m_pTurboSplControl = nullptr;
    *reinterpret_cast<int *>(&bStopBlowoff) = 0;
    *reinterpret_cast<int *>(&bReachedPeak) = 0;
    m_pShiftingCtl = nullptr;
    m_pEngineCtl = nullptr;
    eTurboState = SFXTURBO_NONE;
    m_pTurboData = nullptr;
}

CARSFX_Turbo::~CARSFX_Turbo() {
    Destroy();
}

int CARSFX_Turbo::GetController(int Index) {
    switch (Index) {
    case 0:
        return 2;
    case 1:
        return 7;
    case 2:
        return 4;
    default:
        return -1;
    }
}

void CARSFX_Turbo::AttachController(SFXCTL *psfxctl) {
    int nindex;

    nindex = psfxctl->GetObjectIndex();
    switch (nindex) {
    case 2:
        m_pShiftingCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    case 4:
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    }
}

void CARSFX_Turbo::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    eTurboState = SFXTURBO_NONE;
    m_pTurboData = &m_pEAXCar->mTurboInfo;
}

void CARSFX_Turbo::InitSFX() {
    SndBase::InitSFX();
    m_fTurbo = 0.0f;
    m_MaxTurbo = 1.0f;
    if (m_pTurboData->GetCollection() == 0xEEC2271A) {
        Disable();
    } else {
        Enable();
        m_fDeltaTurbo.Flush(0.0f);
        eTurboState = SFXTURBO_NONE;
        if (!m_pTurboSplControl) {
            PlaySpl(0, 0, 0, 0, 0);
        }
        m_fTurbo = 0.0f;
        *reinterpret_cast<int *>(&bReachedPeak) = 0;
        *reinterpret_cast<int *>(&bStopBlowoff) = 0;
        tLastBlowoffTime = 0.0f;
    }
}

void CARSFX_Turbo::Detach() {
    Destroy();
    Disable();
}

void CARSFX_Turbo::Destroy() {
    if (m_pTurboBlowoffControl) {
        delete m_pTurboBlowoffControl;
        m_pTurboBlowoffControl = nullptr;
        gnMemLeakTurboBLOWOFFCountTest--;
    }

    if (m_pTurboSplControl) {
        delete m_pTurboSplControl;
        m_pTurboSplControl = nullptr;
        gnMemLeakTurboSPOOLCountTest--;
    }
}

void CARSFX_Turbo::UpdateParams(float t) {
    if (IsEnabled()) {
        if (m_pEngineCtl->GetEngTorque() > MIN_TORQUE_FOR_BLOWOFF) {
            m_fTurbo = m_pEngineCtl->GetEngTorque() * 0.01f;
        } else {
            m_fTurbo = 0.0f;
        }

        UpdateBlowOff(t);
        UpdateSpool(t);
        if (eTurboState == SFXTURBO_SPOOLING) {
            if (m_pEngineCtl->GetEngTorque() < MIN_TORQUE_FOR_BLOWOFF) {
                eTurboState = SFXTURBO_BLOWOFF;
                PlayBlowoff(0, 0, 0, 0, m_pEAXCar->m_Rotation);
                ResetSpool();
                UpdateSpool(t);
            }
        } else if (eTurboState < SFXTURBO_BLOWOFF) {
            if (eTurboState == SFXTURBO_NONE && m_pEngineCtl->GetEngTorque() > MIN_TORQUE_FOR_BLOWOFF) {
                eTurboState = SFXTURBO_SPOOLING;
            }
        } else if (eTurboState == SFXTURBO_BLOWOFF && IsBlowOffDone()) {
            eTurboState = SFXTURBO_NONE;
        }
    }
}

bool CARSFX_Turbo::IsBlowOffDone() {
    if (!m_pTurboBlowoffControl) {
        return true;
    }

    if (m_pTurboBlowoffControl->GetRefCount() == 1 || m_BlowoffRampDown.GetValue() == 0.0f) {
        return true;
    }

    return false;
}

void CARSFX_Turbo::UpdateBlowOff(float t) {
    if (m_pTurboBlowoffControl) {
        m_BlowoffRampDown.Update(t);
        if (m_pEAXCar->fTrottle > MIN_TORQUE_FOR_BLOWOFF && m_BlowoffRampDown.GetValue() == 1.0f && !m_pShiftingCtl->IsActive()) {
            m_BlowoffRampDown.Initialize(1.0f, 0.0f, 0x96, LINEAR);
        }

        if (IsBlowOffDone()) {
            StopBlowOff();
        }
    }
}

void CARSFX_Turbo::StopBlowOff() {
    if (m_pTurboBlowoffControl) {
        delete m_pTurboBlowoffControl;
        m_pTurboBlowoffControl = nullptr;
        gnMemLeakTurboBLOWOFFCountTest--;
    }
}

int CARSFX_Turbo::PlayBlowoff(int, int, int, int, int rotation) {
    if (IsEnabled() && IsSoundEnabled == 1 && !m_pTurboBlowoffControl) {
        BlowoffID = 1;
        if (SpoolPercent > 0.75f) {
            BlowoffID = g_pEAXSound->Random(2) + 2;
        }

        if (BlowoffID == 1) {
            BlowoffVol = static_cast<int>(m_pTurboData->Vol_Blowoff1() * SpoolPercent);
        } else {
            BlowoffVol = static_cast<int>(m_pTurboData->Vol_Blowoff2() * SpoolPercent);
        }

        g_pEAXSound->SetCsisName("SND: Turbo");
        m_pTurboBlowoffControl =
            new FX_TURBO_01(BlowoffID, 0, static_cast<int>(SpoolPercent * 1024.0f), 0, rotation, static_cast<int>(GetPhysRPM()));
        gnMemLeakTurboBLOWOFFCountTest++;
        m_refCount = static_cast<unsigned short>(m_pTurboBlowoffControl ? m_pTurboBlowoffControl->GetRefCount() : 0);
        tLastBlowoffTime = m_pStateBase->GetCurTime();
        m_BlowoffRampDown.Initialize(1.0f, 1.0f, 1, LINEAR);
    }

    return 0;
}

int CARSFX_Turbo::PlaySpl(int _ID, int Vol, int PSI, int, int rotation) {
    int nDMixOut;

    if (IsSoundEnabled == 1) {
        nDMixOut = GetDMixOutput(1, DMX_VOL);
        nDMixOut = Vol * nDMixOut >> 15;
        g_pEAXSound->SetCsisName("SND:Turbo Spool");
        m_pTurboSplControl =
            new FX_TURBO_01(_ID, nDMixOut, PSI, GetDMixOutput(0, DMX_AZIM), rotation, static_cast<int>(GetPhysRPM()));
        gnMemLeakTurboSPOOLCountTest++;
    }
    return 0;
}

int CARSFX_Turbo::UpdateSpool(float t) {
    int RPM;
    float SpoolChargeRPMScale;
    float SpoolChargeScale;

    if (!m_pTurboSplControl) {
        PlaySpl(0, 0, 0, 0, 0);
        return 0;
    }

    if (!g_EAXIsPaused()) {
        if (m_fTurbo > 0.01f) {
            SpoolCharge += m_fTurbo;
        } else {
            SpoolCharge -= m_pTurboData->Leak_Rate();
        }
    }

    SpoolCharge = bClamp(SpoolCharge, 0.0f, m_pTurboData->ChargeTime());
    RPM = static_cast<int>(GetPhysRPM()) - 1000;
    SpoolChargeRPMScale = bClamp(static_cast<float>(RPM) * 0.01f, 0.0f, 1.0f) * 0.6f + 0.4f;
    SpoolChargeScale = m_pTurboData->ChargeTime() * SpoolChargeRPMScale;
    SpoolCharge = bClamp(SpoolCharge, 0.0f, SpoolChargeScale);
    if (SpoolCharge == m_pTurboData->ChargeTime() * SpoolChargeRPMScale && !bReachedPeak) {
        bReachedPeak = true;
        m_SpoolDuck.ClearStages();
        m_SpoolDuck.AddStage(0.0f, 0.0f, 1000, LINEAR);
        m_SpoolDuck.AddStage(0.0f, 307.2f, 2000, LINEAR);
    } else if (SpoolCharge != m_pTurboData->ChargeTime() && bReachedPeak) {
        bReachedPeak = false;
    }

    SpoolPercent = SpoolCharge / m_pTurboData->ChargeTime();
    vol_Spool = m_pTurboData->Vol_Spool();
    if (bReachedPeak) {
        m_SpoolDuck.Update(t);
        vol_Spool = vol_Spool * g_nArrayCosTable[m_SpoolDuck.iGetValue()] >> 15;
    }
    return 0;
}

void CARSFX_Turbo::ProcessUpdate() {
    if (m_pTurboBlowoffControl) {
        int nDMixOut;
        int Az;
        int TmpBlowoffVol;

        if (BlowoffID == 1) {
            TmpBlowoffVol = GetDMixOutput(2, DMX_VOL);
            Az = GetDMixOutput(0, DMX_AZIM);
        } else {
            TmpBlowoffVol = GetDMixOutput(3, DMX_VOL);
            Az = GetDMixOutput(0, DMX_AZIM);
        }

        nDMixOut = static_cast<int>(static_cast<float>((BlowoffVol * TmpBlowoffVol) >> 15) * m_BlowoffRampDown.GetValue());
        m_pTurboBlowoffControl->SetAzimuth(Az);
        m_pTurboBlowoffControl->SetVolume(nDMixOut);
        m_pTurboBlowoffControl->CommitMemberData();
    }

    if (m_pTurboSplControl) {
        int Az;
        int nDMixOut;
        int Tmpvol_Spool;

        Az = GetDMixOutput(0, DMX_AZIM);
        nDMixOut = GetDMixOutput(1, DMX_VOL);
        Tmpvol_Spool = vol_Spool * nDMixOut >> 15;
        m_pTurboSplControl->SetVolume(Tmpvol_Spool);
        m_pTurboSplControl->SetPSI(static_cast<int>(SpoolPercent * 1024.0f));
        m_pTurboSplControl->SetAzimuth(Az);
        m_pTurboSplControl->SetRotation(static_cast<int>(GetPhysTRQ() * 10.24f));
        m_pTurboSplControl->SetRPM(static_cast<int>(GetPhysRPM()));
        m_pTurboSplControl->CommitMemberData();
    }
}

void CARSFX_Turbo::SetupLoadData() {
    if (EAXCar::g_TurboInfo) {
        LoadAsset(EAXCar::g_TurboInfo->BankName(), SNDPATH_TURBO, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    } else {
        LoadAsset(m_pEAXCar->mTurboInfo.BankName(), SNDPATH_TURBO, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    }
}

void CARSFX_Turbo::ResetSpool() {
    SpoolCharge = 0.0f;
}
