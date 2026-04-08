#include "./CARSFX_Shifting.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace Csis {
InterfaceId CAR_SWTNId = {"CAR_SWTN", 0x383F, 0x7859};
ClassHandle gCAR_SWTNHandle;
InterfaceId CAR_WHINEId = {"CAR_WHINE", 0x383F, 0x603F};
ClassHandle gCAR_WHINEHandle;
InterfaceId FX_SHIFTING_01Id = {"FX_SHIFTING_01", 0x3EE6, 0x7937};
ClassHandle gFX_SHIFTING_01Handle;
} // namespace Csis

extern int IsSoundEnabled;
extern int g_nArrayCosTable[0x201];
Slope ShiftingAttackVolSlope(1.0f, 32767.0f, 4000.0f, 7000.0f);

CARSFX_Shift::TypeInfo CARSFX_Shift::s_TypeInfo = {
    0x00020030,
    "CARSFX_Shift",
    &SndBase::s_TypeInfo,
    CARSFX_Shift::CreateObject,
};

CARSFX_Shift::TypeInfo *CARSFX_Shift::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_Shift::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_Shift::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_Shift();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_Shift();
}

CARSFX_Shift::CARSFX_Shift()
    : CARSFX() {
    m_ShiftGear = nullptr;
    m_BrakePedal = nullptr;
    m_Disengage = nullptr;
    m_Engage = nullptr;
    m_AccelSnd = nullptr;
    m_DeccelSnd = nullptr;
    m_Whine = nullptr;
    m_pShiftCtl = nullptr;
    m_pShiftingPatternData = nullptr;
    m_pSweetnersData = nullptr;
}

CARSFX_Shift::~CARSFX_Shift() {
    Destroy();
}

void CARSFX_Shift::UpdateMixerOutputs() {
    if (m_pShiftCtl->eShiftState == SHFT_UP_DISENGAGE) {
        SetDMIX_Input(7, 0x7FFF);
    } else {
        SetDMIX_Input(7, 0);
    }
}

void CARSFX_Shift::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pSweetnersData = &m_pEAXCar->mEngineInfo;
    m_UGL = m_pEAXCar->m_TransmissionUGL;
    m_pShiftingPatternData = &m_pEAXCar->mShiftInfo;
}

void CARSFX_Shift::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_Shift::Destroy() {
    if (m_DeccelSnd) {
        delete m_DeccelSnd;
        m_DeccelSnd = nullptr;
    }

    if (m_AccelSnd) {
        delete m_AccelSnd;
        m_AccelSnd = nullptr;
    }

    if (m_ShiftGear) {
        delete m_ShiftGear;
        m_ShiftGear = nullptr;
    }

    if (m_BrakePedal) {
        delete m_BrakePedal;
        m_BrakePedal = nullptr;
    }

    if (m_Disengage) {
        delete m_Disengage;
        m_Disengage = nullptr;
    }

    if (m_Engage) {
        delete m_Engage;
        m_Engage = nullptr;
    }

    if (m_Whine) {
        delete m_Whine;
        m_Whine = nullptr;
    }
}

int CARSFX_Shift::GetController(int Index) {
    switch (Index) {
    case 0:
        return 2;
    case 1:
        return 3;
    case 2:
        return 7;
    default:
        return -1;
    }
}

void CARSFX_Shift::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
    case 2:
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    case 3:
        m_pAccelTransCtl = static_cast<SFXCTL_AccelTrans *>(psfxctl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    }
}

void CARSFX_Shift::UpdateParams(float t) {
    SndBase::UpdateParams(t);

    if (m_DeccelSnd && m_DeccelSnd->GetRefCount() < 2) {
        delete m_DeccelSnd;
        m_DeccelSnd = nullptr;
    }

    if (m_AccelSnd && m_AccelSnd->GetRefCount() < 2) {
        delete m_AccelSnd;
        m_AccelSnd = nullptr;
    }

    if (m_Disengage && m_Disengage->GetRefCount() < 2) {
        delete m_Disengage;
        m_Disengage = nullptr;
    }

    if (m_Engage && m_Engage->GetRefCount() < 2) {
        delete m_Engage;
        m_Engage = nullptr;
    }

    if (*reinterpret_cast<int *>(&m_pShiftCtl->m_bNeed_ShiftGearSnd) != 0) {
        PlayShiftSnd();
    }
    if (*reinterpret_cast<int *>(&m_pShiftCtl->m_bNeed_DisengageSnd) != 0) {
        PlayDisengageSnd();
    }
    if (*reinterpret_cast<int *>(&m_pShiftCtl->m_bNeed_EngageSnd) != 0) {
        PlayEngageSnd();
    }
    if (*reinterpret_cast<int *>(&m_pShiftCtl->m_bNeed_AccelSnd) != 0) {
        PlayAccelSnd();
    }
    if (*reinterpret_cast<int *>(&m_pShiftCtl->m_bNeed_DeccelSnd) != 0) {
        PlayDecelSnd();
    }

    if (*reinterpret_cast<int *>(&m_pShiftCtl->m_bShouldBeWhining) == 0) {
        if (m_Whine) {
            delete m_Whine;
            m_Whine = nullptr;
        }
    } else {
        PlayGearWhine();
    }

    if (*reinterpret_cast<int *>(&m_pShiftCtl->m_bBrakePedalMashed) == 0) {
        if (m_BrakePedal) {
            delete m_BrakePedal;
            m_BrakePedal = nullptr;
        }
    } else {
        PlayBrakesMashed();
    }
}

