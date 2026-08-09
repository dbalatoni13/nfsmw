#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

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
    this->m_fAudioRPM = 0.0f;
    this->m_TransmissionUGL = AEMS_LEVEL0;
    this->m_nTrueEngineUpgradeLevel = AEMS_LEVEL2;
    this->m_nHornState = 0;
    this->t_CurTime = 0.0f;
    this->t_DeltaTime = 0.0f;
    this->m_IsDriveCamera = 0;
    this->m_pPhysicsCTL = nullptr;
    this->m_EngUGL = AEMS_LEVEL2;
    this->m_TurboUGL = AEMS_LEVEL0;
    this->m_NOSUGL = AEMS_LEVEL0;
    this->m_TireUGL = AEMS_LEVEL0;
}

EAXCar::~EAXCar() {
    CSTATE_Base::Destroy();
}
