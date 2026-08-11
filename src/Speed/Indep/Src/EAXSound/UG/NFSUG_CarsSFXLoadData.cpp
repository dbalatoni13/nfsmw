#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_Common.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Nitrous.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Skids.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_SparkChatter.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Turbo.hpp"

void CARSFX_Nitrous::SetupLoadData() {
    // TODO switch?
    eNFSSndNOSClass nbankindex = AEMS_NOS_00;
    if (this->m_UGL != AEMS_LEVEL1) {
        if (this->m_UGL > AEMS_LEVEL1) {
            if (this->m_UGL == AEMS_LEVEL2) {
                nbankindex = AEMS_NOS_01;
            } else if (this->m_UGL == AEMS_LEVEL3) {
                nbankindex = AEMS_NOS_01;
            }
        }
    }

    this->LoadAsset(g_pEAXSound->GetAttributes().AEMS_NOSBanks(nbankindex), SNDPATH_NOS, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
}

void CARSFX_Shift::SetupLoadData() {
    if (EAXCar::g_ShiftInfo != nullptr) {
        this->LoadAsset(EAXCar::g_ShiftInfo->BankName(), SNDPATH_SHIFT, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    } else {
        this->LoadAsset(this->m_pEAXCar->GetShiftInfo().BankName(), SNDPATH_SHIFT, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    }

    if (this->m_pEAXCar->GetEngineAttributes().Num_SweetBank() > 0) {
        for (unsigned int i = 0; i < this->m_pEAXCar->GetEngineAttributes().Num_SweetBank(); i++) {
            this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().SweetBank(i), SNDPATH_ENGINE, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
        }
    }
}

void CARSFX_SparkChatter::SetupLoadData() {
    LoadAsset(this->m_pEAXCar->GetEngineAttributes().SweetBank(0), SNDPATH_ENGINE, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
}

void CARSFX_Turbo::SetupLoadData() {
    if (EAXCar::g_TurboInfo != nullptr) {
        LoadAsset(EAXCar::g_TurboInfo->BankName(), SNDPATH_TURBO, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    } else {
        LoadAsset(this->m_pEAXCar->GetTurboInfo().BankName(), SNDPATH_TURBO, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    }
}

void CARSFX_Skids::SetupLoadData() {
    int nlvl = bClamp(m_UGL, 0, 1);

    this->LoadAsset(g_pEAXSound->GetAttributes().AEMS_SkidBanks(nlvl), SNDPATH_SKIDS, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
}

void CARSFX_AEMSEngine::SetupLoadData() {
    eBANK_SLOT_TYPE type;

    if (m_pEAXCar->GetEngineAttributes().BankName_auxRAM(0).GetString() != "") {
        this->SPU_or_EE = 0;
        type = eBANK_SLOT_NONE;
        if (GetPhysCar()->IsAICar() || GetPhysCar()->IsCopCar()) {
            type = eBANK_SLOT_AI_AEMS_ENGINE;
#ifdef EA_BUILD_A124
            this->SPU_or_EE = 0;
#endif
        }
#ifndef EA_BUILD_A124
        this->SPU_or_EE = 1;
#endif
#ifdef EA_BUILD_A124
        this->LoadAsset(m_pEAXCar->GetEngineAttributes().BankName_auxRAM(0), SNDPATH_ENGINE, SDT_AEMS_ASYNCSPUMEM, type, true);
#else
        this->LoadAsset(m_pEAXCar->GetEngineAttributes().BankName_mainRAM(), SNDPATH_ENGINE, SDT_AEMS_ASYNCSPUMEM, type, true);
#endif
    }
}

void CARSFX_SingleGinsuEng::SetupLoadData() {
#ifdef EA_BUILD_A124
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().BankName_auxRAM(0), SNDPATH_ENGINE, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    this->SPU_or_EE = 0;
#else
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().BankName_mainRAM(), SNDPATH_ENGINE, SDT_AEMS_AUDIOMEM, eBANK_SLOT_NONE, true);
    this->SPU_or_EE = 1;
#endif
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().Filename_GinsuAccel(), SNDPATH_ENGINE, SDT_GENERIC_DATA, eBANK_SLOT_NONE, true);
}

void CARSFX_DualGinsuEng::SetupLoadData() {
    this->SPU_or_EE = 1;
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().BankName_mainRAM(), SNDPATH_ENGINE, SDT_AEMS_AUDIOMEM, eBANK_SLOT_NONE, true);
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().Filename_GinsuAccel(), SNDPATH_ENGINE, SDT_GENERIC_DATA, eBANK_SLOT_NONE, true);
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().Filename_GinsuDecel(), SNDPATH_ENGINE, SDT_GENERIC_DATA, eBANK_SLOT_NONE, true);
}

void SFX_Common::SetupLoadData() {
    this->LoadAsset(g_pEAXSound->GetAttributes().AEMS_FEBanks(1), SNDPATH_GLOBAL, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);

    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        this->LoadAsset(g_pEAXSound->GetAttributes().AEMS_FEBanks(0), SNDPATH_FE, SDT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    }
}
