//
//
//
//
#ifndef EAXAITUNERCAR_HPP
#define EAXAITUNERCAR_HPP

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/World/EventManager.hpp"

// total size: 0x11C
// Decl: 12
class EAXAITunerCar : public EAXCar {
  public:
    DECLARE_STATETYPE();
    EAXAITunerCar();
    ~EAXAITunerCar() override;

    // Overrides: CSTATE_Base
    void UpdateParams(float t) override;

    void UpdatAIDriveBy(float t);

    // Overrides: EAXCar
    int SFXMessage(eSFXMessageType SFXMessageType, uint32 param1, uint32 param2) override;

    bool IsEngineFinishedLoading();

    // Overrides: EAXCar
    void UpdateCarPhysics() override;

    static void ProcessEvent(emEvent *event);

    int m_PitchOffset;        // offset 0x114, size 0x4, Decl: 45
    bool mPhysicsChangedGear; // offset 0x118, size 0x1, Decl: 47
};

// total size: 0x11C
// Decl: 52
class EAXCopCar : public EAXAITunerCar {
  public:
    DECLARE_STATETYPE();
    EAXCopCar() {}           // Decl: 52
    ~EAXCopCar() override {} // Decl: 52

    // Overrides: CSTATE_Base
    void Attach(void *pAttachment) override;

    // Overrides: CSTATE_Base
    void UpdateParams(float t) override;
};

// total size: 0x11C
// Decl: 61
class EAXTruck : public EAXAITunerCar {
  public:
    DECLARE_STATETYPE();
    EAXTruck() {}           // Decl: 61
    ~EAXTruck() override {} // Decl: 61

    // Overrides: CSTATE_Base
    void UpdateParams(float t) override;
};

#endif
