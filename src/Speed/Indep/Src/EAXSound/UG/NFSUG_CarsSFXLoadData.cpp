#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Nitrous.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Skids.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

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
        }
        this->SPU_or_EE = 1;
        this->LoadAsset(m_pEAXCar->GetEngineAttributes().BankName_mainRAM(), SNDPATH_ENGINE, SDT_AEMS_ASYNCSPUMEM, type, true);
    }
}

void CARSFX_SingleGinsuEng::SetupLoadData() {
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().BankName_mainRAM(), SNDPATH_ENGINE, SDT_AEMS_AUDIOMEM, eBANK_SLOT_NONE, true);
    this->SPU_or_EE = 1;
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().Filename_GinsuAccel(), SNDPATH_ENGINE, SDT_GENERIC_DATA, eBANK_SLOT_NONE, true);
}

void CARSFX_DualGinsuEng::SetupLoadData() {
    this->SPU_or_EE = 1;
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().BankName_mainRAM(), SNDPATH_ENGINE, SDT_AEMS_AUDIOMEM, eBANK_SLOT_NONE, true);
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().Filename_GinsuAccel(), SNDPATH_ENGINE, SDT_GENERIC_DATA, eBANK_SLOT_NONE, true);
    this->LoadAsset(this->m_pEAXCar->GetEngineAttributes().Filename_GinsuDecel(), SNDPATH_ENGINE, SDT_GENERIC_DATA, eBANK_SLOT_NONE, true);
}