void CARSFX_Shift::SetupLoadData() {
    unsigned int i;

    if (EAXCar::g_ShiftInfo) {
        LoadAsset(EAXCar::g_ShiftInfo->BankName(), SNDPATH_SHIFT, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    } else {
        LoadAsset(m_pEAXCar->GetShiftInfo().BankName(), SNDPATH_SHIFT, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    }

    for (i = 0; i < m_pEAXCar->GetAttributes().Num_SweetBank(); i++) {
        LoadAsset(m_pEAXCar->GetAttributes().SweetBank(i), SNDPATH_ENGINE, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    }
}

void CARSFX_Shift::PlayGearWhine() {
    int refcount;
    float whine_vol;

    if (IsSoundEnabled == 0) {
        return;
    }

    if (!m_Whine) {
        g_pEAXSound->SetCsisName("Csis:Whine");
        m_Whine = new CAR_WHINE(0, 0, 0, 0, 0, 0, 25000, 0, 0x7FFF);
        if (!m_Whine) {
            return;
        }
        refcount = m_Whine->GetRefCount();
        if (!m_Whine) {
            return;
        }
    }

    whine_vol = 0.0f;
    if (GetPhysCar()->GetMaxRPM() > 0.0f) {
        whine_vol = static_cast<float>(GetDMixOutput(8, DMX_VOL)) * (GetPhysRPM() / GetPhysCar()->GetMaxRPM());
    }

    m_Whine->SetVOL(static_cast<int>(whine_vol));
    m_Whine->SetCOMMON_PARAMETERS_AZIMUTH(GetDMixOutput(0, DMX_AZIM));
    m_Whine->SetRPM(static_cast<int>(GetPhysRPM()));
    m_Whine->CommitMemberData();
}

void CARSFX_Shift::PlayDisengageSnd() {
    float RPM;
    int SampleID;
    int nVol;
    int nDMixOut;

    if (IsSoundEnabled != 0 && (RPM = GetPhysRPM(), RPM >= 7000.0f)) {
        SampleID = 0;
        nVol = m_pSweetnersData->Vol_ShiftSweets();
        RPM = ShiftingAttackVolSlope.GetValue(RPM);
        nDMixOut = GetDMixOutput(4, DMX_VOL);
        nVol = (nVol * static_cast<int>(RPM) >> 15) * nDMixOut >> 15;
        g_pEAXSound->SetCsisName("SND ShiftDisEngage");
        m_Disengage = new CAR_SWTN(SampleID, m_pEAXCar->GetAttributes().CarID(), static_cast<int>(GetPhysRPM()), nVol,
                                   GetDMixOutput(0, DMX_AZIM), 0, 0);
    }
}

void CARSFX_Shift::PlayEngageSnd() {
    float RPM;
    int SampleID;
    int nVol;
    int nDMixOut;

    if (IsSoundEnabled != 0 && (RPM = m_pShiftCtl->RPM_AtShift, RPM >= 7000.0f)) {
        SampleID = 1;
        nVol = m_pSweetnersData->Vol_ShiftSweets();
        RPM = ShiftingAttackVolSlope.GetValue(RPM);
        nDMixOut = GetDMixOutput(3, DMX_VOL);
        nVol = (nVol * static_cast<int>(RPM) >> 15) * nDMixOut >> 15;
        g_pEAXSound->SetCsisName("SND ShiftDisengage");
        m_Engage = new CAR_SWTN(SampleID, m_pEAXCar->GetAttributes().CarID(), static_cast<int>(GetPhysRPM()), nVol,
                                GetDMixOutput(0, DMX_AZIM), 0, 0);
    }
}

void CARSFX_Shift::PlayAccelSnd() {
    float RPM;
    int SampleID;
    int nVol;
    int nDMixOut;

    if (IsSoundEnabled != 0) {
        RPM = m_pShiftCtl->RPM_AtShift;
        SampleID = 1;
        nVol = m_pSweetnersData->Vol_ShiftSweets();
        RPM = ShiftingAttackVolSlope.GetValue(RPM);
        nDMixOut = GetDMixOutput(5, DMX_VOL);
        nVol = (nVol * static_cast<int>(RPM) >> 15) * nDMixOut >> 15;
        m_AccelSnd = new CAR_SWTN(SampleID, m_pEAXCar->GetAttributes().CarID(), static_cast<int>(GetPhysRPM()), nVol,
                                  GetDMixOutput(0, DMX_AZIM), 0, 0);
    }
}

void CARSFX_Shift::PlayDecelSnd() {
    float RPM;
    int SampleID;
    int nVol;
    int nDMixOut;

    if (IsSoundEnabled != 0) {
        RPM = m_pShiftCtl->RPM_AtShift;
        SampleID = 0;
        nVol = m_pSweetnersData->Vol_ShiftSweets();
        RPM = ShiftingAttackVolSlope.GetValue(RPM);
        nDMixOut = GetDMixOutput(6, DMX_VOL);
        nVol = (nVol * static_cast<int>(RPM) >> 15) * nDMixOut >> 15;
        m_DeccelSnd = new CAR_SWTN(SampleID, m_pEAXCar->GetAttributes().CarID(), static_cast<int>(GetPhysRPM()), nVol,
                                   GetDMixOutput(0, DMX_AZIM), 0, 0);
    }
}

void CARSFX_Shift::PlayShiftSnd() {
    int tempVol;
    float RPM;
    float RPMDifScale;
    float ShiftVolScale;
    int nDMixOut;
    AEMS_SHIFTING_SAMPLES SampleID;
    int CameraView;

    if (m_pShiftCtl->eShiftState - SHFT_UP_DISENGAGE < 3) {
        ShiftVolScale = static_cast<float>(m_pShiftingPatternData->Up_Vol_Shift());
    } else {
        ShiftVolScale = static_cast<float>(m_pShiftingPatternData->Down_Vol_Shift());
    }

    RPM = m_pEAXCar->PhysRPM;
    RPMDifScale = bClamp((static_cast<float>(static_cast<int>(RPM)) - 1500.0f) * 0.00015384615f, 0.0f, 1.0f);
    tempVol = g_nArrayCosTable[static_cast<int>(512.0f - (RPMDifScale * 0.9f + 0.1f) * 512.0f)];
    tempVol = static_cast<int>(ShiftVolScale) * tempVol >> 15;

    if (m_pShiftCtl->eShiftState - SHFT_UP_DISENGAGE < 3) {
        nDMixOut = GetDMixOutput(1, DMX_VOL);
    } else {
        nDMixOut = GetDMixOutput(2, DMX_VOL);
    }
    tempVol = tempVol * nDMixOut >> 15;

    if (m_ShiftGear) {
        delete m_ShiftGear;
        m_ShiftGear = nullptr;
    }

    CameraView = m_pEAXCar->m_PovType == 0;
    SampleID =
        static_cast<AEMS_SHIFTING_SAMPLES>((m_pShiftCtl->eShiftState - SHFT_UP_DISENGAGE < 3) ? 1 : 0);
    g_pEAXSound->SetCsisName("SND: Shift");
    m_ShiftGear = new FX_SHIFTING_01(SampleID, tempVol, 0x1000, GetDMixOutput(0, DMX_AZIM), FXSHIFTING01TYPETYPE_SHIFT,
                                     CameraView);
}

void CARSFX_Shift::PlayBrakesMashed() {
    int nDMixOut;
    int CameraView;

    if (IsSoundEnabled != 0) {
        if (!m_BrakePedal) {
            int refcount;

            nDMixOut = GetDMixOutput(10, DMX_VOL);
            CameraView = m_pEAXCar->GetPOV() == 0;
            g_pEAXSound->SetCsisName("SND: BrakeMash");
            m_BrakePedal = new FX_SHIFTING_01(2, nDMixOut, 0x1000, GetDMixOutput(0, DMX_AZIM), FXSHIFTING01TYPETYPE_SHIFT,
                                              CameraView);
            if (m_BrakePedal) {
                refcount = m_BrakePedal->GetRefCount();
            }
        }
    }
}

void CARSFX_Shift::ProcessUpdate() {
    if (g_EAXIsPaused()) {
        if (m_DeccelSnd) {
            m_DeccelSnd->SetVOL(0);
            m_DeccelSnd->CommitMemberData();
        }
        if (m_AccelSnd) {
            m_AccelSnd->SetVOL(0);
            m_AccelSnd->CommitMemberData();
        }
        if (m_ShiftGear) {
            m_ShiftGear->SetVolume(0);
            m_ShiftGear->CommitMemberData();
        }
        if (m_BrakePedal) {
            m_BrakePedal->SetVolume(0);
            m_BrakePedal->CommitMemberData();
        }
        if (m_Disengage) {
            m_Disengage->SetVOL(0);
            m_Disengage->CommitMemberData();
        }
        if (m_Engage) {
            m_Engage->SetVOL(0);
            m_Engage->CommitMemberData();
        }
        if (m_Whine) {
            m_Whine->SetVOL(0);
            m_Whine->CommitMemberData();
        }
    }
}
