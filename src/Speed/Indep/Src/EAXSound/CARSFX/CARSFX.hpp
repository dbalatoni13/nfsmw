#ifndef CARSFX_H
#define CARSFX_H

#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"

// total size: 0x28
class CARSFX : public SFX_Base {
  public:
    CARSFX();
    virtual ~CARSFX();
    float GetPhysTRQ() {
        return m_pEAXCar->GetPhysTRQ();
    }
    float GetPhysRPM() {
        return m_pEAXCar->GetPhysRPM();
    }

    eAemsUpgradeLevel m_UGL; // offset 0x24, size 0x4
};

#endif
